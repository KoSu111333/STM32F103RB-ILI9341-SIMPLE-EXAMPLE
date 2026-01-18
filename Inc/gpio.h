/*
 * gpio.h
 *
 *  Created on: 2026. 1. 18.
 *      Author: minseopkim
 */

#ifndef GPIO_H_
#define GPIO_H_

#include "stdint.h"
#include "stm32f103xb.h" // STM32F103 마이크로컨트롤러의 레지스터 정의 (CMSIS 핵심)
#include "pin_define.h"

// ====================================================================
// ==== GPIO 드라이버 통합 및 재검토 (STM32F1xx 전용) ====================
// ====================================================================
// GPIO Mode (MODE[1:0] 비트): 핀의 최대 출력 속도를 설정
#define GPIO_MODE_INPUT_VAL         0x00 // 입력 모드 (속도 무의미)
#define GPIO_MODE_OUTPUT_10MHZ_VAL  0x01 // 출력 모드, 최대 10MHz
#define GPIO_MODE_OUTPUT_2MHZ_VAL   0x02 // 출력 모드, 최대 2MHz
#define GPIO_MODE_OUTPUT_50MHZ_VAL  0x03 // 출력 모드, 최대 50MHz

// GPIO CNF (Configuration CNF[1:0] 비트): 핀의 입출력 형태를 설정
// Input 모드용 CNF 값
#define GPIO_CNF_ANALOG_IN          0x00 // 입력: 아날로그 모드
#define GPIO_CNF_FLOATING_IN        0x01 // 입력: 플로팅 입력 (기본값)
#define GPIO_CNF_PULLUP_PULLDOWN_IN 0x02 // 입력: 풀업/풀다운 저항 사용

// Output 모드용 CNF 값
#define GPIO_CNF_GP_PP_OUT          0x00 // 출력: 범용 푸시-풀 출력 (General Purpose Push-Pull)
#define GPIO_CNF_GP_OD_OUT          0x01 // 출력: 범용 오픈-드레인 출력 (General Purpose Open-Drain)
#define GPIO_CNF_AF_PP_OUT          0x02 // 출력: 대체 기능 푸시-풀 출력 (Alternate Function Push-Pull)
#define GPIO_CNF_AF_OD_OUT          0x03 // 출력: 대체 기능 오픈-드레인 출력 (Alternate Function Open-Drain)


// PinState 정의: GPIO 핀 출력 상태 설정용
#define GPIO_PIN_RESET ((uint8_t)0x00) // 핀을 LOW로 설정
#define GPIO_PIN_SET   ((uint8_t)0x01) // 핀을 HIGH로 설정


// GPIO_Pin_x 값 (1 << pin_number): 각 핀의 비트 마스크
#define GPIO_PIN_0                 ((uint16_t)0x0001)
#define GPIO_PIN_1                 ((uint16_t)0x0002)
#define GPIO_PIN_2                 ((uint16_t)0x0004)
#define GPIO_PIN_3                 ((uint16_t)0x0008)
#define GPIO_PIN_4                 ((uint16_t)0x0010)
#define GPIO_PIN_5                 ((uint16_t)0x0020)
#define GPIO_PIN_6                 ((uint16_t)0x0040)
#define GPIO_PIN_7                 ((uint16_t)0x0080)
#define GPIO_PIN_8                 ((uint16_t)0x0100)
#define GPIO_PIN_9                 ((uint16_t)0x0200)
#define GPIO_PIN_10                ((uint16_t)0x0400)
#define GPIO_PIN_11                ((uint16_t)0x0800)
#define GPIO_PIN_12                ((uint16_t)0x1000)
#define GPIO_PIN_13                ((uint16_t)0x2000)
#define GPIO_PIN_14                ((uint16_t)0x4000)
#define GPIO_PIN_15                ((uint16_t)0x8000)


#define RCC_SYSTEM_CLCOK           (64000000)

// GPIO 함수 프로토타입 선언
void init_gpio(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint8_t GPIO_Speed_Val, uint8_t GPIO_Cnf_Val);
void GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint8_t PinState);


#endif /* GPIO_H_ */
