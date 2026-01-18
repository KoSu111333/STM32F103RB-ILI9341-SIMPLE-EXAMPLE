#include <stdint.h>    // uint32_t, uint8_t 등 표준 정수 타입 정의
#include <stdbool.h>   // bool 타입 정의 (true, false)
#include "stm32f103xb.h" // STM32F103 마이크로컨트롤러의 레지스터 정의 (CMSIS 핵심)
#include "spi.h"
#include "uart.h"
#include "ILI_9341.h"
#include "5x5font.h"
// FPU 관련 경고 억제 (STM32CubeIDE 등에서 자동으로 추가될 수 있음)
#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

#define RGB565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3))



// ====================================================================
// ==== SysTick 기반 정확한 Delay 함수 구현 ===============================
// ====================================================================
volatile uint32_t ms_tick_count = 0; // 1ms 단위로 카운트하는 변수

// SysTick 인터럽트 핸들러 (vector table에 등록되어 자동으로 호출됨)
void SysTick_Handler(void) {
    if (ms_tick_count > 0) {
        ms_tick_count--; // ms_tick_count가 0이 될 때까지 1ms마다 감소
    }
}

// ms 단위로 지연하는 함수
void delay_ms(uint32_t ms) {
    ms_tick_count = ms;     // 지연할 시간(ms) 설정
    while (ms_tick_count > 0); // ms_tick_count가 0이 될 때까지 대기
}


// ====================================================================
// ==== RCC 드라이버 통합 시작 =========================================
// ====================================================================
/**
  * @brief  RCC(Reset and Clock Control) 초기화 함수
  *         HSI(내부 8MHz RC 발진기)를 PLL 소스로 사용, 64MHz SYSCLK 설정.
  *         Flash Latency 및 AHB/APB 버스 분주율 설정.
  * @retval 없음
  */
void RCC_init(void) {
    // 1. HSI 클럭 활성화 (내부 고속 RC 발진기 8MHz)
    RCC->CR |= RCC_CR_HSION;            // HSION 비트를 1로 설정하여 HSI 활성화
    while (!(RCC->CR & RCC_CR_HSIRDY)); // HSIRDY 비트가 1이 될 때까지 대기 (HSI 준비 확인)

    // 2. FLASH ACR (Access Control Register) 설정: Flash Latency
    // 64MHz @ 3.3V 환경에서는 2 wait state (3 CPU cycles)가 필요
    FLASH->ACR |= FLASH_ACR_PRFTBE;     // Prefetch Buffer Enable
    FLASH->ACR &= ~FLASH_ACR_LATENCY;   // 기존 Latency 설정 클리어
    FLASH->ACR |= FLASH_ACR_LATENCY_1;  // 2 wait states 설정 (48MHz < SYSCLK <= 72MHz)

    // 3. AHB (HCLK) Prescaler 설정: SYSCLK 분주 없음 (HCLK = SYSCLK)
    RCC->CFGR &= ~RCC_CFGR_HPRE;        // HPRE[3:0] 비트를 0으로 클리어 (SYSCLK / 1)

    // 4. APB2 (PCLK2) Prescaler 설정: HCLK 분주 없음 (PCLK2 = HCLK = SYSCLK)
    // APB2 버스(SPI1 등이 연결됨)는 최대 72MHz를 지원, 현재 HCLK가 64MHz이므로 분주 없음.
    RCC->CFGR &= ~RCC_CFGR_PPRE2;       // PPRE2[2:0] 비트를 0으로 클리어 (HCLK / 1)

    // 5. APB1 (PCLK1) Prescaler 설정: HCLK / 2 (PCLK1 = HCLK / 2)
    // APB1 버스(UART2, UART3 등이 연결됨)는 최대 36MHz를 지원, HCLK 64MHz -> PCLK1은 32MHz로 설정.
    RCC->CFGR &= ~RCC_CFGR_PPRE1;       // PPRE1[2:0] 비트를 0으로 클리어
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;   // HCLK / 2 설정

    // 6. PLL (Phase-Locked Loop) 설정
    // PLL 클럭 소스: HSI / 2 (4MHz)
    RCC->CFGR &= ~RCC_CFGR_PLLSRC;      // PLLSRC 비트를 0으로 클리어 (HSI/2 선택)
    // PLL 배율: x16 (4MHz * 16 = 64MHz)
    RCC->CFGR &= ~RCC_CFGR_PLLMULL;     // 기존 PLLMULL 비트 클리어
    RCC->CFGR |= RCC_CFGR_PLLMULL16;    // PLL 배율 x16 설정 (민섭이의 정확한 지적 덕분에 수정!)

    // 7. PLL 활성화
    RCC->CR |= RCC_CR_PLLON;            // PLLON 비트를 1로 설정하여 PLL 활성화
    while (!(RCC->CR & RCC_CR_PLLRDY)); // PLLRDY 비트가 1이 될 때까지 대기 (PLL 준비 확인)

    // 8. SYSCLK (System Clock) 소스 선택: PLL
    RCC->CFGR &= ~RCC_CFGR_SW;          // SW[1:0] 비트 클리어
    RCC->CFGR |= RCC_CFGR_SW_PLL;       // SYSCLK 소스를 PLL로 선택
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL); // SYSCLK가 PLL로 전환될 때까지 대기

}


