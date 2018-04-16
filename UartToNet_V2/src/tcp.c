#include "common.h"

// ˝æ›∑¢ÀÕ
void UartSendDataToFS()
{
    int num;
    //gun1
    if((FS_Data_Len>0)&&(Uart_To_FS[0]!=0))
    {
        if((num = send(Cli_FS_fd, Uart_To_FS, FS_Data_Len, 0)) == -1)
        {
            P_Log(conf.Gun_num,"ERROR:Cli_Uart_To_FS_Thread Failed to sent string.\n");
            Cli_Connect_FS=0;
        }
        else
        {                
            Print_HEX("U_To_F Send>>>>>>>",num,Uart_To_FS);
        } 
        FS_Data_Len=0;
    }
    else
        FS_Data_Len=0;
    //gun2
    if((shared->UtoFnum2>0)&&(shared->Uart_To_FS2[0]!=0))
    {
        if((num = send(Cli_FS_fd, shared->Uart_To_FS2, shared->UtoFnum2, 0)) == -1)
        {
            P_Log(conf.Gun_num,"ERROR:Cli_Uart_To_FS_Thread Failed to sent string.\n");
            Cli_Connect_FS=0;
        }
        else
        {                
            Print_HEX("U_To_F Send>>>>>>>",num,shared->Uart_To_FS2);
        } 
        shared->UtoFnum2=0;
    }
    else
        shared->UtoFnum2=0;
    //gun3
    if((shared->UtoFnum3>0)&&(shared->Uart_To_FS3[0]!=0))
    {
        if((num = send(Cli_FS_fd, shared->Uart_To_FS3, shared->UtoFnum3, 0)) == -1)
        {
            P_Log(conf.Gun_num,"ERROR:Cli_Uart_To_FS_Thread Failed to sent string.\n");
            Cli_Connect_FS=0;
        }
        else
        {                
            Print_HEX("U_To_F Send>>>>>>>",num,shared->Uart_To_FS3);
        } 
        shared->UtoFnum3=0;
    }
    else
        shared->UtoFnum3=0;
    //gun4
    if((shared->UtoFnum4>0)&&(shared->Uart_To_FS4[0]!=0))
    {
        if((num = send(Cli_FS_fd, shared->Uart_To_FS4, shared->UtoFnum4, 0)) == -1)
        {
            P_Log(conf.Gun_num,"ERROR:Cli_Uart_To_FS_Thread Failed to sent string.\n");
            Cli_Connect_FS=0;
        }
        else
        {                
            Print_HEX("U_To_F Send>>>>>>>",num,shared->Uart_To_FS4);
        } 
        shared->UtoFnum4=0;
    }
    else
        shared->UtoFnum4=0;
}

/********************************************************************\
* ∫Ø ˝√˚£∫: Cli_Uart_To_FS_Thread
* Àµ√˜:
* π¶ƒ‹:     º”∆¯ª˙∑¢ÀÕ∏¯FSµƒ ˝æ›¥¶¿Ì
*  ‰»Î:     Œﬁ
* ∑µªÿ÷µ:   Œﬁ
* ¥¥Ω®»À:   Yang Chao Xu
* ¥¥Ω® ±º‰: 2014-8-26
\*********************************************************************/
void *Cli_Uart_To_FS_Thread(void *arg)
{    
    while(1)
    {                
        UartSendDataToFS();
        if(Cli_Connect_FS==0)
        {
            close(Cli_FS_fd);
            Cli_FS_fd=-1;
            return(0);
        }
        if(Cli_FS_fd<0) return 0;
        if(Ser_FS_fd<0)//∑˛ŒÒ∆˜∂œø™∫Û“≤∂œø™øÕªß∂À
        {
            if(Cli_FS_fd>0)
            {
                close(Cli_FS_fd);
                Cli_Connect_FS = 0;
                Cli_FS_fd=-1;
            }
            return 0;
        }
        usleep(1*1000);
    }
    close(Cli_FS_fd);
}

