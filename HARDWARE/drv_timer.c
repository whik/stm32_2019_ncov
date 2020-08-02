#include "drv_timer.h"
#include "drv_led.h"

void TIM3_Int_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能

    TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值     计数到5000为500ms
    TIM_TimeBaseStructure.TIM_Prescaler = psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);  //使能指定的TIM3中断,允许更新中断

    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
    NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

    TIM_Cmd(TIM3, ENABLE);  //使能TIMx外设
}
extern vu16 USART2_RX_STA;
//定时器3中断服务程序
void TIM3_IRQHandler(void)   //TIM3中断
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)//是更新中断
    {
        USART2_RX_STA |= 1 << 15; //标记接收完成
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);    //清除TIM7更新中断标志
        TIM_Cmd(TIM3, DISABLE);  //关闭TIM7
    }
}



extern __IO int32_t OS_TimeMS;

//基本定时器6中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器6!
void TIM6_Int_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); //定时器6时钟使能

    TIM_TimeBaseInitStructure.TIM_Prescaler = psc; //设置分频值，10khz的计数频率
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数模式
    TIM_TimeBaseInitStructure.TIM_Period = arr; //自动重装载值 计数到5000为500ms
    TIM_TimeBaseInitStructure.TIM_ClockDivision = 0; //时钟分割:TDS=Tck_Tim
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseInitStructure);

    TIM_ITConfig(TIM6, TIM_IT_Update | TIM_IT_Trigger, ENABLE); //使能TIM6的更新中断

    NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn; //TIM6中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //先占优先级1级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //从优先级3级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //使能通道
    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM6, ENABLE); //定时器6使能
}

void TIM6_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
    {
//      OS_TimeMS++;
    }
    TIM_ClearITPendingBit(TIM6, TIM_IT_Update); //清除中断标志位
}



