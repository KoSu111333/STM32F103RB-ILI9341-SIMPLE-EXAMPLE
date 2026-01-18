#ifndef ILI9341_H
#define ILI9341_H


// ====================================================================
// ==== /UART2 드라이버 통합 끝 =========================================
#include <stdint.h>
#include "stm32f103xb.h" // STM32F103 마이크로컨트롤러의 레지스터 정의 (CMSIS 핵심)
#include "gpio.h" // STM32F103 마이크로컨트롤러의 레지스터 정의 (CMSIS 핵심)
#include "pin_define.h"
// ====================================================================
// ==== ILI9341 LCD 드라이버 통합 시작 ==================================
// ====================================================================
// ILI9341 LCD 해상도 및 컬러 정의
#define ILI9341_WIDTH  240
#define ILI9341_HEIGHT 320

// ILI9341 명령 (자주 사용되는 것들)
#define ILI9341_NOP         0x00 // No Operation
#define ILI9341_SWRESET     0x01 // Software Reset
#define ILI9341_RDID4       0xD3 // Read ID4
#define ILI9341_SLPOUT      0x11 // Sleep Out
#define ILI9341_DISPOFF     0x28 // Display Off
#define ILI9341_DISPON      0x29 // Display On
#define ILI9341_CASET       0x2A // Column Address Set
#define ILI9341_PASET       0x2B // Page Address Set
#define ILI9341_RAMWR       0x2C // Memory Write
#define ILI9341_MADCTL      0x36 // Memory Access Control
#define ILI9341_PIXFMT      0x3A // Pixel Format Set
#define ILI9341_FRMCTR1     0xB1 // Frame Rate Control (In Normal Mode/Full Colors)
#define ILI9341_DFUNCTR     0xB6 // Display Function Control
#define ILI9341_PWCTR1      0xC0 // Power Control 1
#define ILI9341_PWCTR2      0xC1 // Power Control 2
#define ILI9341_VMCTR1      0xC5 // VCOM Control 1
#define ILI9341_GMCTRP1     0xE0 // Positive Gamma Correction
#define ILI9341_GMCTRN1     0xE1 // Negative Gamma Correction
#define ILI9341_CMD_SET_ADDR_MODE 0x36

// 컬러 정의 (16비트 RGB565 포맷)
#define COLOR_BLACK       0x0000
#define COLOR_NAVY        0x000F
#define COLOR_DARKGREEN   0x03E0
#define COLOR_DARKCYAN    0x03EF
#define COLOR_MAROON      0x7800
#define COLOR_PURPLE      0x780F
#define COLOR_OLIVE       0x7BE0
#define COLOR_LIGHTGREY   0xC618
#define COLOR_DARKGREY    0x7BEF
#define COLOR_BLUE        0x001F
#define COLOR_GREEN       0x07E0
#define COLOR_CYAN        0x07FF
#define COLOR_RED         0xF800
#define COLOR_MAGENTA     0xF81F
#define COLOR_YELLOW      0xFFE0
#define COLOR_WHITE       0xFFFF
#define COLOR_ORANGE      0xFD20
#define COLOR_GREENYELLOW 0xAFE5
#define COLOR_PINK        0xF81F



// ILI9341 함수 프로토타입
void ILI9341_init_pins(void);
void ILI9341_CS_Enable(void);
void ILI9341_CS_Disable(void);
void ILI9341_DC_Set(void);
void ILI9341_DC_Reset(void);
void ILI9341_RST_Set(void);
void ILI9341_RST_Reset(void);
void ILI9341_BL_On(void);
void ILI9341_BL_Off(void);

void ILI9341_WriteCommand(uint8_t cmd);
void ILI9341_WriteData(uint8_t data);
void ILI9341_init(void);
void ILI9341_SetAddressWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void ILI9341_FillScreen(uint16_t color);
void ILI9341_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void ILI9341_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void ILI9341_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const char *image_data) ;

#endif