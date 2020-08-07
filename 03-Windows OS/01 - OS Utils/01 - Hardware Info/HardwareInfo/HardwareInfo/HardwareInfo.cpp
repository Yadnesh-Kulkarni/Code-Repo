#include "HardwareInfo.h"

#pragma comment(lib, "wbemuuid.lib")

SerialNumbers *srNum;

BOOL GetHardwareInfo()
{
	HRESULT hres;
	BOOL boRetVal;

	CoInitialize(NULL);

	if (RemovePreviousHWInfoFile() == FALSE)
	{
		return FALSE;
	}

	//General COM security levels
	hres = CoInitializeSecurity(
		NULL,
		-1,                          // COM authentication
		NULL,                        // Authentication services
		NULL,                        // Reserved
		RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
		RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
		NULL,                        // Authentication info
		EOAC_NONE,                   // Additional capabilities 
		NULL                         // Reserved
	);

	if (FAILED(hres))
	{
		MessageBoxW(NULL, L"Could Not Inititalize Security", L"Error", MB_OK);
		return FALSE;
	}

	srNum = (SerialNumbers*)malloc(sizeof(SerialNumbers));
	ZeroMemory(srNum, sizeof(SerialNumbers));
	//memset(srNum,'1',sizeof(SerialNumbers));
	boRetVal = GetProcessorInfo();
	if (FALSE == boRetVal)
	{
		MessageBox(NULL, TEXT("Failed To get processor info."), TEXT("Error"), MB_OK);
	}

	boRetVal = GetMotherboardInfo();
	if (FALSE == boRetVal)
	{
		MessageBox(NULL, TEXT("Failed To get motherboard info."), TEXT("Error"), MB_OK);
	}

	boRetVal = GetHDDInfo();
	if (FALSE == boRetVal)
	{
		MessageBox(NULL, TEXT("Failed To get HDD info."), TEXT("Error"), MB_OK);
	}

	boRetVal = GetPhysicalMemoryInfo();
	if (FALSE == boRetVal)
	{
		MessageBox(NULL, TEXT("Failed To get physical memory info."), TEXT("Error"), MB_OK);
	}

	boRetVal = GetNetworkCardInfo();
	if (FALSE == boRetVal)
	{
		MessageBox(NULL, TEXT("Failed to get Network Card info"), TEXT("Error"), MB_OK);
	}

	if (!CreateHash((BYTE *)srNum, sizeof(*srNum)))
	{
		MessageBox(NULL,TEXT("Could Not Create Hash"),TEXT("Error"),MB_ICONERROR);
	}

	free(srNum);
	CoUninitialize();
	return boRetVal;
}

BOOL CreateHash(BYTE *byte,int len)
{
	FILE *fp;
	if (_wfopen_s(&fp, L"HardwareInfo.txt", L"a+") != 0)
	{
		return FALSE;
	}

	unsigned char *hashKey;
	hashKey = (unsigned char *)malloc(16);

	BOOL boRetVal;
	boRetVal = CalculateMD5(byte, len, hashKey);

	fprintf_s(fp, "\nHashKey : ");

	for (int i = 0; i < 16; i++)
	{
		fprintf_s(fp,"%x",*(hashKey + i));
	}

	free(hashKey);
	return TRUE;
}

BOOL LogHardwareInfo(HARDWARE hardware, wchar_t *serialNumber)
{
	FILE *fp;
	
	wchar_t filename[MAX_PATH];

	StringCchPrintf(filename, MAX_PATH, L"%c:\\HardwareInfo.txt", ((wchar_t)nSelectedDriveIndex + 'A'));
	
	if (_wfopen_s(&fp, filename, L"a+") != 0)
		return FALSE;

	switch (hardware)
	{
		case PROCESSOR:
			fwprintf_s(fp, L"Processor Serial Number : %s\n", serialNumber);
			break;
		case MOTHERBOARD:			
			fwprintf_s(fp, L"MotherBoard Serial Number : %s\n", serialNumber);
			break; 
		case HARDDRIVE:
			fwprintf_s(fp, L"HardDrive Serial Number : %s\n", serialNumber);
			break;
	}

	fclose(fp);

	return TRUE;
}

