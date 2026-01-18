/*
 * spi.c
 *
 *  Created on: 2026. 1. 18.
 *      Author: minseopkim
 */

#include "spi.h"

/**
  * @brief  SPI1 마스터 모드 초기화 함수
  *         GPIO 핀 설정, SPI 주변장치 설정, RXNE 인터럽트 활성화 및 NVIC 설정.
  * @retval 없음
  */
void SPI1_init(void) {
    // 1. SPI1 주변장치 클럭 활성화 (SPI1은 APB2 버스에 연결됨)
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    // 2. GPIO 핀 설정
    // SCK (PA5) : 대체 기능 푸시-풀 출력, 50MHz
    init_gpio(SPI_PORT, SCK_PIN,  GPIO_MODE_OUTPUT_50MHZ_VAL, GPIO_CNF_AF_PP_OUT);
    // MOSI (PA7) : 대체 기능 푸시-풀 출력, 50MHz
    init_gpio(SPI_PORT, MOSI_PIN, GPIO_MODE_OUTPUT_50MHZ_VAL, GPIO_CNF_AF_PP_OUT);
    // MISO (PA6) : 플로팅 입력 (마스터 모드에서 데이터를 수신하므로 입력으로 설정)
    init_gpio(SPI_PORT, MISO_PIN, GPIO_MODE_INPUT_VAL,        GPIO_CNF_FLOATING_IN); // MISO는 플로팅 입력으로

    // 3. SPI1 주변장치 초기화 (CR1, CR2 레지스터)
    // - Mode 0 (CPOL=0, CPHA=0)으로 설정: 클럭 유휴 상태는 LOW, 첫 번째 클럭 엣지에서 데이터 샘플링
    // - 마스터 모드 (MSTR 비트 설정)
    // - 보드레이트 프리스케일러: PCLK2 / 4 (PCLK2=64MHz -> SPI 클럭 16MHz)
    // - 소프트웨어 슬레이브 관리 (SSM, SSI 비트 설정): CS 핀을 소프트웨어로 제어
    SPI1->CR1 |= ( (0 << SPI_CR1_CPOL_Pos) |        // CPOL=0
                   (0 << SPI_CR1_CPHA_Pos) |        // CPHA=0 (ILI9341 데이터시트에 따라)
                   (1 << SPI_CR1_MSTR_Pos) |        // 마스터 모드
                   (1 << SPI_CR1_BR_Pos)   |        // 보드레이트 프리스케일러: PCLK2 / 4 (0b001)
                   (1 << SPI_CR1_SSM_Pos)  |        // 소프트웨어 슬레이브 관리 활성화
                   (1 << SPI_CR1_SSI_Pos) );        // 내부 슬레이브 셀렉트 (마스터 모드에서 SSM 사용 시)

    // CR2 레지스터: RXNE (Receive buffer Not Empty) 인터럽트 활성화
    // LCD 제어에는 송신만 주로 하지만, ID를 읽거나 하는 경우를 위해 활성화 (디버깅에도 도움)
    SPI1->CR2 |= SPI_CR2_RXNEIE;

    // 4. SPI 주변장치 활성화 (SPE 비트 설정)
    SPI1->CR1 |= SPI_CR1_SPE;

    // 5. NVIC (Nested Vectored Interrupt Controller) 설정
    NVIC_SetPriority(SPI1_IRQn, 0);     // SPI1 인터럽트 우선순위 설정 (0이 가장 높은 우선순위)
    NVIC_EnableIRQ(SPI1_IRQn);          // SPI1 인터럽트 활성화
}

/**
  * @brief  SPI1을 통해 8비트 데이터를 송수신하는 함수 (블로킹 방식)
  * @param  data: 전송할 8비트 데이터
  * @retval 수신된 8비트 데이터
  */
uint8_t SPI1_transfer(uint8_t data) {
    // 1. TX 버퍼가 비어있을 때까지 대기 (데이터 전송 준비)
    while (!(SPI1->SR & SPI_SR_TXE));

    // 2. DR(데이터 레지스터)에 데이터를 씀으로써 전송 시작
    SPI1->DR = data;

    // 3. RX 버퍼에 데이터가 들어올 때까지 대기 (데이터 수신 완료)
    // while (!(SPI1->SR & SPI_SR_RXNE));

    // 4. DR에서 데이터를 읽어와 반환 (수신된 데이터)
    // return (uint8_t)SPI1->DR;
}

/**
  * @brief  SPI1 글로벌 인터럽트 핸들러 (ISR)
  *         startup_stm32f103xb.s 파일의 벡터 테이블에 정의된 `SPI1_IRQHandler` 이름과 일치해야 함.
  */
void SPI1_IRQHandler(void) {
    // RXNE (Receive buffer Not Empty) 플래그가 설정되었는지 확인
    if (SPI1->SR & SPI_SR_RXNE) {
        // DR에서 데이터를 읽으면 RXNE 플래그가 자동으로 클리어됨 (필수!)
        volatile uint8_t received_byte = (uint8_t)SPI1->DR;
    }
    // OVR (Overrun error) 플래그가 설정되었는지 확인
    if (SPI1->SR & SPI_SR_OVR) {
        // OVR 플래그를 클리어하려면 SR 레지스터를 읽고, DR 레지스터를 읽어야 함.
        volatile uint32_t dummy_read_sr = SPI1->SR;
        volatile uint32_t dummy_read_dr = SPI1->DR;
        (void)dummy_read_sr; (void)dummy_read_dr; // unused variable warning 방지
    }
}
// ====================================================================
// ==== /SPI 드라이버 통합 끝 =========================================
