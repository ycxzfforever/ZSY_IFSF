#include "common.h"

#define CLOCKID CLOCK_REALTIME


void ShutDownServer()
{
    FS_OK=0;
    Cli_Connect_FS=0;
    Ser_Connect_FS=0;
    Reconnect_EPS_Flag=0;
    Reconnect_FS_Flag=0;
    Disconnect_EPS_Flag=1;
    if(Cli_FS_fd>0)
    {
        close(Cli_FS_fd);
        Cli_FS_fd=-1;
    }
    if(Ser_FS_fd>0)
    {
        close(Ser_FS_fd);
        Ser_FS_fd=-1;
    }
    if(Cli_EPS_fd>0)
    {
        close(Cli_EPS_fd);
        Cli_EPS_fd=-1;
    }
}

//eps标志处理
void EpsFlagDispose()
{
    if(FaQiYanKaFlag==1)//处理发起验卡的时间，若时间过长，则还是将标志清零
    {
        FaQiYanKaTime++;
        if(FaQiYanKaTime>YANKATIMEOUT)
        {
            FaQiYanKaFlag=0;
            FaQiYanKaTime=0;
        }
    }
    
    if(YanKaFlag==1)//处理验卡的时间，若时间过长，则还是将标志清零
    {
        YanKaCount++;
        if(YanKaCount>10)
        {
            YanKaFlag=0;
            YanKaCount=0;
        }
    }
    else    YanKaCount=0;
    
    if(HuiSuoFlag==1)//处理灰锁的时间，若时间过长，则还是将标志清零
    {
        HuiSuoCount++;
        if(HuiSuoCount>10)
        {
            HuiSuoFlag=0;
            HuiSuoCount=0;
        }
    }
    else  HuiSuoCount=0;

    if(RequestPayFlag==1)//处理申请支付的时间，若时间过长，则还是将标志清零
    {
        RequestPayCount++;
        if(RequestPayCount>10)
        {
            RequestPayFlag=0;
            RequestPayCount=0;
        }
    }
    else    RequestPayCount=0;
    
    if(OverTransFlag==1)//处理通知交易的时间，若时间过长，则还是将标志清零
    {
        OverTransCount++;
        if(OverTransCount>10)
        {
            OverTransFlag=0;
            OverTransCount=0;
        }
    }
    else    OverTransCount=0;

    //处理EPS套接字异常情况
    //由于EPS服务器在发送完数据后都会主动关闭socket，
    //所以当由于异常情况没接收到eps关闭socket命令的时候，处理下
    if(Cli_EPS_fd>0)
    {
        EpsfdErrorCount++;
        if(EpsfdErrorCount>20) Cli_EPS_fd = -1;
    }
    else
        EpsfdErrorCount=0;
}

//只需第一条枪处理的数据
void OnlyGunOneDispose()
{
    Uchar i;
    char str[10],tmp[9];
    if(shared->FtoUflag==1) shared->errcount++;
    if(shared->errcount > 5)//只有第一个进程计数 ，异常情况，必须清标记
    {
        shared->FtoUflag=0;
        shared->errcount=0;
    } 
    
    if((FSbacknum1>0)&&(shared->FtoUflag==0))//处理FS下发的未来得及处理的数据
    {
        memset(shared->FS_To_Uart,0,LENGTH);
        memcpy(shared->FS_To_Uart,FSbackbuf1,FSbacknum1);                    
        shared->FtoUflag=1;
        shared->FtoUnum=FSbacknum1; 
        FSbacknum1=0;
    }
    else if((FSbacknum2>0)&&(shared->FtoUflag==0))//处理FS下发的未来得及处理的数据
    {
        memset(shared->FS_To_Uart,0,LENGTH);
        memcpy(shared->FS_To_Uart,FSbackbuf2,FSbacknum2);                    
        shared->FtoUflag=1;
        shared->FtoUnum=FSbacknum2; 
        FSbacknum2=0;
    }
    
    for(i=0;i<4;i++)
    {
        if(shared->pricever[i] != 0)//更新配置文件中的单价记录
        {                           
            sprintf(str,"Gun_Info%d",i+1);
            P_Log(conf.Gun_num,"===========%s update price===========\n",str);
            iniSetInt((const char*)str, "Pri_Ver", shared->pricever[i], 10);
            iniSetInt((const char*)str, "price", shared->price[i], 10);
            shared->pricever[i]=0;
        }
        if(shared->oilver[i] != 0)//更新配置文件中的油品代码
        {            
            sprintf(str,"Gun_Info%d",i+1);            
            P_Log(conf.Gun_num,"===========%s update oilcode===========\n",str);
            sprintf(tmp,"%02X%02X%02X%02X",shared->oilcode[i][0],
                shared->oilcode[i][1],shared->oilcode[i][2],shared->oilcode[i][3]);
            iniSetString((const char*)str, "oilcode", (const char *)tmp);
            shared->oilver[i]=0;
        }
    }        
}


