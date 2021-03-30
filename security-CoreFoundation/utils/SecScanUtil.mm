//  Created by Shijie Cao on 23/03/2021.
//  Copyright © 2020 shijie. All rights reserved.
//  Ant TechnologyGroup

#import "SecScanUtil.h"
#import <Foundation/Foundation.h>

#include "../lib/macho/mach_parser.h"
#include "../lib/macho/dylib_info.h"

static BOOL isScanning = NO;

enum string_type {
    CLASS, METHOD, CSTRING, RULE
};


@interface SecScanUtil : NSObject

@property (nonatomic, assign) int plugCnt;

@property (nonatomic, strong) NSMutableArray *outputInfos;

@property (nonatomic, strong) NSMutableArray *inputRules;

- (BOOL)startScan:(NSDictionary *)dict;

@end

@implementation SecScanUtil

- (id)init {
    self = [super init];
    if (self) {
        self.inputRules = [NSMutableArray array];
        self.outputInfos = [NSMutableArray array];
    }
    return self;
}

- (NSString *)valueStr:(NSString *)str
{
    return str ? str : @"";
}

- (BOOL)modifyDict:(NSDictionary *)dict {
    if (dict && [dict isKindOfClass:[NSDictionary class]]) {
        for (NSString *key in dict.allKeys) {
            NSDictionary *value = dict[key];
            NSMutableDictionary *item = [NSMutableDictionary dictionaryWithDictionary:value];
            item[@"rule"] = key;
            [self.inputRules addObject:item];
        }
        return YES;
    } else {
        return NO;
    }
}


- (void)initInputRule:(input_rule *)rule  withType:(string_type)type andString:(NSString *)str{
    if (!rule || !str || !str.length) {
        return;
    }
    
    int length = (int)str.length;
    
    if (length > CHAR_MAX && type == RULE) {
        return;
    }
    
    if (length > UCHAR_MAX) {
        return;
    }
    
    switch (type) {
        case CLASS:
            rule->classes = (char *)malloc(length + 1);
            if (rule->classes) {
                strcpy(rule->classes, str.UTF8String);
            }
            break;
        case METHOD:
            rule->method = (char *)malloc(length + 1);
            if (rule->method) {
                strcpy(rule->method, str.UTF8String);
            }
            break;
        case CSTRING:
            rule->cstring = (char *)malloc(length + 1);
            if (rule->cstring) {
                strcpy(rule->cstring, str.UTF8String);
            }
            break;
        case RULE:
            rule->rule = (char *)malloc(length + 1);
            if (rule->rule) {
                strcpy(rule->rule, str.UTF8String);
            }
            break;
        default:
            break;
    }
}

- (void)freeLibNames:(char **)libs withSize:(int)size
{
    for (int i = 0; i < size; i++) {
        free(libs[i]);
    }
    
    free(libs);
}

- (void)freeInputRule:(input_rule *)rules withSize:(int)size
{
    for (int i = 0; i < size; i++) {
        input_rule tmp = rules[i];
        free(tmp.rule);
        free(tmp.classes);
        free(tmp.method);
        free(tmp.cstring);
    }
    
    free(rules);
}

- (void)initOutputInfo:(output_info *)outputs withSize:(int)size
{
    for (int i = 0; i < size; i++) {
        output_info tmp = outputs[i];
        tmp.rule = nil;
        tmp.file_name = nil;
    }
}

- (void)freeOutputInfo:(output_info *)outputs withSize:(int)size
{
    for (int i = 0; i < size; i++) {
        output_info tmp = outputs[i];
        free(tmp.rule);
        free(tmp.file_name);
    }
    
    free(outputs);
}

- (void)initInputRules:(input_rule *)input_rules withArray:(NSArray *)arr {
    for(int i = 0; i < arr.count; i++) {
        input_rule *tmp = input_rules + i;
        
        NSMutableDictionary * dict = arr[i];
        
        NSString *importHash = dict[@"importHash"];
        NSString *exportHash = dict[@"exportHash"];
        NSString *cls = dict[@"class"];
        NSString *method = dict[@"method"];
        NSString *strings = dict[@"strings"];
        NSString *rule = dict[@"rule"];
        
        if (importHash.length > 0) {
            strcpy((char *)tmp->import_hash, importHash.UTF8String);
        }
        
        if (exportHash.length > 0) {
            strcpy((char *)tmp->export_hash, exportHash.UTF8String);
        }
        
        [self initInputRule:tmp withType:CLASS andString:cls];
        
        [self initInputRule:tmp withType:METHOD andString:method];
        
        [self initInputRule:tmp withType:CSTRING andString:strings];
        
        [self initInputRule:tmp withType:RULE andString:rule];
    }
}

