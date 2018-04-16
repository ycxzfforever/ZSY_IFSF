#include "common.h"

/********************************************************************\
* 函数名：: OpenLedDev
* 功能:    打开GPIO设备
* 输入:     无
* 返回值:   0-成功，-1-失败
* 创建人:   Yang Chao Xu
* 创建时间: 2017-10-9 17:20:07
\*********************************************************************/
int OpenLedDev()
{	
    if(conf.Led_ON==0) return -1;//配置文件中控制led是否闪烁
    Gpio_fd = open(DEV_GPIO, O_RDWR);	
    if (Gpio_fd < 0) 
    {       
        P_Log(conf.Gun_num,"OpenLedDev Error, device open fail! %d\n", Gpio_fd);
        return -1;  
    }
    return 0;
}

/********************************************************************\
* 函数名：: LedOnOff
* 功能:    控制LED灯
* 输入:     0-灭，1-亮
* 返回值:   无
* 创建人:   Yang Chao Xu
* 创建时间: 2017-10-9 17:20:51
\*********************************************************************/
void LedOnOff(int state)
{
    if(conf.Led_ON==0) return;//配置文件中控制led是否闪烁
    am335x_gpio_arg arg;
    arg.pin = GPIO_TO_PIN(0, 20);
    arg.data = state;
    ioctl(Gpio_fd, IOCTL_GPIO_SETOUTPUT, &arg);
}


