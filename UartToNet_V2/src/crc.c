#include "common.h"


/***************************************************************************
**函数名称： ModbusCrc16
**函数功能：流量计CRC16计算
**入口参数：buf:需要计算CRC的数据，len:需要计算的数据长度
**返回值：计算后CRC
**创建者：杨朝旭
**创建日期：2016-3-30 15:45:03
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
Ushort ModbusCrc16(Uchar *buf, Ushort Len)
{
    Ushort IX, IY, CRC;
    CRC = 0xFFFF;
    if(Len <= 0)    return 0;
    for(IX = 0; IX < Len; IX++)
    {
        CRC = CRC ^ (Ushort)(buf[IX]);
        for(IY = 0; IY < 8; IY++)
        {
            if((CRC &1) != 0)
                CRC = (CRC >> 1) ^ 0xA001;
            else
                CRC = CRC >> 1;
        }
    }
    return CRC;
}


