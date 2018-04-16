#ifndef _GPIO_H
#define _GPIO_H

#define DEV_GPIO            "/dev/am335x_gpio"
#define GPIO_IOC_MAGIC      'G'
#define IOCTL_GPIO_SETOUTPUT              _IOW(GPIO_IOC_MAGIC, 0, int)       
#define IOCTL_GPIO_SETINPUT               _IOW(GPIO_IOC_MAGIC, 1, int)
#define IOCTL_GPIO_SETVALUE               _IOW(GPIO_IOC_MAGIC, 2, int) 
#define IOCTL_GPIO_GETVALUE    		      _IOR(GPIO_IOC_MAGIC, 3, int)
#define GPIO_TO_PIN(bank, gpio)           (32 * (bank) + (gpio))

typedef struct {
    int pin;
    int data;
}am335x_gpio_arg;    
//led am335x_gpio_arg.pin=GPIO_TO_PIN(0, 20);

int OpenLedDev();
void LedOnOff(int state);

#endif

