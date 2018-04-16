#include "common.h"


void init_sys();
void init_Read_Conf();
void init_Uart();
void Print_Conf();
void Time_Task();
extern void Creat_Timer();


void Para_set()
{
    price.FIE_NU = 1;             //油价记录数
    para.NZN = conf.Gun_num;
    price.NZN = conf.Gun_num;
    price.PRC_N = 8;
    para.ST=3;
    para.T_TYPE=1;
    para.VOL=0;
    fp_id.Default_Fuelling_Mode=1;//默认模式为1
    ReadNoPayFlag=0;
    Cli_Connect_FS = 0;      //等于1时客户端连接上了FS服务器
    Ser_Connect_FS = 0;      //等于1时FS连接上了加气机服务器
    Is_Connect_EPS = 0;      //等于1时连接上了EPS
    FS_OK = 0;               //等于1时，FS通讯良好
    OPT_Server_OK = 0;       //等于1时，加气机server服务创建成功
    Uart_To_EPS_Flag = 0;    //等于1时uart数据开始向eps发送
    Uart_To_FS_Flag = 0;     //等于1时uart数据开始向fs发送
    EPS_To_Uart_Flag = 0;    //等于1时eps数据开始向uart发送
    FS_To_Uart_Flag = 0;     //等于1时fs数据开始向uart发送
    Udp_Send_Flag=0;
    Udp_fd=-1;
    Uart_fd=-1;
    Gpio_fd=-1;
    Cli_FS_fd=-1;
    Ser_FS_fd=-1;
    Cli_EPS_fd=-1;
    FS_Data_Len=0;
    EPS_Data_Len=0;

    Check_Time=0;            //检查网卡是否正常连接
    Time_ms = 0;             //1ms计数
    Time_S = 0;              //秒计数
    Time_100ms = 0;          //100ms计数器
    F0_Time = 0;             //等于1时轮训
    Disconnect_EPS_Flag = 0; //等于1时断开与EPS的连接
    Reconnect_FS_Flag = 0;   //等于1时重新连接FS服务器
    Reconnect_EPS_Flag = 0;  //等于1时重新连接EPS

    StartFuelling=0;
    OPT_IS_Online=conf.OPT_IS_Online;
    DownLoadPrice=0;
    HaveAuth=0;
    YanKaCount=0;
    HuiSuoCount=0;
    RequestPayCount=0;
    OverTransCount=0;
    BoartRequestFlag=0;
    FaQiYanKaFlag=0;
    FaQiYanKaTime=0;
}

void Print_Conf()
{
    P_Log(conf.Gun_num,"IFSF_Node=%s\n", conf.IFSF_Node);
    P_Log(conf.Gun_num,"FS_IP=%s\n", conf.FS_IP);
    P_Log(conf.Gun_num,"FS_Port=%d\n", conf.FS_Port);
    P_Log(conf.Gun_num,"FS_Udp_Port=%d\n", conf.FS_Udp_Port);
    P_Log(conf.Gun_num,"EPS_IP=%s\n", conf.EPS_IP);
    P_Log(conf.Gun_num,"EPS_Port=%d\n", conf.EPS_Port);    
    P_Log(conf.Gun_num,"Time_Out=%d\n", conf.Time_Out);
    P_Log(conf.Gun_num,"Time_Reconnect=%d\n", conf.Time_Reconnect);
    P_Log(conf.Gun_num,"Price_VER=%d\n", price.VER);
    P_Log(conf.Gun_num,"Price=%d\n", price.Price[0]);    
    
    P_Log(conf.Gun_num,"My_IP=%s\n", conf.My_IP);
    P_Log(conf.Gun_num,"My_EPS_Port=%d\n", conf.My_EPS_Port);
    P_Log(conf.Gun_num,"My_FS_Port=%d\n", conf.My_FS_Port);
    P_Log(conf.Gun_num,"My_Mask=%s\n", conf.My_Mask);
    P_Log(conf.Gun_num,"My_Gateway=%s\n", conf.My_Gateway);
    P_Log(conf.Gun_num,"My_Broadcast_IP=%s\n", conf.My_Broadcast_IP);
    P_Log(conf.Gun_num,"My_Broadcast_Port=%d\n", conf.My_Broadcast_Port);

    P_Log(conf.Gun_num,"Gun_num=%d\n", conf.Gun_num);
    P_Log(conf.Gun_num,"My_Node=%s\n", conf.My_Node);    
    P_Log(conf.Gun_num,"Uart_Speed=%d\n", conf.Uart_Speed);
    P_Log(conf.Gun_num,"Uart_Data_Bits=%d\n", conf.Uart_Data_Bits);
    P_Log(conf.Gun_num,"Uart_Stop_Bits=%d\n", conf.Uart_Stop_Bits);
    P_Log(conf.Gun_num,"Uart_Parity=%c\n", conf.Uart_Parity[0]);
}

