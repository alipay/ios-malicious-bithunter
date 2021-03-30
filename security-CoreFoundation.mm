//  Created by admin on 2020/8/23.
//  Copyright © 2020年 . All rights reserved.
//  Ant TechnologyGroup

#include "security-CoreFoundation.h"

#include "utils/SecScanUtil.h"
#import <Foundation/Foundation.h>
#import <pthread.h>

static secinfo * info = NULL;
static dispatch_semaphore_t semaphore = dispatch_semaphore_create(1);

secinfo * Security_CoreFoundation(void)
{
    dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
    
    if(info == NULL)
    {
        info = (secinfo *)malloc(sizeof(secinfo));
        info->scan_result = (unsigned char * ) malloc(1024*1024);
        memset(info->scan_result,0,1024*1024);
        if(info == NULL)
        {
            goto RETURN;
        }
        
        info->is_plugScan = security_scanEngine((unsigned char *)info->scan_result);
    }

RETURN:
    dispatch_semaphore_signal(semaphore);
    
    return info;
}
