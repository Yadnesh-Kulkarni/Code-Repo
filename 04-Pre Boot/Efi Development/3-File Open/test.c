#include <Uefi.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <Library/MemoryAllocationLib.h>
#include <Guid/FileInfo.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h> //for FileDevicePath

#pragma comment(lib,"libefi.lib")
#pragma comment(lib,"UefiApplicationEntryPoint.lib")
#pragma comment(lib,"BaseDebugLibNull.lib")
#pragma comment(lib,"libz.lib")

EFI_GUID gEfiSimpleFileSystemProtocolGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
EFI_BOOT_SERVICES *gBS;

EFI_STATUS
EFIAPI
ProcessFilesInDir (
IN EFI_FILE_HANDLE Dir,
IN EFI_DEVICE_PATH CONST *DirDp
);

EFI_STATUS
EFIAPI PerFileFunc (
IN EFI_FILE_HANDLE Dir,
IN EFI_DEVICE_PATH *DirDp,
IN EFI_FILE_INFO *FileInfo,
IN EFI_DEVICE_PATH *Dp
);

/**
The entry point for the application.

@param[in] ImageHandle The firmware allocated handle for the EFI
image.
@param[in] SystemTable A pointer to the EFI System Table.

@retval EFI_SUCCESS The entry point is executed successfully.
@retval other Some error occurs when executing this entry
point.

loop through all volumes and recurse through all files in
each volume and call a user defined function to do
some processing on each file
*/
EFI_STATUS
EFIAPI
UefiMain (
IN EFI_HANDLE ImageHandle,
IN EFI_SYSTEM_TABLE *SystemTable
)
{


EFI_HANDLE AgentHandle;
EFI_STATUS Status;
UINTN NumHandles;
EFI_HANDLE *Handles;
UINTN Index;
VOID *Context;

AgentHandle = ImageHandle;

InitializeLib(ImageHandle,SystemTable);

// gets all handles with simple file system installed
Status = gBS->LocateHandleBuffer (
ByProtocol,
&gEfiSimpleFileSystemProtocolGuid,
NULL,
&NumHandles,
&Handles
);
if (EFI_ERROR (Status)) {
return Status;
}

// loop through all handles we just got
for (Index = 0; Index < NumHandles; Index++) {
EFI_FILE_HANDLE Root;
EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *Fs;
EFI_DEVICE_PATH *Dp;

// get simple file system protocol instance
// from current handle
Status = gBS->OpenProtocol (
Handles[Index],
&gEfiSimpleFileSystemProtocolGuid,
&Fs,
NULL,
AgentHandle,
EFI_OPEN_PROTOCOL_GET_PROTOCOL
);
if (EFI_ERROR (Status)) {
DEBUG ((EFI_D_ERROR, "Missing EFI_SIMPLE_FILE_SYSTEM_PROTOCOL on handle.\n"));
continue;
}

// get device path instance from current handle
Status = gBS->OpenProtocol (
Handles[Index],
&gEfiDevicePathProtocolGuid,
&Dp,
NULL,
AgentHandle,
EFI_OPEN_PROTOCOL_GET_PROTOCOL
);
if (EFI_ERROR (Status)) {
DEBUG ((EFI_D_ERROR, "Missing EFI_DEVICE_PATH_PROTOCOL on handle.\n"));
continue;
}

// open root dir from current simple file system
Status = Fs->OpenVolume (Fs, &Root);
if (EFI_ERROR (Status)) {
DEBUG ((EFI_D_ERROR, "Unable to open volume.\n"));
continue;
}

// recursively process files in root dir
Context = NULL;
Status = ProcessFilesInDir (Root, Dp);
Root->Close (Root);
if (EFI_ERROR (Status)) {
DEBUG ((EFI_D_ERROR, "ProcessFilesInDir error. Continuing with next volume...\n"));
continue;
}

}

Print(L"Done successfully\n");

return EFI_SUCCESS;
}

//I have no idea what the max length of a file path is in EFI
#define MAX_FILE_INFO_SIZE 1024

/**
recurse through directory, calling a user defined
function for each file
*/
EFI_STATUS
EFIAPI
ProcessFilesInDir (
IN EFI_FILE_HANDLE Dir,
IN EFI_DEVICE_PATH CONST *DirDp
)
{
EFI_STATUS Status;
EFI_FILE_INFO *FileInfo;
CHAR16 *FileName;
UINTN FileInfoSize;
EFI_DEVICE_PATH *Dp;

// big enough to hold EFI_FILE_INFO struct and
// the whole file path
FileInfo = AllocatePool (MAX_FILE_INFO_SIZE);
if (FileInfo == NULL) {
return EFI_OUT_OF_RESOURCES;
}

for (;;) {
// get the next file's info. there's an internal position
// that gets incremented when you read from a directory
// so that subsequent reads gets the next file's info
FileInfoSize = MAX_FILE_INFO_SIZE;
Status = Dir->Read (Dir, &FileInfoSize, (VOID *) FileInfo);
if (EFI_ERROR (Status) || FileInfoSize == 0) { 
    //this is how we eventually exit this function when we run out of files
if (Status == EFI_BUFFER_TOO_SMALL) {
Print (L"EFI_FILE_INFO > MAX_FILE_INFO_SIZE. Increase the size\n");
}
FreePool (FileInfo);
return Status;
}

FileName = FileInfo->FileName;

// skip files named . or ..
if (StrCmp (FileName, L".") == 0 || StrCmp (FileName, L"..") == 0) {
continue;
}

// so we have absolute device path to child file/dir
Dp = FileDevicePath (DirDp, FileName);
if (Dp == NULL) {
FreePool (FileInfo);
return EFI_OUT_OF_RESOURCES;
}

// Do whatever processing on the file
PerFileFunc (Dir, DirDp, FileInfo, Dp);

if (FileInfo->Attribute & EFI_FILE_DIRECTORY) {
//
// recurse
//

EFI_FILE_HANDLE NewDir;

Status = Dir->Open (Dir, &NewDir, FileName, EFI_FILE_MODE_READ, 0);
if (Status != EFI_SUCCESS) {
FreePool (FileInfo);
FreePool (Dp);
return Status;
}
NewDir->SetPosition (NewDir, 0);

Status = ProcessFilesInDir (NewDir,Dp);
Dir->Close (NewDir);
if (Status != EFI_SUCCESS) {
FreePool (FileInfo);
FreePool (Dp);
return Status;
}
}

FreePool (Dp);
}
}

EFI_STATUS
EFIAPI PerFileFunc (
IN EFI_FILE_HANDLE Dir,
IN EFI_DEVICE_PATH *DirDp,
IN EFI_FILE_INFO *FileInfo,
IN EFI_DEVICE_PATH *Dp
)
{
EFI_STATUS Status;
EFI_FILE_HANDLE File;

Print (L"Path = %s FileName = %s\n", ConvertDevicePathToText(DirDp, TRUE,
TRUE), FileInfo->FileName);

// read the file into a buffer
Status = Dir->Open (Dir, &File, FileInfo->FileName, EFI_FILE_MODE_READ,
0);
if (EFI_ERROR (Status)) {
return Status;
}

// reset position just in case
File->SetPosition (File, 0);

// ****Do stuff on the file here****

Dir->Close (File);

return EFI_SUCCESS;
}