void Timer_thread_Creat()
{
    int err;
    pthread_t timer1;
    err = pthread_create(&timer1, NULL, (void*)Creat_Timer, NULL); //创建定时器
    if(err != 0)
        P_Log(conf.Gun_num,"Can't create Creat_Timer thread: %s\n", strerror(err));
    else
        P_Log(conf.Gun_num,"Create Creat_Timer thread OK\n");
}

void init_Read_Conf()
{    
    iniFileLoad(inifile_Name);
    //FS_EPS相关信息    
    iniGetString("FS_EPS", "IFSF_Node", conf.IFSF_Node,sizeof(conf.IFSF_Node), "IFSF_Node fail");
    Str_Split(conf.IFSF_Node,&conf.FS_NODE[0],2);
    iniGetString("FS_EPS", "FS_IP", conf.FS_IP,sizeof(conf.FS_IP), "FS_IP fail");
    conf.FS_Port = iniGetInt("FS_EPS", "FS_Port", 5555);
    conf.FS_Udp_Port = iniGetInt("FS_EPS", "FS_Udp_Port", 3486);
    conf.My_FS_Port = iniGetInt("FS_EPS", "My_FS_Port", 18899);
    iniGetString("FS_EPS", "EPS_IP", conf.EPS_IP,sizeof(conf.EPS_IP), "EPS_IP fail");
    conf.EPS_Port = iniGetInt("FS_EPS", "EPS_Port", 20002);            
    conf.Time_Out = iniGetInt("FS_EPS", "Time_Out", 10);    
    conf.Time_Reconnect = iniGetInt("FS_EPS", "Time_Reconnect", 5);         
    conf.OPT_IS_Online = iniGetInt("FS_EPS", "OPT_IS_Online", 3); 
    
    //转接板信息
    iniGetString("Board_Info", "My_IP", conf.My_IP,sizeof(conf.My_IP), "My_IP fail");
    conf.My_EPS_Port = iniGetInt("Board_Info", "My_EPS_Port", 5000);    
    iniGetString("Board_Info", "My_Mask", conf.My_Mask,sizeof(conf.My_Mask), "My_Mask fail");
    iniGetString("Board_Info", "My_Gateway", conf.My_Gateway,sizeof(conf.My_Gateway), "My_Gateway fail");
    iniGetString("Board_Info", "My_Broadcast_IP", conf.My_Broadcast_IP,sizeof(conf.My_Broadcast_IP), "My_Broadcast_IP fail");
    conf.My_Broadcast_Port = iniGetInt("Board_Info", "My_Broadcast_Port", 3486);
    conf.Led_ON = iniGetInt("Board_Info", "Led_ON", 0);
    //加气枪信息
    switch(Gun_num)
    {
        case 1:
            secname="Gun_Info1";
        break;
        case 2:
            secname="Gun_Info2";
        break;
        case 3:
            secname="Gun_Info3";
        break;
        case 4:
            secname="Gun_Info4";
        break;
    }
    conf.Gun_num = iniGetInt(secname, "Gun_num", 255);
    //单价信息
    price.VER = iniGetInt(secname, "Pri_Ver", 0); 
    conf.Pri_Ver = price.VER;
    price.Price[0] = iniGetInt(secname, "price", 300);
    iniGetString(secname, "oilcode",(char *)conf.oilcode,sizeof(conf.oilcode), "00000003");
    AscToBcdBuf(conf.oilcode,price.O_TYPE,4);
    //fs相关    
    iniGetString(secname, "My_Node", conf.My_Node,sizeof(conf.My_Node), "My_Node fail");
    Str_Split(conf.My_Node,&conf.MY_NODE[0],2);
    //串口相关
    iniGetString(secname, "Uart_NO", conf.Uart_NO,sizeof(conf.Uart_NO), "Uart_NO fail");
    conf.Uart_Speed = iniGetInt(secname, "Uart_Speed", 38400);
    conf.Uart_Data_Bits = iniGetInt(secname, "Uart_Data_Bits", 8);
    conf.Uart_Stop_Bits = iniGetInt(secname, "Uart_Stop_Bits", 1);
    iniGetString(secname, "Uart_Parity", (char *)conf.Uart_Parity,sizeof(conf.Uart_Parity), "Uart_Parity fail");
}

