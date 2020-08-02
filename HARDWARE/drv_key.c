#include "drv_key.h"
#include "sys.h"
#include "delay.h"
//K1-PA0 K2-PA1

//按键初始化函数
void key_init(void) //IO初始化
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

}
//K1-PA0 K2-PA1

//0 有效一次，1支持连按
//返回值1-K1,2-K2,0-无
u8 key_scan(u8 mode)
{
    static u8 key_up = 1; //按键按松开标志
    if (mode)key_up = 1; //支持连按
    if (key_up && (K1 == 0 || K2 == 0))
    {
        delay_ms(10);//去抖动
        key_up = 0;
        if (K1 == 0)
            return 1;
        else if (K2 == 0)
            return 2;
    }
    else if (K1 == 1 && K2 == 1)key_up = 1;
    return 0;// 无按键按下
}
