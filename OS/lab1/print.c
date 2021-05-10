/*
 * Copyright (C) 2001 MontaVista Software Inc.
 * Author: Jun Sun, jsun@mvista.com or jsun@junsun.net
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 *
 * refer to print.c.diff for modification
 */
//#define S_DEBUG
#include	<print.h>
#ifdef S_DEBUG
	// reduce LP_MAX_BUF limitation to 25 to debug buffer overflow
	#undef LP_MAX_BUF
	#define LP_MAX_BUF 100
#endif

/* macros */
#define		IsDigit(x)	( ((x) >= '0') && ((x) <= '9') )
#define		Ctod(x)		( (x) - '0')

/* forward declaration */
extern int PrintChar(char *, char, int, int);
extern int PrintString(char *, char *, int, int);
extern int PrintNum(char *, unsigned long, int, int, int, int, char, int);

// helpers
void localPrintString(void (*)(void*, char*, int), void*, char*, char*, int, int);
int isSpecifier(char c){
	switch(c){
		case 'b':
		case 'd':
		case 'D':
                case 'o':
                case 'O':
                case 'u':
                case 'U':
                case 'x':
                case 'X':
                case 'c':
                case 's':
			return 1;
	}
	return 0;
}
int sreadInt(char** s){
	int result = 0;
	while(IsDigit(**s)){
		result = (result << 3) + (result << 1)	;
		result += Ctod(**s);
		++(*s);
	}
	return result;
}

/* private variable */
static const char theFatalMsg[] = "fatal error in lp_Print!";

/* -*-
 * A low level printf() function.
 */
