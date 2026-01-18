#include "ILI_9341.h"
/**
  * @brief ILI9341 제어 핀들을 GPIO 출력으로 초기화
  */
void ILI9341_init_pins(void) {
    // ILI9341 제어 핀들 (CS, DC, RST, BL) 초기화
    // PA4 (CS)
    init_gpio(ILI9341_CS_PORT, ILI9341_CS_PIN, GPIO_MODE_OUTPUT_50MHZ_VAL, GPIO_CNF_GP_PP_OUT);
    ILI9341_CS_Disable();
    // PB1 (DC)
    init_gpio(ILI9341_DC_PORT, ILI9341_DC_PIN, GPIO_MODE_OUTPUT_50MHZ_VAL, GPIO_CNF_GP_PP_OUT);
    // PB0 (RST)
    init_gpio(ILI9341_RST_PORT, ILI9341_RST_PIN, GPIO_MODE_OUTPUT_50MHZ_VAL, GPIO_CNF_GP_PP_OUT);
    // PB2 (BL)
}

// ILI9341_CS 핀 제어
void ILI9341_CS_Enable(void) {
    GPIO_WritePin(ILI9341_CS_PORT, ILI9341_CS_PIN, GPIO_PIN_RESET); // CS LOW (칩 선택)
}
void ILI9341_CS_Disable(void) {
    GPIO_WritePin(ILI9341_CS_PORT, ILI9341_CS_PIN, GPIO_PIN_SET);   // CS HIGH (칩 선택 해제)
}

// ILI9341_DC 핀 제어
void ILI9341_DC_Set(void) {
    GPIO_WritePin(ILI9341_DC_PORT, ILI9341_DC_PIN, GPIO_PIN_SET);   // DC HIGH (데이터 모드)
}
void ILI9341_DC_Reset(void) {
    GPIO_WritePin(ILI9341_DC_PORT, ILI9341_DC_PIN, GPIO_PIN_RESET); // DC LOW (명령 모드)
}

// ILI9341_RST 핀 제어
void ILI9341_RST_Set(void) {
    GPIO_WritePin(ILI9341_RST_PORT, ILI9341_RST_PIN, GPIO_PIN_SET); // RST HIGH
}
void ILI9341_RST_Reset(void) {
    GPIO_WritePin(ILI9341_RST_PORT, ILI9341_RST_PIN, GPIO_PIN_RESET); // RST LOW
}


/**
  * @brief ILI9341에 명령 바이트를 전송
  * @param cmd: 전송할 명령 (8비트)
  */
void ILI9341_WriteCommand(uint8_t cmd) {
    ILI9341_CS_Enable();  // CS LOW
    ILI9341_DC_Reset(); // DC LOW (명령 모드)
    SPI1_transfer(cmd);   // SPI로 명령 전송
    ILI9341_CS_Disable(); // CS HIGH
}

/**
  * @brief ILI9341에 데이터 바이트를 전송
  * @param data: 전송할 데이터 (8비트)
  */
void ILI9341_WriteData(uint8_t data) {
    ILI9341_CS_Enable();  // CS LOW
    ILI9341_DC_Set();     // DC HIGH (데이터 모드)
    SPI1_transfer(data);  // SPI로 데이터 전송
    ILI9341_CS_Disable(); // CS HIGH
}


/**
  * @brief ILI9341 LCD 초기화 시퀀스 (데이터시트 참조)
  */
