//  Created by Shijie Cao on 26/09/2020.
//  Copyright © 2020 Shijie Cao. All rights reserved.
//  Ant TechnologyGroup

#ifndef dylib_info_h
#define dylib_info_h

#include <stdio.h>

#include "mach_parser.h"

#if defined (__cplusplus)
extern "C"
{
#endif

    char** get_lib_names(char *path, int *n_libs);

    int dump_all_dylib(char *dir, char **lib_names, int n_libs, struct input_rule *rule, int n_rules, struct output_info *output);

    void get_dyld_image(char * dyld_str, int32_t length);
    
#if defined (__cplusplus)
}
#endif

#endif /* dylib_info_h */