BOOL RemovePreviousHWInfoFile(void)
{
	FILE *fp;

	wchar_t filename[MAX_PATH];


	StringCchPrintf(filename, MAX_PATH, L"%c:\\HardwareInfo.txt", ((wchar_t)nSelectedDriveIndex + 'A'));

	if (fopen_s(&fp, filename, "r") != ENOENT)
	{
		fclose(fp);
		if (DeleteFile("HardwareInfo.txt") == 0)
		{
			return FALSE;
		}
	}
	
	return TRUE;
}

BOOL GetProcessorInfo()
{
	FILE *fp;
	HRESULT hres;
	wchar_t serialNumber[1024];
	wchar_t filename[MAX_PATH];

	StringCchPrintf(filename, MAX_PATH, L"%c:\\HardwareInfo.txt", ((wchar_t)nSelectedDriveIndex + 'A'));

	if (_wfopen_s(&fp, filename, L"a+") != 0)
	{
		return FALSE;
	}

	// Obtain the initial locator to WMI
	IWbemLocator *pLoc = NULL;

	hres = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID *)&pLoc);
	if (FAILED(hres))
	{
		fclose(fp);
		return FALSE;
	}


	// Connect to WMI through the IWbemLocator::ConnectServer method
	IWbemServices *pSvc = NULL;

	// Connect to the root\cimv2 namespace with
	// the current user and obtain pointer pSvc
	// to make IWbemServices calls.
	hres = pLoc->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
		NULL,                    // User name. NULL = current user
		NULL,                    // User password. NULL = current
		0,                       // Locale. NULL indicates current
		NULL,                    // Security flags.
		0,                       // Authority (for example, Kerberos)
		0,                       // Context object 
		&pSvc                    // pointer to IWbemServices proxy
	);
	if (FAILED(hres))
	{
		pLoc->Release();
		fclose(fp);
		return FALSE;
	}

	// Set security levels on the proxy
	hres = CoSetProxyBlanket(
		pSvc,                        // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
		NULL,                        // Server principal name 
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		NULL,                        // client identity
		EOAC_NONE                    // proxy capabilities 
	);

	if (FAILED(hres))
	{
		pSvc->Release();
		pLoc->Release();
		fclose(fp);
		return FALSE;
	}

	// PROCESSOR Information
	// Use the IWbemServices pointer to make requests of WMI
	IEnumWbemClassObject* pEnumerator = NULL;
	hres = pSvc->ExecQuery(
		bstr_t("WQL"),
		bstr_t("SELECT * FROM Win32_Processor"),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&pEnumerator);
	if (FAILED(hres))
	{
		pSvc->Release();
		pLoc->Release();
		fclose(fp);
		return FALSE;
	}

	// Get the data from the query
	IWbemClassObject *pclsObj = NULL;
	ULONG uReturn = 0;

	fwprintf_s(fp, L"******** Processor Info ********\n");

	while (pEnumerator)
	{
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
			&pclsObj, &uReturn);

		if (0 == uReturn)
		{
			break;
		}

		CIMTYPE vType;
		VARIANT vtProp;

		hr = pclsObj->Get(L"SerialNumber", 0, &vtProp, &vType, 0);
		if (FAILED(hr) || (vtProp.bstrVal == NULL))
		{
			//LogHardwareInfo(PROCESSOR, L"NA");
			fwprintf_s(fp, L"Serial Number: %s\n", L"NA");
			wcscpy_s(srNum->ProcessorSerial, L"NULL");
			srNum->ProcessorSerial[wcslen(srNum->ProcessorSerial) + 1] = '\0';
			memset(srNum->ProcessorSerial, 0, sizeof(srNum->ProcessorSerial));
		}
		else
		{
			if (!wcscmp(vtProp.bstrVal, L"N/A"))
			{
				StringCchCopyW(serialNumber, 1024, vtProp.bstrVal);
				fwprintf_s(fp, L"Serial Number: %s\n", serialNumber);
				wcscpy_s(srNum->ProcessorSerial, L"NULL");
				srNum->ProcessorSerial[wcslen(srNum->ProcessorSerial) + 1] = '\0';
				//memset(srNum->ProcessorSerial, 0, sizeof(srNum->ProcessorSerial));
			}
			else
			{
				StringCchCopyW(serialNumber, 1024, vtProp.bstrVal);
				fwprintf_s(fp, L"Serial Number: %s\n", serialNumber);
				wcscpy_s(srNum->ProcessorSerial, serialNumber);
				srNum->ProcessorSerial[wcslen(srNum->ProcessorSerial) + 1] = '\0';
				VariantClear(&vtProp);
			}
		}

		hr = pclsObj->Get(L"NumberOfCores", 0, &vtProp, &vType, 0);
		if (FAILED(hr))
		{
			//LogHardwareInfo(PROCESSOR, L"NA");
			fwprintf_s(fp, L"NumberOfCores: %s\n", L"NA");
		}
		else
		{
			fwprintf_s(fp, L"NumberOfCores: %u\n", V_UINT(&vtProp));

			VariantClear(&vtProp);
		}

		hr = pclsObj->Get(L"Architecture", 0, &vtProp, &vType, 0);
		if (FAILED(hr))
		{
			//LogHardwareInfo(PROCESSOR, L"NA");
			fwprintf_s(fp, L"Architecture: %s\n", L"NA");
		}
		else
		{
			fwprintf_s(fp, L"Architecture: %u\n", V_UINT(&vtProp));

			VariantClear(&vtProp);
		}

		hr = pclsObj->Get(L"Family", 0, &vtProp, &vType, 0);
		if (FAILED(hr))
		{
			//LogHardwareInfo(PROCESSOR, L"NA");
			fwprintf_s(fp, L"Family: %s\n", L"NA");
		}
		else
		{
			fwprintf_s(fp, L"Family: %u\n", V_UI2(&vtProp));

			VariantClear(&vtProp);
		}

		hr = pclsObj->Get(L"Name", 0, &vtProp, &vType, 0);
		if (FAILED(hr) || (vtProp.bstrVal == NULL))
		{
			//LogHardwareInfo(PROCESSOR, L"NA");
			fwprintf_s(fp, L"Name: %s\n", L"NA");
		}
		else
		{
			fwprintf_s(fp, L"Name: %s\n", V_BSTR(&vtProp));

			VariantClear(&vtProp);
		}

		hr = pclsObj->Get(L"Manufacturer", 0, &vtProp, &vType, 0);
		if (FAILED(hr) || (vtProp.bstrVal == NULL))
		{
			//LogHardwareInfo(PROCESSOR, L"NA");
			fwprintf_s(fp, L"Manufacturer: %s\n", L"NA");
		}
		else
		{
			fwprintf_s(fp, L"Manufacturer: %s\n", V_BSTR(&vtProp));

			VariantClear(&vtProp);
		}

		pclsObj->Release();
	}

	// CleanUp
	pSvc->Release();
	pLoc->Release();
	pEnumerator->Release();
	fclose(fp);

	return TRUE;
}

