#include "sys.h"
#include "usart.h"
#include "delay.h"
#include 	"stdio.h"
extern uint16_t CCR1_Val ;
//����1��������Զ�ͨѶ
//ֻ����
u8  TX_ADD=0;	
u8  TX_CNT[2]={0};//�����������ݸ����ԣ�һ����ע������һ���Ƿ��ɶ�	     
u8  TX_lend=0;
	  
//����2�����Բ�����ͨѶ
//����2ֻ�н���
u8 receive[36]={0};	//�Բ��������	
u8 receive_ok = 0;  //������ɱ�־
u8 attention = 0;    //ע����
u8 meditation = 0;   //���ɶ�
//�ź�����
u8 signalquality = 0;
//У���
u16  Checksum = 0;

//��ʼ��IO ����1 
//bound:������
void uart_init(u32 bound1,u32 bound2)
{
	
	  //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
	  USART_InitTypeDef USART_InitStructure;
	  NVIC_InitTypeDef NVIC_InitStructure;
	
	  //����1ʱ�� 
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
 	  USART_DeInit(USART1);  //��λ����1	
    USART_DeInit(USART2);  //��λ����2	    
	
	  //����1�������
	  //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA9
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA10

    //Usart1 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
		NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
    //USART1 ��ʼ������
		USART_InitStructure.USART_BaudRate = bound1;//һ������Ϊ9600;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
		USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;//����ģʽ	

    USART_Init(USART1, &USART_InitStructure); //��ʼ������1
		USART_ITConfig(USART1, USART_IT_RXNE,ENABLE);//���������ж�///////////////!!!!!!������һ����ղ�������
    USART_Cmd(USART1, ENABLE);                //ʹ�ܴ��� 1
		
		//����2�������
		//USART2_TX   PA.2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA2
   
    //USART2_RX	  PA.3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA3
		
		//Usart2 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
		NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
		
		//USART2 ��ʼ������
		USART_InitStructure.USART_BaudRate = bound2;//һ������Ϊ9600;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
		USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx;//����ģʽ

    USART_Init(USART2, &USART_InitStructure);//��ʼ������2
		USART_ITConfig(USART2, USART_IT_RXNE,ENABLE);//���������ж�
    USART_Cmd(USART2, ENABLE);                //ʹ�ܴ��� 2
}

/*----------------------------------------
�رս��գ��򿪷���
-----------------------------------------*/
void CloseRX_OpenTX(void)
{   
	  USART1->CR1|= USART_Mode_Tx;	
	  USART1->CR1&= ~(USART_Mode_Rx); 
}

/*----------------------------------------
�򿪽��գ��رշ���
-----------------------------------------*/
void OpenRX_CloseTX(void)
{     
	 	USART1->CR1|= (USART_Mode_Rx |USART_Mode_Tx);	
	  USART1->CR1&= ~( USART_Mode_Tx);	  
}

/*----------------------------------------
//����������õ�ͳһ���ӳ���
 ----------------------------------------*/ 
void subprogram0()
{
	CloseRX_OpenTX();//����ʱ�رս���ʹ��

	USART_ClearFlag(USART1,USART_FLAG_TC|USART_FLAG_RXNE);//������ͽ��ձ�־
	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);		    //�رս����ж�
	USART_ITConfig(USART1, USART_IT_TC, ENABLE);		      //����TC�����ж�
	delay_ms(1);	                                        //��ʼ����,��ȴ�1bit��ʱ(������) 
}

//�Բ����ݽ��մ���
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
				TX_CNT[0] =attention ; //ע����ֵ
				TX_CNT[1] =meditation; //���ɶ�ֵ				
				TX_ADD=1;//�ٷ���һ������
				TX_lend=0;
				subprogram0();//����ʱ�رս���		
				USART_SendData(USART1,TX_CNT[TX_lend]);//����һ�����ݳ�ȥ����������					
			}
		}
	}
}


//����1�жϷ����������������2���չ��������ݷ��͸�����
void USART1_IRQHandler(void)                	
{
	//���ʹ���
  if(USART_GetITStatus(USART1, USART_IT_TC) != RESET)	
	{		
//    she=shi();
//		printf("\nshe:%d\n",she);
		if(TX_ADD)//����û�з�����ɣ���������
		{	
			TX_ADD--;
			TX_lend++;
			USART_SendData(USART1,TX_CNT[TX_lend]);

		}
		else//���ݷ������
		{  
			OpenRX_CloseTX();//�򿪽��չرշ���
			USART_ClearFlag(USART1,USART_FLAG_TC|USART_FLAG_RXNE);//������͡����ձ�־
			USART_ITConfig(USART1, USART_IT_TC, DISABLE);		      //�ر�TC�����ж�
			USART_ITConfig(USART1, USART_IT_RXNE,ENABLE);		      //���������ж�
		}
	}
} 

/***********************************************************************************************************************/
//����2�жϷ�������������������¼���ֻ���մ����������κ�����
//�Բ������������մ���
void USART2_IRQHandler(void)                	
{
	u16 data=0;	
	static u8 count;
	//���մ���
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		data = USART_ReceiveData(USART2);//��ȡ���յ�������
		receive[count] = (u8)(data&0x00ff);//ȡ��8λ����
		
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


///�ض���c�⺯��printf�����ڣ��ض�����ʹ��printf����
int fputc(int ch, FILE *f)
{
		/* ����һ���ֽ����ݵ����� */
		USART_SendData(USART1, (uint8_t) ch);
		
		/* �ȴ�������� */
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}

///�ض���c�⺯��scanf�����ڣ���д����ʹ��scanf��getchar�Ⱥ���
int fgetc(FILE *f)
{
		/* �ȴ������������� */
		while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(USART1);
}


