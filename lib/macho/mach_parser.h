//  Created by Shijie Cao on 26/09/2020.
//  Copyright © 2020 Shijie Cao. All rights reserved.
//  Ant TechnologyGroup

#ifndef mach_parser_h
#define mach_parser_h

#include <stdio.h>

struct input_rule {
    char *rule;
    
    unsigned char import_hash[36];
    unsigned char export_hash[36];
    
    char *classes;
    char *method;
    char *cstring;
};

struct output_info {
    char *file_name;             //文件名称
    long file_size;                  //文件大小
    long modify_time;                //文件修改时间
    char md5[36];                    //整个文件的md5，33bytes
    char sha1[44];                   //整个文件的sha1，41bytes
    
    char *rule;                  //对应的规则名称，如果没有命中，则为空
    int  result;                     //命中了那些配置
    
    char import_hash[36];            //导入表hash
    char export_hash[36];            //导出表hash
};

int dump_dylib(FILE *file, struct input_rule *rules, int n_rules, struct output_info *output);

void dump_file_stat(FILE *file, char *path, struct output_info *output);

#endif /* mach_parser_h */
