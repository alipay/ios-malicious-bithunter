//  Created by admin on 27/09/2020.
//  Copyright © 2020 Shijie Cao. All rights reserved.
//  Ant TechnologyGroup

#ifndef parse_symbols_h
#define parse_symbols_h

#include <stdio.h>
#include "parser_structs.h"

int get_symbols_hash(FILE *file, struct mach_offset_info *info, struct sect_strings *strings);

void get_text_section_strings(FILE *file, struct mach_offset_info *info, struct sect_strings *strings);

#endif /* parse_symbols_h */
