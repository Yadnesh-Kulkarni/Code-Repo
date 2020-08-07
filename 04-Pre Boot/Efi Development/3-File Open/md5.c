/*
 * TODO : PUT COPYRIGHTS TEXT 
 *
 */
//#include <Windows.h>
#include "md5.h"


unsigned char g_uchPadding[64] = {
								0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
								};

int InitMD5(
	P_CONTEXT_MD5 pContextMD5
	)
{
	if (NULL == pContextMD5)
	{
		return 0;
	}

	pContextMD5->ulNumberOfBits[0] = (unsigned long)0;
	pContextMD5->ulNumberOfBits[1] = (unsigned long)0;

	//
	//	Load magic initialization constants.
	//
	pContextMD5->ulOutputBuffer[0] = (unsigned long)MAGIC_INITIALIZATION_CONSTANT_1;
	pContextMD5->ulOutputBuffer[1] = (unsigned long)MAGIC_INITIALIZATION_CONSTANT_2;
	pContextMD5->ulOutputBuffer[2] = (unsigned long)MAGIC_INITIALIZATION_CONSTANT_3;
	pContextMD5->ulOutputBuffer[3] = (unsigned long)MAGIC_INITIALIZATION_CONSTANT_4;

	return 1;
}

int TransformMD5(
	unsigned long *pulInBuffer,
	unsigned long *pulOutBuffer
	)
{
	unsigned long ulValuea = 0;
	unsigned long ulValueb = 0;
	unsigned long ulValuec = 0;
	unsigned long ulValued = 0;

	if ((NULL == pulOutBuffer) || (NULL == pulInBuffer))
	{
		return 0;
	}

	ulValuea = pulOutBuffer[0];
	ulValueb = pulOutBuffer[1];
	ulValuec = pulOutBuffer[2];
	ulValued = pulOutBuffer[3];

	//
	//	Round 1 (1 to 16).
	//
	FF(ulValuea, ulValueb, ulValuec, ulValued, pulInBuffer[0], M_1_1, T_1_01);
	FF(ulValued, ulValuea, ulValueb, ulValuec, pulInBuffer[1], M_1_2, T_1_02);
	FF(ulValuec, ulValued, ulValuea, ulValueb, pulInBuffer[2], M_1_3, T_1_03);
	FF(ulValueb, ulValuec, ulValued, ulValuea, pulInBuffer[3], M_1_4, T_1_04);
	FF(ulValuea, ulValueb, ulValuec, ulValued, pulInBuffer[4], M_1_1, T_1_05);
	FF(ulValued, ulValuea, ulValueb, ulValuec, pulInBuffer[5], M_1_2, T_1_06);
	FF(ulValuec, ulValued, ulValuea, ulValueb, pulInBuffer[6], M_1_3, T_1_07);
	FF(ulValueb, ulValuec, ulValued, ulValuea, pulInBuffer[7], M_1_4, T_1_08);
	FF(ulValuea, ulValueb, ulValuec, ulValued, pulInBuffer[8], M_1_1, T_1_09);
	FF(ulValued, ulValuea, ulValueb, ulValuec, pulInBuffer[9], M_1_2, T_1_10);
	FF(ulValuec, ulValued, ulValuea, ulValueb, pulInBuffer[10], M_1_3, T_1_11);
	FF(ulValueb, ulValuec, ulValued, ulValuea, pulInBuffer[11], M_1_4, T_1_12);
	FF(ulValuea, ulValueb, ulValuec, ulValued, pulInBuffer[12], M_1_1, T_1_13);
	FF(ulValued, ulValuea, ulValueb, ulValuec, pulInBuffer[13], M_1_2, T_1_14);
	FF(ulValuec, ulValued, ulValuea, ulValueb, pulInBuffer[14], M_1_3, T_1_15);
	FF(ulValueb, ulValuec, ulValued, ulValuea, pulInBuffer[15], M_1_4, T_1_16);

	//
	//	Round 2 (17 to 32).
	//
	GG(ulValuea, ulValueb, ulValuec, ulValued, pulInBuffer[1], M_2_1, T_2_01);
	GG(ulValued, ulValuea, ulValueb, ulValuec, pulInBuffer[6], M_2_2, T_2_02);
	GG(ulValuec, ulValued, ulValuea, ulValueb, pulInBuffer[11], M_2_3, T_2_03);
	GG(ulValueb, ulValuec, ulValued, ulValuea, pulInBuffer[0], M_2_4, T_2_04);
	GG(ulValuea, ulValueb, ulValuec, ulValued, pulInBuffer[5], M_2_1, T_2_05);
	GG(ulValued, ulValuea, ulValueb, ulValuec, pulInBuffer[10], M_2_2, T_2_06);
	GG(ulValuec, ulValued, ulValuea, ulValueb, pulInBuffer[15], M_2_3, T_2_07);
	GG(ulValueb, ulValuec, ulValued, ulValuea, pulInBuffer[4], M_2_4, T_2_08);
	GG(ulValuea, ulValueb, ulValuec, ulValued, pulInBuffer[9], M_2_1, T_2_09);
	GG(ulValued, ulValuea, ulValueb, ulValuec, pulInBuffer[14], M_2_2, T_2_10);
	GG(ulValuec, ulValued, ulValuea, ulValueb, pulInBuffer[3], M_2_3, T_2_11);
	GG(ulValueb, ulValuec, ulValued, ulValuea, pulInBuffer[8], M_2_4, T_2_12);
	GG(ulValuea, ulValueb, ulValuec, ulValued, pulInBuffer[13], M_2_1, T_2_13);
	GG(ulValued, ulValuea, ulValueb, ulValuec, pulInBuffer[2], M_2_2, T_2_14);
	GG(ulValuec, ulValued, ulValuea, ulValueb, pulInBuffer[7], M_2_3, T_2_15);
	GG(ulValueb, ulValuec, ulValued, ulValuea, pulInBuffer[12], M_2_4, T_2_16);

	//
	//	Round 3 (33 to 48).
	//
	HH(ulValuea, ulValueb, ulValuec, ulValued, pulInBuffer[5], M_3_1, T_3_01);
	HH(ulValued, ulValuea, ulValueb, ulValuec, pulInBuffer[8], M_3_2, T_3_02);
	HH(ulValuec, ulValued, ulValuea, ulValueb, pulInBuffer[11], M_3_3, T_3_03);
	HH(ulValueb, ulValuec, ulValued, ulValuea, pulInBuffer[14], M_3_4, T_3_04);
	HH(ulValuea, ulValueb, ulValuec, ulValued, pulInBuffer[1], M_3_1, T_3_05);
	HH(ulValued, ulValuea, ulValueb, ulValuec, pulInBuffer[4], M_3_2, T_3_06);
	HH(ulValuec, ulValued, ulValuea, ulValueb, pulInBuffer[7], M_3_3, T_3_07);
	HH(ulValueb, ulValuec, ulValued, ulValuea, pulInBuffer[10], M_3_4, T_3_08);
	HH(ulValuea, ulValueb, ulValuec, ulValued, pulInBuffer[13], M_3_1, T_3_09);
	HH(ulValued, ulValuea, ulValueb, ulValuec, pulInBuffer[0], M_3_2, T_3_10);
	HH(ulValuec, ulValued, ulValuea, ulValueb, pulInBuffer[3], M_3_3, T_3_11);
	HH(ulValueb, ulValuec, ulValued, ulValuea, pulInBuffer[6], M_3_4, T_3_12);
	HH(ulValuea, ulValueb, ulValuec, ulValued, pulInBuffer[9], M_3_1, T_3_13);
	HH(ulValued, ulValuea, ulValueb, ulValuec, pulInBuffer[12], M_3_2, T_3_14);
	HH(ulValuec, ulValued, ulValuea, ulValueb, pulInBuffer[15], M_3_3, T_3_15);
	HH(ulValueb, ulValuec, ulValued, ulValuea, pulInBuffer[2], M_3_4, T_3_16);

	//
	//	Round 4 (49 to 64).
	//
	II(ulValuea, ulValueb, ulValuec, ulValued, pulInBuffer[0], M_4_1, T_4_01);
	II(ulValued, ulValuea, ulValueb, ulValuec, pulInBuffer[7], M_4_2, T_4_02);
	II(ulValuec, ulValued, ulValuea, ulValueb, pulInBuffer[14], M_4_3, T_4_03);
	II(ulValueb, ulValuec, ulValued, ulValuea, pulInBuffer[5], M_4_4, T_4_04);
	II(ulValuea, ulValueb, ulValuec, ulValued, pulInBuffer[12], M_4_1, T_4_05);
	II(ulValued, ulValuea, ulValueb, ulValuec, pulInBuffer[3], M_4_2, T_4_06);
	II(ulValuec, ulValued, ulValuea, ulValueb, pulInBuffer[10], M_4_3, T_4_07);
	II(ulValueb, ulValuec, ulValued, ulValuea, pulInBuffer[1], M_4_4, T_4_08);
	II(ulValuea, ulValueb, ulValuec, ulValued, pulInBuffer[8], M_4_1, T_4_09);
	II(ulValued, ulValuea, ulValueb, ulValuec, pulInBuffer[15], M_4_2, T_4_10);
	II(ulValuec, ulValued, ulValuea, ulValueb, pulInBuffer[6], M_4_3, T_4_11);
	II(ulValueb, ulValuec, ulValued, ulValuea, pulInBuffer[13], M_4_4, T_4_12);
	II(ulValuea, ulValueb, ulValuec, ulValued, pulInBuffer[4], M_4_1, T_4_13);
	II(ulValued, ulValuea, ulValueb, ulValuec, pulInBuffer[11], M_4_2, T_4_14);
	II(ulValuec, ulValued, ulValuea, ulValueb, pulInBuffer[2], M_4_3, T_4_15);
	II(ulValueb, ulValuec, ulValued, ulValuea, pulInBuffer[9], M_4_4, T_4_16);

	pulOutBuffer[0] += ulValuea;
	pulOutBuffer[1] += ulValueb;
	pulOutBuffer[2] += ulValuec;
	pulOutBuffer[3] += ulValued;

	return 1;
}


