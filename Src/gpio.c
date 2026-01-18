/*
 * gpio.c
 *
 *  Created on: 2026. 1. 18.
 *      Author: minseopkim
 */


#include "gpio.h"


  /* @brief  GPIO 핀을 초기화하는 함수 (STM32F1xx의 CRL/CRH 레지스터 구조에 맞춰 재설계)
  * @param  GPIOx: 제어할 GPIO 포트 (예: GPIOA, GPIOB 등)
  * @param  GPIO_Pin: 초기화할 핀의 비트 마스크 (예: GPIO_PIN_0)
  * @param  GPIO_Speed_Val: 핀의 출력 속도 설정 (예: GPIO_MODE_OUTPUT_50MHZ_VAL)
  * @param  GPIO_Cnf_Val: 핀의 컨피규레이션 설정 (예: GPIO_CNF_AF_PP_OUT)
  * @retval 없음
  */
void init_gpio(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint8_t GPIO_Speed_Val, uint8_t GPIO_Cnf_Val) {
    uint32_t pinpos = 0x00;     // 현재 처리 중인 핀 번호 (0~15)
    uint32_t currentpin = 0x00; // GPIO_Pin에 해당하는 비트 마스크
    uint32_t tempreg = 0x00;    // CRL 또는 CRH 레지스터 값을 임시 저장할 변수
    uint32_t pin_config = 0x00; // MODE와 CNF 비트를 합친 4비트 설정 값

    // 1. 해당 GPIO 포트의 클럭 활성화
    if (GPIOx == GPIOA) RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    else if (GPIOx == GPIOB) RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    else if (GPIOx == GPIOC) RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    else if (GPIOx == GPIOD) RCC->APB2ENR |= RCC_APB2ENR_IOPDEN;

    // 각 핀(0~15)에 대해 반복
    for (pinpos = 0x00; pinpos < 0x10; pinpos++) {
        currentpin = (GPIO_Pin & (0x0001 << pinpos)); // GPIO_Pin에 현재 핀이 포함되는지 확인
        if (currentpin == (0x0001 << pinpos)) { // 현재 핀을 설정해야 한다면
            // MODE (2비트)와 CNF (2비트)를 합쳐 4비트 설정 값 생성
            // CNF 비트가 상위 2비트, MODE 비트가 하위 2비트
            pin_config = (GPIO_Cnf_Val << 2) | GPIO_Speed_Val;

            if (pinpos < 8) { // Pin 0-7: CRL (Control Register Low)
                tempreg = GPIOx->CRL;
                // 해당 핀의 4비트(MODE[1:0] + CNF[1:0]) 영역을 0으로 클리어
                tempreg &= ~((uint32_t)(0xF << (pinpos * 4)));
                // 새로운 설정 값(pin_config)을 해당 핀 위치에 설정
                tempreg |= (pin_config << (pinpos * 4));
                GPIOx->CRL = tempreg; // 레지스터에 최종 값 쓰기
            } else { // Pin 8-15: CRH (Control Register High)
                tempreg = GPIOx->CRH;
                // 해당 핀의 4비트 영역을 0으로 클리어 (CRH는 핀 8부터 시작하므로 pinpos-8)
                tempreg &= ~((uint32_t)(0xF << ((pinpos - 8) * 4)));
                // 새로운 설정 값을 해당 핀 위치에 설정
                tempreg |= (pin_config << ((pinpos - 8) * 4));
                GPIOx->CRH = tempreg; // 레지스터에 최종 값 쓰기
            }
        }
    }
}

/**
  * @brief  특정 GPIO 핀의 출력 상태를 설정합니다. (LOW/HIGH)
  * @param  GPIOx: 제어할 GPIO 포트
  * @param  GPIO_Pin: 설정할 핀의 비트 마스크
  * @param  PinState: 설정할 핀 상태 (GPIO_PIN_RESET=LOW, GPIO_PIN_SET=HIGH)
  * @retval 없음
  */
void GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint8_t PinState) {
    if (PinState != GPIO_PIN_RESET) {
        GPIOx->BSRR = GPIO_Pin; // BSRR (Bit Set/Reset Register)를 사용하여 핀을 HIGH로 설정
    } else {
        GPIOx->BRR = GPIO_Pin;  // BRR (Bit Reset Register)를 사용하여 핀을 LOW로 설정
    }
}
// ====================================================================
// ==== /GPIO 드라이버 통합 끝 =========================================

