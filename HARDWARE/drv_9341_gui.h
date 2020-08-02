#ifndef __DRV_9341_GUI_H__
#define __DRV_9341_GUI_H__

#include "drv_9341.h"
#include "usr_ncov.h"


typedef enum {FALSE = 0, TRUE = !FALSE} bool;

typedef enum {
    DIR_X = 0,
    DIR_Y
}direction;

extern const uint8_t F8X16[][16];

void gui_draw_point(uint16_t x0, uint16_t y0, uint16_t en, uint16_t color);
void gui_drawLine(uint16_t x0, uint16_t y0, uint16_t len, uint16_t dir, uint16_t color);
void gui_drawRectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);

void gui_show_F6X8_Char(uint16_t x0, uint16_t y0, uint16_t idx, uint16_t color);
void gui_show_F6X8_String(uint16_t x0, uint16_t y0, char *str, uint16_t color);

void gui_show_F8X16_Char(uint16_t x0, uint16_t y0, uint16_t idx, uint16_t color);
void gui_show_F8X16_String(uint16_t x0, uint16_t y0, char *str, uint16_t color);
void gui_show_num(uint16_t x, uint16_t y, long num, uint16_t color);

void gui_show_F16X16_Char(uint16_t x0, uint16_t y0, uint16_t idx, uint16_t color);
void gui_show_dataChina(void);
void gui_show_dataGlobal(void);

void my_getNum(long num, uint16_t *cnt, bool *minusFlag);
uint32_t my_pow(uint16_t m, uint16_t n);
void gui_show_bar(void);

void gui_show_chn(uint16_t x, uint16_t y, char *chn);
void gui_show_chn_string(uint16_t x0, uint16_t y0, char *str);
void gui_show_ncov_data(struct ncov_data china, struct ncov_data global);
void gui_show_line_data(uint16_t y, char *str1, long num1, char *str2, long num2);

void gui_show_code(void);

#endif
