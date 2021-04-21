#include "logger.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <syslog.h>

#define RESET		0
#define BRIGHT 		1
#define DIM		    2
#define UNDERLINE 	3
#define BLINK		4
#define REVERSE		7
#define HIDDEN		8

#define BLACK 		0
#define RED		    1
#define GREEN		2
#define YELLOW		3
#define BLUE		4
#define MAGENTA		5
#define CYAN		6
#define	WHITE		7

char logTp = 0;  

void textcolor(int attr, int fg, int bg)
{	char command[13];
	sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
	printf("%s", command);
}

int initLogger(char *logType){
    if(strcmp(logType,"stdout")){
        logTp = 1;
        return logTp;
    }
    if(strcmp(logType,"syslog")){
        logTp = 0;
        return logTp;
    }
    return -1;
}

int warnf(const char *format, ...){
    va_list arg;
    va_start(arg, format);
    textcolor(BRIGHT, MAGENTA, BLACK);
    if(logTp){
        vsyslog(1, format, arg);
        return 1;
    }
    int output = vfprintf (stdout, format, arg);
    textcolor(RESET, WHITE, BLACK);	
    va_end(arg);
    return output;
}

int infof(const char *format, ...){
    va_list arg;
    va_start(arg, format);
    textcolor(BRIGHT, BLUE, BLACK);
    if(logTp){
        vsyslog(1, format, arg);
        return 1;
    }
    int output =  vfprintf (stdout, format, arg);
    textcolor(RESET, WHITE, BLACK);	
    va_end(arg);
    return output;
}

int errorf(const char *format, ...){
    va_list arg;
    va_start(arg, format);
    textcolor(BRIGHT, YELLOW, BLACK);
    if(logTp){
        vsyslog(1, format, arg);
        return 1;
    }
    int output = vfprintf (stdout, format, arg);
    textcolor(RESET, WHITE, BLACK);	
    va_end(arg);
    return output;
}
int panicf(const char *format, ...){
    va_list arg;
    va_start(arg, format);
    textcolor(BRIGHT, CYAN, BLACK);
    if(logTp){
        vsyslog(1, format, arg);
        return 1;
    }
	int output = vfprintf (stdout, format, arg);
    textcolor(RESET, WHITE, BLACK);	
    va_end(arg);
    return output;
}