#include "wifista.h"
#include "usart.h"
#include "delay.h"
#include "drv_usart2.h"
#include "malloc.h"
#include "stdlib.h"
#include "string.h"
#include "config.h"

//将收到的AT指令应答数据返回给电脑串口
//mode:0,不清零USART2_RX_STA;
//     1,清零USART2_RX_STA;
void atk_8266_at_response(u8 mode)
{
    if (USART2_RX_STA & 0X8000)     //接收到一次数据了
    {
        USART2_RX_BUF[USART2_RX_STA & 0X7FFF] = 0; //添加结束符
        printf("%s", USART2_RX_BUF); //发送到串口
        if (mode)USART2_RX_STA = 0;
    }
}
//ATK-ESP8266发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果
//    其他,期待应答结果的位置(str的位置)
u8 *atk_8266_check_cmd(u8 *str)
{
    char *strx = 0;
    if (USART2_RX_STA & 0X8000)     //接收到一次数据了
    {
        USART2_RX_BUF[USART2_RX_STA & 0X7FFF] = 0; //添加结束符
        strx = strstr((const char *)USART2_RX_BUF, (const char *)str);
    }
    return (u8 *)strx;
}
//向ATK-ESP8266发送命令
//cmd:发送的命令字符串
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
u8 atk_8266_send_cmd(u8 *cmd, u8 *ack, u16 waittime)
{
    u8 res = 0;
    USART2_RX_STA = 0;
    u2_printf("%s\r\n", cmd);   //发送命令
    if (ack && waittime)    //需要等待应答
    {
        while (--waittime)  //等待倒计时
        {
            delay_ms(10);
            if (USART2_RX_STA & 0X8000) //接收到期待的应答结果
            {
                if (atk_8266_check_cmd(ack))
                {
                    printf("发送：%s 回应:%s\r\n", cmd, (u8 *)ack);
                    break;//得到有效数据
                }
                USART2_RX_STA = 0;
            }
        }
        if (waittime == 0)res = 1;
    }
    return res;
}
//向ATK-ESP8266发送指定数据
//data:发送的数据(不需要添加回车了)
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)luojian
u8 atk_8266_send_data(u8 *data, u8 *ack, u16 waittime)
{
    u8 res = 0;
    USART2_RX_STA = 0;
    u2_printf("%s", data);  //发送命令
    if (ack && waittime)    //需要等待应答
    {
        while (--waittime)  //等待倒计时
        {
            delay_ms(10);
            if (USART2_RX_STA & 0X8000) //接收到期待的应答结果
            {
                if (atk_8266_check_cmd(ack))break; //得到有效数据
                USART2_RX_STA = 0;
            }
        }
        if (waittime == 0)res = 1;
    }
    return res;
}
//ATK-ESP8266退出透传模式
//返回值:0,退出成功;
//       1,退出失败
u8 atk_8266_quit_trans(void)
{
    while ((USART2->SR & 0X40) == 0); //等待发送空
    USART2->DR = '+';
    delay_ms(15);                   //大于串口组帧时间(10ms)
    while ((USART2->SR & 0X40) == 0); //等待发送空
    USART2->DR = '+';
    delay_ms(15);                   //大于串口组帧时间(10ms)
    while ((USART2->SR & 0X40) == 0); //等待发送空
    USART2->DR = '+';
    delay_ms(500);                  //等待500ms
    return atk_8266_send_cmd("AT", "OK", 20); //退出透传判断.
}

//获取ATK-ESP8266模块的连接状态
//返回值:0,未连接;1,连接成功.
u8 atk_8266_consta_check(void)
{
    u8 *p;
    u8 res;
    if (atk_8266_quit_trans())return 0;         //退出透传
    atk_8266_send_cmd("AT+CIPSTATUS", ":", 50); //发送AT+CIPSTATUS指令,查询连接状态
    p = atk_8266_check_cmd("+CIPSTATUS:");
    res = *p;                               //得到连接状态
    return res;
}

//获取Client ip地址
//ipbuf:ip地址输出缓存区
void atk_8266_get_wanip(u8 *ipbuf)
{
    u8 *p, *p1;
    if (atk_8266_send_cmd("AT+CIFSR", "OK", 50)) //获取WAN IP地址失败
    {
        ipbuf[0] = 0;
        return;
    }
    p = atk_8266_check_cmd("\"");
    p1 = (u8 *)strstr((const char *)(p + 1), "\"");
    *p1 = 0;
    sprintf((char *)ipbuf, "%s", p + 1);
}


//配置ESP8266位sta模式，并连接到路由器
u8 atk_8266_wifista_config(void)
{
    u8 p[200];
	char str[200];
    u8 TryConnectTime = 1;
	printf("准备连接\r\n");
    while (atk_8266_send_cmd("AT", "OK", 20)) //检查WIFI模块是否在线
    {
        printf("未检测到模块\r\n");
        atk_8266_quit_trans();//退出透传
		delay_ms(1000);
    }
    atk_8266_send_cmd("AT+RESTORE", "OK", 200); //关闭透传模式
    delay_ms(1000);
    delay_ms(1000);
    delay_ms(1000);
    
    atk_8266_send_cmd("AT+CIPMODE=0", "OK", 200); //关闭透传模式
    while (atk_8266_send_cmd("ATE0", "OK", 20)); //关闭回显
    atk_8266_send_cmd("AT+CWMODE=1", "OK", 50);     //设置WIFI STA模式
//    atk_8266_send_cmd("AT+RST", "OK", 20);      //DHCP服务器关闭(仅AP模式有效)
	delay_ms(1000);         //延时3S等待重启成功
    delay_ms(1000);
    delay_ms(1000);
    delay_ms(1000);
    //设置连接到的WIFI网络名称/加密方式/密码,这几个参数需要根据您自己的路由器设置进行修改!!
    atk_8266_send_cmd("AT+CIPMUX=0", "OK", 20); //0：单连接，1：多连接
    sprintf((char *)p, "AT+CWJAP=\"%s\",\"%s\"", wifista_ssid, wifista_password); //设置无线参数:ssid,密码

	sprintf((char *)str, "%s", wifista_ssid);
//	LCD_ShowString(125, 20, 200, 16, 16, str);

	while (atk_8266_send_cmd(p, "WIFI GOT IP", 300))
    {
        printf("WiFi连接失败,正在尝试第 %d 次连接\r\n", TryConnectTime++);
        if (TryConnectTime >= 250)
            TryConnectTime = 0;
        delay_ms(100);
    };                  //连接目标路由器,并且获得IP
    printf("WiFi连接成功\r\n");
//	LCD_ShowString(0,60,100, 16, 16, "WiFi Connect Success!");
    return 0;
}

