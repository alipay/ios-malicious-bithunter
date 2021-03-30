//  Created by Shijie Cao on 26/09/2020.
//  Copyright © 2020 Shijie Cao. All rights reserved.
//  Ant TechnologyGroup

#define DYLIB_PATH "/Library/MobileSubstrate/DynamicLibraries/"

#include "dylib_info.h"
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <mach-o/dyld.h>

void get_dyld_image(char * dyld_str, int32_t length) {
    uint32_t dyld_count = _dyld_image_count();
    char *name = NULL;
    for (uint32_t i = 0; i < dyld_count; i++) {
        const char *image_name = _dyld_get_image_name(i);
        
        if (!strstr(image_name,"/System/Library/") &&
            !strstr(image_name,"/usr/") &&
            !strstr(image_name,"/Developer/"))
        {
            name = ((char *)strrchr(image_name, '/') + 1);
            
            size_t dyld_len = strlen(dyld_str);
            
            if (strlen(name) + dyld_len < length && strstr(name,".dylib")) {
                if (dyld_len > 0) {
                    strcat(dyld_str, ",");
                }
                strcat(dyld_str, name);
                
            }
        }
    }
}

char** get_lib_names(char *path, int *n_libs) {
    
    DIR *dir;
    
    if (path) {
        dir = opendir(path);
    } else {
        dir = opendir(DYLIB_PATH);
    }
    
    if (!dir) {
        return NULL;
    }
    
    struct dirent *ptr;
    int i = 0;
    
    int init_size = 12;
    
    char **arr = malloc(sizeof(char *) * init_size);
    
    while ((ptr=readdir(dir)))
    {
        char *d_name = ptr->d_name;
        
        if(strcmp(d_name, ".") == 0 || strcmp(d_name, "..") == 0) {
            continue;
        } else if (ptr->d_type == DT_REG && strstr(d_name, ".dylib")) {
            i++;
            if (i > init_size) {
                init_size *= 1.5;
                arr = realloc(arr, sizeof(char *) * init_size);
                if (!arr) {
                    goto EXIT;
                }
            }
            char *name = malloc(strlen(d_name) + 1);
            if (!name) {
                goto EXIT;
            }
            strcpy(name, d_name);
            *(arr + i - 1) = name;
        }
    }
    
    *n_libs = i;
    closedir(dir);
    return arr;
    
EXIT:
    closedir(dir);
    return arr;
}

int dump_all_dylib(char *dir, char **libs, int n_libs, struct input_rule *rules, int n_rules, struct output_info *output) {
    
    if (!libs || !output || !n_libs || !n_rules) {
        return -1;
    }
    
    if (!dir) {
        dir = DYLIB_PATH;
    }
    
    int dir_len = (int)strlen(dir);
    
    for (int i = 0; i < n_libs; i++) {
        
        int name_len = (int)strlen(*(libs+i));
        int path_len = dir_len + name_len + 2;
        char path[1024] = {0};

        snprintf(path, path_len, "%s/%s", dir, libs[i]);
        
        FILE *file = fopen(path, "rb");
        if (file) {
            
            char *name = malloc(name_len + 1);
            
            if (name) {
                output[i].file_name = name;
                strcpy(output[i].file_name, libs[i]);
            } else {
                fclose(file);
                return -1;
            }
            
            dump_file_stat(file, path, &output[i]);
            
            if (output[i].file_size > 0) {
                int error = dump_dylib(file, rules, n_rules, &output[i]);
                fclose(file);
                if (error) {
                    if (error != 0xc && error != 0xb) {
                        return error;
                    }
                }
            } else {
                fclose(file);
            }
        }
    }
    
    return 0;
}



