//#include<Windows.h>

#ifndef _MD5MAIN_H_
#define _MD5MAIN_H_

#include <Uefi.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <Library/MemoryAllocationLib.h>
#include <Guid/FileInfo.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h> //for FileDevicePath
//
//	ROTATE_LEFT rotates x left n bits.
//
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

//
//	F, G and H are basic MD5 functions: selection, majority, parity.
//
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

//
//	FF transformation for round 1.
//	Rotation is separate from addition to prevent recomputation.
//
#define FF(a, b, c, d, x, s, ac) \
	{ \
	(a) += F ((b), (c), (d)) + (x) + (unsigned long)(ac); \
	(a) = ROTATE_LEFT((a), (s)); \
	(a) += (b); \
	}

//
//	GG transformation for round 2.
//	Rotation is separate from addition to prevent recomputation.
//
#define GG(a, b, c, d, x, s, ac) \
	{ \
	(a) += G ((b), (c), (d)) + (x) + (unsigned long)(ac); \
	(a) = ROTATE_LEFT((a), (s)); \
	(a) += (b); \
	}

//
//	HH transformation for round 3.
//	Rotation is separate from addition to prevent recomputation.
//
#define HH(a, b, c, d, x, s, ac) \
	{ \
	(a) += H ((b), (c), (d)) + (x) + (unsigned long)(ac); \
	(a) = ROTATE_LEFT((a), (s)); \
	(a) += (b); \
	}

//
//	II transformation for round 4.
//	Rotation is separate from addition to prevent recomputation.
//
#define II(a, b, c, d, x, s, ac) \
	{ \
	(a) += I ((b), (c), (d)) + (x) + (unsigned long)(ac); \
	(a) = ROTATE_LEFT((a), (s)); \
	(a) += (b); \
	}

#define MESSGE_DIGEST_SIZE				16
#define MESSGE_BUFFER_SIZE				1024
#define MAX_PATH_MD5					1024
//
//	Magic initialization constant that are pre-defined hard-coded
//	values for this algorithm.
//
#define MAGIC_INITIALIZATION_CONSTANT_1	0x67452301
#define MAGIC_INITIALIZATION_CONSTANT_2	0xefcdab89
#define MAGIC_INITIALIZATION_CONSTANT_3	0x98badcfe
#define MAGIC_INITIALIZATION_CONSTANT_4	0x10325476

//
//	M combinations that are pre-defined hard-coded combinations for
//	this algorithm.
//

//
//	Required for round 1.
//
#define M_1_1	7
#define M_1_2	12
#define M_1_3	17
#define M_1_4	22

//
//	Required for round 2.
//
#define M_2_1	5
#define M_2_2	9
#define M_2_3	14
#define M_2_4	20

//
//	Required for round 3.
//
#define M_3_1	4
#define M_3_2	11
#define M_3_3	16
#define M_3_4	23

//
//	Required for round 4.
//
#define M_4_1	6
#define M_4_2	10
#define M_4_3	15
#define M_4_4	21

//
//	T constants that are pre-defined hard-coded values for this
//	algorithm.
//

//
//	Required for round 1.
//
#define T_1_01	3614090360
#define T_1_02	3905402710
#define T_1_03	606105819
#define T_1_04	3250441966
#define T_1_05	4118548399
#define T_1_06	1200080426
#define T_1_07	2821735955
#define T_1_08	4249261313
#define T_1_09	1770035416
#define T_1_10	2336552879
#define T_1_11	4294925233
#define T_1_12	2304563134
#define T_1_13	1804603682
#define T_1_14	4254626195
#define T_1_15	2792965006
#define T_1_16	1236535329

//
//	Required for round 2.
//
#define T_2_01	4129170786
#define T_2_02	3225465664
#define T_2_03	643717713
#define T_2_04	3921069994
#define T_2_05	3593408605
#define T_2_06	38016083
#define T_2_07	3634488961
#define T_2_08	3889429448
#define T_2_09	568446438
#define T_2_10	3275163606
#define T_2_11	4107603335
#define T_2_12	1163531501
#define T_2_13	2850285829
#define T_2_14	4243563512
#define T_2_15	1735328473
#define T_2_16	2368359562

//
//	Required for round 3.
//
#define T_3_01	4294588738
#define T_3_02	2272392833
#define T_3_03	1839030562
#define T_3_04	4259657740
#define T_3_05	2763975236
#define T_3_06	1272893353
#define T_3_07	4139469664
#define T_3_08	3200236656
#define T_3_09	681279174
#define T_3_10	3936430074
#define T_3_11	3572445317
#define T_3_12	76029189
#define T_3_13	3654602809
#define T_3_14	3873151461
#define T_3_15	530742520
#define T_3_16	3299628645

//
//	Required for round 4.
//
#define T_4_01	4096336452
#define T_4_02	1126891415
#define T_4_03	2878612391
#define T_4_04	4237533241
#define T_4_05	1700485571
#define T_4_06	2399980690
#define T_4_07	4293915773
#define T_4_08	2240044497
#define T_4_09	1873313359
#define T_4_10	4264355552
#define T_4_11	2734768916
#define T_4_12	1309151649
#define T_4_13	4149444226
#define T_4_14	3174756917
#define T_4_15	718787259
#define T_4_16	3951481745

#pragma pack(1)
typedef struct tagCONTEXT_MD5
{
	unsigned long ulNumberOfBits[2];				//number of _bits_ handled mod 2^64.
	unsigned char uchMessageDigest[16];				//actual digest after MD5Final call.
	unsigned char uchInputBuffer[64];				//input buffer.
	unsigned long ulOutputBuffer[4];				//scratch buffer.

}	CONTEXT_MD5, *P_CONTEXT_MD5;
#pragma pack()

	 int CalculateMD5(
	const unsigned char *pcuchInputBuffer,
	unsigned long ulInputBufferLength,
	unsigned char *puchMessageDigest
	);

#endif	//_MD5MAIN_H_