void init_Uart()
{
    Open_Uart(conf.Uart_NO);
    set_speed(Uart_fd, conf.Uart_Speed); //设置串口波特率
    set_Parity(Uart_fd, conf.Uart_Data_Bits, conf.Uart_Stop_Bits, conf.Uart_Parity[0]); //设置8位数据位，1位停止位，无校验等其他设置。
    Uart_Pthread_Creat();//串口接收和发送线程创建
}

/********************************************************************\
* 函数名： FindIsHaveSubString
* 功能:    找到一个子串首次出现在源串时的数据下标值
* 输入:     sourceStr 源字符串，sourceLen源字符串长度
                    subStr子串，subLen子串长度
* 返回值:   如果有，返回子串首次出现在源串时的地址数组下标值，
                        没有，返回0xFF
* 创建人:   Yang Chao Xu
* 创建时间: 2017-11-22 09:29:22
\*********************************************************************/
Uint FindIsHaveSubString(Uchar *sourceStr,Uint sourceLen,Uchar *subStr,Uint subLen)
{    
    char *s1, *s2,flag=0;   
    Uint soulen=sourceLen,sblen=subLen;
    s1 = (char*)sourceStr;
    if(subStr==NULL)//子串为NULL
        return 0;
    while(soulen--)
    {
        s2 = (char *)subStr;
        sblen = subLen;
        flag = 0;
        while((!(*s1 - *s2))&&(sblen--))  //判断子串中的每个元素是否都与源串相等 
            s1++, s2++,flag=1;                   
        if(sblen==0)//找到了子串
            return sourceLen-soulen-1;              
        if(flag==0) //源串中有个别字符串与子串相等，已经加了，所以此处不再加
            s1++;
    }
    return 0xFF;
}

/********************************************************************\
* 函数名： IsAllCharacter
* 功能:   判断某个数组中是否全部包含某个元素
* 输入:     str:数组
                    c:查询的元素
                     len:数组长度
* 返回值:   0:不全是，1:全是
* 创建人:   Yang Chao Xu
* 创建时间: 2017-11-16 10:16:24
\*********************************************************************/
int IsAllCharacter(Uchar *str,Uchar c,int len)
{
    while(len--)
    {
        if(*str!=c) return 0;
        str++;
    }
    return 1;
}

/********************************************************************\
* 函数名： WriteDataToShm
* 功能: 向共享内存写入串口数据
* 输入:    无
* 返回值:   无
* 创建人:   Yang Chao Xu
* 创建时间: 2017-11-24 10:17:17
\*********************************************************************/
void WriteDataToShm()
{
    if(Uart_To_FS_Flag==1)//向共享内存写入串口数据
    {
        if(Gun_num==2)
        {
            memset(shared->Uart_To_FS2,0,LENGTH);
            memcpy(shared->Uart_To_FS2,Uart_To_FS,FS_Data_Len);
            shared->UtoFnum2=FS_Data_Len;
        }
        else if(Gun_num==3)
        {
            memset(shared->Uart_To_FS3,0,LENGTH);
            memcpy(shared->Uart_To_FS3,Uart_To_FS,FS_Data_Len);
            shared->UtoFnum3=FS_Data_Len;
        }
        else if(Gun_num==4)
        {
            memset(shared->Uart_To_FS4,0,LENGTH);
            memcpy(shared->Uart_To_FS4,Uart_To_FS,FS_Data_Len);
            shared->UtoFnum4=FS_Data_Len;
        }
        Uart_To_FS_Flag=0;
    } 
}

