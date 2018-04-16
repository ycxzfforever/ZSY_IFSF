#include "common.h"


/********************************************************************\
* 函数名: Time_Process
* 说明:
* 功能:    将时间转换成7字节的yyyymmddhhmmss
* 输入:
* 返回值:   无
* 创建人:   Yang Chao Xu
* 创建时间: 2014-9-18
\*********************************************************************/
void Time_Process()
{
    ftime(&tb);
    now=localtime(&tb.time);
    para.FSC_TIME[0]=((now->tm_year+1900)/1000)*16+((now->tm_year+1900)/100)%10;
    para.FSC_TIME[1]=((now->tm_year+1900)%100)/10*16+((now->tm_year+1900)%100)%10;
    para.FSC_TIME[2]=(now->tm_mon+1)/10*16+(now->tm_mon+1)%10;
    para.FSC_TIME[3]=(now->tm_mday)/10*16+(now->tm_mday)%10;
    para.FSC_TIME[4]=(now->tm_hour)/10*16+(now->tm_hour)%10;
    para.FSC_TIME[5]=(now->tm_min)/10*16+(now->tm_min)%10;
    para.FSC_TIME[6]=(now->tm_sec)/10*16+(now->tm_sec)%10;
}

void FCS_To_OPT_HeadandTail(Uchar cmdID,Ushort len)//帧头+帧尾
{
    char str[15];
    Ushort crc;
    //帧头
    Uart_Tx_Buff[0] = 0xAA;       //起始符 AAH
    Uart_Tx_Buff[1] = 0xFF;       //起始符 FFH
    Uart_Tx_Buff[2] = para.NZN;   //枪号号 ，目标设备
    Uart_Tx_Buff[3] = 0x00;       //源设备
    Uart_Tx_Buff[4] = cmdID;      //命令字 01H~90H
    Uart_Tx_Buff[5] = len>>8;
    Uart_Tx_Buff[6] = (Uchar)len;
    //帧尾
    crc=ModbusCrc16(&Uart_Tx_Buff[2], len+5);
    Uart_Tx_Buff[len+7] = crc>>8;
    Uart_Tx_Buff[len+8] = (Uchar)crc;
    Uart_Tx_Buff[len+9] = 0xCC; //结束符CC
    Uart_Tx_Buff[len+10] = 0xFF; //结束符FF
    Uart_Tx_Flag=1;
    Uart_Data_Len=len+11;
    sprintf(str,"U_Send_%02X",cmdID);
    Print_HEX(str,Uart_Data_Len,Uart_Tx_Buff);
}

//********************************************************************
// 函数名: FCS_To_JKOPT_51
// 功能:   FSC 轮训加气机
// 输入:
// 返回值:   无
// 创建人:   Yang Chao Xu
// 创建时间: 2015-06-03
//*********************************************************************
void FCS_To_JKOPT_51()
{
    Uchar tt=7,i=0;    
    Time_Process();
    for(i=0; i<7; i++)      //时间
    {
        Uart_Tx_Buff[tt++]=para.FSC_TIME[i];
    }
    if(Stop_Opt==1)
        Uart_Tx_Buff[tt++]=0;
    else
        Uart_Tx_Buff[tt++]=FS_OK;                    //fs状态
    Uart_Tx_Buff[tt++]=ReadNoPayFlag;                //读取未支付交易标识
    Uart_Tx_Buff[tt++]=fp_id.Default_Fuelling_Mode;  //模式选择
    Uart_Tx_Buff[tt++]=BoartRequestFlag; //主板开始申请支付标记
    Uart_Tx_Buff[tt++]=price.VER;
    FCS_To_OPT_HeadandTail(0x51,tt-7);

    if(OPT_IS_Online)
        OPT_IS_Online--;
}

//********************************************************************
// 函数名: FCS_To_JKOPT_52
// 功能:   FSC 回复加气机上传交易记录
// 输入:
// 返回值:   无
// 创建人:   Yang Chao Xu
// 创建时间: 2017-8-30 10:19:11
//*********************************************************************
void FCS_To_JKOPT_52()
{
    Uchar tt=7,i=0;  
    UnionU32 urdv;
    Uart_Tx_Buff[tt++]=1;//是否成功标志
    Uart_Tx_Buff[tt++]=0;//交易类型
    urdv.data=para.POS_TTC;
    for(i=0;i<4;i++)
    {
        Uart_Tx_Buff[tt++]=urdv.rdcv[i];
    }
    FCS_To_OPT_HeadandTail(0x52,tt-7);
}

//********************************************************************
// 函数名: FCS_To_JKOPT_57
// 功能:   FSC读加气机累计
// 输入:
// 返回值:   无
// 创建人:   Yang Chao Xu
// 创建时间: 2017-8-30 10:52:34
//*********************************************************************
void FCS_To_JKOPT_57()
{
    Uchar tt=7;  
    Uart_Tx_Buff[tt++]=para.NZN;
    FCS_To_OPT_HeadandTail(0x57,tt-7);
}

//********************************************************************
// 函数名: FCS_To_JKOPT_58
// 功能:   FSC读加气机指定流水
// 输入:
// 返回值:   无
// 创建人:   Yang Chao Xu
// 创建时间: 2017-8-30 11:06:18
//*********************************************************************
void FCS_To_JKOPT_58()
{
    Uchar tt=7;  
    Uart_Tx_Buff[tt++]=para.NZN;
    FCS_To_OPT_HeadandTail(0x58,tt-7);
}

