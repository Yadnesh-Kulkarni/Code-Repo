
#include "md5.h"

#pragma comment(lib,"libefi.lib")
#pragma comment(lib,"libefishell.lib")
#pragma comment(lib,"UefiLib.lib")

EFI_STATUS main(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE *systemTable)
{
	InitializeLib(imageHandle, systemTable);
	EFI_FILE_HANDLE *fileHandle;

	EFI_STATUS efiStatus = EFI_SUCCESS;
	EFI_GUID sfspGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
	EFI_HANDLE *handles = NULL;
	UINTN handleCount = 0;
	int sizeofBuffer1 = 512;
	int sizeofBuffer2 = 512;
	void *buffer = NULL;
	void *buffer1 = NULL;
	// BOOLEAN isHash1 = FALSE;
	// BOOLEAN isHash2 = FALSE;
	unsigned char tempVal1[16];
	unsigned char tempVal2[16];
	CHAR16 tempString[512];
	int index = 0;
	//systemTable->BootServices->SetWatchdogTimer(0, 0, 0, NULL);
	//systemTable->ConOut->OutputString(systemTable->ConOut, L"Before LocateHandleBuffer\r\n");
	efiStatus = systemTable->BootServices->LocateHandleBuffer(ByProtocol, &sfspGuid, NULL, &handleCount, &handles);
	//systemTable->ConOut->OutputString(systemTable->ConOut, L"After LocateHandleBuffer\r\n");
	if (!EFI_ERROR(efiStatus))
	{
		for (index = 0; index < (int)handleCount; ++index)
		{
			EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* fs = NULL;
			//systemTable->ConOut->OutputString(systemTable->ConOut, L"Before HandleProtocol\r\n");
			efiStatus = systemTable->BootServices->HandleProtocol(handles[index], &sfspGuid, (void **)&fs);
			//systemTable->ConOut->OutputString(systemTable->ConOut, L"After HandleProtocol\r\n");
			if (!EFI_ERROR(efiStatus))
			{
				EFI_FILE_PROTOCOL* root = NULL;
				//systemTable->ConOut->OutputString(systemTable->ConOut, L"Before OpenVolume\r\n");
				efiStatus = fs->OpenVolume(fs, &root);
				//systemTable->ConOut->OutputString(systemTable->ConOut, L"After OpenVolume\r\n");
				if (!EFI_ERROR(efiStatus))
				{
					//systemTable->ConOut->OutputString(systemTable->ConOut, L"OpenVolume Succeeded\r\n");
					EFI_FILE_PROTOCOL* token = NULL;

					efiStatus = root->Open(
						root,
						&token,
						L"Test-Dir1\\test_edk.txt",
						EFI_FILE_MODE_READ,
						EFI_FILE_READ_ONLY | EFI_FILE_SYSTEM);

					if (!EFI_ERROR(efiStatus))
					{
						buffer = AllocateZeroPool(sizeofBuffer1);
						if(buffer != NULL)
						{
							//systemTable->ConOut->OutputString(systemTable->ConOut, L"OpenFile Succeeded 1\r\n");
							//Print(L"Size Of Buffer 1 : %s\r\n", ValueToString(sizeofBuffer1));
							efiStatus = token->Read(token,(UINTN *)&sizeofBuffer1, buffer);
							if (!EFI_ERROR(efiStatus))
							{
								//Print(L"Size Of Buffer 1 : %s\r\n", ValueToString(sizeofBuffer1));
								int ret = CalculateMD5((unsigned char *)buffer,sizeofBuffer1,tempVal1);
								if(ret == TRUE)
								{
									//isHash1 = TRUE;
									systemTable->ConOut->OutputString(systemTable->ConOut, L"Hash Calculated Successfully 1 :");
									for(int i = 0; i < 16; i++)
									{
										Print(L"%x",tempVal1[i]);
									}
									systemTable->ConOut->OutputString(systemTable->ConOut, L"\r\n");
								}
								else
								{
									systemTable->ConOut->OutputString(systemTable->ConOut, L"Hash Calculation Failed\r\n");
									systemTable->BootServices->Exit(imageHandle,0,0,0);
								}
								EFI_FILE_PROTOCOL* token1 = NULL;

								efiStatus = root->Open(
											root,
											&token1,
											L"Test-Dir2\\test1_edk.txt",
											EFI_FILE_MODE_READ,
											EFI_FILE_READ_ONLY | EFI_FILE_SYSTEM);
										if (!EFI_ERROR(efiStatus))
										{
											buffer1 = AllocateZeroPool(sizeofBuffer2);
											if(buffer1 != NULL)
											{
													//Print(L"Size Of Buffer 2 : %s\r\n", ValueToString(sizeofBuffer2));
													efiStatus = token1->Read(token1,(UINTN *)&sizeofBuffer2, buffer1);
													if (!EFI_ERROR(efiStatus))
													{
														//Print(L"Size Of Buffer 2 : %s\r\n", ValueToString(sizeofBuffer2));
														
														int ret = CalculateMD5((unsigned char *)buffer1,sizeofBuffer2,tempVal2);
														if(ret == TRUE)
														{
															//isHash2 = TRUE;
															systemTable->ConOut->OutputString(systemTable->ConOut,L"Hash Calculated Successfully 2 :");
															for(int i = 0; i < 16; i++)
															 {
																Print(L"%x",tempVal2[i]);
															 }
														    systemTable->ConOut->OutputString(systemTable->ConOut,L"\r\n");
															INTN cmpRet = CompareMem(tempVal1,tempVal2,16);
															if(cmpRet != 0)
															{
																//Print(L"Files are different\r\n");
																systemTable->ConOut->OutputString(systemTable->ConOut, L"Files are different\r\n");
																// if(buffer)
																// {
																// 	FreePool(buffer);
																// }
																// *token = NULL;
																// *token1 = NULL;
																// *root = NULL;
																// *fs = NULL;
																systemTable->BootServices->Exit(imageHandle,0,0,0);
																//return EFI_SUCCESS;
															}
															else
															{
																//Print(L"Files are similar\r\n");
																systemTable->ConOut->OutputString(systemTable->ConOut, L"Files are similar\r\n");
																// if(buffer)
																// {
																// 	FreePool(buffer);
																// }
																// *token = NULL;
																// *token1 = NULL;
																// *root = NULL;
																// *fs = NULL;
																systemTable->BootServices->Exit(imageHandle,0,0,0);
																//return EFI_SUCCESS;
															}
														}
														else
														{
															systemTable->ConOut->OutputString(systemTable->ConOut, L"Hash Calculation Failed\r\n");
															systemTable->BootServices->Exit(imageHandle,0,0,0);
														}		
													}
													else
													{
														StatusToString(tempString,efiStatus);
														systemTable->ConOut->OutputString(systemTable->ConOut,tempString);
														systemTable->ConOut->OutputString(systemTable->ConOut, L"Could not read second file\r\n");
													}
											}
											else
											{
												systemTable->ConOut->OutputString(systemTable->ConOut, L"Out of memory\r\n");	
											}
										}
							else
							{
								systemTable->ConOut->OutputString(systemTable->ConOut, L"Could Not Open File\r\n");
							}
							}
							else
							{
								StatusToString(tempString,efiStatus);
								systemTable->ConOut->OutputString(systemTable->ConOut,tempString);
							}
						}
						else
						{
							systemTable->ConOut->OutputString(systemTable->ConOut, L"Allocate Failed\r\n");
						}
					}
					else
					{
						systemTable->ConOut->OutputString(systemTable->ConOut, L"OpenFile Failed\r\n");
					}

					
							// if(isHash1 && isHash2)
							// 	{
							// 		for(int i = 0; i < 16 ; i++)
							// 		{
							// 			if(tempVal1[i] != tempVal2[i])
							// 			{
							// 				Print(L"Files are different\r\n");
							// 				return EFI_SUCCESS;
							// 			}
							// 		}
							// 		Print(L"Files are similar\r\n");
							// 	}
				}
				else
				{
					systemTable->ConOut->OutputString(systemTable->ConOut, L"OpenVolume Failed\r\n");
				}
			}
			else
			{
				systemTable->ConOut->OutputString(systemTable->ConOut, L"Handle Protocol Failed\r\n");
			}
		}
	}
	else
	{
		systemTable->ConOut->OutputString(systemTable->ConOut, L"LocateHandleBuffer Failed\r\n");
	}
	
	systemTable->ConOut->OutputString(systemTable->ConOut, L"Exiting Application\r\n");
	return EFI_SUCCESS;
}