int UpdateMD5(
	P_CONTEXT_MD5 pContextMD5,
	const unsigned char *pcuchMessageBuffer,
	unsigned long ulMessageBufferLength
	)
{
	int iMdi;
	int iRet = 0;
	unsigned int uiIndex = 0;
	unsigned int uiLoopCount = 0;
	unsigned long ulInBuffer[MESSGE_DIGEST_SIZE];

	if ((NULL == pContextMD5) || (NULL == pcuchMessageBuffer))
	{	
		return 0;
	}

	//
	//	Compute number of bytes mod 64.
	//
	iMdi = (int)((pContextMD5->ulNumberOfBits[0] >> 3) & 0x3F);

	//
	//	Update number of bits.
	//
	if ((pContextMD5->ulNumberOfBits[0] + (ulMessageBufferLength << 3)) < pContextMD5->ulNumberOfBits[0])
	{
		pContextMD5->ulNumberOfBits[1]++;
	}

	pContextMD5->ulNumberOfBits[0] += (ulMessageBufferLength << 3);
	pContextMD5->ulNumberOfBits[1] += (ulMessageBufferLength >> 29);

	while (ulMessageBufferLength--)
	{
		//
		//	Add new character to buffer, increment mdi.
		//
		pContextMD5->uchInputBuffer[iMdi++] = *pcuchMessageBuffer++;

		//
		//	Transform if necessary.
		//
		if (0x40 == iMdi)
		{
			for (uiLoopCount = 0, uiIndex = 0; uiLoopCount < 16; uiLoopCount++, uiIndex += 4)
			{
				ulInBuffer[uiLoopCount] = (
										(((unsigned long)pContextMD5->uchInputBuffer[uiIndex + 3]) << 24) |
										(((unsigned long)pContextMD5->uchInputBuffer[uiIndex + 2]) << 16) |
										(((unsigned long)pContextMD5->uchInputBuffer[uiIndex + 1]) << 8) |
										((unsigned long)pContextMD5->uchInputBuffer[uiIndex + 0])
										);
			}

			iRet = TransformMD5(ulInBuffer, pContextMD5->ulOutputBuffer);
			if (0 == iRet)
			{
				return 0;
			}
			iMdi = 0;
		}
	}
	return 1;
}