BOOL GetMotherboardInfo()
{
	FILE *fp;
	HRESULT hres;
	wchar_t serialNumber[1024];
	wchar_t filename[MAX_PATH];

	StringCchPrintf(filename, MAX_PATH, L"%c:\\HardwareInfo.txt", ((wchar_t)nSelectedDriveIndex + 'A'));

	if (_wfopen_s(&fp, filename, L"a+") != 0)
	{
		return FALSE;
	}

	// Obtain the initial locator to WMI
	IWbemLocator *pLoc = NULL;

	hres = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID *)&pLoc);
	if (FAILED(hres))
	{
		fclose(fp);
		return FALSE;
	}


	// Connect to WMI through the IWbemLocator::ConnectServer method
	IWbemServices *pSvc = NULL;

	// Connect to the root\cimv2 namespace with
	// the current user and obtain pointer pSvc
	// to make IWbemServices calls.
	hres = pLoc->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
		NULL,                    // User name. NULL = current user
		NULL,                    // User password. NULL = current
		0,                       // Locale. NULL indicates current
		NULL,                    // Security flags.
		0,                       // Authority (for example, Kerberos)
		0,                       // Context object 
		&pSvc                    // pointer to IWbemServices proxy
	);
	if (FAILED(hres))
	{
		pLoc->Release();
		fclose(fp);
		return FALSE;
	}

	// Set security levels on the proxy
	hres = CoSetProxyBlanket(
		pSvc,                        // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
		NULL,                        // Server principal name 
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		NULL,                        // client identity
		EOAC_NONE                    // proxy capabilities 
	);

	if (FAILED(hres))
	{
		pSvc->Release();
		pLoc->Release();
		fclose(fp);
		return FALSE;
	}

	// PROCESSOR Information
	// Use the IWbemServices pointer to make requests of WMI
	IEnumWbemClassObject* pEnumerator = NULL;
	hres = pSvc->ExecQuery(
		bstr_t("WQL"),
		bstr_t("SELECT * FROM Win32_BaseBoard "),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&pEnumerator);
	if (FAILED(hres))
	{
		pSvc->Release();
		pLoc->Release();
		fclose(fp);
		return FALSE;
	}

	// Get the data from the query
	IWbemClassObject *pclsObj = NULL;
	ULONG uReturn = 0;
	fwprintf_s(fp, L"\n******** Motherboard Info ********\n");

	while (pEnumerator)
	{
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
			&pclsObj, &uReturn);

		if (0 == uReturn)
		{
			break;
		}

		CIMTYPE vType;
		VARIANT vtProp;

		hr = pclsObj->Get(L"SerialNumber", 0, &vtProp, &vType, 0);
		if (FAILED(hr) || (vtProp.bstrVal == NULL))
		{
			//LogHardwareInfo(PROCESSOR, L"NA");
			fwprintf_s(fp, L"Serial Number: %s\n", L"NA");
			wcscpy_s(srNum->MotherboardSerial, L"NULL");
			srNum->MotherboardSerial[wcslen(srNum->MotherboardSerial) + 1] = '\0';
			//memset(srNum->MotherboardSerial, 0, sizeof(srNum->MotherboardSerial));
		}
		else
		{
			if (!wcscmp(vtProp.bstrVal, L"N/A"))
			{
				StringCchCopyW(serialNumber, 1024, vtProp.bstrVal);
				fwprintf_s(fp, L"Serial Number: %s\n", serialNumber);
				wcscpy_s(srNum->MotherboardSerial, L"NULL");
				srNum->MotherboardSerial[wcslen(srNum->MotherboardSerial) + 1] = '\0';
				//memset(srNum->MotherboardSerial, 0, sizeof(srNum->MotherboardSerial));
			}
			else
			{
				StringCchCopyW(serialNumber, 1024, vtProp.bstrVal);
				fwprintf_s(fp, L"Serial Number: %s\n", serialNumber);
				wcscpy_s(srNum->MotherboardSerial, serialNumber);
				srNum->MotherboardSerial[wcslen(srNum->MotherboardSerial) + 1] = '\0';
			}
			VariantClear(&vtProp);
		}

		hr = pclsObj->Get(L"Model", 0, &vtProp, &vType, 0);
		if (FAILED(hr))
		{
			//LogHardwareInfo(PROCESSOR, L"NA");
			fwprintf_s(fp, L"Model: %s\n", L"NA");
		}
		else
		{
			fwprintf_s(fp, L"Model: %s\n", V_BSTR(&vtProp));

			VariantClear(&vtProp);
		}

		hr = pclsObj->Get(L"Manufacturer", 0, &vtProp, &vType, 0);
		if (FAILED(hr) || (vtProp.bstrVal == NULL))
		{
			//LogHardwareInfo(PROCESSOR, L"NA");
			fwprintf_s(fp, L"Manufacturer: %s\n", L"NA");
		}
		else
		{
			fwprintf_s(fp, L"Manufacturer: %s\n", V_BSTR(&vtProp));

			VariantClear(&vtProp);
		}
		
		pclsObj->Release();
	}

	// CleanUp
	pSvc->Release();
	pLoc->Release();
	pEnumerator->Release();
	fclose(fp);

	return TRUE;
}

