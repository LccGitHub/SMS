/*************************************************************************
  > File Name: SmsFormat.h
  > Author: ma6174
  > Mail: ma6174@163.com 
  > Created Time: Sat 29 Sep 2018 01:50:15 PM CST
 ************************************************************************/
#ifndef __SMS_FORMAT
#define __SMS_FORMAT
#include<stdio.h>
#include<stdlib.h>
#include<string.h>


/*sub max bytes*/
#define SCA_MAX_BYTE 12
#define PDU_TYPE_BYTE 1
#define MR_MAX_BYTE 1
#define OA_MAX_BYTE 12
#define DA_MAX_BYTE 12
#define PID_MAX_BYTE 1
#define DCS_MAX_BYTE 1
#define SCTS_MAX_BYTE 7
#define VP_MAX_BYTE 7
#define UDL_MAX_BYTE 1
#define UD_MAX_BYTE 140
#define BTYE1 1
#define BTYE2 2

/*some contants*/
#define TYPE_INTERNATIONAL "91"
#define TYPE_DOMESTIC "81"

#define DCS_GSM7 "00"
#define DCS_GSM8 "F6"
#define DCS_UCS2 "08"

struct NumAddr{
    int len;
    char type[BTYE2*2+1];
    char addr[DA_MAX_BYTE*2+1];
};

struct PDUType{
    int tpRP;
    int tpUDHI;
    int tpSRR;
    int tpVPF;
    int tpRD;
    int tpMTI;
};
enum DCS{
    GSM7Bit,
    GSM8Bit,
    USC2,
    UNKNOWN
};

struct SMS{
    char num[128];
    unsigned char context[140];
    char scaNum[128];
};
class SMS_Format{
    public:
        SMS_Format(const char* pdu);
        int decodeSCA(char* temp);
        int decodePDUType(char* temp);
        int decodeMR(char* temp);
        int decodeOA(char* temp);
        int decodeDA(char* temp);
        int decodePID(char* temp);
        int decodeDCS(char* temp);
        int decodeVP(char* temp);
        int decodeSCTS(char* temp);
        int decodeUDL(char* temp);
        int decodeUD(char* temp);

        static int smsGSmEncode(const SMS sendSms, char* pdu, size_t pddLen);

    public:
        PDUType mPDUTpe;
        struct NumAddr mSCA;
        struct NumAddr mOA;
        struct NumAddr mDA;
        int mMR;
        int mPID;
        int mDCS;
        int mVP;
        char mVPFormat[VP_MAX_BYTE*2+1];
        char mSCTS[SCTS_MAX_BYTE*2+1];
        int mUDL;
        int mUDH;
        unsigned char mUD[UD_MAX_BYTE*2+1];
        char mSMSContext[UD_MAX_BYTE*4+1];
};




#endif