void
lp_Print(void (*output)(void *, char *, int),
	 void * arg,
	 char *fmt,
	 va_list ap)
{

#define 	OUTPUT(arg, s, l)  \
  { if (((l) < 0) || ((l) > LP_MAX_BUF)) { \
       (*output)(arg, (char*)theFatalMsg, sizeof(theFatalMsg)-1); for(;;); \
    } else { \
      (*output)(arg, s, l); \
    } \
  }
    char buf[LP_MAX_BUF];

    char c;
    char *s;
    long int num;

    int longFlag;
    int negFlag;
    int width;
    int prec;
    int ladjust;
    char padc;

    int length;

    /*
        Exercise 1.4. Please fill in two parts in this file.
    */
	// define the state of prefix recognization
	enum PrefixState{
		PS_FLAG,
		PS_WIDTH,
		PS_PRECISION,
		PS_LENGTH
	};
	int prefix_state;

    for(;;) {

        /* Part1: your code here */
	{
	    /* scan for the next '%' */
		while(1){
			for(length = 0; *fmt != '%' && *fmt != '\0'; ++fmt, ++length){
				// prevent buffer overflow
				if(length == LP_MAX_BUF) break;
				buf[length] = *fmt;
			}
			// flush buffer
			OUTPUT(arg, buf, length);
			// jump out iff a '%' or a '\0' is encountered
			// do not return since special calls shall be done before the return
			if(*fmt == '%' || *fmt == '\0') break;
		}
	    /* check "are we hitting the end?" */
		if(*fmt == '\0') break;	// YES!
	}
	/* we found a '%' */
	++fmt;	// we need to see into the future
	// clear flags
	longFlag = 0;
	negFlag = 0;
	width = 0;	// use 0 for no padding
	prec = 0;	// well we just don't care about this. POOR prec.
	ladjust = 0;
	padc = ' ';
	prefix_state = PS_FLAG;
	while(!isSpecifier(*fmt)){
		switch(prefix_state){
			case PS_FLAG:
				if(*fmt == '0'){
					padc = '0';
					break;
				}else if(*fmt == '-'){
					ladjust = 1;
					break;
				}
				prefix_state = PS_WIDTH;
			case PS_WIDTH:
				if(IsDigit(*fmt)){
					width = sreadInt(&fmt);
				}
				prefix_state = PS_PRECISION;
			case PS_PRECISION:
				if(*fmt == '.'){
					++fmt;
					prec = sreadInt(&fmt);
				}
				prefix_state = PS_LENGTH;
			case PS_LENGTH:
				if(*fmt == 'l'){
					longFlag = 1;
				}else{
					--fmt;
				}
			default:	// do not exist
				break;
		}
		++fmt;	// get next character
	}

	switch (*fmt) {
	 case 'b':
	    if (longFlag) {
		num = va_arg(ap, long int);
	    } else {
		num = va_arg(ap, int);
	    }
		#ifdef S_DEBUG
			printf("u: num=%ld width=%d ladjust=%d padc=%c\n", (long int)num, width, ladjust, padc);
		#endif
	    length = PrintNum(buf, num, 2, 0, width, ladjust, padc, 0);
	    OUTPUT(arg, buf, length);
	    break;

	 case 'd':
	 case 'D':
	    if (longFlag) {
		num = va_arg(ap, long int);
	    } else {
		num = va_arg(ap, int);
	    }
		/*  Part2:
			your code here.
			Refer to other part (case 'b',case 'o' etc.) and func PrintNum to complete this part.
			Think the difference between case 'd' and others. (hint: negFlag).
		*/
		if(num < 0){
			negFlag = 1;
			num = -num;
		}
		length = PrintNum(buf, num, 10, negFlag, width, ladjust, padc, 0);
		OUTPUT(arg, buf, length);
		break;

	 case 'o':
	 case 'O':
	    if (longFlag) {
		num = va_arg(ap, long int);
	    } else {
		num = va_arg(ap, int);
	    }
	    length = PrintNum(buf, num, 8, 0, width, ladjust, padc, 0);
	    OUTPUT(arg, buf, length);
	    break;

	 case 'u':
	 case 'U':
	    if (longFlag) {
		num = va_arg(ap, long int);
	    } else {
		num = va_arg(ap, int);
	    }
		#ifdef S_DEBUG
			printf("u: num=%ld width=%d ladjust=%d padc=%c\n", (long int)num, width, ladjust, padc);
		#endif
	    length = PrintNum(buf, num, 10, 0, width, ladjust, padc, 0);
	    OUTPUT(arg, buf, length);
	    break;
	 case 'x':
	    if (longFlag) {
		num = va_arg(ap, long int);
	    } else {
		num = va_arg(ap, int);
	    }
		#ifdef S_DEBUG
			printf("x: num=%ld width=%d ladjust=%d padc=%c\n", (long int)num, width, ladjust, padc);
		#endif
	    length = PrintNum(buf, num, 16, 0, width, ladjust, padc, 0);
	    OUTPUT(arg, buf, length);
	    break;

	 case 'X':
	    if (longFlag) {
		num = va_arg(ap, long int);
	    } else {
		num = va_arg(ap, int);
	    }
	    length = PrintNum(buf, num, 16, 0, width, ladjust, padc, 1);
	    OUTPUT(arg, buf, length);
	    break;

	 case 'c':
	    c = (char)va_arg(ap, int);
	    length = PrintChar(buf, c, width, ladjust);
	    OUTPUT(arg, buf, length);
	    break;

	 case 's':
	    s = (char*)va_arg(ap, char *);
	    localPrintString(output, arg, buf, s, width, ladjust);
	    break;

	 case '\0':
	    fmt--;
	    break;

	 default:
	    /* output this char as it is */
	    OUTPUT(arg, fmt, 1);
	}	/* switch (*fmt) */

	fmt ++;
    }		/* for(;;) */

    /* special termination call */
    OUTPUT(arg, "\0", 1);
}


/* --------------- local help functions --------------------- */
int
PrintChar(char * buf, char c, int length, int ladjust)
{
    int i;
    if (length < 1) length = 1;
    if (ladjust) {
	*buf = c;
	for (i=1; i< length; i++) buf[i] = ' ';
    } else {
	for (i=0; i< length-1; i++) buf[i] = ' ';
	buf[length - 1] = c;
    }
    return length;
}