BOOL GetHDDInfo()
{
	FILE *fp;
	HRESULT hres;
	wchar_t filename[MAX_PATH];

	StringCchPrintf(filename, MAX_PATH, L"%c:\\HardwareInfo.txt", ((wchar_t)nSelectedDriveIndex + 'A'));

	if (_wfopen_s(&fp, filename, L"a+") != 0)
	{
		return FALSE;
	}

	// Obtain the initial locator to WMI
	IWbemLocator *pLoc = NULL;

	hres = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID *)&pLoc);
	if (FAILED(hres))
	{
		fclose(fp);
		return FALSE;
	}


	// Connect to WMI through the IWbemLocator::ConnectServer method
	IWbemServices *pSvc = NULL;

	// Connect to the root\cimv2 namespace with
	// the current user and obtain pointer pSvc
	// to make IWbemServices calls.
	hres = pLoc->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
		NULL,                    // User name. NULL = current user
		NULL,                    // User password. NULL = current
		0,                       // Locale. NULL indicates current
		NULL,                    // Security flags.
		0,                       // Authority (for example, Kerberos)
		0,                       // Context object 
		&pSvc                    // pointer to IWbemServices proxy
	);
	if (FAILED(hres))
	{
		pLoc->Release();
		fclose(fp);
		return FALSE;
	}

	// Set security levels on the proxy
	hres = CoSetProxyBlanket(
		pSvc,                        // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
		NULL,                        // Server principal name 
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		NULL,                        // client identity
		EOAC_NONE                    // proxy capabilities 
	);

	if (FAILED(hres))
	{
		pSvc->Release();
		pLoc->Release();
		fclose(fp);
		return FALSE;
	}

	// PROCESSOR Information
	// Use the IWbemServices pointer to make requests of WMI
	IEnumWbemClassObject* pEnumerator = NULL;
	hres = pSvc->ExecQuery(
		bstr_t("WQL"),
		bstr_t("SELECT * FROM Win32_DiskDrive "),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&pEnumerator);
	if (FAILED(hres))
	{
		pSvc->Release();
		pLoc->Release();
		fclose(fp);
		return FALSE;
	}

	// Get the data from the query
	IWbemClassObject *pclsObj = NULL;
	ULONG uReturn = 0;

	fwprintf_s(fp, L"\n******** HDD Info ********\n");

	while (pEnumerator)
	{
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
			&pclsObj, &uReturn);

		if (0 == uReturn)
		{
			break;
		}

		CIMTYPE vType;
		VARIANT vtProp;

		hr = pclsObj->Get(L"SerialNumber", 0, &vtProp, &vType, 0);
		if (FAILED(hr) || (vtProp.bstrVal == NULL))
		{
			fwprintf_s(fp, L"Serial Number: %s\n", L"NA");
			wcscpy_s(srNum->HardDiskDriveSerial, L"NULL");
			srNum->HardDiskDriveSerial[wcslen(srNum->HardDiskDriveSerial) + 1] = '\0';
			//memset(srNum->HardDiskDriveSerial, 0, sizeof(srNum->HardDiskDriveSerial));
		}
		else
		{
			if (!wcscmp(vtProp.bstrVal, L"N/A"))
			{
				fwprintf_s(fp, L"Serial Number: %s\n", V_BSTR(&vtProp));
				wcscpy_s(srNum->HardDiskDriveSerial, L"NULL");
				srNum->HardDiskDriveSerial[wcslen(srNum->HardDiskDriveSerial) + 1] = '\0';
				//memset(srNum->HardDiskDriveSerial, 0, sizeof(srNum->HardDiskDriveSerial));
			}
			else
			{
				fwprintf_s(fp, L"Serial Number: %s\n", V_BSTR(&vtProp));
				wcscpy_s(srNum->HardDiskDriveSerial, vtProp.bstrVal);
				srNum->HardDiskDriveSerial[wcslen(srNum->HardDiskDriveSerial) + 1] = '\0';
			}
			VariantClear(&vtProp);
		}

		hr = pclsObj->Get(L"Size", 0, &vtProp, &vType, 0);
		if (FAILED(hr))
		{
			//LogHardwareInfo(PROCESSOR, L"NA");
			fwprintf_s(fp, L"Block Size: %s\n", L"NA");
		}
		else
		{
			fwprintf_s(fp, L"Block Size: %I64u\n", V_UI8(&vtProp));

			VariantClear(&vtProp);
		}

		hr = pclsObj->Get(L"Model", 0, &vtProp, &vType, 0);
		if (FAILED(hr))
		{
			//LogHardwareInfo(PROCESSOR, L"NA");
			fwprintf_s(fp, L"Model: %s\n", L"NA");
		}
		else
		{
			fwprintf_s(fp, L"Model: %s\n", V_BSTR(&vtProp));

			VariantClear(&vtProp);
		}

		hr = pclsObj->Get(L"Manufacturer", 0, &vtProp, &vType, 0);
		if (FAILED(hr) || (vtProp.bstrVal == NULL))
		{
			//LogHardwareInfo(PROCESSOR, L"NA");
			fwprintf_s(fp, L"Manufacturer: %s\n", L"NA");
		}
		else
		{
			fwprintf_s(fp, L"Manufacturer: %s\n", V_BSTR(&vtProp));

			VariantClear(&vtProp);
		}
		
		pclsObj->Release();
	}

	// CleanUp
	pSvc->Release();
	pLoc->Release();
	pEnumerator->Release();
	fclose(fp);

	return TRUE;
}

