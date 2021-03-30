//  Created by Shijie Cao on 29/09/2020.
//  Copyright © 2020 Shijie Cao. All rights reserved.
//  Ant TechnologyGroup

#ifndef parser_util_h
#define parser_util_h

#include <stdio.h>
#import <CommonCrypto/CommonDigest.h>
#include "stdbool.h"

#define HASH_LENGTH CC_MD5_DIGEST_LENGTH * 2 + 1

void *load_bytes(FILE *file, int offset, size_t size);

void md5_hash(char *buffer, uint32_t len, unsigned char *result);

bool has_all_tokens(char *big, char *little);

void replace_null_char(char *buffer, size_t len);

#endif /* parser_util_h */