int FinalMD5(
	P_CONTEXT_MD5 pContextMD5
	)
{
	int iMdi;
	int iRet = 0;
	unsigned int uiIndex = 0;
	unsigned int uiLoopCount = 0;
	unsigned long ulInBuffer[16];
	unsigned int uiPaddingLength = 0;

	if (NULL == pContextMD5)
	{
		return 0;
	}

	//
	//	Save number of bits.
	//
	ulInBuffer[14] = pContextMD5->ulNumberOfBits[0];
	ulInBuffer[15] = pContextMD5->ulNumberOfBits[1];

	//
	//	Compute number of bytes mod 64.
	//
	iMdi = (int)((pContextMD5->ulNumberOfBits[0] >> 3) & 0x3F);

	//
	//	Pad out to 56 mod 64.
	//
	uiPaddingLength = (iMdi < 56) ? (56 - iMdi) : (120 - iMdi);
	iRet = UpdateMD5(pContextMD5, g_uchPadding, uiPaddingLength);
	if (0 == iRet)
	{
		return 0;
	}

	//
	//	Append length in bits and transform.
	//
	for (uiLoopCount = 0, uiIndex = 0; uiLoopCount < 14; uiLoopCount++, uiIndex += 4)
	{
		ulInBuffer[uiLoopCount] = (
								((unsigned long)pContextMD5->uchInputBuffer[uiIndex+3]) << 24) |
								(((unsigned long)pContextMD5->uchInputBuffer[uiIndex+2]) << 16) |
								(((unsigned long)pContextMD5->uchInputBuffer[uiIndex+1]) << 8) |
								((unsigned long)pContextMD5->uchInputBuffer[uiIndex]
								);
	}

	iRet = TransformMD5(ulInBuffer, pContextMD5->ulOutputBuffer);
	if (0 == iRet)
	{
		return 0;
	}

	//
	//	Store buffer in digest.
	//
	for (uiLoopCount = 0, uiIndex = 0; uiLoopCount < 4; uiLoopCount++, uiIndex += 4)
	{
		pContextMD5->uchMessageDigest[uiIndex + 0] = (unsigned char)(pContextMD5->ulOutputBuffer[uiLoopCount] & 0xFF);
		pContextMD5->uchMessageDigest[uiIndex + 1] = (unsigned char)((pContextMD5->ulOutputBuffer[uiLoopCount] >> 8) & 0xFF);
		pContextMD5->uchMessageDigest[uiIndex + 2] = (unsigned char)((pContextMD5->ulOutputBuffer[uiLoopCount] >> 16) & 0xFF);
		pContextMD5->uchMessageDigest[uiIndex + 3] = (unsigned char)((pContextMD5->ulOutputBuffer[uiLoopCount] >> 24) & 0xFF);
	}

	return 1;
}

