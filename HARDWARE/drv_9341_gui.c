#include "drv_9341_gui.h"
#include "font.h"
#include "math.h"
#include "string.h"
#include "stdio.h"

/* 画直线，水平或垂直方向 */
void gui_drawLine(uint16_t x0, uint16_t y0, uint16_t len, uint16_t direction, uint16_t color)
{
    uint16_t x, y;
    uint16_t size_x, size_y;

    if(direction == DIR_Y)
    {
        size_y = y0 + len;
        if(len > LCD_SIZE_X)
            size_y = LCD_SIZE_X;
        for(y = y0; y < size_y; y++)
            lcd_drawPoint(x0, y, color);
    }
    else if(direction == DIR_X)
    {
        size_x = x0 + len;
        if(len > LCD_SIZE_Y)
            size_x = LCD_SIZE_Y;
        for(x = x0; x < size_x; x++)
            lcd_drawPoint(x, y0, color);
    }
}

/* 画点 */
void gui_draw_point(uint16_t x0, uint16_t y0, uint16_t en, uint16_t color)
{
    if(en)
        lcd_drawPoint(x0, y0, color);
}


#ifdef FONT_6X8
/* 显示6X8字体的字符，idx查询字库索引 */
void gui_show_F6X8_Char(uint16_t x0, uint16_t y0, uint16_t idx, uint16_t color)
{
    uint16_t x, y;
    for(y = y0; y < y0+8; y++)
    {
        for(x = x0; x < x0+6; x++)
        {
            if(F6X8[idx][y-y0] & (0x80 >> (x-x0)))
                lcd_drawPoint(x, y, color);
        }
    }
}

/* 显示6X8字体的字符串: "06-24 11:56" */
void gui_show_F6X8_String(uint16_t x0, uint16_t y0, char *str, uint16_t color)
{
    uint16_t x = x0;
    while(*str)
    {
        if(*str >= '0' && *str <= '9')
            gui_show_F6X8_Char(x, y0, *str-'0', color);
        else if(*str == '-')    //11
            gui_show_F6X8_Char(x, y0, 11, color);
        else if(*str == ':')    //10
            gui_show_F6X8_Char(x, y0, 10, color);
        else if(*str == ' ')
            gui_show_F6X8_Char(x, y0, 12, color);
        str++;
        x += 6;
    }
}
#endif
/* 显示8X16字体的字符, idx查询字库索引 */
void gui_show_F8X16_Char(uint16_t x0, uint16_t y0, uint16_t idx, uint16_t color)
{
    uint16_t x, y;

    for(y = y0; y < y0 + 16; y++)
    {
        for(x = x0; x < x0 + 8; x++)
        {
//            gui_draw_point(x, y, F8X16[idx][y-y0] & (0x80>>(x-x0)), color);
            if(F8X16[idx][y - y0] & (0x80 >> (x - x0)))
                lcd_drawPoint(x, y, color);
        }
    }
}

/* 显示8X16字体的字符串: "06-24 11:56" */
void gui_show_F8X16_String(uint16_t x0, uint16_t y0, char *str, uint16_t color)
{
    uint16_t x = x0;
    while(*str)
    {
        if(*str >= '0' && *str <= '9')
            gui_show_F8X16_Char(x, y0, *str-'0', color);
        else if(*str == '-')    //11
            gui_show_F8X16_Char(x, y0, 11, color);
        else if(*str == ':')    //10
            gui_show_F8X16_Char(x, y0, 10, color);
        else if(*str == ' ')
            gui_show_F8X16_Char(x, y0, 12, color);
        str++;
        x += 8;
    }
}

uint32_t my_pow(uint16_t m, uint16_t n)
{
    uint32_t result = 1;
    while(n--)result *= m;
    return result;
}

/* 获取数据位数和正负 */
void my_getNum(long num, uint16_t *cnt, bool *minusFlag)
{
    if(num < 0)
    {
        *minusFlag = TRUE;
        *cnt = (int)log10(-num) + 1;
    }
    else
    {
        *minusFlag = FALSE;
        *cnt = (int)log10(num) + 1;    
    }
}

/* 显示1个N位的数字，支持有符号数 */
void gui_show_num(uint16_t x, uint16_t y, long num, uint16_t color)
{
    uint16_t len;
    bool minusFlag;
    long num_p;
    
    uint16_t t, temp;
    uint16_t enshow = 0;
    /* 获取数据位数和正负标志 */
    my_getNum(num, &len, &minusFlag);
    num_p = (minusFlag) ? -num : num;
    
    for(t = 0; t < len; t++)
    {
        temp = (num_p / my_pow(10, len - t - 1)) % 10;
        if(enshow == 0 && t < (len - 1))
        {
            if(temp == 0) //高位为0的显示空白
            {
                if(minusFlag == TRUE)
                    gui_show_F8X16_Char(x + 8 * t + 8, y, 12, color); //显示空白
                else
                    gui_show_F8X16_Char(x + 8 * t, y, 12, color); //显示空白
                continue;
            }
            else
                enshow = 1;
        }
        if(minusFlag == TRUE)
            gui_show_F8X16_Char(x + 8 * t + 8, y, temp, color);
        else 
            gui_show_F8X16_Char(x + 8 * t, y, temp, color);
    }
    if(minusFlag == TRUE)
        gui_show_F8X16_Char(x, y, 11, color);       //"-"
}

