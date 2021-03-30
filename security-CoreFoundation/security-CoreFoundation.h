//  Created by admin on 2020/8/23.
//  Copyright © 2020年 . All rights reserved.
//  Ant TechnologyGroup

#ifndef security_CoreFoundation_h
#define security_CoreFoundation_h


#include "securityHeader.h"

typedef struct _secinfo
{
    unsigned char * scan_result;
    bool is_plugScan;
}secinfo;

#if defined (__cplusplus)
extern "C"
{
#endif
    secinfo * Security_CoreFoundation(void);
#if defined (__cplusplus)
}
#endif



#endif /* security_CoreFoundation_h */
