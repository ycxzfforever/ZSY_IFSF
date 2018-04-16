#ifndef _CMD_H
#define _CMD_H

void Time_Process();


void FCS_To_OPT_HeadandTail(Uchar cmdID,Ushort len);//帧头+帧尾


void FCS_To_JKOPT_51();
void FCS_To_JKOPT_52();
void FCS_To_JKOPT_57();
void FCS_To_JKOPT_58();
void FCS_To_JKOPT_5E();
void FCS_To_JKOPT_70();
void FCS_To_JKOPT_71();
void FCS_To_JKOPT_72();
void FCS_To_JKOPT_73();

void EpsBackDataDispose();//eps备份数据处理





#endif

