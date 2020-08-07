#include "HardwareInfo.h"

int main(int argc,char *argv[])
{
    BOOL retVal;
    retVal = GetHardwareInfo();
	if (!retVal)
	{
		MessageBoxW(NULL, L"Failed Hardware Info", L"Error", MB_ICONERROR);
	}
}