// ====================================================================
// ==== main 함수 시작 ================================================
// ====================================================================

const uint16_t small_test_image[] = {
    COLOR_RED,   COLOR_GREEN,   // 첫 번째 줄: 빨강, 초록
    COLOR_BLUE,  COLOR_WHITE    // 두 번째 줄: 파랑, 흰색
};
#define SMALL_IMAGE_WIDTH  2
#define SMALL_IMAGE_HEIGHT 2

#define FONT_CHAR_WIDTH  5 // 폰트 자체의 가로 픽셀 수 (주석 상 5)
#define FONT_CHAR_HEIGHT 5 // 폰트 자체의 세로 픽셀 수 (주석 상 5)
#define FONT_COL_BYTES   6 // 폰트 데이터에서 한 문자가 차지하는 바이트 수

void ili9341_draw_char_custom(char c, uint16_t x, uint16_t y, uint16_t color, uint16_t bg_color, uint8_t scale) {
    if (c < 32 || c > 32 + 95) {
        return;
    }

    int char_index = c - 32;

    for (int col = 0; col < FONT_CHAR_WIDTH; col++) {
        unsigned char column_data = font[char_index][col]; 
        
        for (int row = 0; row < FONT_CHAR_HEIGHT; row++) {
            // 이 부분이 핵심! (row + FONT_BIT_OFFSET)으로 비트를 정확한 위치에서 읽는다.
            if ((column_data >> (row + 2)) & 0x01) { 
                for (int sx = 0; sx < scale; sx++) {
                    for (int sy = 0; sy < scale; sy++) {
                        // ILI9341_DrawPixel 함수는 네가 제공한 함수를 사용!
                        ILI9341_DrawPixel(x + (col * scale) + sx, y + (row * scale) + sy, color);
                    }
                }
            } else {
                for (int sx = 0; sx < scale; sx++) {
                    for (int sy = 0; sy < scale; sy++) {
                        // ILI9341_DrawPixel 함수는 네가 제공한 함수를 사용!
                        ILI9341_DrawPixel(x + (col * scale) + sx, y + (row * scale) + sy, bg_color);
                    }
                }
            }
        }
    }
}

// --- 8. 문자열 그리기 함수 (폰트 너비, 높이, 스케일 고려하여 조정) ---
void ili9341_draw_string_custom(const char* str, uint16_t x, uint16_t y, uint16_t color, uint16_t bg_color, uint8_t scale) {
    uint16_t current_x = x;
    
    // 확대된 글자 하나의 너비 (폰트 너비 * 스케일) + 글자 간 여백 (스케일 값만큼 줄 수도 있고, 고정 값 줄 수도 있음)
    // 여기서는 FONT_CHAR_WIDTH * scale + 1 픽셀 여백을 줘볼게.
    const int char_rendered_width = (FONT_CHAR_WIDTH * scale) + 1; 
    const int char_rendered_height = (FONT_CHAR_HEIGHT * scale) + 2; // 폰트 높이 * 스케일 + 줄 간 여백

    while (*str) {
        ili9341_draw_char_custom(*str, current_x, y, color, bg_color, scale);
        current_x += char_rendered_width; // 다음 문자의 X 좌표

        // 화면 너비 초과 시 줄 바꿈
        // ILI9341의 최대 너비는 320 픽셀이야.
        if (current_x + char_rendered_width >= 320) {
            current_x = x; // 시작 X 위치로 리셋
            y += char_rendered_height; // 다음 줄의 Y 좌표
            // 화면 높이 초과 시 중단 (ILI9341 최대 높이는 240 픽셀)
            if (y >= 240) break; 
        }
        str++;
    }
}


