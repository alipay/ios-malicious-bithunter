# iOS Malicious Bit Hunter

## Abstract
  iOS Malicious Bit Hunter is a malicious plug-in detection engine for iOS applications. It can analyze the head of the macho file of the injected dylib dynamic library based on runtime, and can perform behavior analysis through interface input characteristics to determine the behavior of the dynamic library feature. The program does not rely on the jailbreak environment and can be used on the AppStore.

## What’s new feature 
  This is a new way to accurately identify the behavior of iOS malicious code. Based on the method of analyzing malicious code running in the memory based on the Mach-O format in the App, it can perform flexible behavior recognition for analysis and continuous tracking. This is a very accurate anti-plug-in defense method, including behaviors, variables, and highly obfuscated code (including ollvm), which has achieved very good results in our defense process.

## Logic Priciple 
  The tool provides an API interface for obtaining externally match rules. It will analyze the behavior of jailbroken and repackaged dynamic libraries at runtime, mainly through static analysis of the MachO file format, through the Load Command and different segments Obtain the import table, export table, class name, function name, string and other information, and judge whether the dynamic library has been loaded into the memory through the memory map at runtime.
  
![Logic Priciple](https://github.com/SecurityLife/iOS_Malicious_Bit_Hunter/blob/main/logic%20priciple.jpg)

## Detect demo result
  We used our SDK to analyze tens of thousands of jailbreak dynamic libraries. These resources come from some public jailbreak plugins. Below are our current test results：

filename,rule

ALS.dylib,FAKEDEV_H

AWZ.dylib,FAKEDEV_H

CallAssist.dylib,FAKEDEV_H

EGrimaceTweak.dylib,FAKEDEV_H

Lynx.dylib,FAKEDEV_H

NZT.dylib,FAKEDEV_H

SystemInfo.dylib,FAKEDEV_H

YOY.dylib,FAKEDEV_H

fakephonelib.dylib,FAKEDEV_H

fakid.dylib,FAKEDEV_H

nt.dylib,FAKEDEV_H

AWZ.dylib,FAKEDEV_M

1feaks.dylib,FAKETOUCH

ATTweak.dylib,FAKETOUCH

BTC-Springboard.dylib,FAKETOUCH

Callofdutygarena.dylib,FAKETOUCH

Cercube.dylib,FAKETOUCH

EASimulateTower.dylib,FAKETOUCH

EPEventTweak.dylib,FAKETOUCH

EvilHunterTycoon.dylib,FAKETOUCH

FlyBirdRemoteControl.dylib,FAKETOUCH

GamePlayerUI.dylib,FAKETOUCH

HAWK.dylib,FAKETOUCH

HonkaiImpactTW.dylib,FAKETOUCH

HookedInc.dylib,FAKETOUCH

IdleFitnessGymTycoon.dylib,FAKETOUCH

JudeTaxi_jb.dylib,FAKETOUCH

LienQuanMobile.dylib,FAKETOUCH

P4UTweak.dylib,FAKETOUCH

PUBGMOBILEFREEiOSGodsCom.dylib,FAKETOUCH

ScarFalliOSGodsCom.dylib,FAKETOUCH

SimulateTouch.dylib,FAKETOUCH

TSEventTweak.dylib,FAKETOUCH

TSTweak.dylib,FAKETOUCH

WildRift.dylib,FAKETOUCH

bitcoinminer.dylib,FAKETOUCH

coin.dylib,FAKETOUCH

dls2019.dylib,FAKETOUCH

dq2.dylib,FAKETOUCH

dungeoncorp.dylib,FAKETOUCH

easimulatetower.thirdparty.2.dylib,FAKETOUCH

freefire.dylib,FAKETOUCH

gameisbugged.dylib,FAKETOUCH

hidspringboard.dylib,FAKETOUCH

mycafe.dylib,FAKETOUCH

raidthedungeon.dylib,FAKETOUCH

royaldice.dylib,FAKETOUCH

rushroyale.dylib,FAKETOUCH

tetweak.dylib,FAKETOUCH

tkasmtouch.dylib,FAKETOUCH

1.dylib,FAKE_LBS

AliDt.dylib,FAKE_LBS

CyDown.dylib,FAKE_LBS

DouTu.dylib,FAKE_LBS

GpsHookLibrary.dylib,FAKE_LBS

Lamo.dylib,FAKE_LBS

LamoClient.dylib,FAKE_LBS

OTRLocation.dylib,FAKE_LBS

WeChatHookPro.dylib,FAKE_LBS

abcd.dylib,FAKE_LBS

appstoreplusUI.dylib,FAKE_LBS

easimulatetower.client.dylib,FAKE_LBS

kfc.dylib,FAKE_LBS

lk.dylib,FAKE_LBS

m.dylib,FAKE_LBS

mmm.dylib,FAKE_LBS

pbyy.dylib,FAKE_LBS

phonetweak.dylib,FAKE_LBS

tou111.dylib,FAKE_LBS

txytweak.dylib,FAKE_LBS

xiaoqi.dylib,FAKE_LBS

xin.dylib,FAKE_LBS

zorro.dylib,FAKE_LBS

zzzzzLiberty.dylib,FAKE_LBS