void ILI9341_init(void) {
    // 1. 하드웨어 리셋
    ILI9341_RST_Set();   // RST HIGH
    delay_ms(5);
    ILI9341_RST_Reset(); // RST LOW
    delay_ms(20);
    ILI9341_RST_Set();   // RST HIGH
    delay_ms(150);       // 리셋 후 대기

    // 2. 소프트웨어 리셋
    ILI9341_WriteCommand(ILI9341_SWRESET); // 소프트웨어 리셋 명령
    delay_ms(150);

    // 3. Sleep Out
    ILI9341_WriteCommand(ILI9341_SLPOUT); // Sleep Out 명령
    delay_ms(150);

    // 4. Power Control A
    ILI9341_WriteCommand(0xCB);
    ILI9341_WriteData(0x39);
    ILI9341_WriteData(0x2C);
    ILI9341_WriteData(0x00);
    ILI9341_WriteData(0x34);
    ILI9341_WriteData(0x02);

    // 5. Power Control B
    ILI9341_WriteCommand(0xCF);
    ILI9341_WriteData(0x00);
    ILI9341_WriteData(0xC1);
    ILI9341_WriteData(0x30);

    // 6. Driver timing control A
    ILI9341_WriteCommand(0xE8);
    ILI9341_WriteData(0x85);
    ILI9341_WriteData(0x00);
    ILI9341_WriteData(0x78);

    // 7. Driver timing control B
    ILI9341_WriteCommand(0xEA);
    ILI9341_WriteData(0x00);
    ILI9341_WriteData(0x00);

    // 8. Power on Sequence control
    ILI9341_WriteCommand(0xED);
    ILI9341_WriteData(0x64);
    ILI9341_WriteData(0x03);
    ILI9341_WriteData(0x12);
    ILI9341_WriteData(0x81);

    // 9. Pump ratio control
    ILI9341_WriteCommand(0xF7);
    ILI9341_WriteData(0x20);

    // 10. Power Control 1 (Vcore)
    ILI9341_WriteCommand(ILI9341_PWCTR1);
    ILI9341_WriteData(0x23); // VRH[5:0]

    // 11. Power Control 2 (VGH, VGL)
    ILI9341_WriteCommand(ILI9341_PWCTR2);
    ILI9341_WriteData(0x10); // SAP[2:0];BT[3:0]

    // 12. VCOM Control 1
    ILI9341_WriteCommand(ILI9341_VMCTR1);
    ILI9341_WriteData(0x3E); // VMF[6:0]
    ILI9341_WriteData(0x28); // VML[6:0]
    // 13. VCOM Control 2
    ILI9341_WriteCommand(0xC7);
    ILI9341_WriteData(0x86); // VMF

    // 14. Memory Access Control (MADCTL)
    ILI9341_WriteCommand(ILI9341_MADCTL);
    // Bit D7: MY (Row Address Order)
    // Bit D6: MX (Column Address Order)
    // Bit D5: MV (Row/Column Exchange) - 1: Exchange, 0: Normal
    // Bit D4: ML (Line Address Order)
    // Bit D3: BGR (RGB/BGR Order) - 1: BGR, 0: RGB
    // Bit D2: MH (Display Data Latch Order)
    // D5=1, D4=0, D3=1, D2=0, D1=0, D0=0 (0b01001000)
    // Set for Portrait mode, BGR color order
    // 0x48 = MX (0), MV (1), MY (0), BGR (1) = Landscape (MV=1)
    // 0x28 = MX (0), MV (0), MY (0), BGR (1) = Portrait (BGR)
    // 0x08 = MX (0), MV (0), MY (0), BGR (1) = Portrait (RGB)
    // For standard portrait (top-down, left-right, BGR): 0b00101000 = 0x28
    ILI9341_WriteData(0x88); // 기본값: 0x28 (Portrait, BGR)

    // 15. Pixel Format Set
    ILI9341_WriteCommand(ILI9341_PIXFMT);
    ILI9341_WriteData(0x55); // 16bit/pixel (RGB565)

    // 16. Frame Rate Control (In Normal Mode/Full Colors)
    ILI9341_WriteCommand(ILI9341_FRMCTR1);
    ILI9341_WriteData(0x00);
    ILI9341_WriteData(0x18); // 70Hz

    // 17. Display Function Control
    ILI9341_WriteCommand(ILI9341_DFUNCTR);
    ILI9341_WriteData(0x08); // Display inversion control, normally (0x08)
    ILI9341_WriteData(0x82); // Gate scan reverse
    ILI9341_WriteData(0x27);

    // 18. Gamma Function Disable
    ILI9341_WriteCommand(0xF2);
    ILI9341_WriteData(0x00);

    // 19. Gamma Correction Positive Pole
    ILI9341_WriteCommand(ILI9341_GMCTRP1);
    ILI9341_WriteData(0x0F);
    ILI9341_WriteData(0x31);
    ILI9341_WriteData(0x2B);
    ILI9341_WriteData(0x0C);
    ILI9341_WriteData(0x0E);
    ILI9341_WriteData(0x08);
    ILI9341_WriteData(0x4E);
    ILI9341_WriteData(0xF1);
    ILI9341_WriteData(0x37);
    ILI9341_WriteData(0x07);
    ILI9341_WriteData(0x10);
    ILI9341_WriteData(0x03);
    ILI9341_WriteData(0x0E);
    ILI9341_WriteData(0x09);
    ILI9341_WriteData(0x00);

    // 20. Gamma Correction Negative Pole
    ILI9341_WriteCommand(ILI9341_GMCTRN1);
    ILI9341_WriteData(0x00);
    ILI9341_WriteData(0x0E);
    ILI9341_WriteData(0x14);
    ILI9341_WriteData(0x03);
    ILI9341_WriteData(0x11);
    ILI9341_WriteData(0x07);
    ILI9341_WriteData(0x31);
    ILI9341_WriteData(0xC1);
    ILI9341_WriteData(0x48);
    ILI9341_WriteData(0x08);
    ILI9341_WriteData(0x0F);
    ILI9341_WriteData(0x0C);
    ILI9341_WriteData(0x31);
    ILI9341_WriteData(0x36);
    ILI9341_WriteData(0x0F);

    // 21. Display On
    ILI9341_WriteCommand(ILI9341_DISPON);
    delay_ms(150);

}

