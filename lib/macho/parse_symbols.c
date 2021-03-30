//  Created by min on 27/09/2020.
//  Copyright © 2020 Shijie Cao. All rights reserved.
//  Ant TechnologyGroup

#include "parse_symbols.h"
#include <mach-o/nlist.h>
#include "parser_util.h"
#include "stdint.h"
#include "stdlib.h"
#include "limits.h"
#include "string.h"
#include "AssertMacros.h"

#define PS_SYMBOLS_HASH_ERR 0xb
#define PS_STR_SIZE_ERR 0xc
#define SYM_SIZE_LIMIT 256
#define STR_SIZE_LIMIT 200000

__attribute__((__annotate__(("noobfus"))))
int strcat_sym(char *big, char *symbol) {
    if (symbol) {
        size_t len = strlen(symbol);
        if (len > 0 && len < SYM_SIZE_LIMIT && strcmp(symbol, " ")) {
            if (strlen(big)) {
                strcat(big, ",");
            }
            strcat(big, symbol);
            return 0;
        } else {
            return -1;
        }
    }
    return -1;
}

__attribute__((__annotate__(("noobfus"))))
int get_symbols_hash(FILE *file, struct mach_offset_info *info, struct sect_strings *strings) {
    
    struct symtab_command sc = info->sc;

    if (sc.strsize > STR_SIZE_LIMIT) {
        return PS_STR_SIZE_ERR;
    }
    
    char *string_table = load_bytes(file, info->arch_offset + sc.stroff, sc.strsize);
    __Require(string_table != NULL, exit);
    
    char *sym_in = (char *)calloc(1, sc.strsize + 1);
    __Require(sym_in != NULL, error);
    
    char *sym_ex = (char *)calloc(1, sc.strsize + 1);
    __Require(sym_in != NULL, error1);
    
    int sym_off = info->arch_offset + sc.symoff;
    
    if (info->is_64) {
        int size = sizeof(struct nlist_64) * sc.nsyms;
        struct nlist_64 *ptr_64 = load_bytes(file, sym_off, size);
        if(ptr_64 != NULL)
        {
            for (int i = 0; i < sc.nsyms; i++) {
                struct nlist_64 nl = ptr_64[i];
                if (nl.n_type == 0x1) {
                    
                    char *symbol = string_table + nl.n_un.n_strx;
                    __Require(strcat_sym(sym_in, symbol) == 0, error2);
                    
                } else if (nl.n_type == 0xf) {
                    
                    char *symbol = string_table + nl.n_un.n_strx;
                    __Require(strcat_sym(sym_ex, symbol) == 0, error2);
                }
            }
            free(ptr_64);
        }
    } else {
        int size = sizeof(struct nlist) * sc.nsyms;
        struct nlist *ptr_32 = load_bytes(file, sym_off, size);
        if(ptr_32 != NULL)
        {
            for (int i = 0; i < sc.nsyms; i++) {
                struct nlist nl = ptr_32[i];
                if (nl.n_type == 0x1) {
                    
                    char *symbol = string_table + nl.n_un.n_strx;
                    __Require(strcat_sym(sym_in, symbol) == 0, error2);

                } else if (nl.n_type == 0xf) {
                    
                    char *symbol = string_table + nl.n_un.n_strx;
                    __Require(strcat_sym(sym_ex, symbol) == 0, error2);
                }
            }
            free(ptr_32);
        }
    }

    
    uint32_t len = (uint32_t)strlen(sym_in);
    if (len) {
        strings->import_hash = malloc(HASH_LENGTH);
        if (strings->import_hash) {
            md5_hash(sym_in, len, strings->import_hash);
        }
        
        strings->sym_in = malloc(len + 1);
        if (strings->sym_in) {
            strcpy((char *)strings->sym_in, sym_in);
        }
    }
    
    len = (uint32_t)strlen(sym_ex);
    if (len) {
        strings->export_hash = malloc(HASH_LENGTH);
        if (strings->export_hash) {
            md5_hash(sym_ex, len, strings->export_hash);
        }
        
        strings->sym_ex = malloc(len + 1);
        if (strings->sym_ex) {
            strcpy((char *)strings->sym_ex, sym_ex);
        }
    }
    
    free(string_table);
    free(sym_in);
    free(sym_ex);
    
    return 0;

error2:
    free(sym_ex);
error1:
    free(sym_in);
error:
    free(string_table);
exit:
    return PS_SYMBOLS_HASH_ERR;
}

__attribute__((__annotate__(("noobfus"))))
void *load_bytes_item(FILE *file, uint32_t arch_offset, struct data_item item) {
    if (item.offset > 0 && item.size > 0) {
        char *buffer = load_bytes(file, arch_offset + item.offset, item.size);
        if (buffer) {
            replace_null_char(buffer, item.size);
            return buffer;
        }
    }
    return NULL;
}

__attribute__((__annotate__(("noobfus"))))
void *load_bytes_item_64(FILE *file, uint32_t arch_offset, struct data_item_64 item) {
    if (item.offset > 0 && item.size > 0) {
        char *buffer = load_bytes(file, arch_offset + item.offset, (size_t)item.size);
        if (buffer) {
            replace_null_char(buffer, (size_t)item.size);
            return buffer;
        }
    }
    return NULL;
}

__attribute__((__annotate__(("noobfus"))))
void get_text_section_strings(FILE *file, struct mach_offset_info *info, struct sect_strings *strings) {
    
    if (info->is_64) {
        strings->method = load_bytes_item_64(file, info->arch_offset, info->section_64.method);
        strings->classes = load_bytes_item_64(file, info->arch_offset, info->section_64.classes);
        strings->cstring = load_bytes_item_64(file, info->arch_offset, info->section_64.cstring);
        
    } else {
        
        strings->method = load_bytes_item(file, info->arch_offset, info->section.method);
        strings->classes = load_bytes_item(file, info->arch_offset, info->section.classes);
        strings->cstring = load_bytes_item(file, info->arch_offset, info->section.cstring);
    }
}