//********************************************************************
// 函数名: FCS_To_JKOPT_5E
// 功能:   加气机申请下发单价
// 输入:
// 返回值:   无
// 创建人:   Yang Chao Xu
// 创建时间: 2017-8-30 10:52:34
//*********************************************************************
void FCS_To_JKOPT_5E()
{
    Uchar tt=7,i;  
    Time_Process();
    Uart_Tx_Buff[tt++]=price.VER;    //单价版本号
    Uart_Tx_Buff[tt++]=para.devicetype; //设备类型
    for(i=0; i<7; i++)      //时间
    {
        Uart_Tx_Buff[tt++]=para.FSC_TIME[i];
    }
    Uart_Tx_Buff[tt++]=4;//单价数目

    Uart_Tx_Buff[tt++]=price.Price[0];
    Uart_Tx_Buff[tt++]=price.Price[0]>>8;
    Uart_Tx_Buff[tt++]=price.Price[1];
    Uart_Tx_Buff[tt++]=price.Price[1]>>8;
    Uart_Tx_Buff[tt++]=price.Price[2];
    Uart_Tx_Buff[tt++]=price.Price[2]>>8;
    Uart_Tx_Buff[tt++]=price.Price[3];
    Uart_Tx_Buff[tt++]=price.Price[3]>>8;
    
    FCS_To_OPT_HeadandTail(0x5E,tt-7);
}



//********************************************************************
// 函数名: FCS_To_JKOPT_70
// 功能:    FSC 回复加气机申请授权
// 输入:   
// 返回值:   无
// 创建人:   Yang Chao Xu
// 创建时间: 2017-8-30 11:02:44
//*********************************************************************
void FCS_To_JKOPT_70()
{
    Uchar tt=7,i=0;
    UnionU32 urdv;
    Uart_Tx_Buff[tt++]=Auth_Type; //授权类型
    urdv.data=Auth_Value;
    for(i=0; i<4; i++)            //授权值
    {
        Uart_Tx_Buff[tt++]=urdv.rdcv[i];
    }
    FCS_To_OPT_HeadandTail(0x70,tt-7);
}

//********************************************************************
// 函数名: FCS_To_JKOPT_71
// 功能:    FSC 回复EPS数据给加气机
// 输入:   无
// 返回值:   无
// 创建人:   Yang Chao Xu
// 创建时间: 2015-06-03
//*********************************************************************
void FCS_To_JKOPT_71()
{
    Uchar tt=7,i=0;
    for(i=0; i<EPS_Data_Len; i++)
    {
        Uart_Tx_Buff[tt++]=EPS_To_Uart[i];
        EPS_Back_Buf[i]=EPS_To_Uart[i];
    }
    EPS_Back_num = EPS_Data_Len;
    
    if((EPS_To_Uart[26]==0x02)&&
        (EPS_To_Uart[27]==0x00)&&
        (EPS_To_Uart[28]==0x02)&&
        (HuiSuoFlag==0))//  EPS验卡返回
    {
        YanKaFlag=1;
        OverTransFlag=0;
    }
    else if((EPS_To_Uart[26]==0x02)&&
        (EPS_To_Uart[27]==0x00)&&
        (EPS_To_Uart[28]==0x03)&&
        (YanKaFlag==1))//  EPS通知灰锁结果返回
    {
        HuiSuoFlag=1;
        YanKaFlag=0;
    }
    else if((EPS_To_Uart[26]==0x02)&&
        (EPS_To_Uart[27]==0x00)&&
        (EPS_To_Uart[28]==0x04)&&
        (OverTransFlag==0))//  EPS返回加气机申请支付
    {
        RequestPayFlag=1;
        HuiSuoFlag=0;
    }
    else if((EPS_To_Uart[26]==0x02)&&
        (EPS_To_Uart[27]==0x00)&&
        (EPS_To_Uart[28]==0x05))//  EPS返回加气机通知交易结果
    {
        OverTransFlag=1;
        RequestPayFlag=0;
    }
    bzero(EPS_To_Uart,LENGTH);
    FCS_To_OPT_HeadandTail(0x71,tt-7);
}


//********************************************************************
// 函数名: FCS_To_JKOPT_72
// 功能:    FSC 停止加气机
// 输入:   无
// 返回值:   无
// 创建人:   Yang Chao Xu
// 创建时间: 2015-06-03
//*********************************************************************
void FCS_To_JKOPT_72()
{
    Uchar tt=7;
    Uart_Tx_Buff[tt++]=para.NZN;
    FCS_To_OPT_HeadandTail(0x72,tt-7);
}

//********************************************************************
// 函数名: FCS_To_JKOPT_73
// 功能:    FSC 回复加气机上传参数
// 输入:   无
// 返回值:   无
// 创建人:   Yang Chao Xu
// 创建时间: 2017-8-30 11:55:56
//*********************************************************************
void FCS_To_JKOPT_73()
{
    Uchar tt=7;
    Uart_Tx_Buff[tt++]=0;
    FCS_To_OPT_HeadandTail(0x73,tt-7);
}

//eps备份数据处理
void EpsBackDataDispose()
{
    Uchar tt=7,i=0;
    for(i=0; i<EPS_Back_num; i++)
    {
        Uart_Tx_Buff[tt++]=EPS_Back_Buf[i];
    }
    FCS_To_OPT_HeadandTail(0x71,tt-7);
}


