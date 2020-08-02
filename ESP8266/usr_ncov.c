#include "usr_ncov.h"
#include "time.h"

struct ncov_data dataChina = {12345678, -100, 23456789, 456, 4567890, -920, 123445, 12324, 12324, 123412, "06-13 16:22"};
struct ncov_data dataGlobal = {12345678, -100, 23456789, 456, 4567890, -920, 0, 0, 0, 0, NULL};

extern void gui_show_ncov_data(struct ncov_data china, struct ncov_data global);


uint8_t build_ssl_connect(char *type, char *ip, char *port)
{
    char str_tmp[200];
    uint8_t TryConnectTime = 0;
    atk_8266_send_cmd("AT+CIPMUX=0", "OK", 100);   
    atk_8266_send_cmd("AT+CIPSSLSIZE=4096", "OK", 100);  

    sprintf((char *)str_tmp, "AT+CIPSTART=\"%s\",\"%s\",%s", type, ip, port); 
    
    while (atk_8266_send_cmd((uint8_t *)str_tmp, "OK", 200))
    {
        printf("api连接失败,正在尝试第 %d 次连接\r\r\n", TryConnectTime++);
        delay_ms(100);
        if (TryConnectTime >= 10)
        {
            return 1;
        }
    };  
    
    delay_ms(300);
    atk_8266_send_cmd("AT+CIPMODE=1", "OK", 100);    //传输模式为：透传

    USART2_RX_STA = 0;
    atk_8266_send_cmd("AT+CIPSEND", "OK", 100);       //开始透传
    return 0;
}

uint8_t get_ncov_api(char *api_addr, uint8_t (*parse_fun)(void))
{
    printf("获取疫情数据:GET %s\r\r\n", api_addr);
    u2_printf("GET %s\r\n\r\n", api_addr);

    delay_ms(20);
    USART2_RX_STA = 0;
    delay_ms(1000);

    if (USART2_RX_STA & 0X8000)     
    {
        USART2_RX_BUF[USART2_RX_STA & 0X7FFF] = 0; //添加结束符
    }
   
//	printf("接收到的数据:%d-%d\r\r\n%s\r\r\n", sizeof(USART2_RX_BUF), strlen((const char*)USART2_RX_BUF),USART2_RX_BUF);	//JSON原始数据
   
    parse_fun();
    USART2_RX_STA = 0;
    memset(USART2_RX_BUF, 0, sizeof(USART2_RX_BUF));
//    atk_8266_quit_trans();
//    printf("退出透传\r\n");
    return 0;
}

uint8_t parse_ncov_data(void)
{
    int ret = 0;
    cJSON *root, *result_arr;
    cJSON *result, *global;
    time_t updateTime;
    struct tm *time;

    //root = cJSON_Parse((const char *)str);   //创建JSON解析对象，返回JSON格式是否正确
    printf("接收到的数据:%d\r\r\n", strlen((const char*)USART2_RX_BUF));	//JSON原始数据
    root = cJSON_Parse((const char*)USART2_RX_BUF);
    
    if (root != 0)
    {
        printf("JSON format ok, start parse!!!\r\n");
        result_arr = cJSON_GetObjectItem(root, "results");
        if(result_arr->type == cJSON_Array)
        {
            printf("result is array\r\n");
            result = cJSON_GetArrayItem(result_arr, 0);
            if(result->type == cJSON_Object)
            {
                printf("result_arr[0] is object\r\n");

                /* china data parse */
                dataChina.currentConfirmedCount = cJSON_GetObjectItem(result, "currentConfirmedCount")->valueint;
                dataChina.currentConfirmedIncr = cJSON_GetObjectItem(result, "currentConfirmedIncr")->valueint;
                dataChina.confirmedCount = cJSON_GetObjectItem(result, "confirmedCount")->valueint;
                dataChina.confirmedIncr = cJSON_GetObjectItem(result, "confirmedIncr")->valueint;
                dataChina.curedCount = cJSON_GetObjectItem(result, "curedCount")->valueint;
                dataChina.curedIncr = cJSON_GetObjectItem(result, "curedIncr")->valueint;
                dataChina.deadCount = cJSON_GetObjectItem(result, "deadCount")->valueint;
                dataChina.deadIncr = cJSON_GetObjectItem(result, "deadIncr")->valueint;

                printf("------------国内疫情-------------\r\n");
                printf("现存确诊:   %5d, 较昨日:%3d\r\n", dataChina.currentConfirmedCount, dataChina.currentConfirmedIncr);
                printf("累计确诊:   %5d, 较昨日:%3d\r\n", dataChina.confirmedCount, dataChina.confirmedIncr);
                printf("累计治愈:   %5d, 较昨日:%3d\r\n", dataChina.curedCount, dataChina.curedIncr);
                printf("累计死亡:   %5d, 较昨日:%3d\r\n", dataChina.deadCount, dataChina.deadIncr);
                printf("现存无症状: %5d, 较昨日:%3d\r\n\r\n", dataChina.seriousCount, dataChina.seriousIncr);

                global = cJSON_GetObjectItem(result, "globalStatistics");
                if(global->type == cJSON_Object)
                {
                    dataGlobal.currentConfirmedCount = cJSON_GetObjectItem(global, "currentConfirmedCount")->valueint;
                    dataGlobal.currentConfirmedIncr = cJSON_GetObjectItem(global, "currentConfirmedIncr")->valueint;
                    dataGlobal.confirmedCount = cJSON_GetObjectItem(global, "confirmedCount")->valueint;
                    dataGlobal.confirmedIncr = cJSON_GetObjectItem(global, "confirmedIncr")->valueint;
                    dataGlobal.curedCount = cJSON_GetObjectItem(global, "curedCount")->valueint;
                    dataGlobal.curedIncr = cJSON_GetObjectItem(global, "curedIncr")->valueint;
                    dataGlobal.deadCount = cJSON_GetObjectItem(global, "deadCount")->valueint;
                    dataGlobal.deadIncr = cJSON_GetObjectItem(global, "deadIncr")->valueint;

                    printf("\r\n**********global ncov data**********\r\n");

                    printf("------------全球疫情-------------\r\n");
                    printf("现存确诊: %8d, 较昨日:%5d\r\n", dataGlobal.currentConfirmedCount, dataGlobal.currentConfirmedIncr);
                    printf("累计确诊: %8d, 较昨日:%5d\r\n", dataGlobal.confirmedCount, dataGlobal.confirmedIncr);
                    printf("累计死亡: %8d, 较昨日:%5d\r\n", dataGlobal.deadCount, dataGlobal.deadIncr);
                    printf("累计治愈: %8d, 较昨日:%5d\r\n\r\n", dataGlobal.curedCount, dataGlobal.curedIncr);

                } else return 1;
                
                
                /* 毫秒级时间戳转字符串 */
                updateTime = (time_t )(cJSON_GetObjectItem(result, "updateTime")->valuedouble / 1000);
                updateTime += 8 * 60 * 60; /* UTC8校正 */
                time = localtime(&updateTime);
                /* 格式化时间 */
                strftime(dataChina.updateTime, 20, "%m-%d %H:%M", time);
                printf("更新于:%s\r\n", dataChina.updateTime);/* 06-24 11:21 */
            } else return 1;
        } else return 1;
        printf("\r\nparse complete \r\n");
        gui_show_ncov_data(dataChina, dataGlobal);
    }
    else
    {
        printf("JSON format error:%s\r\n", cJSON_GetErrorPtr()); //输出json格式错误信息
        return 1;
    }
    cJSON_Delete(root);

    return ret;
}