int CalculateMD5(
	const unsigned char *pcuchInputBuffer,
	unsigned long ulInputBufferLength,
	unsigned char *puchMessageDigest
	)
{
	int iRet = 0;
	CONTEXT_MD5 ContextMD5;
	if ((NULL == pcuchInputBuffer) || (NULL == puchMessageDigest))
	{
		return FALSE;
	}

//	DoTraceEx(TRACE_LEVEL_VERBOSE, FIT_TRACING, "CalculateMD5: fileName = %S, Name length = 0x%x", (wchar_t *)pcuchInputBuffer, ulInputBufferLength);

	SetMem(&ContextMD5, 0L, sizeof(CONTEXT_MD5));

	//
	//	Initializing MD5 context.
	//
	iRet = InitMD5(&ContextMD5);
	if (0 == iRet)
	{
		return FALSE;
	}

	//
	//	Updating the MD5 context with new values.
	//
	iRet = UpdateMD5(&ContextMD5, pcuchInputBuffer, ulInputBufferLength);
	if (0 == iRet)
	{
		return FALSE;
	}

	//
	//	Final Message Digest calculation.
	//
	iRet = FinalMD5(&ContextMD5);
	if (0 == iRet)
	{
		return FALSE;
	}

	//
	//	Copying the obtained MD5 value into MessageDigest.As both of them are of same size(16).
	//
	CopyMem(puchMessageDigest, ContextMD5.uchMessageDigest, sizeof(ContextMD5.uchMessageDigest));

//	DoTraceEx(TRACE_LEVEL_VERBOSE, FIT_TRACING, "CalculateMD5: Done fileName = %S, Name length = 0x%x", (wchar_t *)pcuchInputBuffer, ulInputBufferLength);
	return TRUE;
}