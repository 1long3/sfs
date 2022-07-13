#include "sys.h"
#include "usart.h"
#include "delay.h"
#include 	"stdio.h"
extern uint16_t CCR1_Val ;
//串口1负责与电脑端通讯
//只发送
u8  TX_ADD=0;	
u8  TX_CNT[2]={0};//发送两个数据给电脑，一个是注意力，一个是放松度	     
u8  TX_lend=0;
	  
//串口2负责脑波蓝牙通讯
//串口2只有接收
u8 receive[36]={0};	//脑波大包数据	
u8 receive_ok = 0;  //接收完成标志
u8 attention = 0;    //注意力
u8 meditation = 0;   //放松度
//信号质量
u8 signalquality = 0;
//校验和
u16  Checksum = 0;

//初始化IO 串口1 
//bound:波特率
void uart_init(u32 bound1,u32 bound2)
{
	
	  //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	  USART_InitTypeDef USART_InitStructure;
	  NVIC_InitTypeDef NVIC_InitStructure;
	
	  //串口1时钟 
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
 	  USART_DeInit(USART1);  //复位串口1	
    USART_DeInit(USART2);  //复位串口2	    
	
	  //串口1相关配置
	  //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA9
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA10

    //Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
		NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
    //USART1 初始化设置
		USART_InitStructure.USART_BaudRate = bound1;//一般设置为9600;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
		USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;//接收模式	

    USART_Init(USART1, &USART_InitStructure); //初始化串口1
		USART_ITConfig(USART1, USART_IT_RXNE,ENABLE);//开启接收中断///////////////!!!!!!不加这一句接收不到数据
    USART_Cmd(USART1, ENABLE);                //使能串口 1
		
		//串口2相关配置
		//USART2_TX   PA.2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA2
   
    //USART2_RX	  PA.3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA3
		
		//Usart2 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
		NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
		
		//USART2 初始化设置
		USART_InitStructure.USART_BaudRate = bound2;//一般设置为9600;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
		USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx;//接收模式

    USART_Init(USART2, &USART_InitStructure);//初始化串口2
		USART_ITConfig(USART2, USART_IT_RXNE,ENABLE);//开启接收中断
    USART_Cmd(USART2, ENABLE);                //使能串口 2
}

/*----------------------------------------
关闭接收，打开发送
-----------------------------------------*/
void CloseRX_OpenTX(void)
{   
	  USART1->CR1|= USART_Mode_Tx;	
	  USART1->CR1&= ~(USART_Mode_Rx); 
}

/*----------------------------------------
打开接收，关闭发送
-----------------------------------------*/
void OpenRX_CloseTX(void)
{     
	 	USART1->CR1|= (USART_Mode_Rx |USART_Mode_Tx);	
	  USART1->CR1&= ~( USART_Mode_Tx);	  
}

/*----------------------------------------
//发送里面采用的统一的子程序
 ----------------------------------------*/ 
void subprogram0()
{
	CloseRX_OpenTX();//发送时关闭接收使能

	USART_ClearFlag(USART1,USART_FLAG_TC|USART_FLAG_RXNE);//清除发送接收标志
	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);		    //关闭接收中断
	USART_ITConfig(USART1, USART_IT_TC, ENABLE);		      //开启TC发送中断
	delay_ms(1);	                                        //初始化后,需等待1bit延时(见资料) 
}

//脑波数据接收处理
void receive_del(void)
{
	u8 i = 0;	
	if(receive_ok)
	{
		receive_ok = 0;
		for(i = 0; i < 32; i++)
		{  
			Checksum += receive[i+3];
		}      
		Checksum = (~Checksum)&0xff; 
		if(Checksum == receive[35])
		{      
			Checksum = 0; 
			signalquality = 0;
			attention = 0;    
			meditation = 0;  
			
			signalquality = receive[4];     
			attention = receive[32];
			meditation = receive[34];
      
			if(signalquality!=29&&signalquality!=54&&signalquality!=55&&signalquality!=56&&signalquality!=80&&signalquality!=81&&signalquality!=82&&signalquality!=107&&signalquality!=200)
			{
				TX_CNT[0] =attention ; //注意力值
				TX_CNT[1] =meditation; //放松度值				
				TX_ADD=1;//再发送一个数据
				TX_lend=0;
				subprogram0();//发送时关闭接收		
				USART_SendData(USART1,TX_CNT[TX_lend]);//发送一个数据出去，启动发送					
			}
		}
	}
}


//串口1中断服务程序，用来将串口2接收过来的数据发送给电脑
void USART1_IRQHandler(void)                	
{
	//发送处理
  if(USART_GetITStatus(USART1, USART_IT_TC) != RESET)	
	{		
//    she=shi();
//		printf("\nshe:%d\n",she);
		if(TX_ADD)//数据没有发送完成，发送数据
		{	
			TX_ADD--;
			TX_lend++;
			USART_SendData(USART1,TX_CNT[TX_lend]);

		}
		else//数据发送完成
		{  
			OpenRX_CloseTX();//打开接收关闭发送
			USART_ClearFlag(USART1,USART_FLAG_TC|USART_FLAG_RXNE);//清除发送、接收标志
			USART_ITConfig(USART1, USART_IT_TC, DISABLE);		      //关闭TC发送中断
			USART_ITConfig(USART1, USART_IT_RXNE,ENABLE);		      //开启接收中断
		}
	}
} 

/***********************************************************************************************************************/
//串口2中断服务程序，用来处理蓝牙事件，只接收处理，不发送任何数据
//脑波数据蓝牙接收处理
void USART2_IRQHandler(void)                	
{
	u16 data=0;	
	static u8 count;
	//接收处理
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		data = USART_ReceiveData(USART2);//读取接收到的数据
		receive[count] = (u8)(data&0x00ff);//取低8位数据
		
		if(count==0&&receive[count]==0xAA)
    {  
      count++;  
    } 
    else if(count==1&&receive[count]==0xAA)
    {  
      count++;  
    }
    else if(count==2&&receive[count]==0x20)
    {  
      count++;  
    }    
    else if(count==3&&receive[count]==0x02)
    {  
      count++;  
    }  
    else if(count==4)
    {  
      count++;
    }
    else if(count==5&&receive[count]==0x83)
    {  
      count++;
    }
    else if(count==6&&receive[count]==0x18)
    {  
      count++;
    }
    else if (count >=7 && count < 35)
    {
      count++; 
    }        
    else if(count==35)
    {  
      count=0;    
      receive_ok = 1;
//			printf("2");
    }  
    else
    {  
      count=0;
    }
	}
} 


///重定向c库函数printf到串口，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{
		/* 发送一个字节数据到串口 */
		USART_SendData(USART1, (uint8_t) ch);
		
		/* 等待发送完毕 */
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}

///重定向c库函数scanf到串口，重写向后可使用scanf、getchar等函数
int fgetc(FILE *f)
{
		/* 等待串口输入数据 */
		while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(USART1);
}


