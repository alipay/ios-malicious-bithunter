//  Created by admin on 2020/8/23.
//  Copyright © 2020年 . All rights reserved.
//  Ant TechnologyGroup

#ifndef mem_h
#define mem_h

#include <stdio.h>
#include "../../security-CoreFoundation.h"

struct libRange
{
    char name[256];
    long start;
    long end;
};

#if defined (__cplusplus)
extern "C"
{
#endif
    bool getTextSegmentAddr(void * srcAddr);
#if defined (__cplusplus)
}
#endif

#endif /* mem_h */
