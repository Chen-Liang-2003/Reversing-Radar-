#include "hcsr04.h"
#include "sys.h"
#include "delay.h"
#include "ds18b20.h" 

#define HCSR04_PORT     GPIOF
#define HCSR04_CLK      RCC_APB2Periph_GPIOF
#define HCSR04_TRIG     GPIO_Pin_1
#define HCSR04_ECHO     GPIO_Pin_0
#define HCSR04_TRIG1     GPIO_Pin_3
#define HCSR04_ECHO1     GPIO_Pin_2

#define TRIG_Send  PFout(1) 
#define ECHO_Reci  PFin(0)
#define TRIG_Send1  PFout(3) 
#define ECHO_Reci1  PFin(2)


u16 msHcCount = 0;
u16 msHcCount1 = 0;

float v;//�����ٶ�
short temper; //�����¶�
float v1;//�����ٶ�
short temper1; //�����¶�

void Hcsr04Init()
{      
    GPIO_InitTypeDef GPIO_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	
    RCC_APB2PeriphClockCmd(HCSR04_CLK, ENABLE);
     
    //IO�ڳ�ʼ��   
    GPIO_InitStructure.GPIO_Pin =HCSR04_TRIG;       
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        //����IO�����ģʽΪ�������
    GPIO_Init(HCSR04_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(HCSR04_PORT,HCSR04_TRIG);
	
    //IO�ڳ�ʼ��
    GPIO_InitStructure.GPIO_Pin =   HCSR04_ECHO;     
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   //����IO������ģʽΪ��������
    GPIO_Init(HCSR04_PORT, &GPIO_InitStructure);  
		GPIO_ResetBits(HCSR04_PORT,HCSR04_ECHO);

   //IO�ڳ�ʼ��   
    GPIO_InitStructure.GPIO_Pin =HCSR04_TRIG1;       
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        //����IO�����ģʽΪ�������
    GPIO_Init(HCSR04_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(HCSR04_PORT,HCSR04_TRIG1);
	
    //IO�ڳ�ʼ��
    GPIO_InitStructure.GPIO_Pin =   HCSR04_ECHO1;     
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   //����IO������ģʽΪ��������
    GPIO_Init(HCSR04_PORT, &GPIO_InitStructure);  
		GPIO_ResetBits(HCSR04_PORT,HCSR04_ECHO1);	
 

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);   //ʹ�ܶ�ʱ��ʱ�
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);   //ʹ�ܶ�ʱ��ʱ���
		

		TIM_TimeBaseStructure.TIM_Period = (1000-1); //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
		TIM_TimeBaseStructure.TIM_Prescaler =(72-1); //����������ΪTIM3ʱ��Ƶ�ʳ�����Ԥ��Ƶ            
		TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;      //����ʱ�ӷָ�:TDTS = Tck_tim
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
		TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 	
		TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 	 
		
		TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);    //ʹ��ָ����TIM3�ж�,��������ж�
		TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);    //ʹ��ָ����TIM3�ж�,��������ж�
		
		//�ж����ȼ�NVIC����
	  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	  NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���
		
		TIM_Cmd(TIM3,DISABLE);     

		//�ж����ȼ�NVIC����
	  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM3�ж�
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	  NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���
		
		TIM_Cmd(TIM4,DISABLE);     
}
 
 
 

void OpenTimer()       
{
	TIM_SetCounter(TIM3,0);  //����TIM3�����Ĵ�����ֵΪ0
	msHcCount = 0;   
	TIM_Cmd(TIM3, ENABLE);  //ʹ�ܶ�ʱ��TIM3
}
void OpenTimer1()       
{
	TIM_SetCounter(TIM4,0);  //����TIM3�����Ĵ�����ֵΪ0
	msHcCount1 = 0;   
	TIM_Cmd(TIM4, ENABLE);  //ʹ�ܶ�ʱ��TIM3
}
 
void CloseTimer()        
{
	TIM_Cmd(TIM3, DISABLE);     //ֹͣʹ�ܶ�ʱ��TIM3	
}
void CloseTimer1()        
{
	TIM_Cmd(TIM4, DISABLE);     //ֹͣʹ�ܶ�ʱ��TIM3	
}
 
 
//��ʱ��3�жϷ������
void TIM3_IRQHandler(void)   
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //���TIM3�ĸ����жϷ������
	{
			TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );   //���TIM3�����жϱ�־        
			msHcCount++;
	}
}

void TIM4_IRQHandler(void)   
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) //���TIM3�ĸ����жϷ������
	{
			TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );   //���TIM3�����жϱ�־        
			msHcCount1++;
	}
}
 
 

u32 GetEchoTimer(void)
{
	u32 t = 0;
	t = msHcCount*1000;     //��ʱ��ת��Ϊ΢��
	t += TIM_GetCounter(TIM3);   //��ȡ��ǰ�Ĵ�����ֵ
	TIM3->CNT = 0;  //���������
	delay_ms(50);
	return t;
}

u32 GetEchoTimer1(void)
{
	u32 t = 0;
	t = msHcCount1*1000;     //��ʱ��ת��Ϊ΢��
	t += TIM_GetCounter(TIM4);   //��ȡ��ǰ�Ĵ�����ֵ
	TIM4->CNT = 0;  //���������
	delay_ms(50);
	return t;
}
 
 

float Hcsr04GetLength(void )
{
	u32 t = 0;
	int i = 0;
	float lengthTemp = 0;
	float sum = 0;
		while (i!=5){
		i++;
		TRIG_Send = 1;    //PA5���һ���ߵ�ƽ   
		delay_us(20);
		TRIG_Send = 0;    //PA5���һ���͵�ƽ
		while(ECHO_Reci == 0);     
		OpenTimer();   //��ʼ��ʱ   
		while(ECHO_Reci == 1);
		CloseTimer();     //������ʱ 
		t = GetEchoTimer();//��λΪus
		temper = DS18B20_Get_Temp();
		v = 331.4+0.6*temper;
		lengthTemp = (float) (v*t/10000.0/2.0);   //���㵥���������   
		sum = sum + lengthTemp;
		}
	lengthTemp = sum/5.0; //����ξ����ƽ��ֵ
	return lengthTemp;
}

float Hcsr04GetLength1(void )
{
	u32 t = 0;
	int i = 0;
	float lengthTemp1 = 0;
	float sum = 0;
		while (i!=5){
		i++;
		TRIG_Send1 = 1;    //PA5���һ���ߵ�ƽ   
		delay_us(20);
		TRIG_Send1 = 0;    //PA5���һ���͵�ƽ
		while(ECHO_Reci1 == 0);     
		OpenTimer1();   //��ʼ��ʱ   
		while(ECHO_Reci1 == 1);
		CloseTimer1();     //������ʱ 
		t = GetEchoTimer1();//��λΪus
		temper1 = DS18B20_Get_Temp();
		v1 = 331.4+0.6*temper1;
		lengthTemp1 = (float) (v1*t/10000.0/2.0);   //���㵥���������   
		sum = sum + lengthTemp1;
		}
	lengthTemp1 = sum/5.0; //����ξ����ƽ��ֵ
	return lengthTemp1;
}
 
 






