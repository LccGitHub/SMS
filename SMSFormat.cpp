/*************************************************************************
	> File Name: SmsFormat.cpp
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: Sat 29 Sep 2018 01:50:04 PM CST
 ************************************************************************/
#include <string.h>
#include "SMSFormat.h"
#include "GsmSMSUicode.h"
#include "Unicode.h"
//using namespace std;


//#define SAC "00"
//static char SAC[102] = "+8613032551239";
static char SAC[102] = "00";
static const char * INTERNATIONAL_TYPE = "91"; 
static const char * NATIONAL_TYPE = "81";
#define PDU_TYPE "31"
#define MR "00"
#define PID "00"
#define DCS_STR "08"
#define DCS_UCS2 "08"
#define DCS_GSM7 "00"
#define DCS_GSM8 "F6"
#define VP "A7"
#define NUM_INTERNAL_TYPE "A7"

/*decode number format
* input:683110304105F0
* output:8613010314500
*/
static void decodeFormatNumber(char * num) {
   int len = strlen(num);
   int i;
   char tmp;
   for(i = 0; i < len/2; ++i) {
       tmp = num[2*i];
       num[2*i] = num[2*i + 1];
       num[2*i + 1] = tmp;
   }

   if(num[strlen(num) - 1] == 'f' || num[strlen(num) - 1] == 'F') {
    num[strlen(num) - 1] = '\0';
    }
}

static void decodeFormatTime(char * num) {
   int len = strlen(num);
   int i;
   char tmp;
   for(i = 0; i < len/2; ++i) {
       tmp = num[2*i];
       num[2*i] = num[2*i + 1];
       num[2*i + 1] = tmp;
   }
}


static int XcharToInt(char val)
{
    int res = 0;
    if (val >= '0' && val <= '9') {
        res = val - '0' + 0;
    }
    else if (val >= 'A' && val <= 'F'){
        res = val - 'A' + 10;
    }
    else if (val >= 'a' && val <= 'f'){
        res = val - 'a' + 10;
    }
    else {
        printf("%s,%d, c[%c] is error", __func__, __LINE__,val);
    }
    return res;
}

static int XcharToAcii(const unsigned char* src, unsigned char* dst)
{
    int index = 0;
    if (src == NULL || dst == NULL) {
    }
    else {
        size_t i;
        char transfer[3] = {0};
        for(i = 0; i < strlen((const char*)src); i += 2, index++) {
            memset(transfer,0,3);
            memcpy(transfer,src + i,2);
            dst[index] = strtol(transfer,NULL,16);
        }
    }
    return index-1;
}


static int XcharToUcs2(const unsigned char * str, size_t len, char16_t* ucs2Data)
{
    int index = 0;
    if(str == NULL || ucs2Data == NULL) {
       //my_print("str %p,ucs2Data %p\n",str,ucs2Data);
    }
    else {
        size_t i;
        char tmp[5] = {0};

        for(i = 0; i < len; i += 4, index++) {
            memset(tmp,0,sizeof(tmp));
            memcpy(tmp,str + i,4);
            ucs2Data[index] = strtol(tmp,NULL,16);
        }
    }
    return index;
}


/* If the length is odd, then append an F, then loop to swap the adjacent 2 characters */
static void formatNumber(char * num) {
   int len = strlen(num);
   if(len % 2 != 0) {
      strcat(num,"F");
      len++;
   }
   int i;
   char tmp;
   for(i = 0; i < len/2; ++i) {
       tmp = num[2*i];
       num[2*i] = num[2*i + 1];
       num[2*i + 1] = tmp;
   }
}

static void ucs2_to_str(const char16_t* ucs2Data,size_t ucs2Len, unsigned char * str_out)
{
    if(str_out == NULL) {
      printf("str_out is nil\n");
      return;
    }
    int i;
    char tmp[5] = { 0 };
    for(i = 0; i < ucs2Len; ++i) {
       memset(tmp, 0, sizeof(tmp));
       sprintf(tmp, "%04x", ucs2Data[i]);
       strcat((char*)str_out, tmp);
    }
}

static void str_to_hexstr(const unsigned char* ucs2Data,size_t ucs2Len, unsigned char * str_out)
{
    if(str_out == NULL) {
      printf("str_out is nil\n");
      return;
    }
    int i;
    char tmp[3] = { 0 };
    for(i = 0; i < ucs2Len; ++i) {
       memset(tmp, 0, sizeof(tmp));
       sprintf(tmp, "%02X", ucs2Data[i]);
       strcat((char*)str_out, tmp);
    }
}

