/*
 * uart.c
 *
 *  Created on: 2026. 1. 18.
 *      Author: minseopkim
 */

#include "uart.h"

/**
  * @brief  UART2 초기화 함수 (STLink Virtual COM Port용으로 사용 가정)
  *         TX: PA2, RX: PA3 핀 사용.
  * @param  baud_rate: 통신 속도 (예: 115200)
  * @retval 없음
  */
void UART2_init(uint32_t baud_rate) {
    // 1. GPIOA 클럭 활성화 (USART2 TX: PA2, RX: PA3 핀이 GPIOA에 연결됨)
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    // 2. USART2 주변장치 클럭 활성화 (USART2는 APB1 버스에 연결됨)
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    // 3. GPIO 핀 설정
    // PA2 (TX) 핀 설정: 대체 기능 푸시-풀 출력, 50MHz 속도
    init_gpio(GPIOA, GPIO_PIN_2, GPIO_MODE_OUTPUT_50MHZ_VAL, GPIO_CNF_AF_PP_OUT);
    // PA3 (RX) 핀 설정: 플로팅 입력
    init_gpio(GPIOA, GPIO_PIN_3, GPIO_MODE_INPUT_VAL, GPIO_CNF_FLOATING_IN);

    // 4. 보드레이트 설정 (UART2는 PCLK1 클럭을 사용)
    // PCLK1은 RCC_init에서 32MHz로 설정됨 (SCoreClock / 2)
    // USARTDIV = PCLK1_Clock / (16 * BaudRate)
    uint32_t pclk1_freq = RCC_SYSTEM_CLCOK / 2; // PCLK1 = 32MHz
    uint32_t usartdiv = (pclk1_freq * 25) / (4 * baud_rate); // 정수 연산을 위한 트릭
    USART2->BRR = ( (usartdiv / 100) << 4) | ( ( (usartdiv % 100) * 16 + 50) / 100 ); // BRR 레지스터 값 계산

    // 5. USART2 제어 레지스터 (CR1) 설정
    // - UE 비트 설정: USART 주변장치 활성화
    // - TE 비트 설정: 송신(Transmit) 기능 활성화
    // - RE 비트 설정: 수신(Receive) 기능 활성화
    // (기본 설정: 8 데이터 비트, 패리티 없음, 1 스톱 비트)
    USART2->CR1 |= USART_CR1_UE;  // USART 주변장치 활성화
    USART2->CR1 |= USART_CR1_TE;  // 송신 기능 활성화
    USART2->CR1 |= USART_CR1_RE;  // 수신 기능 활성화
}

/**
  * @brief  UART2로 문자 하나를 전송하는 함수 (블로킹 방식)
  * @param  data: 전송할 문자
  * @retval 없음
  */
void UART2_transmit_char(char data) {
    // TXE (Transmit data register Empty) 플래그가 설정될 때까지 대기
    // TXE는 TDR이 비어있음을 나타내며, 새로운 데이터를 쓸 수 있다는 의미.
    while (!(USART2->SR & USART_SR_TXE));
    USART2->DR = (uint16_t)data; // 데이터를 DR(데이터 레지스터)에 써서 전송 시작
}

/**
  * @brief  UART2로 문자열을 전송하는 함수 (블로킹 방식)
  * @param  str: 전송할 NULL 종료 문자열
  * @retval 없음
  */
void UART2_transmit_string(const char* str) {
    while (*str) { // 문자열의 끝(NULL 문자)에 도달할 때까지 반복
        UART2_transmit_char(*str++); // 한 문자씩 전송하고 다음 문자로 이동
    }
}

/**
  * @brief  UART2로 부호 없는 32비트 정수를 문자열로 변환하여 전송하는 함수
  * @param  num: 전송할 부호 없는 정수
  * @retval 없음
  */
void UART2_transmit_int(uint32_t num) {
    char buffer[12]; // 최대 10자리 숫자 (2^32-1) + NULL 문자 = 11자리 + 안전 여유 = 12자리
    int i = 0;       // 버퍼 인덱스

    if (num == 0) { // 숫자가 0인 경우, '0'만 전송
        UART2_transmit_char('0');
        return;
    }

    // 숫자를 역순으로 버퍼에 저장 (예: 123 -> '3', '2', '1')
    while (num > 0 && i < sizeof(buffer) - 1) { // num이 0보다 크고 버퍼 공간이 남아있으면
        buffer[i++] = (num % 10) + '0'; // 현재 숫자의 마지막 자리를 문자로 변환하여 저장
        num /= 10;                     // 숫자를 10으로 나누어 다음 자리 준비
    }
    buffer[i] = '\0'; // 버퍼의 끝을 NULL 문자로 표시

    // 버퍼의 내용을 역순으로 전송 (예: '1', '2', '3')
    for (int j = i - 1; j >= 0; j--) {
        UART2_transmit_char(buffer[j]);
    }
}