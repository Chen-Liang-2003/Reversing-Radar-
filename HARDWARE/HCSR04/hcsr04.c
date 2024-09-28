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

float v;//¶¨ÒåËÙ¶È
short temper; //¶¨ÒåÎÂ¶È
float v1;//¶¨ÒåËÙ¶È
short temper1; //¶¨ÒåÎÂ¶È

void Hcsr04Init()
{      
    GPIO_InitTypeDef GPIO_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	
    RCC_APB2PeriphClockCmd(HCSR04_CLK, ENABLE);
     
    //IO¿Ú³õÊ¼»¯   
    GPIO_InitStructure.GPIO_Pin =HCSR04_TRIG;       
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        //ÉèÖÃIO¿ÚÊä³öÄ£Ê½ÎªÍÆÍìÊä³ö
    GPIO_Init(HCSR04_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(HCSR04_PORT,HCSR04_TRIG);
	
    //IO¿Ú³õÊ¼»¯
    GPIO_InitStructure.GPIO_Pin =   HCSR04_ECHO;     
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   //ÉèÖÃIO¿ÚÊäÈëÄ£Ê½Îª¸¡¿ÕÊäÈë
    GPIO_Init(HCSR04_PORT, &GPIO_InitStructure);  
		GPIO_ResetBits(HCSR04_PORT,HCSR04_ECHO);

   //IO¿Ú³õÊ¼»¯   
    GPIO_InitStructure.GPIO_Pin =HCSR04_TRIG1;       
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        //ÉèÖÃIO¿ÚÊä³öÄ£Ê½ÎªÍÆÍìÊä³ö
    GPIO_Init(HCSR04_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(HCSR04_PORT,HCSR04_TRIG1);
	
    //IO¿Ú³õÊ¼»¯
    GPIO_InitStructure.GPIO_Pin =   HCSR04_ECHO1;     
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   //ÉèÖÃIO¿ÚÊäÈëÄ£Ê½Îª¸¡¿ÕÊäÈë
    GPIO_Init(HCSR04_PORT, &GPIO_InitStructure);  
		GPIO_ResetBits(HCSR04_PORT,HCSR04_ECHO1);	
 

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);   //Ê¹ÄÜ¶¨Ê±Æ÷Ê±Ö
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);   //Ê¹ÄÜ¶¨Ê±Æ÷Ê±ÖÓÓ
		

		TIM_TimeBaseStructure.TIM_Period = (1000-1); //ÉèÖÃÔÚÏÂÒ»¸ö¸üĞÂÊÂ¼ş×°Èë»î¶¯µÄ×Ô¶¯ÖØ×°ÔØ¼Ä´æÆ÷ÖÜÆÚµÄÖµ
		TIM_TimeBaseStructure.TIM_Prescaler =(72-1); //ÉèÖÃÓÃÀ´×÷ÎªTIM3Ê±ÖÓÆµÂÊ³ıÊıµÄÔ¤·ÖÆµ            
		TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;      //ÉèÖÃÊ±ÖÓ·Ö¸î:TDTS = Tck_tim
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //¸ù¾İÖ¸¶¨µÄ²ÎÊı³õÊ¼»¯TIMxµÄÊ±¼ä»ùÊıµ¥Î»
		TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 	
		TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 	 
		
		TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);    //Ê¹ÄÜÖ¸¶¨µÄTIM3ÖĞ¶Ï,ÔÊĞí¸üĞÂÖĞ¶Ï
		TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);    //Ê¹ÄÜÖ¸¶¨µÄTIM3ÖĞ¶Ï,ÔÊĞí¸üĞÂÖĞ¶Ï
		
		//ÖĞ¶ÏÓÅÏÈ¼¶NVICÉèÖÃ
	  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3ÖĞ¶Ï
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //ÏÈÕ¼ÓÅÏÈ¼¶0¼¶
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //´ÓÓÅÏÈ¼¶3¼¶
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQÍ¨µÀ±»Ê¹ÄÜ
	  NVIC_Init(&NVIC_InitStructure);  //³õÊ¼»¯NVIC¼Ä´æÆ÷
		
		TIM_Cmd(TIM3,DISABLE);     

		//ÖĞ¶ÏÓÅÏÈ¼¶NVICÉèÖÃ
	  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM3ÖĞ¶Ï
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //ÏÈÕ¼ÓÅÏÈ¼¶0¼¶
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //´ÓÓÅÏÈ¼¶3¼¶
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQÍ¨µÀ±»Ê¹ÄÜ
	  NVIC_Init(&NVIC_InitStructure);  //³õÊ¼»¯NVIC¼Ä´æÆ÷
		
		TIM_Cmd(TIM4,DISABLE);     
}
 
 
 

