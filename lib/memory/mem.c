//  Created by admin on 2020/8/23.
//  Copyright © 2020年 . All rights reserved.
//  Ant TechnologyGroup

#include "mem.h"
#include <stdlib.h>


bool getTextSegmentAddr(void * srcAddr)
{
    const struct mach_header *mach_hdr;
    mach_hdr = _dyld_get_image_header(0);
    if(mach_hdr == NULL)
        return false;
    
    intptr_t vmSlid = _dyld_get_image_vmaddr_slide(0);
    if(vmSlid == 0)
        return false;
    
    const struct load_command *cmds = NULL;
    if(mach_hdr->magic == MH_MAGIC_64)
    {
        cmds = (struct load_command *)((Byte *)mach_hdr + sizeof(struct mach_header_64));
    }
    if(mach_hdr->magic == MH_MAGIC)
    {
        cmds = (struct load_command *)((Byte *) mach_hdr + sizeof(struct mach_header));
    }
    
    if(cmds == NULL)
        return false;
    
    uint32_t cmdsleft;
    const struct load_command *lc;
    
    for(lc = cmds, cmdsleft = mach_hdr->ncmds; cmdsleft--;)
    {
        if(lc->cmd == LC_SEGMENT) {
            const struct segment_command * sc = (void *) lc;
            const struct section *sect = (void *) (sc + 1);
            for(uint32_t sect_idx = 0; sect_idx < sc->nsects; sect_idx++) {
                if(!strcmp("__TEXT", sect->segname) && !strcmp("__text", sect->sectname)) {
                    unsigned long memAddr = (sc->vmaddr + vmSlid + sect->offset - sc->fileoff);
                    if((unsigned long)srcAddr < memAddr || (unsigned long)srcAddr > memAddr + sect->size)
                        return true;
                    break;
                }
                sect++;
            }
        }
        else if(lc->cmd == LC_SEGMENT_64)
        {
            const struct segment_command_64 * sc = (void *) lc;
            const struct section_64 *sect = (void *) (sc + 1);
            for(uint32_t sect_idx = 0; sect_idx < sc->nsects; sect_idx++) {
                if(!strcmp("__TEXT", sect->segname) && !strcmp("__text", sect->sectname)) {
                    unsigned long long memAddr = (sc->vmaddr + vmSlid + sect->offset - sc->fileoff);
                    if((unsigned long)srcAddr < memAddr || (unsigned long)srcAddr > memAddr + sect->size)
                        return true;
                    break;
                }
                sect++;
            }
        }
        
        
        lc = (void *) ((char *) lc + lc->cmdsize);
    }
    return true;
}
