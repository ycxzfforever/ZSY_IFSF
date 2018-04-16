#ifndef _LOG_H
#define _LOG_H


#define     CRITICAL_SECTION    pthread_mutex_t
#define     _vsnprintf          vsnprintf
#define     MAXLOGSIZE          4*1024*1024     //log¥Û–°‘º4M
#define     MAXLINSIZE          1024

char    logfilename[30];
char    logstr[MAXLINSIZE+1];
char    datestr[16];
char    timestr[16];
char    mss[4]; 

void P_Log(Uchar gunno,const char *pszFmt,...) ;
void Lock(CRITICAL_SECTION *l);
void Unlock(CRITICAL_SECTION *l) ;
void Deal_Log_Bak(Uchar gunno);
void LogV(Uchar gunno,const char *pszFmt,va_list argp) ;



#endif

