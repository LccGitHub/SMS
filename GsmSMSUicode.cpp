#include <string.h>
#include "GsmSMSUicode.h"


/* @Brief Encode Gsm7Bit
 * @Param pSrc: source string pointer
 * @Param pDst: destion string pointer
 * @Param nSrcLength: source string length
 * @Return: destion string length
*/
int gsmEncode7bit(const char* pSrc, unsigned char* pDst, int nSrcLength)
{
    int nSrc = 0; /*src cnt val*/
    int nDst = 0; /*dst cnt val*/
    int nChar = 0; /*id in 8 byte, range is 0-7*/
    unsigned char nLeft = 0; /*last byte of residual data*/

/* 
 * 1> make a group 8 byte(in source data) to 7 byte
 * 2> loop excute setp 1 until nsrc = nSrcLength
*/
    while(nSrc <= nSrcLength) {
       // Take the lowest 3 bit of src cnt val
        nChar = nSrc & 7;/*Group processing by 8*/
        /* deal with each byte of src */
        if(nChar == 0) {
            /* The first byte in the group, just saved, used when the next byte is pending */
            nLeft = *pSrc;
        }
        else {
            /* Other bytes in the group, add the right part of the group to the residual data to get a target coded byte. */
            *pDst = (*pSrc << (8-nChar)) | nLeft;
            /* Save the left part of the byte as residual data */
            nLeft = *pSrc >> nChar;
            /* Modify the pointer and count value of the target string */
            pDst++;
            nDst++;
        } 
        /* Modify the pointer and count value of the source string */
        pSrc++;
        nSrc++;
    }
    //printf("nLeft = %02x\n",nLeft);
    if(nLeft != 0) {
        *pDst = nLeft;
        nDst++;
    }
    /* Return destion string length */
    return nDst; 
}

/* @Brief decode Gsm7Bit
 * @Param pSrc: source string pointer
 * @Param pDst: destion string pointer
 * @Param nSrcLength: source string length
 * @Return: destion string length
*/
int gsmDecode7bit(const unsigned char* pSrc, char* pDst, int nSrcLength)
{
    int nSrc = 0; /*src cnt val*/
    int nDst = 0; /*dst cnt val*/
    int nByte = 0; /*id in 7 byte, range is 0-6*/
    unsigned char nLeft = 0; /*last byte of residual data*/

/* 
 * 1> make a group 7 byte(in source data) to 8 byte
 * 2> loop excute setp 1 until nsrc = nSrcLength
*/
    while(nSrc < nSrcLength) {
        /* Add the right part of the source byte to the residual data, remove the highest bit, and get a target decoded byte. */
        *pDst = (((unsigned char)(*pSrc) << nByte) | nLeft) & 0x7f;
        /* Save the left part of the byte as residual data */
        nLeft = (unsigned char)(*pSrc)>> (7-nByte);
        /* fix dst point and cnt */
        pDst++;
        nDst++;
        /* Modify the byte count value */
        nByte++;
        /* Go to the last byte of a group */
        if(nByte == 7) {
            /* Extra get a target decode byte */
            *pDst = nLeft;
            /* fix dst point and cnt */
            pDst++;
            nDst++;
            /* Intra-byte byte number and residual data initialization */
            nByte = 0;
            nLeft = 0;
        }
        /* fix src point and cnt */
        pSrc++;
        nSrc++;
    } 
    *pDst = 0;
    /* return len of dst str */
    return nDst;
}

/* @Brief Encode Gsm 8Bit
 * @Param pSrc: source string pointer
 * @Param pDst: destion string pointer
 * @Param nSrcLength: source string length
 * @Return: destion string length
*/
int gsmEncode8bit(const char* pSrc, unsigned char* pDst, int nSrcLength)
{
    memcpy(pDst, pSrc, nSrcLength);

    return nSrcLength;
}

/* @Brief decode Gsm 8Bit
 * @Param pSrc: source string pointer
 * @Param pDst: destion string pointer
 * @Param nSrcLength: source string length
 * @Return: destion string length
*/
int gsmDecode8bit(const unsigned char* pSrc, char* pDst, int nSrcLength)
{
    memcpy(pDst, pSrc, nSrcLength);
    pDst[nSrcLength] = '\0';
    return nSrcLength;
}

/* @Brief Encode Gsm UCS2 uf8->uft16
 * @Param pSrc: source string pointer
 * @Param pDst: destion string pointer
 * @Param nSrcLength: source string length
 * @Return: destion string length
*/
int gsmEncodeUcs2(const unsigned char* pSrc, char16_t* pDst, size_t nSrcLength)
{
    utf8_to_utf16(pSrc, nSrcLength, pDst);
    return strlen16(pDst);
}

/* @Brief Decode Gsm UCS2 uft16->uft8
 * @Param pSrc: source string pointer
 * @Param pDst: destion string pointer
 * @Param nSrcLength: source string length
 * @Return: destion string length
*/
int gsmDecodeUcs2(const char16_t* pSrc, char* pDst, int nSrcLength)
{
    utf16_to_utf8(pSrc, nSrcLength, pDst);
    size_t len = utf16_to_utf8_length(pSrc,nSrcLength);

    /* return dst len */
    return len;
}