BOOL GetPhysicalMemoryInfo()
{
	FILE *fp;
	HRESULT hres;
	wchar_t filename[MAX_PATH];

	StringCchPrintf(filename, MAX_PATH, L"%c:\\HardwareInfo.txt", ((wchar_t)nSelectedDriveIndex + 'A'));

	if (_wfopen_s(&fp, filename, L"a+") != 0)
	{
		return FALSE;
	}

	// Obtain the initial locator to WMI
	IWbemLocator *pLoc = NULL;

	hres = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID *)&pLoc);
	if (FAILED(hres))
	{
		fclose(fp);
		return FALSE;
	}


	// Connect to WMI through the IWbemLocator::ConnectServer method
	IWbemServices *pSvc = NULL;

	// Connect to the root\cimv2 namespace with
	// the current user and obtain pointer pSvc
	// to make IWbemServices calls.
	hres = pLoc->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
		NULL,                    // User name. NULL = current user
		NULL,                    // User password. NULL = current
		0,                       // Locale. NULL indicates current
		NULL,                    // Security flags.
		0,                       // Authority (for example, Kerberos)
		0,                       // Context object 
		&pSvc                    // pointer to IWbemServices proxy
	);
	if (FAILED(hres))
	{
		pLoc->Release();
		fclose(fp);
		return FALSE;
	}

	// Set security levels on the proxy
	hres = CoSetProxyBlanket(
		pSvc,                        // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
		NULL,                        // Server principal name 
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		NULL,                        // client identity
		EOAC_NONE                    // proxy capabilities 
	);

	if (FAILED(hres))
	{
		pSvc->Release();
		pLoc->Release();
		fclose(fp);
		return FALSE;
	}

	// PROCESSOR Information
	// Use the IWbemServices pointer to make requests of WMI
	IEnumWbemClassObject* pEnumerator = NULL;
	hres = pSvc->ExecQuery(
		bstr_t("WQL"),
		bstr_t("SELECT * FROM Win32_PhysicalMemory "),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&pEnumerator);
	if (FAILED(hres))
	{
		pSvc->Release();
		pLoc->Release();
		fclose(fp);
		return FALSE;
	}

	// Get the data from the query
	IWbemClassObject *pclsObj = NULL;
	ULONG uReturn = 0;

	fwprintf_s(fp, L"\n******** Physical Memory Info ********\n");

	while (pEnumerator)
	{
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
			&pclsObj, &uReturn);

		if (0 == uReturn)
		{
			break;
		}

		CIMTYPE vType;
		VARIANT vtProp;

		hr = pclsObj->Get(L"SerialNumber", 0, &vtProp, &vType, 0);
		if (FAILED(hr) || (vtProp.bstrVal == NULL))
		{
			fwprintf_s(fp, L"Serial Number: %s\n", L"NA");
			wcscpy_s(srNum->PhysicalMemorySerial, L"NULL");
			srNum->PhysicalMemorySerial[wcslen(srNum->PhysicalMemorySerial) + 1] = '\0';
		}
		else
		{
			if (!wcscmp(vtProp.bstrVal, L"N/A"))
			{
				fwprintf_s(fp, L"Serial Number: %s\n", V_BSTR(&vtProp));
				wcscpy_s(srNum->PhysicalMemorySerial, L"NULL");
				srNum->PhysicalMemorySerial[wcslen(srNum->PhysicalMemorySerial) + 1] = '\0';
				//memset(srNum->PhysicalMemorySerial,0,sizeof(srNum->PhysicalMemorySerial));
			}
			else
			{
				fwprintf_s(fp, L"Serial Number: %s\n", V_BSTR(&vtProp));
				wcscpy_s(srNum->PhysicalMemorySerial, vtProp.bstrVal);
				srNum->PhysicalMemorySerial[wcslen(srNum->PhysicalMemorySerial) + 1] = '\0';
			}
			VariantClear(&vtProp);
		}

		hr = pclsObj->Get(L"Capacity", 0, &vtProp, &vType, 0);
		if (FAILED(hr))
		{
			//LogHardwareInfo(PROCESSOR, L"NA");
			fwprintf_s(fp, L"Capacity: %s\n", L"NA");
		}
		else
		{
			fwprintf_s(fp, L"Capacity: %llu\n", V_UI8(&vtProp));

			VariantClear(&vtProp);
		}

		hr = pclsObj->Get(L"Model", 0, &vtProp, &vType, 0);
		if (FAILED(hr))
		{
			//LogHardwareInfo(PROCESSOR, L"NA");
			fwprintf_s(fp, L"Model: %s\n", L"NA");
		}
		else
		{
			fwprintf_s(fp, L"Model: %s\n", V_BSTR(&vtProp));

			VariantClear(&vtProp);
		}

		hr = pclsObj->Get(L"Name", 0, &vtProp, &vType, 0);
		if (FAILED(hr) || (vtProp.bstrVal == NULL))
		{
			//LogHardwareInfo(PROCESSOR, L"NA");
			fwprintf_s(fp, L"Name: %s\n", L"NA");
		}
		else
		{
			fwprintf_s(fp, L"Name: %s\n", V_BSTR(&vtProp));

			VariantClear(&vtProp);
		}

		hr = pclsObj->Get(L"Manufacturer", 0, &vtProp, &vType, 0);
		if (FAILED(hr) || (vtProp.bstrVal == NULL))
		{
			//LogHardwareInfo(PROCESSOR, L"NA");
			fwprintf_s(fp, L"Manufacturer: %s\n", L"NA");
		}
		else
		{
			fwprintf_s(fp, L"Manufacturer: %s\n", V_BSTR(&vtProp));

			VariantClear(&vtProp);
		}
		
		pclsObj->Release();
	}

	// CleanUp
	pSvc->Release();
	pLoc->Release();
	pEnumerator->Release();
	fclose(fp);

	return TRUE;
}

