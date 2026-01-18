/*
 * spi.h
 *
 *  Created on: 2026. 1. 18.
 *      Author: minseopkim
 */

#ifndef SPI_H_
#define SPI_H_

#include "gpio.h"
#include "pin_define.h"
// ====================================================================
// ==== SPI 드라이버 통합 시작 =========================================
// ====================================================================
// SPI1 주변장치와 연결된 GPIO 핀 정의 (GPIOA 포트 사용)
// ILI9341 LCD의 경우 SDO (MISO) 핀이 데이터를 읽는 데만 사용되므로, 
// 일반적으로는 연결하지 않아도 되지만, MCU의 SPI 통신은 RX 핀이 필요하다.
// SPI1의 MISO (PA6) 핀은 루프백 테스트와 ID 읽기 등에서 필요하다.

// SPI1 RX 인터럽트 발생 횟수를 저장할 전역 변수 (SPI 디버깅용)

// SPI 함수 프로토타입 선언
void SPI1_init(void);
uint8_t SPI1_transfer(uint8_t data);


#endif /* SPI_H_ */
