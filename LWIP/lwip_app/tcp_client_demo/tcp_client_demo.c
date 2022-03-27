#include "tcp_client_demo.h"
#include "lwip/opt.h"
#include "lwip_comm.h"
#include "lwip/lwip_sys.h"
#include "lwip/api.h"
#include "lwip/arch.h"
#include "key.h"
#include "usart.h"
#include "led.h"
#include "beep.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//NETCONN API编程方式的TCP客户端测试代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/8/15
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//*******************************************************************************
//修改信息
//无
////////////////////////////////////////////////////////////////////////////////// 	   
 

struct netconn *tcp_clientconn;					//TCP CLIENT网络连接结构体
u8 tcp_client_recvbuf[TCP_CLIENT_RX_BUFSIZE];	//TCP客户端接收数据缓冲区
u8 *tcp_client_sendbuf="GET / HTTP/1.1\r\nHost: 192.168.1.5:8888\r\nConnection: close\r\n\r\n";	//TCP客户端发送数据缓冲区
//u8 *tcp_client_sendbuf="Explorer STM32F407 NETCONN TCP Client send data\r\n";	//TCP客户端发送数据缓冲区
u8 tcp_client_flag;		//TCP客户端数据发送标志位

////TCP客户端任务
//#define TCPCLIENT_PRIO		6
////任务堆栈大小
//#define TCPCLIENT_STK_SIZE	300
////任务堆栈
//TaskHandle_t TCPCLIENT_TASK_STK[TCPCLIENT_STK_SIZE];

//任务优先级
#define TCPCLIENT_PRIO		6
//任务堆栈大小	
#define TCPCLIENT_STK_SIZE 	300  
//任务句柄
TaskHandle_t TCPCLIENT_Task_Handler;
//任务函数
static void tcp_client_thread(void *pvParameters);






//tcp客户端任务函数
static void tcp_client_thread(void *pvParameters)
{
	
	u32 data_len = 0;
	struct pbuf *q;
	err_t err,recv_err;
	static ip_addr_t server_ipaddr,loca_ipaddr;
	static u16_t 		 server_port,loca_port;
	u8 *CommandStr;
	u8 CommandValue;
	
	LWIP_UNUSED_ARG(0);
	server_port = REMOTE_PORT;
	IP4_ADDR(&server_ipaddr, lwipdev.remoteip[0],lwipdev.remoteip[1], lwipdev.remoteip[2],lwipdev.remoteip[3]);
	
	while (1) 
	{
		tcp_clientconn = netconn_new(NETCONN_TCP);  //创建一个TCP链接
		err = netconn_connect(tcp_clientconn,&server_ipaddr,server_port);//连接服务器
		if(err != ERR_OK)  netconn_delete(tcp_clientconn); //返回值不等于ERR_OK,删除tcp_clientconn连接
		else if (err == ERR_OK)    //处理新连接的数据
		{ 
			struct netbuf *recvbuf;
			tcp_clientconn->recv_timeout = 10;   //超时时间
			netconn_getaddr(tcp_clientconn,&loca_ipaddr,&loca_port,1); //获取本地IP主机IP地址和端口号
			printf("连接上服务器%d.%d.%d.%d,本机端口号为:%d\r\n",lwipdev.remoteip[0],lwipdev.remoteip[1], lwipdev.remoteip[2],lwipdev.remoteip[3],loca_port);
			while(1)
			{
				if((tcp_client_flag & LWIP_SEND_DATA) == LWIP_SEND_DATA) //有数据要发送
				{
					err = netconn_write(tcp_clientconn ,tcp_client_sendbuf,strlen((char*)tcp_client_sendbuf),NETCONN_COPY); //发送tcp_server_sentbuf中的数据
					if(err != ERR_OK)
					{
						printf("发送失败\r\n");
					}
					tcp_client_flag &= ~LWIP_SEND_DATA;
				}
					
				
				
				
				if((recv_err = netconn_recv(tcp_clientconn,&recvbuf)) == ERR_OK)  //接收到数据
				{	
					taskENTER_CRITICAL(); //关中断
					memset(tcp_client_recvbuf,0,TCP_CLIENT_RX_BUFSIZE);  //数据接收缓冲区清零
					for(q=recvbuf->p;q!=NULL;q=q->next)  //遍历完整个pbuf链表
					{
						//判断要拷贝到TCP_CLIENT_RX_BUFSIZE中的数据是否大于TCP_CLIENT_RX_BUFSIZE的剩余空间，如果大于
						//的话就只拷贝TCP_CLIENT_RX_BUFSIZE中剩余长度的数据，否则的话就拷贝所有的数据
						if(q->len > (TCP_CLIENT_RX_BUFSIZE-data_len)) memcpy(tcp_client_recvbuf+data_len,q->payload,(TCP_CLIENT_RX_BUFSIZE-data_len));//拷贝数据
						else memcpy(tcp_client_recvbuf+data_len,q->payload,q->len);
						data_len += q->len;  	
						if(data_len > TCP_CLIENT_RX_BUFSIZE) break; //超出TCP客户端接收数组,跳出	
					}
					taskEXIT_CRITICAL();  //开中断
					
				
					CommandStr=tcp_client_recvbuf;
//					LowerToCap(CommandStr,q->len);				//将字符串转换为大写	
//					CommandValue=CommandProcess(CommandStr);
					if(strcmp((char*)CommandStr,"LEDON")==0) LED3=0;
					else if (strcmp((char*)CommandStr,"LEDOFF")==0) LED3=1;
					else if (strcmp((char*)CommandStr,"BEEPON")==0) BEEP=1;
					else if (strcmp((char*)CommandStr,"BEEPOFF")==0) BEEP=0;
					
					printf("%s\r\n",CommandStr);
					memset(CommandStr,0,10);
					data_len=0;  //复制完成后data_len要清零。					
					netbuf_delete(recvbuf);
				}else if(recv_err == ERR_CLSD)  //关闭连接
				{
					netconn_close(tcp_clientconn);
					netconn_delete(tcp_clientconn);
					printf("服务器%d.%d.%d.%d断开连接\r\n",lwipdev.remoteip[0],lwipdev.remoteip[1], lwipdev.remoteip[2],lwipdev.remoteip[3]);
					break;
				}
			}
		}
	}
}
               
//创建TCP客户端线程
//返回值:0 TCP客户端创建成功
//		其他 TCP客户端创建失败
u8 tcp_client_init(void)
{
	UBaseType_t res;

	taskENTER_CRITICAL();	//关中断
	res = xTaskCreate((TaskFunction_t )tcp_client_thread,     //任务函数
              (const char*    )"tcp_client_thread",           //任务名称
              (uint16_t       )TCPCLIENT_STK_SIZE,            //任务堆栈大小
              (void*          )NULL,                 		  //传递给任务函数的参数
              (UBaseType_t    )TCPCLIENT_PRIO,       	      //任务优先级
              (TaskHandle_t*  )&TCPCLIENT_Task_Handler);   	  //任务句柄 

//	res = vTaskCreate(tcp_client_thread,(void*)0,(OS_STK*)&TCPCLIENT_TASK_STK[TCPCLIENT_STK_SIZE-1],TCPCLIENT_PRIO); //UCOSII创建TCP客户端线程
	taskEXIT_CRITICAL(); 		//开中断
	
	return (!res);
}