/********************************************************************\
* ∫Ø ˝√˚£∫: Cli_EPS_To_Uart_Thread
* Àµ√˜:
* π¶ƒ‹:    EPS∑¢ÀÕ∏¯ º”∆¯ª˙µƒ ˝æ›¥¶¿Ì
*  ‰»Î:     Œﬁ
* ∑µªÿ÷µ:   Œﬁ
* ¥¥Ω®»À:   Yang Chao Xu
* ¥¥Ω® ±º‰: 2014-8-26
\*********************************************************************/
void *Cli_EPS_To_Uart_Thread(void *arg)
{
    int num;
    /* Try to connect the server */
    while(1)       // Check remoter command
    {        
        num = recv(Cli_EPS_fd, EPS_To_Uart, LENGTH, 0);
        switch(num)
        {
            case -1:
                P_Log(conf.Gun_num,"ERROR:Cli_EPS_To_Uart_Thread Receive string error!\n");
                Disconnect_EPS_Flag=1;
                break;
            case  0:
                P_Log(conf.Gun_num,"Info:EPS_Server close the socket!\n");
                Disconnect_EPS_Flag=1;
                break;
            default:
                P_Log(conf.Gun_num,"OK:Cli_EPS_To_Uart_Thread Receviced numbytes = %d\n", num);
                EPS_Data_Len=num;
                Print_HEX("E_To_U Recv",num,EPS_To_Uart);
                CmdFlag=0x71;//œ¬∑¢EPS√¸¡Ó
                break;
        }
        if(Disconnect_EPS_Flag==1)
        {
            Disconnect_EPS_Flag=0;
            Is_Connect_EPS=0;
            close(Cli_EPS_fd);           
            Cli_EPS_fd=-1;
            return(0);
        }
        if(Cli_EPS_fd<0) return 0;
    }
    close(Cli_EPS_fd);
}

/********************************************************************\
* ∫Ø ˝√˚£∫: Cli_Uart_To_EPS_Thread
* Àµ√˜:
* π¶ƒ‹:     º”∆¯ª˙∑¢ÀÕ∏¯EPSµƒ ˝æ›¥¶¿Ì
*  ‰»Î:     Œﬁ
* ∑µªÿ÷µ:   Œﬁ
* ¥¥Ω®»À:   Yang Chao Xu
* ¥¥Ω® ±º‰: 2014-8-26
\*********************************************************************/
void *Cli_Uart_To_EPS_Thread(void *arg)
{
    int num;
    while(1)
    {
        if((Uart_To_EPS_Flag==1)&&(Cli_EPS_fd>0))
        {
            Uart_To_EPS_Flag=0;
            if((num = send(Cli_EPS_fd, Uart_To_EPS, EPS_Data_Len, 0)) == -1)
            {
                P_Log(conf.Gun_num,"ERROR:Cli_Uart_To_EPS_Thread Failed to sent string.\n");
                Disconnect_EPS_Flag=1;
            }
            bzero(Uart_To_EPS,LENGTH);
        }
        if(Disconnect_EPS_Flag==1)
        {
            Disconnect_EPS_Flag=0;
            Is_Connect_EPS=0;
            close(Cli_EPS_fd);
            Cli_EPS_fd=-1;
            return(0);
        }
        if(Cli_EPS_fd<0) return 0;//∑˛ŒÒ∆˜∂Àπÿ±’¡¨Ω”∫Û£¨–Ë“™ ÷∂ØÕÀ≥ˆ
        usleep(10*1000);
    }
    close(Cli_EPS_fd);
}

