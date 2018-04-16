#include "common.h"

void Lock(CRITICAL_SECTION *l)
{
    pthread_mutex_lock(l);
}
void Unlock(CRITICAL_SECTION *l)
{
    pthread_mutex_unlock(l);
}

void Deal_Log_Bak(Uchar gunno)
{
    char    file_name[100];
    sprintf(file_name,"/JKJN/log/RunLogGun%d_%s%s.log",gunno,datestr,timestr);
    if(access(file_name,F_OK)==-1)  //文件不存在
    {
        if(rename(logfilename,file_name))      //将logfilename1名称改为file_name
        {
            remove(file_name);                  //如果更名不成功直接删除file_name
            rename(logfilename,file_name);     //然后再次更改名字
        }
    }
    system("deallogback");//保留40个文件 
}
void LogV(Uchar gunno,const char *pszFmt,va_list argp)
{   
    FILE    *flog;
    ftime(&tb);
    now=localtime(&tb.time);
    if(NULL==pszFmt||0==pszFmt[0]) return;
    sprintf(logfilename,"/JKJN/RunLogGun%d.log",gunno);
    _vsnprintf(logstr,MAXLINSIZE,pszFmt,argp);
    sprintf(datestr,"%04d-%02d-%02d",now->tm_year+1900,now->tm_mon+1,now->tm_mday);
    sprintf(timestr,"%02d:%02d:%02d",now->tm_hour     ,now->tm_min  ,now->tm_sec);
    sprintf(mss,"%03d",tb.millitm);
    flog=fopen(logfilename,"a");
    if(NULL!=flog)
    {
        fprintf(flog,"%s %s.%s %s",datestr,timestr,mss,logstr);        
        if(ftell(flog)>MAXLOGSIZE)
        {
            fclose(flog);
            Deal_Log_Bak(gunno);
        }       
        else
            fclose(flog);
    }
}

void P_Log(Uchar gunno,const char *pszFmt,...)
{
    va_list argp;
    Lock(&cs_log);    
    va_start(argp,pszFmt);
    LogV(gunno,pszFmt,argp);
    va_end(argp);
    Unlock(&cs_log);
}
