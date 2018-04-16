#ifndef _UART_H_
#define _UART_H_


Uchar   Recv70_Flag;
Uchar   Recv57_Flag;
Uchar   Recv72_Flag;
Uchar   Status_Code;
int     Uart_Data_Len;
Uchar   Uart_Tx_Flag;

Uchar   Auth_Type;//授权类型，0=金额，1=气量
long    Auth_Value;//授权量
Uchar   HaveAuth;   //0-未授权，1-已授权


Uchar   StartFuelling;   //0:started 6状态开始加气，1:fuelling 8状态
Uchar   uploadtransagain;//在线交易时，若收到交易记录后，3秒之内没有收到读取未支付交易，则再上传一次
Uchar   uploadtransagaincount;

void set_speed(int fd, int speed);
int set_Parity(int fd, int databits, int stopbits, int parity);
void Reply_FS_UploadStatus(Uchar st);
void Deal_Uart_data();
void Deal_FS_To_Uart_data();
void Deal_EPS_To_Uart_data();
int Uart_Pthread_Creat();
void Reply_FS_UploadRealtimeData();
void Reply_FS_UploadOverFuel();
void Reply_FS_ReadTotal();
void Reply_FS_Auth_OPT();
void Reply_FS_Stop_OPT();
void Reply_FS_CantAuth_OPT();
/*回复fs变价成功*/
void Reply_FS_DownLoad_Price_Success();
/*从FP中读取未支付交易*/
void Reply_FS_ReadNO_Pay_Trans(Uchar flag);
/*主动上传5,6,8状态*/
void Reply_FS_Upload568Status();





#endif

