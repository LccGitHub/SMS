#include <stdbool.h>
#include "SmsFormat.h"
#include "GsmSMSUicode.h"





int smsGSmEncode(const SMS sendSms, char* pdu, size_t pddLen)
{
    return SMS_Format::smsGSmEncode(sendSms, pdu, pddLen);
}

int smsGSmDecode(const char* pdu)
{
    int res = 0;
    SMS_Format smsFormat(pdu);
    if (smsFormat.isMT()) {
        printf("this is MT SMS\n");
        if (smsFormat.isVPFormat()) {
            printf("reciever[%s], contex[%s], vaild time[%s]\n", smsFormat.mDA.addr, smsFormat.mSMSContext,
                smsFormat.mVPFormat);
        }
        else {
            printf("reciever[%s], contex[%s], vaild time[%d] min\n", smsFormat.mDA.addr, smsFormat.mSMSContext,
                smsFormat.mVP);
        }
    }
    else {
        printf("this is MO SMS\n");
        printf("sender[%s], contex[%s], SMSC time[%s] min\n", smsFormat.mOA.addr, smsFormat.mSMSContext,
                smsFormat.mSCTS);
    }
    return res;
}

int main(int argc, char* argv[])
{
    bool loop = true;
    char buff[1024];
    int opt;
    while(loop) {
        memset(buff, 0, sizeof(buff));
        printf("\nplease input opt:\n 0(decode);\n 1(encode);\n 2(exit)\n\n");
        scanf("%d", &opt);
        if (opt == 0) {
            printf("please input decode pud or exit\n");
            scanf("%s", (char*)buff);
            smsGSmDecode(buff);
        }
        else if (opt == 1) {
            char pdu[512] = {0};
            SMS sendSms;
            memset(&sendSms, 0, sizeof(sendSms));
            printf("please input number\n");
            scanf("%s", (char*)sendSms.num);
            printf("please input sms context\n");
            scanf("%s", (char*)sendSms.context);
            printf("please input sms code type\n");
            scanf("%d", (int*)(&sendSms.codeType));
            smsGSmEncode(sendSms,pdu, sizeof(pdu));
        }
        else if (opt == 2) {
            loop = false;
        }
        else {
            printf("unkown opt, please reinput\n");
        }
    }
    printf("exit maminn");
    return 0;
}
