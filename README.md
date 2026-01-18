# STM32F103RB-ILI9341-SIMPLE-EXAMPLE
 이 문서는 STM32F103RB 마이크로컨트롤러를 사용하여 ILI9341 TFT LCD를 제어하는 간단한 프로젝트입니다.
 
# ILI9341 LCD 연결 및 동작 스펙 (STM32F103RB)

*   **디스플레이 해상도**: 320x240 픽셀
*   **MCU Board**: STM32F103RB
*   **시스템 클럭 (SYSCLK)**: 64MHz
*   **LCD VCC**: 3.3V
*   **SPI 통신 모드**: SPI Mode 0 (CPOL=0, CPHA=0)
*   **SPI 클럭 속도**: 16MHz 

### 핀 연결 정보

| PIN (ILI9341) | 설명                                   | STM32F103RB PIN |
|:--------------|:---------------------------------------|:----------------|
| SDO(MISO)     | SPI 통신 슬레이브 데이터 수신용        | PA6             |
| LED           | 백라이트 핀 (상시 전원)                | 5V              |
| SCK           | SPI 클럭 핀                            | PA5             |
| SDI(MOSI)     | SPI 통신 커맨드/데이터 전송용          | PA7             |
| DC            | Data/Command 선택 핀 (0: Command / 1: Data) | PB1             |
| RESET         | 리셋 핀 (LOW 액티브)                   | PB0             |
| CS            | SPI 통신 칩 선택 핀 (LOW일 때 동작)    | PA4             |
| GND           | 접지                                   | GND             |
| VCC           | 전원                                   | 3.3V            |