SMS_Format::SMS_Format(const char* pdu)
{
    char temp[1024] = {0};
    char tempLast[1024] = {0};
    memset(temp, 0, sizeof(temp));
    memcpy(temp, pdu, sizeof(temp)-1);
    printf("%s,%d, ori pdu[%s]\n", __func__, __LINE__, temp);
    
    int lastSize = decodeSCA(temp);
    lastSize = lastSize + decodePDUType((char*)(temp+lastSize));
    lastSize = lastSize + decodeMR((char*)(temp+lastSize));
    lastSize = lastSize + decodeOA((char*)(temp+lastSize));
    lastSize = lastSize + decodeDA((char*)(temp+lastSize));
    lastSize = lastSize + decodePID((char*)(temp+lastSize));
    lastSize = lastSize + decodeDCS((char*)(temp+lastSize));
    lastSize = lastSize + decodeVP((char*)(temp+lastSize));
    lastSize = lastSize + decodeSCTS((char*)(temp+lastSize));
    lastSize = lastSize + decodeUDL((char*)(temp+lastSize));
    lastSize = lastSize + decodeUD((char*)(temp+lastSize));
    
    
}
/*SCA format
* 0891683110304105F0
* SCA-len SCA-type SCA-addr
*  08       91      683110304105F0
*/
int SMS_Format::decodeSCA(char* temp)
{
    int lastSize = 0;
    memset(&mSCA, 0, sizeof(struct NumAddr));
    printf("%s,%d, temp0[%c]\n", __func__, __LINE__, temp[0]);
    printf("%s,%d, temp1[%c]\n", __func__, __LINE__, temp[1]);
    mSCA.len = XcharToInt(temp[0])*16 + XcharToInt(temp[1]);
    printf("%s,%d, SCA-LEN[%d]\n", __func__, __LINE__, mSCA.len);
    if (mSCA.len == 0) {
        printf("%s,%d, SCA len is 0, no privede SCA addr \n", __func__, __LINE__);
        lastSize = BTYE1*2;
    }
    else {
        sscanf(temp, "%*2s%2s", mSCA.type);
        int numLen = (mSCA.len-1)*2;
        memcpy(mSCA.addr, temp+BTYE2*2, numLen);
        printf("%s,%d, SCA[%d-%s-%s]\n", __func__, __LINE__, mSCA.len, mSCA.type, mSCA.addr);
        decodeFormatNumber(mSCA.addr);
        printf("%s,%d, SCA[%d-%s-%s]\n", __func__, __LINE__, mSCA.len, mSCA.type, mSCA.addr);
        lastSize = (mSCA.len+1)*2; /*SCA byte = SCA len + 1 byte*/
    }
    return lastSize;
}
/*PDU-type format
* for example "60"
* 0110 0000
*tpRp tpUDHI tpSRR tpVPF tpRD tpMTI
*0      1      1    00    0     00
*/
int SMS_Format::decodePDUType(char* temp)
{
    int lastSize = 0;
    memset(&mPDUTpe, 0, sizeof(struct PDUType));
    printf("%s,%d, temp[%2s]\n\n", __func__, __LINE__, temp);
    //printf("%s,%d, PDU[0][%c]\n", __func__, __LINE__, temp[0]);
    //printf("%s,%d, PDU[1][%c]\n", __func__, __LINE__, temp[1]);
    mPDUTpe.tpRP = (temp[0] & 0x8) >> 3;
    mPDUTpe.tpUDHI = (temp[0] & 0x4) >>2;
    mPDUTpe.tpSRR = (temp[0] & 0x2) >> 1;
    mPDUTpe.tpVPF = ((temp[0] & 0x1) * 2) + ((temp[1] & 0x8) >> 3);
    //printf("%s,%d, VPT[%d-%d-%d]\n", __func__, __LINE__, temp[0] & 0x1, temp[1] & 0x8, (temp[0] & 0x1) + (temp[1] & 0x8));
    mPDUTpe.tpRD = (temp[1] & 0x4) >>2;
    mPDUTpe.tpMTI = (temp[1] & 0x3);
    //printf("%s,%d, tpMTI[%d]\n", __func__, __LINE__, temp[1] & 0x3);
    lastSize = BTYE1*2;/*PDU-type is 1 byte*/
    printf("%s,%d, PDU-type[tpRP:%d-tpUDHI:%d-tpSRR:%d-tpVPF:%d-tpRD:%d-tpMTI:%d]\n", __func__, __LINE__, mPDUTpe.tpRP, mPDUTpe.tpUDHI, mPDUTpe.tpSRR,
        mPDUTpe.tpVPF, mPDUTpe.tpRD, mPDUTpe.tpMTI);
    return lastSize;
}
/* decodeMR
* this function only decode submit SMS
* in PDU-tpe tpMTI:
* mPDUTpe.tpMTI = 0, SMS-DELIVER (SMSC->MS)
* mPDUTpe.tpMTI = 0, SMS-DELIVER REPORT (MS->SMSC)
* mPDUTpe.tpMTI = 1, SMS-SBUMIT (MS->SMSC)
* mPDUTpe.tpMTI = 1, SMS-DELIVER REPORT (MS->SMSC)
* mPDUTpe.tpMTI = 2, SMS-SBUMIT REPROT (MS->SMSC)
* mPDUTpe.tpMTI = 2, SMS-COMMAND (MS->SMSC)
*/
int SMS_Format::decodeMR(char* temp)
{
    int lastSize = 0;
    if (mPDUTpe.tpMTI == 01) {
        sscanf(temp, "%2d", &mMR);
        printf("%s,%d, MR[%d]\n", __func__, __LINE__, mMR);
        lastSize = 1*2;/*MR is 1 byte*/
    }
    return lastSize;
}
/* decodeOA: decode OA
* only decode SMS-DELIVER
*/
int SMS_Format::decodeOA(char* temp)
{
    int lastSize = 0;
    if (mPDUTpe.tpMTI == 0) {
        memset(&mOA, 0, sizeof(struct NumAddr));
        printf("%s,%d, OA[0][%c]\n", __func__, __LINE__, temp[0]);
        printf("%s,%d, OA[1][%c]\n", __func__, __LINE__, temp[1]);
        mOA.len = XcharToInt(temp[0])*16 + XcharToInt(temp[1]);
        if (mOA.len %2 !=0) {
            mOA.len = mOA.len + 1;
        }
        sscanf(temp + 2, "%2s",  mOA.type);
        int numLen = mOA.len; /* this is different with SCA.len, this is OA.addr len not byte */
        char number[DA_MAX_BYTE*2+1] = { 0 };
        memset(number, 0, sizeof(number));
        strncat(number, temp+BTYE2*2, numLen);
        printf("%s,%d, OA[%d-%s-%s]\n", __func__, __LINE__, mOA.len, mOA.type, number);
        decodeFormatNumber(number);

        if (strncmp(TYPE_INTERNATIONAL, mOA.type, strlen(TYPE_INTERNATIONAL)) == 0) {
            strncat(mOA.addr, "+", 1);
        }
        strncat(mOA.addr, number, numLen);

        printf("%s,%d, OA[%d-%s-%s]\n", __func__, __LINE__, mOA.len, mOA.type, mOA.addr);
        lastSize = numLen+2*2;; /*DA byte = DA len + 1*2*/
    }
    return lastSize;
}
/* decodeOA: decode DA
* only decode SMS-SUBMIT
*/
int SMS_Format::decodeDA(char* temp)
{
    int lastSize = 0;
    if (mPDUTpe.tpMTI == 1) {
        memset(&mDA, 0, sizeof(struct NumAddr));
        mDA.len = XcharToInt(temp[0])*16 + XcharToInt(temp[1]);
        if (mDA.len %2 !=0)
            mDA.len = mDA.len + 1;
        sscanf(temp, "%*2s%2s", mDA.type);
        int numLen = mDA.len; /* this is different with SCA.len, this is OA.addr len not byte */
        char number[DA_MAX_BYTE*2+1] = { 0 };
        memset(number, 0, sizeof(number));
        strncat(number, temp+BTYE2*2, numLen);
        printf("%s,%d, DA[%d-%s-%s]\n", __func__, __LINE__, mDA.len, mDA.type, number);
        decodeFormatNumber(number);

        if (strncmp(TYPE_INTERNATIONAL, mDA.type, strlen(TYPE_INTERNATIONAL)) == 0) {
            strncat(mDA.addr, "+", 1);
        }
        strncat(mDA.addr, number, numLen);

        printf("%s,%d, DA[%d-%s-%s]\n", __func__, __LINE__, mDA.len, mDA.type, mDA.addr);
        lastSize = mDA.len+2*2;; /*DA byte = DA len + DA_type + DA_addr*/
    }
    return lastSize;
}
int SMS_Format::decodePID(char* temp)
{
    int lastSize = 0;
    mPID = XcharToInt(temp[0])*16 + XcharToInt(temp[1]);
    printf("%s,%d, PID[0x%2x]\n", __func__, __LINE__, mPID);
    lastSize = 1*2;/*MR is 1 byte*/
    return lastSize;
}
/* decodeDCS
* 1 byte: bit NO.3 & NO.2 is deoce way
* 00: 7Bit
* 01: 8Bit
* 10: USC2
* 11: Reserved
*/
int SMS_Format::decodeDCS(char* temp)
{
    int lastSize = 0;
    mDCS = (temp[1] & 0xC) >> 2;
    printf("%s,%d, mDCS[0x%2x]\n", __func__, __LINE__, mDCS);
    if (mDCS == 0x0) {
        printf("%s,%d, 7 bit \n", __func__, __LINE__);
        mDCS = GSM7Bit;
    }
    else if (mDCS == 0x1) {
        printf("%s,%d, 8 bit \n", __func__, __LINE__);
        mDCS = GSM8Bit;
    }
    else if (mDCS == 0x2) {
        printf("%s,%d, UCS2 \n", __func__, __LINE__);
        mDCS = USC2;
    }
    else {
        printf("%s,%d, Unkonwn decode\n", __func__, __LINE__);
        mDCS = UNKNOWN;
    }
    lastSize = 1*2;/*MR is 1 byte*/
    return lastSize;
}
/* decodeVP :decode Validity Period
* have two format as follows;
* 1> VP=10, this is 1 byte
* 2> VP=11, this is 7 byte, this is vaild period absolute time :YY/MM/DD/HH/MM/SS/Time zone
* Note: can judy by Tp-VPF in PDU-Type as follows:
*       Tp-VPF=0, this value is none,
*       Tp-VPF=1, Reserved,
*       Tp-VPF=2, this value is 1 byte,Provided by integer
*       Tp-VPF=3, this value is 7 byte,
*/
int SMS_Format::decodeVP(char* temp)
{
    int lastSize = 0;
    if (mPDUTpe.tpVPF== 0x0) {
        printf("%s,%d, no need to decode Vp \n", __func__, __LINE__);
    }
    else if (mPDUTpe.tpVPF== 0x1) {
        printf("%s,%d, Vp is Reserved \n", __func__, __LINE__);
    }
    else if (mPDUTpe.tpVPF== 0x2) {
        sscanf(temp, "%2s", mVPFormat);
        char *endptr;
        mVP = strtol(mVPFormat, &endptr, 16);
        printf("%s,%d, mVP[%d ] \n", __func__, __LINE__, mVP);
        if ((mVP >=0x00) && (mVP <= 0x8F)) {
            mVP = (mVP + 1) * 5; /* range: 5min ~ 12 hour */
            printf("%s,%d, mVP[%d Min] \n", __func__, __LINE__, mVP);
        }
        else if ((mVP >=0x90) && (mVP <= 0xA7)) {
            mVP = 12 * 60+ (mVP - 143) * 30; /* range: 12hour ~ 24 hour */
            printf("%s,%d, mVP[%d Hour] \n", __func__, __LINE__, mVP/60);
        }
        else if ((mVP >=0xA8) && (mVP <= 0xC4)) {
            mVP = (mVP - 166) * 1 * 24 * 60; /* range: 1day ~ 7 day */
            printf("%s,%d, mVP[%d Day] \n", __func__, __LINE__, mVP/(24 * 60));
        }
        else if ((mVP >=0xC5) && (mVP <= 0xFF)) {
            mVP = (mVP - 192) * 1 * 7* 24 * 60; /* range: 1week ~  */
            printf("%s,%d, mVP[%d Week] \n", __func__, __LINE__, mVP/(7* 24 * 60));
        }
        else {
        }
        lastSize = 1*2;
        printf("%s,%d, mVP[%s] \n", __func__, __LINE__, mVPFormat);
        memset(mVPFormat, 0, sizeof(mVPFormat));
    }
    else if (mPDUTpe.tpVPF== 0x3) {
        sscanf(temp, "%14s", mVPFormat);
        lastSize = VP_MAX_BYTE*2;
        printf("%s,%d, VPFormat[%s] \n", __func__, __LINE__, mVPFormat);
    }
    else {
        printf("%s,%d, Unkonwn decode\n", __func__, __LINE__);
    }
    return lastSize;
}
/* decodeSCTS :decode Service Center TimeStamp
 * format :YY/MM/DD/HH/MM/SS/Time zone
 * only decode SMS-DELIVER
*/
int SMS_Format::decodeSCTS(char* temp)
{
    int lastSize = 0;
    if (mPDUTpe.tpMTI == 0) {
        sscanf(temp, "%14s", mSCTS);
        lastSize = SCTS_MAX_BYTE*2;
        printf("%s,%d, SCTS[%s] \n", __func__, __LINE__, mSCTS);
        decodeFormatTime(mSCTS);
        printf("%s,%d, format SCTS[%s] \n", __func__, __LINE__, mSCTS);
    }
    return lastSize;
}
/* decodeUDL :decode User-Data-Length
*  Tp-UDHI=0, this value is none,
*  Tp-UDHI=1, this value is 1 byte,
*/
int SMS_Format::decodeUDL(char* temp)
{
    int lastSize = 0;

        //printf("%s,%d, temp[0][%c] \n", __func__, __LINE__, temp[0]);
        //printf("%s,%d, temp[1][%c] \n", __func__, __LINE__, temp[1]);
        //sscanf(temp, "%2d%*s", &mUDL); /*why result is temp[0]*/
    mUDL = XcharToInt(temp[0])*16 + XcharToInt(temp[1]);
    printf("%s,%d, UDL[%d] \n", __func__, __LINE__, mUDL);
    lastSize = 1*2;
    return lastSize;
}