BOOL GetNetworkCardInfo()
{
	FILE *fp;
	HRESULT hres;
	wchar_t filename[MAX_PATH];

	StringCchPrintf(filename, MAX_PATH, L"%c:\\HardwareInfo.txt", ((wchar_t)nSelectedDriveIndex + 'A'));

	if (_wfopen_s(&fp, filename, L"a+") != 0)
	{
		return FALSE;
	}

	// Obtain the initial locator to WMI
	IWbemLocator *pLoc = NULL;

	hres = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID *)&pLoc);
	if (FAILED(hres))
	{
		fclose(fp);
		return FALSE;
	}


	// Connect to WMI through the IWbemLocator::ConnectServer method
	IWbemServices *pSvc = NULL;

	// Connect to the root\cimv2 namespace with
	// the current user and obtain pointer pSvc
	// to make IWbemServices calls.
	hres = pLoc->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
		NULL,                    // User name. NULL = current user
		NULL,                    // User password. NULL = current
		0,                       // Locale. NULL indicates current
		NULL,                    // Security flags.
		0,                       // Authority (for example, Kerberos)
		0,                       // Context object 
		&pSvc                    // pointer to IWbemServices proxy
	);
	if (FAILED(hres))
	{
		pLoc->Release();
		fclose(fp);
		return FALSE;
	}

	// Set security levels on the proxy
	hres = CoSetProxyBlanket(
		pSvc,                        // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
		NULL,                        // Server principal name 
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		NULL,                        // client identity
		EOAC_NONE                    // proxy capabilities 
	);

	if (FAILED(hres))
	{
		pSvc->Release();
		pLoc->Release();
		fclose(fp);
		return FALSE;
	}

	// PROCESSOR Information
	// Use the IWbemServices pointer to make requests of WMI
	IEnumWbemClassObject* pEnumerator = NULL;
	hres = pSvc->ExecQuery(
		bstr_t("WQL"),
		bstr_t("SELECT * FROM Win32_NetworkAdapter Where DeviceID = 1"),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&pEnumerator);
	if (FAILED(hres))
	{
		pSvc->Release();
		pLoc->Release();
		fclose(fp);
		return FALSE;
	}

	// Get the data from the query
	IWbemClassObject *pclsObj = NULL;
	ULONG uReturn = 0;

	fwprintf_s(fp, L"\n******** Network Card Info ********\n");


	while (pEnumerator)
	{
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
			&pclsObj, &uReturn);

		
		if (0 == uReturn)
		{
			break;
		}

		CIMTYPE vType;
		VARIANT vtProp;

		hr = pclsObj->Get(L"Name", 0, &vtProp, &vType, 0);
		if (FAILED(hr) || (vtProp.bstrVal == NULL))
		{
			fwprintf_s(fp, L"Name: %s\n", L"NA");
		}
		else
		{
			fwprintf_s(fp, L"Name: %s\n", V_BSTR(&vtProp));
			VariantClear(&vtProp);
		}
		hr = pclsObj->Get(L"MACAddress", 0, &vtProp, &vType, 0);
		if (FAILED(hr) || (vtProp.bstrVal == NULL))
		{
			fwprintf_s(fp, L"MAC Address: %s\n", L"NA");
			wcscpy_s(srNum->NetWorkMacID, L"NULL");
			srNum->NetWorkMacID[wcslen(srNum->NetWorkMacID) + 1] = '\0';
			//memset(srNum->NetWorkMacID, 0, sizeof(srNum->NetWorkMacID));
		}
		else
		{
			if (!wcscmp(vtProp.bstrVal, L"N/A"))
			{
				fwprintf_s(fp, L"MAC Address: %s\n", V_BSTR(&vtProp));
				wcscpy_s(srNum->NetWorkMacID, L"NULL");
				srNum->NetWorkMacID[wcslen(srNum->NetWorkMacID) + 1] = '\0';
				//memset(srNum->NetWorkMacID, 0, sizeof(srNum->NetWorkMacID));
			}
			else
			{
				fwprintf_s(fp, L"MAC Address: %s\n", V_BSTR(&vtProp));
				wcscpy_s(srNum->NetWorkMacID, vtProp.bstrVal);
				srNum->NetWorkMacID[wcslen(srNum->NetWorkMacID) + 1] = '\0';
			}
			VariantClear(&vtProp);
		}
		hr = pclsObj->Get(L"Device ID", 0, &vtProp, &vType, 0);
		if (FAILED(hr) || (vtProp.bstrVal == NULL))
		{
			fwprintf_s(fp, L"DeviceID: %s\n", L"NA");
		}
		else
		{
			fwprintf_s(fp, L"DeviceID: %s\n", V_BSTR(&vtProp));
			VariantClear(&vtProp);
		}
		hr = pclsObj->Get(L"AdapterType", 0, &vtProp, &vType, 0);
		if (FAILED(hr) || (vtProp.bstrVal == NULL))
		{
			fwprintf_s(fp, L"AdapterType: %s\n\n", L"NA");
		}
		else
		{
			fwprintf_s(fp, L"AdapterType: %s\n\n", V_BSTR(&vtProp));
			VariantClear(&vtProp);
		}
		pclsObj->Release();
	}

	pSvc->Release();
	pLoc->Release();
	pEnumerator->Release();
	fclose(fp);

	return TRUE;
}