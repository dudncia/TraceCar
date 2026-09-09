#ifndef __GRAY_H
#define __GRAY_H

#include "main.h"
#include "stm32f1xx_hal.h"

/**
 * 8 路灰度传感器驱动（雅博 8 路灰度模块，USART3 接口）
 *
 * 硬件:  USART3 部分重映射 -> TX=PC10, RX=PC11, 波特率 115200, 8N1
 * 协议:  '$' 开头 '#' 结尾
 *   数字量帧: $D,x1:0,x2:1,x3:0,x4:0,x5:0,x6:0,x7:1,x8:1#
 *             （0=压到黑线, 1=白/未压线）
 *   模拟量帧: $A,x1:1000,x2:3450,...,x8:80#   （0~4096 灰度值）
 *   配置帧(主控->模块): $0,模拟,数字#   例 $0,0,1# = 请求数字量
 */

#define GRAY_NUM   8

extern uint8_t  gray_digital[GRAY_NUM];   // 数字量 0/1
extern uint16_t gray_analog[GRAY_NUM];    // 模拟量 0~4096
extern volatile uint8_t gray_updated;     // 收到新帧 = 1

void Gray_Init(UART_HandleTypeDef *huart);      // 传入 &huart2
void Gray_UartRxCallback(void);                 // 在接收完成回调里调用
uint8_t  Gray_GetDigital(uint8_t ch);           // ch = 0~7
uint16_t Gray_GetAnalog(uint8_t ch);
float Gray_CalcLineError(void);                 // 数字量加权偏差(-3.5~+3.5)
float Gray_CalcLineErrorAnalog(void);           // 模拟量加权偏差(-3.5~+3.5)
void Gray_SendConfig(uint8_t analog, uint8_t digital);

#endif
