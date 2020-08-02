#include "delay.h"
#include "drv_usart2.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "drv_timer.h"

//串口接收缓存区
u8 USART2_RX_BUF[USART2_MAX_RECV_LEN];              //接收缓冲,最大USART2_MAX_RECV_LEN个字节.
u8  USART2_TX_BUF[USART2_MAX_SEND_LEN];             //发送缓冲,最大USART2_MAX_SEND_LEN字节

//通过判断接收连续2个字符之间的时间差不大于10ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到
//任何数据,则表示此次接收完毕.
//接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14:0]:接收到的数据长度
vu16 USART2_RX_STA = 0;


void USART2_IRQHandler(void)
{
    u8 res;
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //接收到数据
    {
        res = USART_ReceiveData(USART2);
        if ((USART2_RX_STA & (1 << 15)) == 0) //接收完的一批数据,还没有被处理,则不再接收其他数据
        {
            if (USART2_RX_STA < USART2_MAX_RECV_LEN) //还可以接收数据
            {
                TIM_SetCounter(TIM3, 0); //计数器清空                         //计数器清空
                if (USART2_RX_STA == 0)             //使能定时器3的中断
                {
                    TIM_Cmd(TIM3, ENABLE); //使能定时器3
                }
                USART2_RX_BUF[USART2_RX_STA++] = res; //记录接收到的值
            }
            else
            {
                USART2_RX_STA |= 1 << 15;           //强制标记接收完成
            }
        }
    }
}


//初始化IO 串口3
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率
void usart2_init(u32 bound)
{
    GPIO_InitTypeDef gpio_initstruct;
    USART_InitTypeDef usart_initstruct;
    NVIC_InitTypeDef nvic_initstruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    //PA2   TXD
    gpio_initstruct.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio_initstruct.GPIO_Pin = GPIO_Pin_2;
    gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio_initstruct);
    //PA3   RXD
    gpio_initstruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpio_initstruct.GPIO_Pin = GPIO_Pin_3;
    gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio_initstruct);

    usart_initstruct.USART_BaudRate = bound;
    usart_initstruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;        //无硬件流控
    usart_initstruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                        //接收和发送
    usart_initstruct.USART_Parity = USART_Parity_No;                                    //无校验
    usart_initstruct.USART_StopBits = USART_StopBits_1;                             //1位停止位
    usart_initstruct.USART_WordLength = USART_WordLength_8b;                            //8位数据位
    USART_Init(USART2, &usart_initstruct);
    USART_Cmd(USART2, ENABLE);                                                      //使能串口
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);                                  //使能接收中断

    nvic_initstruct.NVIC_IRQChannel = USART2_IRQn;
    nvic_initstruct.NVIC_IRQChannelCmd = ENABLE;
    nvic_initstruct.NVIC_IRQChannelPreemptionPriority = 0;
    nvic_initstruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&nvic_initstruct);

    TIM3_Int_Init(1000 - 1, 7200 - 1);  //10ms中断
    USART2_RX_STA = 0;      //清零
    TIM_Cmd(TIM3, DISABLE);         //关闭定时器7
}

//串口3,printf 函数
//确保一次发送数据不超过USART2_MAX_SEND_LEN字节
void u2_printf(char *fmt, ...)
{
    u16 i, j;
    va_list ap;
    va_start(ap, fmt);
    vsprintf((char *)USART2_TX_BUF, fmt, ap);
    va_end(ap);
    i = strlen((const char *)USART2_TX_BUF);    //此次发送数据的长度
    for (j = 0; j < i; j++)                     //循环发送数据
    {
        while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET); //循环发送,直到发送完毕
        USART_SendData(USART2, USART2_TX_BUF[j]);
    }
}
void Usart_SendString(USART_TypeDef *USARTx, u8 *str)
{
    while (*str)
    {
        USART_SendData(USARTx, *str);                                   //发送数据
        while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);    //等待发送完成
        str++;
    }
}