/********************************************************************\
* ∫Ø ˝√˚£∫: EPS_Tcp_Client_Creat
* Àµ√˜:
* π¶ƒ‹:     EPSøÕªß∂À¥¥Ω®
*  ‰»Î:     Œﬁ
* ∑µªÿ÷µ:   Œﬁ
* ¥¥Ω®»À:   Yang Chao Xu
* ¥¥Ω® ±º‰: 2014-8-26
\*********************************************************************/
int EPS_Tcp_Client_Creat()
{
    const char *argv=(const char *)conf.EPS_IP;
    int sockfd,err;                        // Socket file descriptor
    pthread_t Cli_Send,Cli_Receive;
    pthread_attr_t attr;//œﬂ≥Ã Ù–‘
    struct sockaddr_in remote_addr;    // Host address information

    /* Get the Socket file descriptor */
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        P_Log(conf.Gun_num,"ERROR:EPS_Tcp_Client_Creat Failed to obtain Socket Descriptor!\n");
        return (0);
    }
    /* Fill the socket address struct */
    remote_addr.sin_family = AF_INET;                       // Protocol Family
    remote_addr.sin_port = htons(conf.EPS_Port);            // Port number
    err=inet_pton(AF_INET, argv, &remote_addr.sin_addr);    // Net Address
    if(err <= 0)
    {
        if(err == 0)
            fprintf(stderr, "EPS_Tcp_Client_Creat Not in presentation format\n");
        else
            perror("EPS_Tcp_Client_Creat inet_pton error\n");
        return (0);
    }
    bzero(&(remote_addr.sin_zero), 8);                  // Flush the rest of struct

    /* Try to connect the remote */
    if(connect(sockfd, (struct sockaddr *)&remote_addr,  sizeof(struct sockaddr)) == -1)
    {
        EPS_Error++;
        if(EPS_Error>5){
            Reconnect_EPS_Flag=0;
        }
        P_Log(conf.Gun_num,"ERROR:EPS_Tcp_Client_Creat Failed to connect to the host!\n");
        sleep(5);
        return (0);
    }
    else
    {
        EPS_Error=0;
        P_Log(conf.Gun_num,"OK:EPS_Tcp_Client_Creat Have connected to the %s\n",argv);
    }
    //¥¥Ω®œﬂ≥Ã
    Cli_EPS_fd=sockfd;
    Reconnect_EPS_Flag=0;
    Is_Connect_EPS=1;//¡¨Ω”…œ¡Àeps
    pthread_attr_init(&attr);//œﬂ≥Ã Ù–‘≥ı ºªØ
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);//…Ë÷√Œ™∑÷¿Îœﬂ≥Ã£¨’‚—˘ø…“‘‘⁄œﬂ≥ÃΩ· ¯ ±¬Ì…œ Õ∑≈◊ ‘¥
    err=pthread_create(&Cli_Send,&attr,(void*)Cli_Uart_To_EPS_Thread,NULL);//¥¥Ω®Ω” ’œﬂ≥Ã
    if(err != 0)
        P_Log(conf.Gun_num,"EPS_Tcp_Client_Creat can't create Cli_Uart_To_EPS_Thread thread: %s\n", strerror(err));
    else
        P_Log(conf.Gun_num,"EPS_Tcp_Client_Creat create Cli_Uart_To_EPS_Thread thread OK\n");
    pthread_attr_destroy(&attr);//»•µÙœﬂ≥Ã Ù–‘
    
    pthread_attr_init(&attr);//œﬂ≥Ã Ù–‘≥ı ºªØ
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);//…Ë÷√Œ™∑÷¿Îœﬂ≥Ã£¨’‚—˘ø…“‘‘⁄œﬂ≥ÃΩ· ¯ ±¬Ì…œ Õ∑≈◊ ‘¥
    err=pthread_create(&Cli_Receive,&attr,(void*)Cli_EPS_To_Uart_Thread,NULL);//¥¥Ω®Ω” ’œﬂ≥Ã
    if(err != 0)
        P_Log(conf.Gun_num,"EPS_Tcp_Client_Creat can't create Cli_EPS_To_Uart_Thread thread: %s\n", strerror(err));
    else
        P_Log(conf.Gun_num,"EPS_Tcp_Client_Creat create Cli_EPS_To_Uart_Thread thread OK\n");
    
    pthread_attr_destroy(&attr);//»•µÙœﬂ≥Ã Ù–‘
    return (0);
}

