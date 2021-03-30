//  Created by admin on 28/09/2020.
//  Copyright © 2020 Shijie Cao. All rights reserved.
//  Ant TechnologyGroup

#ifndef macho_structs_h
#define macho_structs_h

#include <mach-o/loader.h>

struct sect_strings {
    
    char *method;
    uint64_t method_len;
    
    char *classes;
    uint64_t classes_len;
    
    char *cstring;
    uint64_t cstring_len;
    
    char *sym_in;
    char *sym_ex;
    
    unsigned char *import_hash;
    unsigned char *export_hash;
    
    char code_sign_hash[40];
};

struct data_item {
    uint32_t offset;
    uint32_t size;
};

struct text_item {
    uint32_t fileoffset;
    uint32_t addr;
};

struct data_item_64 {
    uint32_t offset;
    uint64_t size;
};

struct text_section_table {
    struct data_item method;
    struct data_item classes;
    struct data_item cstring;
    struct text_item text;
};

struct text_section_table_64 {
    struct data_item_64 method;
    struct data_item_64 classes;
    struct data_item_64 cstring;
};

struct mach_offset_info {
    uint32_t arch_offset;
    int is_64;
    
    int is_encrypt;
    int pie;

    char code_sign_name[256];
    
    struct text_section_table section;
    struct text_section_table_64 section_64;
    
    //symbol table offset and size
    struct symtab_command sc;
};


#endif /* macho_structs_h */
