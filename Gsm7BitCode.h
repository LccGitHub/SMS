#ifndef _GSM7BIT_CODE_
#define _GSM7BIT_CODE_
#include "Unicode.h"
int gsmEncode7bit(const char* pSrc, unsigned char* pDst, int nSrcLength);
int gsmDecode7bit(const unsigned char* pSrc, char* pDst, int nSrcLength);
int gsmEncode8bit(const char* pSrc, unsigned char* pDst, int nSrcLength);
int gsmDecode8bit(const unsigned char* pSrc, char* pDst, int nSrcLength);
int gsmEncodeUcs2(const unsigned char* pSrc, char16_t* pDst, size_t nSrcLength);
int gsmDecodeUcs2(const char16_t* pSrc, char* pDst, int nSrcLength);


#endif
