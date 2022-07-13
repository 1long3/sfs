#include <stm32f10x.h>

#include 	"sys.h"
#include	"delay.h"
#include	"usart.h"
#include 	"stdio.h"

int main(void)
{		 	
	SystemInit(); 			   //ϵͳʱ�ӳ�ʼ��Ϊ72M	  SYSCLK_FREQ_72MHz
	delay_init();	    	   //��ʱ������ʼ��	  
	NVIC_Configuration();  //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(9600,57600);  //����1����2��ʼ��    
	delay_ms(500);         //��ʱһ��ʱ��	
	while(1)
	{		
		receive_del();
		delay_ms(1);
	}	
}
