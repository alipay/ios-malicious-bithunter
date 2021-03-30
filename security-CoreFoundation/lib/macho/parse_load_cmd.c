//  Created by min on 27/09/2020.
//  Copyright © 2020 Shijie Cao. All rights reserved.
//  Ant TechnologyGroup


#include <mach-o/swap.h>
#include "stdlib.h"
#include "string.h"

#include "parser_util.h"
#include "parse_symbols.h"
#include "parse_load_cmd.h"

__attribute__((__annotate__(("noobfus"))))
int dump_lc_segment_section(FILE *file, uint32_t nsects, struct text_section_table *sect) {
    if (nsects > 100) {
        return -1;
    }
    
    for (int i = 0; i < nsects; i++) {
        struct section section;
        size_t result = fread(&section, sizeof(section), 1, file);
        if (result != 1) return -1;
        if (strcmp(section.sectname, "__objc_methname") == 0) {
            sect->method.offset = section.offset;
            sect->method.size = section.size;
        } else if (strncmp(section.sectname, "__objc_classname",sizeof(section.sectname)) == 0) {
            sect->classes.offset = section.offset;
            sect->classes.size = section.size;
        } else if (strcmp(section.sectname, "__cstring") == 0) {
            sect->cstring.offset = section.offset;
            sect->cstring.size = section.size;
        } else if (strcmp(section.sectname, "__text") == 0) {
            sect->text.fileoffset = section.offset;
            sect->text.addr = section.addr;
        }
    }
    
    return 0;
}

__attribute__((__annotate__(("noobfus"))))
int dump_lc_segment_section_64(FILE *file, uint32_t nsects, struct text_section_table_64 *sect) {
    if (nsects > 100) {
        return -1;
    }
    
    for (int i = 0; i < nsects; i++) {
        struct section_64 section;
        size_t result = fread(&section, sizeof(section), 1, file);
        if (result != 1) return -1;
        
        if (strcmp(section.sectname, "__objc_methname") == 0) {
            sect->method.offset = section.offset;
            sect->method.size = section.size;
        } else if (strstr(section.sectname, "__objc_classname")) {
            sect->classes.offset = section.offset;
            sect->classes.size = section.size;
        } else if (strcmp(section.sectname, "__cstring") == 0) {
            sect->cstring.offset = section.offset;
            sect->cstring.size = section.size;
        }
    }
    
    return 0;
}

__attribute__((__annotate__(("noobfus"))))
int dump_lc_segment(FILE *file, struct text_section_table *sect) {
    struct segment_command sc;
    size_t result = fread(&sc, sizeof(sc), 1, file);
    if (result != 1) return -1;
    
    if (!strcmp(sc.segname, SEG_TEXT)) {
        return dump_lc_segment_section(file, sc.nsects, sect);
    }
    
    return 0;
}

__attribute__((__annotate__(("noobfus"))))
int dump_lc_segment_64(FILE *file, struct text_section_table_64 *sect) {
    struct segment_command_64 sc;
    
    size_t result = fread(&sc, sizeof(sc), 1, file);
    if (result != 1) return -1;
    
    if (!strcmp(sc.segname, SEG_TEXT)) {
        return dump_lc_segment_section_64(file, sc.nsects, sect);
    }
    
    return 0;
}