void OpenTimer()       
{
	TIM_SetCounter(TIM3,0);  //ÉèÖÃTIM3¼ÆÊı¼Ä´æÆ÷µÄÖµÎª0
	msHcCount = 0;   
	TIM_Cmd(TIM3, ENABLE);  //Ê¹ÄÜ¶¨Ê±Æ÷TIM3
}
void OpenTimer1()       
{
	TIM_SetCounter(TIM4,0);  //ÉèÖÃTIM3¼ÆÊı¼Ä´æÆ÷µÄÖµÎª0
	msHcCount1 = 0;   
	TIM_Cmd(TIM4, ENABLE);  //Ê¹ÄÜ¶¨Ê±Æ÷TIM3
}
 
void CloseTimer()        
{
	TIM_Cmd(TIM3, DISABLE);     //Í£Ö¹Ê¹ÄÜ¶¨Ê±Æ÷TIM3	
}
void CloseTimer1()        
{
	TIM_Cmd(TIM4, DISABLE);     //Í£Ö¹Ê¹ÄÜ¶¨Ê±Æ÷TIM3	
}
 
 
//¶¨Ê±Æ÷3ÖĞ¶Ï·şÎñ³ÌĞò
void TIM3_IRQHandler(void)   
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //¼ì²éTIM3µÄ¸üĞÂÖĞ¶Ï·¢ÉúÓë·ñ
	{
			TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );   //Çå³ıTIM3¸üĞÂÖĞ¶Ï±êÖ¾        
			msHcCount++;
	}
}

void TIM4_IRQHandler(void)   
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) //¼ì²éTIM3µÄ¸üĞÂÖĞ¶Ï·¢ÉúÓë·ñ
	{
			TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );   //Çå³ıTIM3¸üĞÂÖĞ¶Ï±êÖ¾        
			msHcCount1++;
	}
}
 
 

u32 GetEchoTimer(void)
{
	u32 t = 0;
	t = msHcCount*1000;     //½«Ê±¼ä×ª»¯ÎªÎ¢Ãë
	t += TIM_GetCounter(TIM3);   //»ñÈ¡µ±Ç°¼Ä´æÆ÷µÄÖµ
	TIM3->CNT = 0;  //ÇåÁã¼ÆÊıÆ÷
	delay_ms(50);
	return t;
}

u32 GetEchoTimer1(void)
{
	u32 t = 0;
	t = msHcCount1*1000;     //½«Ê±¼ä×ª»¯ÎªÎ¢Ãë
	t += TIM_GetCounter(TIM4);   //»ñÈ¡µ±Ç°¼Ä´æÆ÷µÄÖµ
	TIM4->CNT = 0;  //ÇåÁã¼ÆÊıÆ÷
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
		TRIG_Send = 1;    //PA5Êä³öÒ»¸ö¸ßµçÆ½   
		delay_us(20);
		TRIG_Send = 0;    //PA5Êä³öÒ»¸öµÍµçÆ½
		while(ECHO_Reci == 0);     
		OpenTimer();   //¿ªÊ¼¼ÆÊ±   
		while(ECHO_Reci == 1);
		CloseTimer();     //½áÊø¼ÆÊ± 
		t = GetEchoTimer();//µ¥Î»Îªus
		temper = DS18B20_Get_Temp();
		v = 331.4+0.6*temper;
		lengthTemp = (float) (v*t/10000.0/2.0);   //¼ÆËãµ¥´ÎÎïÌå¾àÀë   
		sum = sum + lengthTemp;
		}
	lengthTemp = sum/5.0; //ÇóÎå´Î¾àÀëµÄÆ½¾ùÖµ
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
		TRIG_Send1 = 1;    //PA5Êä³öÒ»¸ö¸ßµçÆ½   
		delay_us(20);
		TRIG_Send1 = 0;    //PA5Êä³öÒ»¸öµÍµçÆ½
		while(ECHO_Reci1 == 0);     
		OpenTimer1();   //¿ªÊ¼¼ÆÊ±   
		while(ECHO_Reci1 == 1);
		CloseTimer1();     //½áÊø¼ÆÊ± 
		t = GetEchoTimer1();//µ¥Î»Îªus
		temper1 = DS18B20_Get_Temp();
		v1 = 331.4+0.6*temper1;
		lengthTemp1 = (float) (v1*t/10000.0/2.0);   //¼ÆËãµ¥´ÎÎïÌå¾àÀë   
		sum = sum + lengthTemp1;
		}
	lengthTemp1 = sum/5.0; //ÇóÎå´Î¾àÀëµÄÆ½¾ùÖµ
	return lengthTemp1;
}
 
 






