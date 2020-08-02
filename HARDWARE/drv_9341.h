#ifndef __DRV_9341_H__
#define __DRV_9341_H__

#include "sys.h"

/**
LCD_BL = PA8
LCD_CS = PC9
LCD_RS = PC8
LCD_WR = PC7
LCD_RD = PC6
LCD_DATA_INT = GPIOB->IDR
LCD_DATA_OUT(x) GPIOB->ODR=x; 
**/

#define LCD_BL  PAout(8)
#define LCD_CS  PCout(9)
#define LCD_RS  PCout(8)
#define LCD_WR  PCout(7)
#define LCD_RD  PCout(6)
#define LCD_DATA_OUT(data) GPIOB->ODR=data
#define LCD_DATA_IN     GPIOB->IDR

//LCD重要参数集
typedef struct
{
    uint16_t width;          //LCD 宽度
    uint16_t height;         //LCD 高度
    uint16_t id;             //LCD ID
    uint8_t  dir;            //横屏还是竖屏控制：0，竖屏；1，横屏。
    uint16_t wramcmd;        //开始写gram指令
    uint16_t setxcmd;        //设置x坐标指令
    uint16_t  setycmd;       //设置y坐标指令
} _lcd_dev;

//LCD参数
extern _lcd_dev lcddev; //管理LCD重要参数
//LCD的画笔颜色和背景色
extern uint16_t  POINT_COLOR;//默认红色
extern uint16_t  BACK_COLOR; //背景颜色.默认为白色

#define L2R_U2D  0 //从左到右,从上到下
#define L2R_D2U  1 //从左到右,从下到上
#define R2L_U2D  2 //从右到左,从上到下
#define R2L_D2U  3 //从右到左,从下到上

#define U2D_L2R  4 //从上到下,从左到右
#define U2D_R2L  5 //从上到下,从右到左
#define D2U_L2R  6 //从下到上,从左到右
#define D2U_R2L  7 //从下到上,从右到左


//#define DFT_SCAN_DIR  L2R_U2D  //屏幕旋转180度
#define DFT_SCAN_DIR  R2L_D2U

//画笔颜色
#define WHITE            0xFFFF
#define BLACK            0x0000
#define BLUE             0x001F
#define BRED             0XF81F
#define GRED             0XFFE0
#define GBLUE            0X07FF
#define RED              0xF800
#define MAGENTA          0xF81F
#define GREEN            0x07E0
#define CYAN             0x7FFF
#define YELLOW           0xFFE0
#define BROWN            0XBC40 //棕色
#define BRRED            0XFC07 //棕红色
#define GRAY             0X8430 //灰色
//GUI颜色

#define DARKBLUE         0X01CF //深蓝色
#define LIGHTBLUE        0X7D7C //浅蓝色  
#define GRAYBLUE         0X5458 //灰蓝色
//以上三色为PANEL的颜色

#define LIGHTGREEN       0X841F //浅绿色 
#define LGRAY            0XC618 //浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)
#define LCD_SIZE_X 240
#define LCD_SIZE_Y 320

void lcd_init(uint16_t color);                                                     //初始化
void lcd_clear(uint16_t color);                                                  //清屏
void lcd_setCursor(uint16_t Xpos, uint16_t Ypos);                                     //设置光标
void lcd_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color);                   //填充单色
void lcd_drawPoint(uint16_t x, uint16_t y, uint16_t color);

void lcd_write_reg(uint16_t LCD_Reg, uint16_t LCD_RegValue);
uint16_t lcd_read_reg(uint16_t LCD_Reg);
void lcd_setScanDir(uint8_t dir);                                  //设置屏扫描方向
void lcd_setDisplayDir(uint8_t dir);                               //设置屏幕显示方向
void lcd_setBL(uint8_t pwm);

//写数据函数
#define lcd_write_data(data){\
LCD_RS = 1;\
LCD_CS = 0;\
LCD_DATA_OUT(data);\
LCD_WR = 0;\
LCD_WR = 1;\
LCD_CS = 1;\
}


#endif


