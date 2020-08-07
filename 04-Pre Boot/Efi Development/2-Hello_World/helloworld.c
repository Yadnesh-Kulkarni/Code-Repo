#include <efi.h>

EFI_STATUS main(EFI_HANDLE imageHandle,EFI_SYSTEM_TABLE *systemTable)
{
    systemTable->ConOut->OutputString(systemTable->ConOut,L"Hello World\r\n");
    return EFI_SUCCESS;
}