int SMS_Format::decodeUD(char* temp)
{
    int lastSize = 0;
    memset(mUD, 0, sizeof(mUD));
    if (mPDUTpe.tpUDHI== 0x0) {
        printf("%s,%d, no UDH \n", __func__, __LINE__);
        memcpy(mUD, temp, mUDL*2);
        lastSize = mUDL*2;
        printf("%s,%d, UD[%s] \n", __func__, __LINE__, mUD);
    }
    else if (mPDUTpe.tpUDHI== 0x1) {
        mUDH = XcharToInt(temp[0])*16 + XcharToInt(temp[1]);
        memcpy(mUD, temp + 2 + mUDH*2, mUDL*2-2);
        lastSize = mUDL*2 -2 - mUDH*2;
        printf("%s,%d, UDL[0x%02x][%s] \n", __func__, __LINE__, mUDH, mUD);
    }
    else {
        printf("%s,%d, Unkonwn decode\n", __func__, __LINE__);
    }
    if (mDCS == GSM7Bit) {

        unsigned char src[140] = {0};
        XcharToAcii(mUD, src);

        memset(mSMSContext, 0, sizeof(mSMSContext));
        gsmDecode7bit(src, mSMSContext, strlen((const char*)src));
        printf("%s,%d, SMS 7 Bit Context \n", __func__, __LINE__);
    }
    else if (mDCS == GSM8Bit) {
        unsigned char src[140] = {0};

        memset(mSMSContext, 0, sizeof(mSMSContext));
        gsmDecode8bit(mUD, mSMSContext, strlen((const char*)mUD));
        printf("%s,%d, SMS 8 BitContext \n", __func__, __LINE__);
    }
    else if (mDCS == USC2) {
        char16_t src[256] = {0};
        int lenOfChar16 = XcharToUcs2(mUD, strlen((const char*)mUD), src);

        memset(mSMSContext, 0, sizeof(mSMSContext));
        gsmDecodeUcs2(src, mSMSContext, lenOfChar16);
        /*
        utf16_to_utf8(src, lenOfChar16, mSMSContext);
        size_t len = utf16_to_utf8_length(src,lenOfChar16);
        */
        printf("%s,%d, SMS USC2 Context\n", __func__, __LINE__);
    }
    else {
    }

    printf("[%s]\n", mSMSContext);
    return lastSize;
}

