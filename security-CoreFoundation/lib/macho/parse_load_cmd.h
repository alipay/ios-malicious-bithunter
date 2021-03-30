//  Created by admin on 27/09/2020.
//  Copyright © 2020 Shijie Cao. All rights reserved.
//  Ant TechnologyGroup

#ifndef parse_load_cmd_h
#define parse_load_cmd_h

#include <stdio.h>
#include "parser_structs.h"

int dump_all_commands(FILE *file, int offset, int is_swap, int ncmds, struct mach_offset_info *info);

#endif /* parse_load_cmd_h */
