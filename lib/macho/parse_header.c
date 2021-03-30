//  Created by min on 27/09/2020.
//  Copyright © 2020 Shijie Cao. All rights reserved.
//  Ant TechnologyGroup

#include "parse_header.h"
#include <mach-o/swap.h>
#include <mach-o/fat.h>
#include "stdlib.h"
#include "parser_util.h"
#include "parse_load_cmd.h"
#include "AssertMacros.h"

__attribute__((__annotate__(("noobfus"))))
int read_magic(FILE *file, int offset, uint32_t *magic) {
    fseek(file, offset, SEEK_SET);
    size_t result = fread(magic, sizeof(uint32_t), 1, file);
    return result == 1 ? 0 : -1;
}

__attribute__((__annotate__(("noobfus"))))
int is_magic_64(uint32_t magic) {
    return magic == MH_MAGIC_64 || magic == MH_CIGAM_64;
}

//MH_CIGAM = 3472551422, MH_CIGAM_64 = 3489328638, FAT_CIGAM = 3199925962
//MH_MAGIC = 4277009102, MH_MAGIC_64 = 4277009103, FAT_MAGIC = 3405691582
__attribute__((__annotate__(("noobfus"))))
int need_swap_bytes(uint32_t magic) {
    return magic == MH_CIGAM || magic == MH_CIGAM_64 || magic == FAT_CIGAM;
}

__attribute__((__annotate__(("noobfus"))))
int is_fat(uint32_t magic) {
    return magic == FAT_MAGIC || magic == FAT_CIGAM;
}

__attribute__((__annotate__(("noobfus"))))
int dump_mach_header(FILE *file, int is_swap, struct mach_offset_info *info) {
    uint32_t ncmds;
    int load_commands_offset = info->arch_offset;
    
    if (info->is_64) {
        int header_size = sizeof(struct mach_header_64);
        struct mach_header_64 *header = load_bytes(file, info->arch_offset, header_size);
        
        if (!header) return -1;
        
        if (is_swap) {
            swap_mach_header_64(header, 0);
        }
        ncmds = header->ncmds;
        load_commands_offset += header_size;
        
        info->pie = (header->flags & MH_PIE) != 0;
        
        free(header);
    } else {
        int header_size = sizeof(struct mach_header);
        struct mach_header *header = load_bytes(file, info->arch_offset, header_size);
        
        if (!header) return -1;

        if (is_swap) {
            swap_mach_header(header, 0);
        }
        
        ncmds = header->ncmds;
        load_commands_offset += header_size;
        
        info->pie = (header->flags & MH_PIE) != 0;
        
        free(header);
    }
    
    dump_all_commands(file, load_commands_offset, is_swap, ncmds, info);
    
    return 0;

}

__attribute__((__annotate__(("noobfus"))))
int dump_fat_header(FILE *file, int is_swap, uint32_t *magic, int *first_arch_offset) {
    int header_size = sizeof(struct fat_header);
    struct fat_header *header = load_bytes(file, 0, header_size);
    
    if (!header) return -1;
    
    int arch_offset = header_size;
    
    if (is_swap) {
        swap_fat_header(header, 0);
    }
    
    struct fat_arch *arch = NULL;
    
    for (int i = 0; i < header->nfat_arch; i++) {
        int arch_size = sizeof(struct fat_arch);
        
        arch = load_bytes(file, arch_offset, arch_size);
        
        __Require(arch != NULL, exit);
        
        if (is_swap) {
            swap_fat_arch(arch, 1, 0);
        }
        
        if (i == 0) {
            *first_arch_offset = arch->offset;
            __Require(read_magic(file, *first_arch_offset, magic) == 0, error);
        }
        
        free(arch);
        arch_offset += arch_size;
        
    }
    free(header);
    return 0;
    
error:
    free(arch);

exit:
    free(header);
    return -1;
}

__attribute__((__annotate__(("noobfus"))))
int dump_offset_table(FILE *file, struct mach_offset_info *info) {
    
    uint32_t magic = 0;
    
    if (read_magic(file, 0, &magic)) return -1;
    
    int fat = is_fat(magic);
    
    int arch_offset = 0;
    
    if (fat) {
        int is_swap = need_swap_bytes(magic);
        int error = dump_fat_header(file, is_swap, &magic, &arch_offset);
        if (error) return -1;
    }
    
    int is_64 = is_magic_64(magic);
    int is_swap = need_swap_bytes(magic);
    
    info->arch_offset = arch_offset;
    info->is_64 = is_64;
    
    int error = dump_mach_header(file, is_swap, info);

    return error;
}
