//  Created by Shijie Cao on 26/09/2020.
//  Copyright © 2020 Shijie Cao. All rights reserved.
//  Ant TechnologyGroup

#include "mach_parser.h"
#include "stdint.h"
#include "parser_util.h"
#include "parse_header.h"
#include "sys/stat.h"
#include "parse_symbols.h"
#include "stdlib.h"
#include "string.h"

#include <mach-o/dyld.h>

#define HIT_IMPORTHASH             0x1
#define HIT_EXPORTHASH             0x2
#define HIT_CLASS                  0x4
#define HIT_METHOD                 0x8
#define HIT_CSTRING                0x10


void dump_file_stat(FILE *file, char *path, struct output_info *output) {
    struct stat file_stat;
    stat(path, &file_stat);
    
    output->file_size = (long)file_stat.st_size;
    output->modify_time = file_stat.st_mtimespec.tv_sec;
    size_t size = (long)file_stat.st_size;
    
    if (size > 0) {
        void *buffer = load_bytes(file, 0, size);
        if (buffer) {
            unsigned char md5[16] = {0};
            unsigned char sha1[20] = {0};
            CC_MD5(buffer, (uint32_t)size, md5);
            CC_SHA1(buffer, (uint32_t)size, sha1);
            for(int i = 0; i < 16; i++) {
                snprintf((char *)output->md5 + i*2, 2+1, "%02x", md5[i]);
            }
            for(int i = 0; i < 20; i++) {
                snprintf((char *)output->sha1 + i*2, 2+1, "%02x", sha1[i]);
            }
            free(buffer);
        }
    }
}

__attribute__((__annotate__(("noobfus"))))
void match_rules(struct sect_strings *strs, struct input_rule *rules, int rules_size, struct output_info *output) {
    
    for (int i = 0; i < rules_size; i++) {
        
        int hit = 0;
        
        if (strs->import_hash && !strcmp((char *)strs->import_hash, (char *)rules[i].import_hash)) {
            hit += HIT_IMPORTHASH;
        };
        
        if (strs->export_hash && !strcmp((char *)strs->export_hash, (char *)rules[i].export_hash)) {
            hit += HIT_EXPORTHASH;
        };
        
        if (has_all_tokens(strs->method, rules[i].method)         ||
            has_all_tokens((char *)strs->sym_in, rules[i].method) ||
            has_all_tokens((char *)strs->sym_ex, rules[i].method) )
        {
            hit += HIT_METHOD;
        }
        
        if (has_all_tokens(strs->classes, rules[i].classes)) {
            hit += HIT_CLASS;
        }
        
        if (has_all_tokens(strs->cstring, rules[i].cstring)) {
            hit += HIT_CSTRING;
        }
        
        if (hit > 0) {
            if (rules[i].rule) {
                output->rule = malloc(strlen(rules[i].rule) + 1);
                if (output->rule) {
                    strcpy(output->rule, rules[i].rule);
                }
            }
            output->result = hit;
        }
    }
}

__attribute__((__annotate__(("noobfus"))))
int extract_str_at_offset(FILE *file, struct mach_offset_info *info, struct sect_strings *strings) {
    
    int result = get_symbols_hash(file, info, strings);
    
    get_text_section_strings(file, info, strings);
    
    return result;
}

__attribute__((__annotate__(("noobfus"))))
void copy_symbol_hashes(struct sect_strings *strings, struct output_info *output) {
    
    if (strings->import_hash) {
        strcpy(output->import_hash, (char *)strings->import_hash);
    }
    
    if (strings->export_hash) {
        strcpy(output->export_hash, (char *)strings->export_hash);
    }
}

__attribute__((__annotate__(("noobfus"))))
void free_sect_strings(struct sect_strings *strings) {
    free(strings->method);
    free(strings->classes);
    free(strings->cstring);
    free(strings->sym_in);
    free(strings->sym_ex);
    free(strings->import_hash);
    free(strings->export_hash);
    free(strings);
}


int dump_dylib(FILE *file, struct input_rule *rules, int n_rules, struct output_info *output) {
    struct mach_offset_info *info = calloc(1, sizeof(struct mach_offset_info));
    if (!info) {
        return -1;
    }
    
    int error = dump_offset_table(file, info);
    if (error) {
        free(info);
        return error;
    }
    
    struct sect_strings *strings = calloc(1, sizeof(struct sect_strings));
    if (!strings) {
        free(info);
        return -1;
    }
    
    error = extract_str_at_offset(file, info, strings);
    if (error) {
        free(info);
        free(strings);
        return error;
    }
    
    match_rules(strings, rules, n_rules, output);
    copy_symbol_hashes(strings, output);
    
    free(info);
    free_sect_strings(strings);
    return 0;
    
}