/* 显示16X16字体的字符 */
void gui_show_F16X16_Char(uint16_t x0, uint16_t y0, uint16_t idx, uint16_t color)
{
    uint16_t x, y;

    for(y = y0; y < y0 + 16; y++)
    {
        for(x = x0; x < x0 + 16; x++)
        {
            if(x - x0 < 8)      //0-7
                gui_draw_point(x, y, FONT_16X16_TABLE[idx].table[(y - y0) * 2] & (0x80 >> (x - x0)), color);
            else
                gui_draw_point(x, y, FONT_16X16_TABLE[idx].table[(y - y0) * 2 + 1] & (0x80 >> (x - x0 - 8)), color);
        }
    }
}


void gui_show_chn_string(uint16_t x0, uint16_t y0, char *str)
{
    int x = x0;
    char ch[2]; 
    
    while(*str)
    {
        ch[0] = *str;
        ch[1] = *(str+1);
        gui_show_chn(x, y0, ch);
        str += 2;
        x += 16;
    }
}

void gui_show_ncov_data(struct ncov_data china, struct ncov_data global)
{
    uint8_t y0 = 20;
    
    /* "国内疫情" */
    gui_drawLine(0, 18, 320, DIR_X, BLACK);
    gui_show_chn_string(128, y0, "国内疫情");
    gui_drawLine(0, 38, 320, DIR_X, BLACK);
    gui_show_line_data(y0+20, "现存确诊：", china.currentConfirmedCount, "较昨日：", china.currentConfirmedIncr);
    gui_show_line_data(y0+40, "累计确诊：", china.confirmedCount, "较昨日：", china.confirmedIncr);
    gui_show_line_data(y0+60, "累计治愈：", china.curedCount, "较昨日：", china.curedIncr);
    gui_show_line_data(y0+80, "现存重症：", china.seriousCount, "较昨日：", china.seriousIncr);
    gui_show_line_data(y0+100, "累计死亡：", china.deadCount, "较昨日：", china.deadIncr);

    /* 全球疫情 */
    gui_drawLine(0, 138, 320, DIR_X, BLACK);
    gui_show_chn_string(128, 140, "全球疫情");
    gui_drawLine(0, 158, 320, DIR_X, BLACK);

    gui_show_line_data(y0+140, "现存确诊：", global.currentConfirmedCount, "较昨日：", global.currentConfirmedIncr);
    gui_show_line_data(y0+160, "累计治愈", global.curedCount, "较昨日：", global.curedIncr);
    gui_show_line_data(y0+180, "累计死亡：", global.deadCount, "较昨日：", global.deadIncr);
    gui_drawLine(0, 220, 320, DIR_X, BLACK);
    
    gui_show_chn_string(160, 222, "更新于：");
    gui_show_F8X16_String(230, 222, china.updateTime, BLUE);
}

/* "str1": num1   "str2": num2 */
void gui_show_line_data(uint16_t y, char *str1, long num1, char *str2, long num2)
{
    gui_show_chn_string(0, y, str1);
    gui_show_chn_string(192, y, str2);
    gui_show_num(80, y, num1, BLUE);
    gui_show_num(256, y, num2, BLUE);
}

void gui_show_chn(uint16_t x0, uint16_t y0, char *chn)
{
    uint8_t idx = 0;
    char *code[2]; /* 汉字内码：内=C4DA，国=B9FA */
    uint8_t size = sizeof(FONT_16X16_TABLE) / sizeof(FONT_16X16_TABLE[0]);
    /* 遍历汉字，获取索引 */
    for(idx = 0; idx < size; idx++)
    {
        code[0] = FONT_16X16_TABLE[idx].chn;
        code[1] = FONT_16X16_TABLE[idx].chn + 1;
        //汉字内码一致
        if(strcmp(code[0], chn) == 0 && strcmp(code[1], chn+1) == 0)
            break;
    }
    gui_show_F16X16_Char(x0, y0, idx, BLACK);
}

void gui_show_code(void)
{
    int idx;
    int arrSize = sizeof(FONT_16X16_TABLE) / sizeof(FONT_16X16_TABLE[0]);
    for(idx = 0; idx < arrSize; idx++)
    {
        //汉字内码：内=C4DA，国=B9FA
        printf("%d-%x%x\r\n", idx, *FONT_16X16_TABLE[idx].chn, *(FONT_16X16_TABLE[idx].chn+1));
    }
}

extern const uint8_t ICON_BAR[640];
void gui_show_bar(void)
{
    uint16_t x, y;
    for(x = 0; x < 320; x++)
    {
        for(y = 0; y < 8; y++)
        {
            gui_draw_point(x, y, ICON_BAR[x*2] & (0x80 >> y), BLACK);
            gui_draw_point(x, y+8, ICON_BAR[x*2+1] & (0x80 >> y), BLACK);
        }        
    }
}