/**
  * @brief  LCD의 그리기 영역(Address Window)을 설정
  * @param  x1, y1: 시작점 좌표
  * @param  x2, y2: 끝점 좌표
  */
void ILI9341_SetAddressWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    ILI9341_WriteCommand(ILI9341_CASET); // Column Address Set
    ILI9341_WriteData(x1 >> 8); // X start High
    ILI9341_WriteData(x1 & 0xFF); // X start Low
    ILI9341_WriteData(x2 >> 8); // X end High
    ILI9341_WriteData(x2 & 0xFF); // X end Low

    ILI9341_WriteCommand(ILI9341_PASET); // Page Address Set
    ILI9341_WriteData(y1 >> 8); // Y start High
    ILI9341_WriteData(y1 & 0xFF); // Y start Low
    ILI9341_WriteData(y2 >> 8); // Y end High
    ILI9341_WriteData(y2 & 0xFF); // Y end Low

    ILI9341_WriteCommand(ILI9341_RAMWR); // Memory Write
}

/**
  * @brief  LCD 화면 전체를 단색으로 채움
  * @param  color: 채울 색상 (16비트 RGB565)
  */
void ILI9341_FillScreen(uint16_t color) {
    uint32_t i;
    ILI9341_SetAddressWindow(0, 0, ILI9341_WIDTH - 1, ILI9341_HEIGHT - 1); // 전체 화면 영역 설정
    ILI9341_CS_Enable(); // CS LOW
    ILI9341_DC_Set();     // DC HIGH (데이터 모드)

    // 모든 픽셀에 색상 데이터 전송
    for (i = 0; i < (uint32_t)ILI9341_WIDTH * ILI9341_HEIGHT; i++) {
        SPI1_transfer(color >> 8);   // High byte
        SPI1_transfer(color & 0xFF); // Low byte
    }
    ILI9341_CS_Disable(); // CS HIGH
}