enum PrintStringStatus{
	PSS_STRING,
	PSS_SPACE
};
void localPrintString(void (*output)(void *, char *, int), void* arg, char* buf, char* s, int length, int ladjust){
	#ifdef S_DEBUG
		printf("localPrintString length=%d ladjust=%d\n", length, ladjust);
	#endif
	int i;
	int len=0;
	char* s1 = s;
	while (*s1++) len++;
	if (length < len) length = len;
	int space_count = length - len;
	int pss_status = ladjust ? PSS_STRING : PSS_SPACE;
	s1 = s;
	int buffer_i = 0;
	#ifdef S_DEBUG
		printf("now length=%d\n", length);
	#endif
	for(i = 0; i < length; ++i){
		switch(pss_status){
			case PSS_STRING:
				if(s1 == '\0'){
					pss_status = PSS_SPACE;
					--i;
				}else{
					buf[buffer_i++] = *s1;
					#ifdef S_DEBUG
						printf("add %c at %d\n", *s1, buffer_i - 1);
					#endif
					++s1;
				}
				break;
			case PSS_SPACE:
				if(space_count == 0){
					pss_status = PSS_STRING;
					--i;
				}else{
					buf[buffer_i++] = ' ';
					--space_count;
				}
				break;
			default:	// impossible
				break;
		}
		if(buffer_i == LP_MAX_BUF){
			#ifdef S_DEBUG
				printf("buffer flush\n");
			#endif
			OUTPUT(arg, buf, LP_MAX_BUF);
			buffer_i = 0;
		}
	}
	if(buffer_i){
		#ifdef S_DEBUG
			printf("last part\n");
		#endif
		OUTPUT(arg, buf, buffer_i);
	}
}

int
PrintString(char * buf, char* s, int length, int ladjust)
{
    int i;
    int len=0;
    char* s1 = s;
    while (*s1++) len++;
    if (length < len) length = len;

    if (ladjust) {
	for (i=0; i< len; i++) buf[i] = s[i];
	for (i=len; i< length; i++) buf[i] = ' ';
    } else {
	for (i=0; i< length-len; i++) buf[i] = ' ';
	for (i=length-len; i < length; i++) buf[i] = s[i-length+len];
    }
    return length;
}

int
PrintNum(char * buf, unsigned long u, int base, int negFlag,
	 int length, int ladjust, char padc, int upcase)
{
    /* algorithm :
     *  1. prints the number from left to right in reverse form.
     *  2. fill the remaining spaces with padc if length is longer than
     *     the actual length
     *     TRICKY : if left adjusted, no "0" padding.
     *		    if negtive, insert  "0" padding between "0" and number.
     *  3. if (!ladjust) we reverse the whole string including paddings
     *  4. otherwise we only reverse the actual string representing the num.
     */

    int actualLength =0;
    char *p = buf;
    int i;

    do {
	int tmp = u %base;
	if (tmp <= 9) {
	    *p++ = '0' + tmp;
	} else if (upcase) {
	    *p++ = 'A' + tmp - 10;
	} else {
	    *p++ = 'a' + tmp - 10;
	}
	u /= base;
    } while (u != 0);

    if (negFlag) {
	*p++ = '-';
    }

    /* figure out actual length and adjust the maximum length */
    actualLength = p - buf;
    if (length < actualLength) length = actualLength;

    /* add padding */
    if (ladjust) {
	padc = ' ';
    }
    if (negFlag && !ladjust && (padc == '0')) {
	for (i = actualLength-1; i< length-1; i++) buf[i] = padc;
	buf[length -1] = '-';
    } else {
	for (i = actualLength; i< length; i++) buf[i] = padc;
    }

    /* prepare to reverse the string */
    {
	int begin = 0;
	int end;
	if (ladjust) {
	    end = actualLength - 1;
	} else {
	    end = length -1;
	}

	while (end > begin) {
	    char tmp = buf[begin];
	    buf[begin] = buf[end];
	    buf[end] = tmp;
	    begin ++;
	    end --;
	}
    }

    /* adjust the string pointer */
    return length;
}