int SMS_Format::smsGSmEncode(const SMS sendSms, char* pdu, size_t pddLen)
{
    int res = 0;
    char numTemp[128] = {0};
    bool isInternational = false;

/* encode DA */
    char DATpe[3] = {0};
    if(strstr(sendSms.num,"+")) {
      memcpy(numTemp,sendSms.num + 1, strlen(sendSms.num) - 1);
      isInternational = true;
      strncpy(DATpe, TYPE_INTERNATIONAL, sizeof(DATpe));
    }
    else {
      memcpy(numTemp, sendSms.num, strlen(sendSms.num));
      strncpy(DATpe, TYPE_DOMESTIC, sizeof(DATpe));
    }
    int DALen = strlen(numTemp); /*DA addr len is not contain F*/

    formatNumber(numTemp);


/* encode UD */
    int UDlen = 0;
    unsigned char ucs2Str[70*4] = {0};
    char16_t ucs2[70*4] = {0};
    char dCS[10] = {0};
    if (USC2 == sendSms.codeType) {
        int lenUcs2 = gsmEncodeUcs2(sendSms.context, ucs2, strlen((const char*)sendSms.context));
        ucs2_to_str(ucs2, lenUcs2, ucs2Str);
        UDlen = strlen((char*)ucs2Str)/2;
        strncat(dCS, DCS_UCS2, strlen(DCS_UCS2));
    }
    else if (GSM7Bit == sendSms.codeType) {
        printf("%s,%d\n", __func__, __LINE__);
        unsigned char gsm7Str[70*4] = {0};
        int lenUcs2 = gsmEncode7bit((char*)sendSms.context, gsm7Str, strlen((const char*)sendSms.context));
        str_to_hexstr(gsm7Str, lenUcs2, ucs2Str);
        printf("%s,%d, encode str=%s\n", __func__, __LINE__, ucs2Str);
        UDlen = strlen((char*)ucs2Str)/2;
        printf("%s,%d, UDlen = %d\n", __func__, __LINE__, UDlen);
        strncat(dCS, DCS_GSM7, strlen(DCS_GSM7));
    }
    else if (GSM8Bit == sendSms.codeType) {
        unsigned char gsm7Str[70*4] = {0};
        int lenUcs2 = gsmEncode8bit((char*)sendSms.context, gsm7Str, strlen((const char*)sendSms.context));
        str_to_hexstr(gsm7Str, lenUcs2, ucs2Str);
        UDlen = strlen((char*)ucs2Str)/2;
        strncat(dCS, DCS_GSM8, strlen(DCS_GSM8));
    }
    else {
        printf("%s,%d, code type is Err, will use ucs2\n", __func__, __LINE__);
        int lenUcs2 = gsmEncodeUcs2(sendSms.context, ucs2, strlen((const char*)sendSms.context));
        ucs2_to_str(ucs2, lenUcs2, ucs2Str);
        UDlen = strlen((char*)ucs2Str)/2;
        strncat(dCS, DCS_UCS2, strlen(DCS_UCS2));
    }
    printf("%s,%d\n", __func__, __LINE__);

/* SCA PDUType MR DA-len DA-tpe DAAddr PID DCS VP UDL UD*/

    if (strlen(sendSms.scaNum) == 0) {
        printf("%s,%d\n", __func__, __LINE__);
        snprintf(pdu, pddLen, "%s%s%s%02X%s%s%s%s%s%02X%s", 
            SAC, PDU_TYPE, MR, DALen, DATpe, numTemp, PID, dCS, VP, UDlen, ucs2Str);
        printf("%s,%d\n", __func__, __LINE__);
    }
    else if (sendSms.scaNum[0] == '+'){
        char sca[56] = {0};
        strcpy(sca, sendSms.scaNum+1);
        formatNumber((sca));
        int SCALen = strlen(sca)/2 + 1;
        snprintf(pdu, pddLen, "%02x%s%s%s%s%02X%s%s%s%s%s%02X%s", 
            SCALen, "91", sca, PDU_TYPE, MR, DALen, DATpe, numTemp, PID, dCS, VP, UDlen, ucs2Str);
    }
    else {
        char sca[56] = {0};
        strcpy(sca, sendSms.scaNum);
        int SCALen = strlen(sca)/2 + 1 + 1;
        formatNumber(sca);
        snprintf(pdu, pddLen, "%02x%s%s%s%s%02X%s%s%s%s%s%02X%s", 
            SCALen, "81", sca, PDU_TYPE, MR, DALen, DATpe, numTemp, PID, dCS, VP, UDlen, ucs2Str);
    }

    printf("Encode SMS PDU\n");
    printf("[%s]\n", pdu);
    return strlen(pdu);
}


bool SMS_Format::isMT(void)
{
    return mPDUTpe.tpMTI == 1;
}

bool SMS_Format::isVPFormat(void)
{
    return strlen(mVPFormat) > 0;
}

