/*
 * uart.h
 *
 *  Created on: 2026. 1. 18.
 *      Author: minseopkim
 */

#ifndef UART_H_
#define UART_H_

#include "gpio.h" // STM32F103 마이크로컨트롤러의 레지스터 정의 (CMSIS 핵심)

// ====================================================================
// ==== UART2 드라이버 통합 (STLink VCP용 - PA2/PA3) ===================
// ====================================================================
// UART2 함수 프로토타입 선언
void UART2_init(uint32_t baud_rate);
void UART2_transmit_char(char data);
void UART2_transmit_string(const char* str);
void UART2_transmit_int(uint32_t num);



#endif /* UART_H_ */