/********************************************************************\
* ∫Ø ˝√˚£∫: FS_Tcp_Client_Creat
* Àµ√˜:
* π¶ƒ‹:     FSøÕªß∂À¥¥Ω®
*  ‰»Î:     Œﬁ
* ∑µªÿ÷µ:   Œﬁ
* ¥¥Ω®»À:   Yang Chao Xu
* ¥¥Ω® ±º‰: 2014-8-26
\*********************************************************************/
int FS_Tcp_Client_Creat()
{
    const char *argv=(const char *)conf.FS_IP;
    int sockfd,err;                        // Socket file descriptor
    pthread_t Cli_Send;
    struct sockaddr_in remote_addr;    // Host address information

    /* Get the Socket file descriptor */
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        P_Log(conf.Gun_num,"ERROR:FS_Tcp_Client_Creat Failed to obtain Socket Descriptor!\n");
        return (0);
    }
    /* Fill the socket address struct */
    remote_addr.sin_family = AF_INET;                       // Protocol Family
    remote_addr.sin_port = htons(conf.FS_Port);             // Port number
    err=inet_pton(AF_INET, argv, &remote_addr.sin_addr);    // Net Address
    if(err <= 0)
    {
        if(err == 0)
            fprintf(stderr, "FS_Tcp_Client_Creat Not in presentation format\n");
        else
            perror("FS_Tcp_Client_Creat inet_pton error\n");
        return (0);
    }
    bzero(&(remote_addr.sin_zero), 8);                  // Flush the rest of struct
    /* Try to connect the remote */
    if((Cli_Connect_FS==0)&&((FS_OK==1)||FS_OK==2))
    {
        if(connect(sockfd, (struct sockaddr *)&remote_addr,  sizeof(struct sockaddr)) == -1)
        {
            P_Log(conf.Gun_num,"ERROR:FS_Tcp_Client_Creat Failed to connect to the host!\n");
        }
        else
        {
            P_Log(conf.Gun_num,"OK:FS_Tcp_Client_Creat Have connected to the %s\n",argv);
        }
        Cli_Connect_FS=1;//¡¨Ω”…œ¡Àfsª
        Reconnect_FS_Flag=0;
        //¥¥Ω®œﬂ≥Ã
        Cli_FS_fd=sockfd;
        err=pthread_create(&Cli_Send,NULL,(void*)Cli_Uart_To_FS_Thread,NULL);//¥¥Ω®œÚFS∑¢ÀÕ ˝æ›œﬂ≥Ã
        if(err != 0)
            P_Log(conf.Gun_num,"FS_Tcp_Client_Creat can't create Cli_Uart_To_FS_Thread thread: %s\n", strerror(err));
        else
            P_Log(conf.Gun_num,"FS_Tcp_Client_Creat create Cli_Uart_To_FS_Thread thread OK\n");        
    }
    return 0;
}

/********************************************************************\
* ∫Ø ˝√˚: CopyToShm
* π¶ƒ‹:     øΩ±¥FS ˝æ›µΩπ≤œÌƒ⁄¥Ê÷– 
*  ‰»Î:      ˝æ›≥§∂»
* ∑µªÿ÷µ:   Œﬁ
* ¥¥Ω®»À:   Yang Chao Xu
* ¥¥Ω® ±º‰: 2017-11-20 12:01:27
\*********************************************************************/
void CopyToShm(int num)    
{
    //num=24,¡Ω÷° ˝æ›
    //01.0B.02.01.00.5D.00.04.01.21.3C.00.01.0A.02.01.00.53.00.04.01.21.3C.00
    //num=36,»˝÷° ˝æ›
    //01.09.02.01.00.5E.00.04.01.21.3D.00.01.0B.02.01.00.41.00.04.01.21.3D.00.01.0A.02.01.00.56.00.04.01.21.3D.00.
    if(shared->FtoUflag==0)
    {          
        memset(shared->FS_To_Uart,0,LENGTH);
        memcpy(shared->FS_To_Uart,FS_To_Uart,num);                    
        shared->FtoUflag=1;
        shared->FtoUnum=num;                        
    }
    else if((shared->FtoUflag==1)&&(num>0)&&(FSbacknum1==0))//¥¶¿Ì÷Æ«∞FS∑¢∏¯¥Æø⁄µƒ ˝æ›ªπ√ª¥¶¿ÌÕÍ£¨Ω¯––±∏∑›
    {
        memset(FSbackbuf1,0,LENGTH);
        memcpy(FSbackbuf1,FS_To_Uart,num);  
        FSbacknum1=num;
    }
    else if((FSbacknum1>0)&&(FSbacknum2==0))//»Ùfsœ¬∑¢ ˝æ›π˝øÏ£¨‘Ú÷Æ«∞µƒ±∏∑› ˝æ›ø…ƒ‹ªπ√ª¥¶¿ÌÕÍ£¨‘Ÿ¥Œ±∏∑›
    {        
        memset(FSbackbuf2,0,LENGTH);
        memcpy(FSbackbuf2,FS_To_Uart,num);  
        FSbacknum2=num;
    }  
}

