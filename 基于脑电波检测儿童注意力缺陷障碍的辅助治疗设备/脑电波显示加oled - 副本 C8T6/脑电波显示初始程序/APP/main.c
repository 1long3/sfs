#include <stm32f10x.h>

#include 	"sys.h"
#include	"delay.h"
#include	"usart.h"
#include 	"stdio.h"

int main(void)
{		 	
	SystemInit(); 			   //系统时钟初始化为72M	  SYSCLK_FREQ_72MHz
	delay_init();	    	   //延时函数初始化	  
	NVIC_Configuration();  //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(9600,57600);  //串口1串口2初始化    
	delay_ms(500);         //延时一段时间	
	while(1)
	{		
		receive_del();
		delay_ms(1);
	}	
}