int main(void)
{
	// 1. 시스템 클럭 초기화 (HSI(8MHz)를 이용한 PLL 구성, 64MHz SYSCLK 설정)
	RCC_init();

    // !!! 필수: AFIO (Alternate Function I/O) 모듈 클럭 활성화 !!!
    // SPI와 UART는 모두 Alternate Function 핀을 사용하므로, AFIO 클럭 활성화는 필수!
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    // GPIOB 클럭 활성화 (ILI9341 제어 핀 PB0, PB1, PB2 때문)
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

    // 2. SysTick 타이머 초기화 (정확한 delay_ms() 함수를 위해)
    // SCoreClock 변수는 RCC_init()에서 64000000으로 업데이트됩니다.
    SysTick_Config(RCC_SYSTEM_CLCOK / 1000); // 1ms마다 SysTick 인터럽트 발생
    NVIC_SetPriority(SysTick_IRQn, 15);     // SysTick 인터럽트 우선순위 설정 (가장 낮은 우선순위)

    // --- 테스트용 LED 설정 (GPIO 동작 및 MCU Alive 확인용) ---
    // GPIOC_PIN_13은 대부분의 Discovery/Nucleo 보드에 내장된 LED입니다.
    init_gpio(GPIOC, GPIO_PIN_13, GPIO_MODE_OUTPUT_2MHZ_VAL, GPIO_CNF_GP_PP_OUT);
    // ---------------------------------------------

    // 3. UART2 초기화 (STLink VCP용으로 가정, PA2/PA3 핀 사용, 115200 보드레이트)
    UART2_init(115200);

    // 4. 전역 인터럽트 활성화 (SysTick, SPI RX 인터럽트 등 모든 인터럽트 동작에 필수)
    __enable_irq();

    // 초기 LED 깜빡임으로 MCU 동작 확인 (UART 초기화 전에도 깜빡여서 MCU 살아있음 보여줌)
    GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET); // LED ON
    delay_ms(200);
    GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);   // LED OFF
    delay_ms(200);

    // --- 1단계 테스트: UART "Hello World!" 메시지 출력 확인 ---
    // 이 메시지가 터미널에 성공적으로 출력되면 UART 통신은 정상 작동하는 것.
    UART2_transmit_string("\r\n--- STM32F103 VCP Test Start! (via UART2 PA2/PA3) ---\r\n");
    UART2_transmit_string("Hello World! UART is working.\r\n");
    UART2_transmit_string("Initializing ILI9341 LCD next...\r\n");
    delay_ms(1000); // 1초 대기

    // --- 2단계 테스트: SPI1 및 ILI9341 LCD 초기화 및 테스트 ---
    ILI9341_init_pins(); // ILI9341 제어 핀 GPIO 초기화
    SPI1_init();         // SPI1 주변장치 초기화
    
    // ILI9341 초기화 시퀀스 실행
    ILI9341_init();
    UART2_transmit_string("ILI9341 LCD Initialized. Starting graphics tests.\r\n");
    delay_ms(1000);
    ILI9341_FillScreen(RGB565(0, 0, 0)); // 배경을 검은색으로

    // "Hello Cworld" 출력!
    // 스케일 1 (기본 5x5 폰트)
    ili9341_draw_string_custom("Hello Cworld!", 10, 10, RGB565(0, 255, 0), RGB565(0, 0, 0), 1); 

    // 스케일 2 (10x10 폰트처럼 보임)
    ili9341_draw_string_custom("Temp : 25.5 C", 10, 30, RGB565(255, 255, 0), RGB565(0, 0, 0), 2); 

    // 스케일 3 (15x15 폰트처럼 보임)
    ili9341_draw_string_custom("Humid: 60.2 %", 10, 70, RGB565(0, 255, 255), RGB565(0, 0, 0), 3); 

    // 스케일 4 (20x20 폰트처럼 보임)
    ili9341_draw_string_custom("Test", 10, 130, RGB565(255, 0, 255), RGB565(0, 0, 0), 4);

    while(true) // 무한 루프
	{

	}
}
