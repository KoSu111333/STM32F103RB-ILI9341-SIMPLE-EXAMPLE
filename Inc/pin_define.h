#ifndef PIN_DEF_H
#define PIN_DEF_H

// ILI9341 제어 핀 정의
#define ILI9341_CS_PORT   GPIOA
#define ILI9341_CS_PIN    GPIO_PIN_4  // PA4
#define ILI9341_DC_PORT   GPIOB
#define ILI9341_DC_PIN    GPIO_PIN_1  // PB1 (민섭이 요청)
#define ILI9341_RST_PORT  GPIOB
#define ILI9341_RST_PIN   GPIO_PIN_0  // PB0
#define ILI9341_BL_PIN    GPIO_PIN_2  // PB2 (백라이트)


// SPI PIN Define
#define SPI_PORT GPIOA
#define SCK_PIN  GPIO_PIN_5 // Serial Clock (PA5)
#define MISO_PIN GPIO_PIN_6 // Master In Slave Out (PA6)
#define MOSI_PIN GPIO_PIN_7 // Master Out Slave In (PA7)

#endif