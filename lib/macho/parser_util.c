//  Created by Shijie Cao on 29/09/2020.
//  Copyright © 2020 Shijie Cao. All rights reserved.
//  Ant TechnologyGroup

#include "parser_util.h"
#include "stdlib.h"
#include "string.h"
__attribute__((__annotate__(("noobfus"))))
void *load_bytes(FILE *file, int offset, size_t size) {
    void *buf = calloc(1, size);
    if (!buf) return NULL;
    fseek(file, offset, SEEK_SET);
    if (fread(buf, size, 1, file) == 1) {
        return buf;
    } else {
        free(buf);
        return NULL;
    }
}

__attribute__((__annotate__(("noobfus"))))
void md5_hash(char *buffer, uint32_t len, unsigned char *result) {
    if (!buffer) {
        return;
    }
    unsigned char md5_value[CC_MD5_DIGEST_LENGTH];
    CC_MD5(buffer, len, md5_value);
    for(int i = 0; i < CC_MD5_DIGEST_LENGTH; i++) {
        snprintf((char *)result + i*2, 2+1, "%02x", md5_value[i]);
    }
}

__attribute__((__annotate__(("noobfus"))))
void replace_null_char(char *buffer, size_t len) {
    if (!buffer || !len) {
        return;
    }
    for (int i = 0; i < len; i++) {
        if (buffer[i] == '\0') {
            buffer[i] = ',';
        }
    }
}

__attribute__((__annotate__(("noobfus"))))
bool has_all_tokens(char *big, char *little) {
    char *sep = ",";
    
    if (!big || !little || !sep || !little[0] || !big[0]) {
        return false;
    }
    
    char *temp_str = calloc(strlen(little) + 1, sizeof(char));
    if (!temp_str) return false;
    
    strcpy(temp_str, little);
    
    char *token = strtok(temp_str, sep);
    
    while (token) {
        if (!strstr(big, token)) {
            free(temp_str);
            return false;
        }
        token = strtok(NULL, sep);
    }
    
    free(temp_str);
    return true;
}




