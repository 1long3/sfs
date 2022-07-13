
// 脑电波程序，可控制电机，led灯等其他设备。
#include "stm32f10x.h"
#include "bsp_GeneralTim.h"
#include "bsp_i2c_gpio.h"
#include 	"sys.h"
#include "delay.h"
#include "usart.h"
#include "stdio.h"
#include "OLED_I2C.h"

GPIO_InitTypeDef GPIO_InitStructure;
/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
	extern uint16_t CCR1_Val ;
	extern u8 attention;
	extern u8 meditation;
	
int main(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5|GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	SystemInit(); 			   //系统时钟初始化为72M	  SYSCLK_FREQ_72MHz
	delay_init();	    	   //延时函数初始化	
	i2c_CfgGpio();
	NVIC_Configuration();  //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(9600,57600);  //串口1串口2初始化    	
	GENERAL_TIM_Init();/* 定时器初始化 */        
	OLED_Init();
	OLED_Fill(0x00);//全屏灭
  delay_ms(500); //延时一段时间
	OLED_ShowCN(5, 5, 0);//专注力：
	OLED_ShowCN(25, 5, 1);
	OLED_ShowCN(45, 5, 2);
	OLED_ShowCN(65, 5, 3);
	OLED_ShowCN(5, 2, 4);//放松度：
	OLED_ShowCN(25, 2, 5);
	OLED_ShowCN(45, 2, 6);
	OLED_ShowCN(65, 2, 7);
  while(1)
  { 
	GPIO_WriteBit(GPIOB,GPIO_Pin_4,Bit_SET);
	GPIO_WriteBit(GPIOB,GPIO_Pin_5,Bit_RESET);
	GPIO_WriteBit(GPIOB,GPIO_Pin_6,Bit_SET);
	TIM_SetCompare1(TIM3,attention);	
	receive_del();
	
	OLED_Show_num(75,5,attention,2);
	OLED_Show_num(75,2,meditation,2);
  delay_ms(1000); 
	
  }
}

/*********************************************END OF FILE**********************/