/********************************************************************\
* 函数名： ShmthreadCreat
* 功能:    共享内存读写线程
* 输入:     无
* 返回值:   无
* 创建人:   Yang Chao Xu
* 创建时间: 2017-11-16 10:16:24
\*********************************************************************/
void ShmthreadCreat()
{   
    Uint index,len; 
    Uchar substr[4]={conf.MY_NODE[0],conf.MY_NODE[1],conf.FS_NODE[0],conf.FS_NODE[1]};                        
    while(1)
    {
        /*=====start 从共享内存中读取信息,可能是多帧数据start=====*/
        if(shared->FtoUflag==1)
        {               
            index=FindIsHaveSubString(shared->FS_To_Uart,shared->FtoUnum,substr,4);
            if(index!=0xFF)//找到子串
            {
                Lock(&FS_Recv_Data);                
                memset(FS_To_Uart,0,LENGTH);
                len=shared->FS_To_Uart[index+7]+8;
                memcpy(FS_To_Uart,&shared->FS_To_Uart[index],len);
                memset(&shared->FS_To_Uart[index],0,len);
                Print_HEX("Deal Recv<<<<<<<",len,FS_To_Uart);
                Deal_FS_To_Uart_data(); 
                Unlock(&FS_Recv_Data);
                if(((index+len)==shared->FtoUnum)&&(IsAllCharacter(shared->FS_To_Uart,0,shared->FtoUnum)))
                {   
                    shared->FtoUflag=0;
                    shared->errcount=0;
                }
            }
            else if((shared->FS_To_Uart[0]==0)||(shared->FS_To_Uart[1]==0)||//节点号不可能为0
                (shared->FS_To_Uart[2]==0)||(shared->FS_To_Uart[3]==0)||//节点号不可能为0
                (shared->FS_To_Uart[7]==0)||(shared->FS_To_Uart[8]==0)||//数据长度和数据库长度不可能为0
                (shared->FtoUnum==0))
            {
                shared->FtoUflag=0;
                shared->errcount=0;
            }                                   
        }
        if(!IsAllCharacter(shared->FS_To_Uart,0,shared->FtoUnum))//内容非空，则继续处理
        {
            shared->FtoUflag=1;
        }
        else
        {
            shared->FtoUflag=0;
        }
        /*=====end 从共享内存中读取信息,可能是多帧数据end=====*/
        WriteDataToShm();//向共享内存写入串口数据       
        usleep(1 * 1000);
    }
}

/********************************************************************\
* 函数名： SharedMemoryCreat
* 功能:    创建共享内存
* 输入:     无
* 返回值:   无
* 创建人:   Yang Chao Xu
* 创建时间: 2017-11-16 10:16:24
\*********************************************************************/
void SharedMemoryCreat()
{
    int err;
    pthread_t shmthread;
    void *shm = NULL;//分配的共享内存的原始首地址      
    int shmid;//共享内存标识符  
    //创建共享内存  
    shmid = shmget((key_t)1623, sizeof(struct shared_use_st), 0666|IPC_CREAT);  
    if(shmid == -1)  
    {  
        P_Log(conf.Gun_num,"Shmget Failed %s\n",strerror(errno));  
        exit(EXIT_FAILURE);  
    }  
    //将共享内存连接到当前进程的地址空间  
    shm = shmat(shmid, NULL, 0);  
    if(shm == (void*)-1)  
    {  
        P_Log(conf.Gun_num, "Shmat Failed %s\n", strerror(errno));  
        exit(EXIT_FAILURE);  
    }  
    P_Log(conf.Gun_num,"\nMemory Attached at %X\n", (int)shm); 
    //设置共享内存  
    shared = (struct shared_use_st*)shm;  
    shared->FtoUflag = 0;  
    shared->errcount = 0;
    
    err=pthread_create(&shmthread,NULL,(void*)ShmthreadCreat,NULL);//创建线程
    if(err != 0)
    {
        P_Log(conf.Gun_num,"can't create ShmthreadCreat thread: %s\n", strerror(err));
        return ;
    }
    else
        P_Log(conf.Gun_num,"create ShmthreadCreat thread OK\n");
}
void init_sys()
{
    pthread_mutex_init(&Data_To_Uart, NULL);
    pthread_mutex_init(&FS_Recv_Data, NULL);
    pthread_mutex_init(&cs_log, NULL);
    pthread_mutex_init(&Uart_Recv_Data, NULL);    
    
#ifdef Debug
    Print_Conf();
#endif

    //串口初始化
    init_Uart();

    //定时器线程创建
    Timer_thread_Creat();

    //创建共享内存
    SharedMemoryCreat();

    //TCP线程创建
    Tcp_Pthread_Creat();

    //UDP线程创建
    Udp_Pthread_Creat();   

    //打开led心跳控制
    OpenLedDev();
}

void ycxtest()
{
#if 0
    
#endif
}
int main(int argc, char *argv[])
{
    if(argc!=2) return 0;//必须跟枪信息参数
    daemon(1, 0);
    char sh[40];
    Gun_num=atoi(argv[1]);
    if((Gun_num==0)||(Gun_num>4)) return 0;//最多4把枪
    //读取配置文件
    init_Read_Conf(Gun_num);
    
    Para_set();
    init_sys();
    ycxtest();
    
    sprintf(sh, "ntpdate %s", conf.FS_IP); //向FS请求网络对时
#ifdef Debug
    P_Log(conf.Gun_num,"network time sync:%s\n", sh);
#endif
    system(sh);
    pause();
    return 0;
}