int get_code_signature(FILE *file, struct mach_offset_info *info, struct data_item code_sign) {
    
    char *buffer = load_bytes(file, info->arch_offset + code_sign.offset, code_sign.size);
    char *tmp = NULL;
    int length = 0;
    if (!buffer) return -1;
    
    memset(info->code_sign_name,0,sizeof(info->code_sign_name));
    
    for(int i = 0 ;i < code_sign.size;i++)
    {
        if(buffer[i] == '\0')
            buffer[i] = ' ';
    }
    buffer[code_sign.size - 1] = '\0';
    
    tmp = strstr(buffer,"iPhone Distribution: ");
    length = strlen("iPhone Distribution: ");
    if(tmp == NULL)
    {
        tmp = strstr(buffer, "iPhone Developer: ");
        length = strlen("iPhone Developer: ");
    }
    
    if(tmp != NULL)
    {
        //标识前1个字节代表签名的长度，size = 签名的总长度 - 标识的长度
        int size = (int)*(tmp - 1) - length;
        
        //判断签名是否超长
        size = size < sizeof(info->code_sign_name) ? size : sizeof(info->code_sign_name) - 1;
        
        //指向签名内容
        tmp += length;
        strncat(info->code_sign_name,tmp,size);
    }
    else
    {
        strcat(info->code_sign_name,"Apple Inc");
    }
    
    free(buffer);
    
    return 0;
}

__attribute__((__annotate__(("noobfus"))))
int dump_lc_code_signature(FILE *file, struct mach_offset_info *info) {
    struct linkedit_data_command ldc;
    
    size_t result = fread(&ldc, sizeof(ldc), 1, file);
    if (result != 1) return -1;
    
    struct data_item code_sign;
    code_sign.offset = ldc.dataoff;
    code_sign.size = ldc.datasize;
    
    return get_code_signature(file, info, code_sign);
}

__attribute__((__annotate__(("noobfus"))))
int dump_lc_encryption_info(FILE *file, int *encrypt) {
    struct encryption_info_command eic;
    size_t result = fread(&eic, sizeof(eic), 1, file);
    if (result != 1) return -1;
    
    *encrypt = eic.cryptid ? 1 : 0;
    return 0;
}

__attribute__((__annotate__(("noobfus"))))
int dump_lc_encryption_info_64(FILE *file, int *encrypt) {
    struct encryption_info_command_64 eic;
    size_t result = fread(&eic, sizeof(eic), 1, file);
    if (result != 1) return -1;
    
    *encrypt = eic.cryptid ? 1 : 0;
    return 0;
}

__attribute__((__annotate__(("noobfus"))))
int dump_lc_symtab(FILE *file, struct symtab_command *sc) {
    
    return !(fread(sc, sizeof(struct symtab_command), 1, file) == 1);
}

__attribute__((__annotate__(("noobfus"))))
int dump_load_command(FILE *file, struct load_command *lc, struct mach_offset_info *info) {
    
    int error = 0;
    
    switch (lc->cmd) {
        case LC_SEGMENT:
            error = dump_lc_segment(file, &info->section);
            break;
        case LC_SEGMENT_64:
            error = dump_lc_segment_64(file, &info->section_64);
            break;
        case LC_CODE_SIGNATURE:
            error = dump_lc_code_signature(file, info);
            break;
        case LC_ENCRYPTION_INFO:
            error = dump_lc_encryption_info(file, &info->is_encrypt);
            break;
        case LC_ENCRYPTION_INFO_64:
            error = dump_lc_encryption_info_64(file, &info->is_encrypt);
            break;
        case LC_SYMTAB:
            error = dump_lc_symtab(file, &info->sc);
            break;
        default:
            break;
    }
    
    return error;
}

__attribute__((__annotate__(("noobfus"))))
int dump_all_commands(FILE *file, int offset, int is_swap, int ncmds, struct mach_offset_info *info) {
    if (ncmds > 100) {
        return - 1;
    }
    
    int cmd_size = sizeof(struct load_command);
    int cur_offset = offset;
    struct load_command *cmd = NULL;
    
    for (int i = 0; i < ncmds; i++) {
        cmd = load_bytes(file, cur_offset, cmd_size);
        
        if (!cmd) return -1;
        
        if (is_swap) {
            swap_load_command(cmd, 0);
        }
        
        fseek(file, -cmd_size, SEEK_CUR);
        
        int error = dump_load_command(file, cmd, info);
        
        if (error) {
            free(cmd);
            return -1;
        }
        
        cur_offset += cmd->cmdsize;
        free(cmd);
        
    }
    
    return 0;
}