/**
  * @brief  단일 픽셀을 그림
  * @param  x, y: 픽셀 좌표
  * @param  color: 픽셀 색상 (16비트 RGB565)
  */
void ILI9341_DrawPixel(uint16_t x, uint16_t y, uint16_t color) {
    // 좌표가 화면 범위를 벗어나면 그리지 않음
    if (x >= ILI9341_WIDTH || y >= ILI9341_HEIGHT) return;

    ILI9341_SetAddressWindow(x, y, x, y); // 단일 픽셀 영역 설정
    ILI9341_CS_Enable();  // CS LOW
    ILI9341_DC_Set();     // DC HIGH
    SPI1_transfer(color >> 8);   // High byte
    SPI1_transfer(color & 0xFF); // Low byte
    ILI9341_CS_Disable(); // CS HIGH
}

/**
  * @brief  직사각형을 그림
  * @param  x, y: 시작 좌표
  * @param  w, h: 가로, 세로 길이
  * @param  color: 색상 (16비트 RGB565)
  */
void ILI9341_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    if (x >= ILI9341_WIDTH || y >= ILI9341_HEIGHT || w == 0 || h == 0) return;
    if (x + w > ILI9341_WIDTH) w = ILI9341_WIDTH - x;
    if (y + h > ILI9341_HEIGHT) h = ILI9341_HEIGHT - y;

    ILI9341_SetAddressWindow(x, y, x + w - 1, y + h - 1); // 직사각형 영역 설정
    ILI9341_CS_Enable(); // CS LOW
    ILI9341_DC_Set();     // DC HIGH

    for (uint32_t i = 0; i < (uint32_t)w * h; i++) {
        SPI1_transfer(color >> 8);   // High byte
        SPI1_transfer(color & 0xFF); // Low byte
    }
    ILI9341_CS_Disable(); // CS HIGH
}
// main.c (ILI9341 LCD 드라이버 통합 섹션)

// ... (기존 ILI9341_DrawRectangle 함수 이후에 추가) ...

/**
  * @brief  화면에 이미지를 그림
  * @param  x: 이미지를 그릴 시작 X 좌표
  * @param  y: 이미지를 그릴 시작 Y 좌표
  * @param  w: 이미지의 가로 길이 (픽셀)
  * @param  h: 이미지의 세로 길이 (픽셀)
  * @param  image_data: RGB565 형식의 픽셀 데이터 배열 포인터
  */
void ILI9341_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const char *image_data) {
    uint32_t total_pixels = (uint32_t)w * h;
    uint32_t i;
    const uint8_t *current_pixel_data = image_data; // 현재 픽셀 데이터 포인터

    // 이미지가 화면 범위를 벗어나지 않도록 클리핑
    if (x >= ILI9341_WIDTH || y >= ILI9341_HEIGHT || w == 0 || h == 0) return;
    if (x + w > ILI9341_WIDTH) w = ILI9341_WIDTH - x;
    if (y + h > ILI9341_HEIGHT) h = ILI9341_HEIGHT - y;

    // 이미지가 그려질 영역 설정
    ILI9341_SetAddressWindow(x, y, x + w - 1, y + h - 1);

    ILI9341_CS_Enable();  // CS LOW
    ILI9341_DC_Set();     // DC HIGH (데이터 모드)

    // 모든 픽셀 데이터를 순서대로 전송
    for (i = 0; i < total_pixels; i++) {
        // 픽셀 1개는 2바이트 (RGB565). uint8_t 배열에서는 두 번의 SPI 전송으로 픽셀 1개를 구성.
        // 현재 이미지 데이터 배열은 High Byte 먼저, Low Byte 나중 순서로 되어 있다고 가정.
        SPI1_transfer(*current_pixel_data++);   // High byte 전송 (예: 0x10)
        SPI1_transfer(*current_pixel_data++);   // Low byte 전송 (예: 0xC4)
    }
    ILI9341_CS_Disable(); // CS HIGH
}