/********************************************************************\
* 函数名: timer_thread
* 说明:
* 功能:    定时器函数
* 输入:
* 返回值:   无
* 创建人:   Yang Chao Xu
* 创建时间: 2014-9-12
\*********************************************************************/
void timer_thread(union sigval v)
{
    static Uchar st=3;
    Time_100ms++;
    F0_Time++;
    if(Gun_num==1)//只需第一条枪处理的数据
    {
        OnlyGunOneDispose();
    }
    if(F0_Time%5==0) LedOnOff(1);
    
    if(((Time_S%conf.Time_Reconnect)==0)&&(Cli_Connect_FS==0)&&(FS_OK==1)&&(Reconnect_FS_Flag==0))
    {
        Reconnect_FS_Flag=1;
    }
    if(Time_100ms%10==0)
    {
        Time_S++;
        Check_Time++;
        udptimeoutcount++;
        if((uploadtransagain==1)&&(Stop_Opt==0))
        {
            uploadtransagaincount++;
            if((uploadtransagaincount%16==0)&&(uploadtransagaincount<50)) //最多传3次
            {
                para = paraonline;
                Reply_FS_UploadOverFuel(); // 16秒未收到清交易，则再次上传交易记录
            }
        }
        if(shared->TCPOK==0) FS_OK = 0;//TCP服务器断线
        if(udptimeoutcount>30)//距离上次udp发送时间超多30s，则Shutdown Server
        {   
            FS_OK=0;
            ReadNoPayFlag=0;
            P_Log(conf.Gun_num,"===============Shutdown Server!=============\n");
            if(Gun_num!=1)  ShutDownServer();
        }       
        EpsFlagDispose();   //eps标志处理
        
        if(StartFuelling==9)
        {
            StartFuelling=0;
            st=3;//防止加完气后马上是3状态，造成不传流水，而主动上传了3状态
        }
        if((para.ST!=8)&&(st!=para.ST)&&(para.ST!=0)&&(StartFuelling==0)) //状态改变，主动上传
        {
            st=para.ST;
            Reply_FS_UploadStatus(para.ST);
        }
        else if(statusflag>0)
        {
            Reply_FS_UploadStatus(statusflag);
            statusflag=0;
        }
  
        if(Check_Netlink("eth0") == -1) //网络未连接好
        {
            Stop_Opt=1;
            if(Check_Time>30)   ShutDownServer();
        }
        else //网络正常
        {
            Check_Time=0;
            Stop_Opt=0;
            if((Reconnect_FS_Flag==1)&&(Ser_Connect_FS==1)&&(Cli_Connect_FS==0)&&(Cli_FS_fd<1))
            {
                FS_Tcp_Client_Creat();
            }
        } 
    }
    if(F0_Time%8==0)
    {
        LedOnOff(0);
        Lock(&Data_To_Uart);
        switch(CmdFlag)
        {
            case 0x52:
                FCS_To_JKOPT_52();
                break;
            case 0x57:
                FCS_To_JKOPT_57();
                break;
            case 0x58:
                FCS_To_JKOPT_58();
                break;
            case 0x5E:
                FCS_To_JKOPT_5E();
                break;
            case 0x70:
                FCS_To_JKOPT_70();
                break;
            case 0x71:
                FCS_To_JKOPT_71();
                break;
            case 0x72:
                FCS_To_JKOPT_72();
                break;
            case 0x73:
                FCS_To_JKOPT_73();
                break;
            default:
                FCS_To_JKOPT_51();
                break;
        }
        CmdFlag=0;
        Unlock(&Data_To_Uart);           
    }
    if((Reconnect_EPS_Flag==1)&&(Check_Netlink("eth0")==0))
    {
        EPS_Tcp_Client_Creat();
    }

    if(Time_S==8000)
    {
        Time_S=0;
    }
    if(Time_100ms==80000)
    {
        Time_100ms=0;
        F0_Time=1;
    }
}

void Creat_Timer()
{
    // XXX int timer_create(clockid_t clockid, struct sigevent *evp, timer_t *timerid);
    // clockid--值：CLOCK_REALTIME,CLOCK_MONOTONIC，CLOCK_PROCESS_CPUTIME_ID,CLOCK_THREAD_CPUTIME_ID
    // evp--存放环境值的地址,结构成员说明了定时器到期的通知方式和处理方式等
    // timerid--定时器标识符
    timer_t timerid;
    struct sigevent evp;
    memset(&evp, 0, sizeof(struct sigevent));   //清零初始化

    evp.sigev_value.sival_int = 111;            //也是标识定时器的，这和timerid有什么区别？回调函数可以获得
    evp.sigev_notify = SIGEV_THREAD;            //线程通知的方式，派驻新线程
    evp.sigev_notify_function = timer_thread;   //线程函数地址

    if(timer_create(CLOCKID, &evp, &timerid) == -1)
    {
        perror("fail to timer_create");
        exit(-1);
    }

    // XXX int timer_settime(timer_t timerid, int flags, const struct itimerspec *new_value,struct itimerspec *old_value);
    // timerid--定时器标识
    // flags--0表示相对时间，1表示绝对时间
    // new_value--定时器的新初始值和间隔，如下面的it
    // old_value--取值通常为0，即第四个参数常为NULL,若不为NULL，则返回定时器的前一个值

    //第一次间隔it.it_value这么长,以后每次都是it.it_interval这么长,就是说it.it_value变0的时候会装载it.it_interval的值
    struct itimerspec it;
    it.it_interval.tv_sec = 0;
    it.it_interval.tv_nsec = 100000000;//100ms
    it.it_value.tv_sec = 1;
    it.it_value.tv_nsec = 0;

    if(timer_settime(timerid, 0, &it, NULL) == -1)
    {
        perror("fail to timer_settime");
        exit(-1);
    }
    pause();
}
/*
 * int timer_gettime(timer_t timerid, struct itimerspec *curr_value);
 * 获取timerid指定的定时器的值，填入curr_value
 *
 */