/********************************************************************\
* ∫Ø ˝√˚: FS_Tcp_Server_Creat
* Àµ√˜: ”√”⁄fs øÕªß∂À¡¨Ω”º”∆¯ª˙
* π¶ƒ‹:     FS∑˛ŒÒ∂À¥¥Ω®
*  ‰»Î:     Œﬁ
* ∑µªÿ÷µ:   Œﬁ
* ¥¥Ω®»À:   Yang Chao Xu
* ¥¥Ω® ±º‰: 2014-8-26
\*********************************************************************/
int FS_Tcp_Server_Creat()
{
    int sockfd;                         // Socket file descriptor
    int nsockfd;                        // New Socket file descriptor
    int num;
    int sin_size;                       // to store struct size
    int opt=1;                          //…Ë÷√∂Àø⁄÷ÿ”√(wait◊¥Ã¨)
    int keepalive = 1;                  // ø™∆Ùkeepalive Ù–‘
    int keepidle = 30;                  // »Á∏√¡¨Ω”‘⁄30√Îƒ⁄√ª”–»Œ∫Œ ˝æ›Õ˘¿¥,‘ÚΩ¯––ÃΩ≤‚
    int keepinterval = 5;               // ÃΩ≤‚ ±∑¢∞¸µƒ ±º‰º‰∏ÙŒ™5 √Î
    int keepcount = 3;                  // ÃΩ≤‚≥¢ ‘µƒ¥Œ ˝.»Áπ˚µ⁄1¥ŒÃΩ≤‚∞¸æÕ ’µΩœÏ”¶¡À,‘Ú∫Û2¥Œµƒ≤ª‘Ÿ∑¢.

    struct sockaddr_in addr_local;
    struct sockaddr_in addr_remote;

    /* Get the Socket file descriptor */
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        P_Log(conf.Gun_num,"ERROR:FS_Tcp_Server_Creat Failed to obtain Socket Despcritor.\n");
        return (0);
    }
    else
    {
        P_Log(conf.Gun_num,"OK:FS_Tcp_Server_Creat Obtain Socket Despcritor sucessfully.\n");
    }
    setsockopt(sockfd, SOL_SOCKET,SO_REUSEADDR,(void *)&opt,sizeof(opt));
    setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepalive , sizeof(keepalive));
    setsockopt(sockfd, SOL_TCP, TCP_KEEPIDLE, (void*)&keepidle , sizeof(keepidle));
    setsockopt(sockfd, SOL_TCP, TCP_KEEPINTVL, (void *)&keepinterval , sizeof(keepinterval));
    setsockopt(sockfd, SOL_TCP, TCP_KEEPCNT, (void *)&keepcount , sizeof(keepcount));
    /* Fill the local socket address struct */
    addr_local.sin_family = AF_INET;                // Protocol Family
    addr_local.sin_port = htons(conf.My_FS_Port);   // Port number
    addr_local.sin_addr.s_addr  = INADDR_ANY;       // AutoFill local address
    bzero(&(addr_local.sin_zero), 8);               // Flush the rest of struct

    /*  Blind a special Port */
    if(bind(sockfd, (struct sockaddr*)&addr_local, sizeof(struct sockaddr)) == -1)
    {
        P_Log(conf.Gun_num,"ERROR:FS_Tcp_Server_Creat Failed to bind Port %d.\n",conf.My_FS_Port);
        return (0);
    }
    else
    {
        P_Log(conf.Gun_num,"OK:FS_Tcp_Server_Creat Bind the Port %d sucessfully.\n",conf.My_FS_Port);
    }

    /*  Listen remote connect/calling */
    if(listen(sockfd,BACKLOG) == -1)
    {
        P_Log(conf.Gun_num,"ERROR:FS_Tcp_Server_Creat Failed to listen Port %d.\n", conf.My_FS_Port);
        return (0);
    }
    else
    {
        P_Log(conf.Gun_num,"OK:FS_Tcp_Server_Creat Listening the Port %d sucessfully.\n", conf.My_FS_Port);
    }
    OPT_Server_OK=1;
    while(1)
    {
        FS_OK=0;
        sin_size = sizeof(struct sockaddr_in);
        /*  Wait a connection, and obtain a new socket file despriptor for single connection */
        if((nsockfd = accept(sockfd, (struct sockaddr *)&addr_remote, (socklen_t *)&sin_size)) == -1)
        {
            P_Log(conf.Gun_num,"ERROR:FS_Tcp_Server_Creat Obtain new Socket Despcritor error.\n");
            continue;
        }
        else
        {
            P_Log(conf.Gun_num,"OK:FS_Tcp_Server_Creat Server has got connect from %s.\n", inet_ntoa(addr_remote.sin_addr));
        }
        Ser_Connect_FS=1;
        Ser_FS_fd=nsockfd;
        FS_OK=2;
        FS_Tcp_Client_Creat();
        shared->TCPOK=1;
        while(1)
        {
            num = recv(Ser_FS_fd, FS_To_Uart, LENGTH, 0);
            switch(num)
            {
                case -1:
                    P_Log(conf.Gun_num,"ERROR:Ser_FS_To_Uart_Thread Receive string error!\n");
                    Ser_Connect_FS=0;
                    break;
                case  0:
                    Ser_Connect_FS=0;
                    break;
                default:                    
                    break;
            }
            if(Ser_Connect_FS==0)
            {
                close(Ser_FS_fd);
                Ser_FS_fd=-1;
                break;
            }
            else
            {
                //øΩ±¥µΩπ≤œÌƒ⁄¥Ê÷–     
                CopyToShm(num);
                Print_HEX("F_To_U Recv<<<<<<<",num,FS_To_Uart);
            }
        }
        shared->TCPOK=0;
    }
    close(Ser_FS_fd);
    Ser_FS_fd=-1;
    OPT_Server_OK=0;
    return -1;
}


/********************************************************************\
* ∫Ø ˝√˚£∫: Tcp_Pthread_Creat
* Àµ√˜:
* π¶ƒ‹:     øÕªß∂ÀTCPœﬂ≥Ã¥¥Ω®
*  ‰»Î:     Œﬁ
* ∑µªÿ÷µ:   Œﬁ
* ¥¥Ω®»À:   Yang Chao Xu
* ¥¥Ω® ±º‰: 2014-8-26
\*********************************************************************/
int Tcp_Pthread_Creat()
{
    if(Gun_num==1)//÷ª”–µ⁄“ªÃı«π¥¥Ω®TCP∑˛ŒÒ∆˜
    {
        int err;
        pthread_t Tcp_Server;
        err=pthread_create(&Tcp_Server,NULL,(void*)FS_Tcp_Server_Creat,NULL);//¥¥Ω®Ω” ’œﬂ≥Ã
        if(err != 0)
        {
            P_Log(conf.Gun_num,"can't create FS_Tcp_Server_Creat thread: %s\n", strerror(err));
            return 1;
        }
        else
            P_Log(conf.Gun_num,"create FS_Tcp_Server_Creat thread OK\n");
        return 0;
    }
    else 
        return 0;
}