- (void)generateOutputInfos:(output_info *)output_infos withSize:(int)n_libs {
    char memoryDylib[512] = {0};
    get_dyld_image(memoryDylib,sizeof(memoryDylib));
    
    for (int i = 0; i < n_libs; i++) {
        struct output_info output = output_infos[i];
        if(output.file_name != NULL)
        {
            NSMutableDictionary * dict = [NSMutableDictionary dictionary];
            dict[@"rule"] = [NSString stringWithUTF8String:output.rule ? output.rule : ""];
            dict[@"md5"] = [NSString stringWithUTF8String:output.md5];
            dict[@"sha1"] = [NSString stringWithUTF8String:output.sha1];
            dict[@"importHash"] = [NSString stringWithUTF8String:output.import_hash];
            dict[@"exportHash"] = [NSString stringWithUTF8String:output.export_hash];
            dict[@"filename"] = [NSString stringWithUTF8String:output.file_name ? output.file_name : ""];
            dict[@"result"] = @(output.result);
            dict[@"modifytime"] = @(output.modify_time);
            dict[@"filesize"] = @(output.file_size);
            if(strstr(memoryDylib,output.file_name))
            {
                dict[@"inMem"] = @(1);
            }
            else
            {
                dict[@"inMem"] = @(0);
            }
            [self.outputInfos addObject:dict];
        }
    }
}

- (BOOL)startScan:(NSDictionary *)dict {
    if (![self modifyDict:dict]) {
        return NO;
    }
    
    //规则初始化
    int n_rules = (int)self.inputRules.count;
    struct input_rule *input_rules = (struct input_rule *)calloc(1, sizeof(struct input_rule) * n_rules);
    if (!input_rules) {
        return NO;
    }
    [self initInputRules:input_rules withArray:self.inputRules];

    int n_libs = 0;
    
    //1. 普通越狱目录的扫描
    char jalibreak_path[512] = "/Library/MobileSubstrate/DynamicLibraries";
    //2. App bundle目录的扫描
    const char * image_name = _dyld_get_image_name(0);
    char executable_path[1024] = {0};
    char framework_path[1048] = {0};
    if(strstr(image_name,"/var/") && strlen(image_name) < 1024)
    {
        memcpy(executable_path, image_name, (strrchr(image_name,'/') - image_name));
    }
    
    if(strlen(executable_path) > 0)
    {
        strcat(framework_path,executable_path);
        strcat(framework_path,"/Frameworks");
    }
    
    char * the_path[3] = {executable_path,framework_path,jalibreak_path};
    
    for(int i = 0;i < 3;i++)
    {
        char **lib_names = get_lib_names((char *)the_path[i], &n_libs);
        
        //如果lib_names == nil，则continue
        if (!lib_names) {
            continue;
        }
        
        if(lib_names && n_libs == 0) {
            free(lib_names);
            continue;
        }
        
        struct output_info *output_infos = (struct output_info *)calloc(1, sizeof(struct output_info) * n_libs);
        
        if (!output_infos) {
            [self freeLibNames:lib_names withSize:n_libs];
            return NO;
        }
        
        int error = dump_all_dylib((char *)the_path[i], lib_names, n_libs, input_rules, n_rules, output_infos);
        if (error) {
            [self freeLibNames:lib_names withSize:n_libs];
            [self freeOutputInfo:output_infos withSize:n_libs];
            [self freeInputRule:input_rules withSize:n_rules];
            return NO;
        }
        
        [self generateOutputInfos:output_infos withSize:n_libs];
        [self initOutputInfo:output_infos withSize:n_libs];
        
        [self freeLibNames:lib_names withSize:n_libs];
        [self freeInputRule:input_rules withSize:n_rules];
        [self freeOutputInfo:output_infos withSize:n_libs];
    }
    return YES;
}
@end


bool security_scanEngine(unsigned char * result)
{
    NSDictionary * conf = @{
                            @"FAKELBS_2":@{
                                    @"method":@"startUpdatingLocation"
                            },
                            @"FAKELBS_CLL": @{
                                    @"method": @"method_exchangeImplementation",
                                    @"strings": @"CLLocationManager,locationManager:did"
                                    },
                            @"FAKEDEV_IRES": @{
                                    @"method": @"IORegistryEntrySearch",
                                    @"strings": @"battery,SSID,osversion,hostname,machine,NSFileManager,UDID,Screen"
                                    },
                            @"FAKEDEV_NZT": @{
                                    @"importHash": @"58b52d35a63419f3819209dbfbbce019",
                                    @"exportHash": @"2407aaffa600ec174cffe63f3cdf2c40",
                                    @"class": @"iGTMBase64",
                                    @"method": @"_x5,_x6,_y6",
                                    @"strings": @"/var/mobile/Library/Preferences/org.ioshack.igrimace.adv.plist"
                                    },
                            @"FAKETOUCH_IOHIDEVENT": @{
                                    @"method": @"IOHIDEventAppendEvent,IOHIDEventCreateDigitizerEvent",
                                    @"strings": @"mainScreen"
                                    },
                            @"FAKEDEV_IORCREATE": @{
                                    @"method": @"IORegistryEntryCreate",
                                    @"strings": @"systemVersion,IOPlatformSerialNumber,ASIdentifierManager"
                                    },
                            };
    
    if (isScanning) {
        return false;
    }
    
    SecScanUtil *scanUtil = [[SecScanUtil alloc] init];
    if([scanUtil startScan:conf] == NO)
        return false;
    
    NSString * scan_result = [scanUtil.outputInfos componentsJoinedByString:@","];
    
    strncpy((char *)result,(char *)[scan_result UTF8String],[scan_result length]);
    return true;
}
