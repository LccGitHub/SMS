/*************************************************************************
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#include<stdint.h>


#define MAX_CHARACTER_SIZE 8

unsigned char* UnicodeToUTF8(int unicode, unsigned char *p);
int UTF8ToUnicode (unsigned char *ch, int *unicode);
unsigned char* UnicodeStrToUTF8Str (unsigned short *unicode_str, unsigned char *utf8_str, int utf8_str_size);
int UTF8StrToUnicodeStr(unsigned char *utf8_str, uint16_t *unicode_str, int unicode_str_size);

unsigned char* UnicodeToUTF8(int unicode, unsigned char *p)
{
    unsigned char *e = NULL;
    e = p;
    if (e) {
        if (unicode < 0x80) {
            *e++ = unicode;
        }
        else if (unicode < 0x800) {
            *e++ = ((unicode >> 6) & 0x1f) | 0xc0;
            *e++ = (unicode & 0x3f) | 0x80;
        }
        else if (unicode < 0x10000) {
            *e++ = ((unicode >> 12) & 0x0f) | 0xe0;
            *e++ = ((unicode >> 6) & 0x3f) | 0x80;
            *e++ = (unicode & 0x3f) | 0x80;
        }
        else if (unicode < 0x200000) {
            *e++ = ((unicode >> 18) & 0x07) | 0xf0;
            *e++ = ((unicode >> 12) & 0x3f) | 0x80;
            *e++ = ((unicode >> 6) & 0x3f) | 0x80;
            *e++ = (unicode & 0x3f) | 0x80;
        }
        else if (unicode < 0x4000000) {
            *e++ = ((unicode >> 24) & 0x03) | 0xf8;
            *e++ = ((unicode >> 18) & 0x3f) | 0x80;
            *e++ = ((unicode >> 12) & 0x3f) | 0x80;
            *e++ = ((unicode >> 6) & 0x3f) | 0x80;
            *e++ = (unicode & 0x3f) | 0x80;
        }
        else {
            *e++ = ((unicode >> 30) & 0x01) | 0xfc;
            *e++ = ((unicode >> 24) & 0x3f) | 0x80;
            *e++ = ((unicode >> 18) & 0x3f) | 0x80;
            *e++ = ((unicode >> 12) & 0x3f) | 0x80;
            *e++ = ((unicode >> 6) & 0x3f) | 0x80;
            *e++ = (unicode & 0x3f) | 0x80;
        }
    }

    return e;
}

int UTF8ToUnicode (unsigned char *ch, int *unicode)
{
    unsigned char *p = NULL;
    int n = 0;
    p = ch;
    if ((p != NULL) && unicode) {
        int e = 0;
        if (*p >= 0xfc) {
            e = (p[0] & 0x01) << 30;
            e |= (p[1] & 0x3f) << 24;
            e |= (p[2] & 0x3f) << 18;
            e |= (p[3] & 0x3f) << 12;
            e |= (p[4] & 0x3f) << 6;
            e |= (p[5] & 0x3f);
            n = 6;
        }
        else if (*p >= 0xf8) {
            e = (p[0] & 0x03) << 24;
            e |= (p[1] & 0x3f) << 18;
            e |= (p[2] & 0x3f) << 12;
            e |= (p[3] & 0x3f) << 6;
            e |= (p[4] & 0x3f);
            n = 5;
        }
        else if (*p >= 0xf0) {
            e = (p[0] & 0x07) << 18;
            e |= (p[1] & 0x3f) << 12;
            e |= (p[2] & 0x3f) << 6;
            e |= (p[3] & 0x3f);
            n = 4;
        }
        else if (*p >= 0xe0) {
            e = (p[0] & 0x0f) << 12;
            e |= (p[1] & 0x3f) << 6;
            e |= (p[2] & 0x3f);
            n = 3;
        }
        else if (*p >= 0xc0) {
            e = (p[0] & 0x1f) << 6;
            e |= (p[1] & 0x3f);
            n = 2;
        }
        else {
            e = p[0];
            n = 1;
        }
        *unicode = e;
    }

    return n;
}

unsigned char* UnicodeStrToUTF8Str (unsigned short *unicode_str, unsigned char *utf8_str, int utf8_str_size)
{
    int unicode = 0;
    unicode; /* fix cppcheck */
    unsigned char *s = NULL;

    s = utf8_str;
    if ((unicode_str) && (s)) {
        unicode = (int) (*unicode_str);
        while (unicode) {
            unsigned char *e = NULL;
            unsigned char utf8_ch[MAX_CHARACTER_SIZE] = { 0 };

#if 1 // Little endian
                unicode = ((unicode&0xFF)<<8) | ((unicode&0xFF00) >> 8);  // error
#else // Big enddian
                unicode = unicode;
#endif
            memset (utf8_ch, 0, sizeof (utf8_ch));
            utf8_ch; /* fix cppcheck */

            e = UnicodeToUTF8 (unicode, utf8_ch);
            if (e > utf8_ch) {
                *e = '\0';

                if ((s - utf8_str + strlen ((const char *) utf8_ch)) >= (unsigned int)utf8_str_size) {
                    return s;
                }
                else {
                    memcpy (s, utf8_ch, strlen ((const char *) utf8_ch));
                    s += strlen ((const char *) utf8_ch);
                    *s = '\0';
                }
            }
            else {
                return s;
            }

            unicode_str++;
            unicode = (int) (*unicode_str);
        }
    }

    return s;
}


int UTF8StrToUnicodeStr(unsigned char *utf8_str, uint16_t *unicode_str, int unicode_str_size)
{
    int unicode = 0;
    int count = 0;
    unsigned char *s = NULL;
    uint16_t *e = NULL;

    s = utf8_str;
    e = unicode_str;

    if ((utf8_str) && (unicode_str)) {
        while (*s) {
            int n = 0;
            n; /* fix cppcheck */
            n = UTF8ToUnicode (s, &unicode);
            if (n > 0) {
                if (count >= unicode_str_size)  {
                    return count;
                }
                else {

#if 0 // Little endian
                    *e = (unsigned short) unicode;
#else // Big endian
                    *e = ((unicode&0xFF)<<8) | ((unicode&0xFF00) >> 8);
#endif
                    e++;
                    *e = 0;
                    s += n;
                    count++;
                }
            }
            else {
                return count;
            }
        }
    }

    return count;
}

int main(void)
{
    unsigned char temp[100] = "你好";
    unsigned char utf8[200] = {0};
    unsigned char uicodestr[200]  = { 0 };
    int i = 0;

    int len = UTF8StrToUnicodeStr(temp, (uint16_t*)uicodestr, strlen(temp));
    
    printf("raw UTF-16 len=%d, data\n", len*2);
    for(i = 0; i < len*2; i++) {
        printf("%.2X ", uicodestr[i]);
    }
    printf("\n");
    
    UnicodeStrToUTF8Str (uicodestr, utf8, 200);
    printf("utf8:%s\n", utf8);
    return 0;
}

