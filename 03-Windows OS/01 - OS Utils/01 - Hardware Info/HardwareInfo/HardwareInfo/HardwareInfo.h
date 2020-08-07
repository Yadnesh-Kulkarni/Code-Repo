#pragma once
#include <windows.h>

#define _WIN32_DCOM
#include <iostream>

#include <comdef.h>
#include <Wbemidl.h>
#include <Strsafe.h>

#include "md5.h"
enum HARDWARE
{
	PROCESSOR,
	MOTHERBOARD,
	HARDDRIVE
};

typedef struct SerialNumbers
{
	wchar_t ProcessorSerial[64];
	wchar_t MotherboardSerial[64];
	wchar_t PhysicalMemorySerial[64];
	wchar_t HardDiskDriveSerial[64];
	wchar_t NetWorkMacID[64];
}SerialNumbers;

extern int nSelectedDriveIndex;

BOOL GetHardwareInfo(void);
BOOL LogHardwareInfo(HARDWARE hardware, wchar_t *serialNumber);
BOOL RemovePreviousHWInfoFile(void);
BOOL GetProcessorInfo();
BOOL GetMotherboardInfo();
BOOL GetHDDInfo();
BOOL GetPhysicalMemoryInfo();
BOOL GetNetworkCardInfo();
BOOL CreateHash(BYTE*,int);
