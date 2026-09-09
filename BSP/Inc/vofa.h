#ifndef __VOFA_H
#define __VOFA_H

#include "main.h"

/**
 * @brief 初始化
 * @param USART_HandleTypeDef* huart: 指向USART句柄的指针
 * @return 成功返回0，失败返回非0值
 */
void VOFA_Init(UART_HandleTypeDef* huart);

/**
 *  @brief 发送数据
 *  @param  data: 指向要发送的数据数组的指针
 *  @param  count: 要发送的数据数量
 */ 
void VOFA_SendData(float*data,uint8_t count);

#endif
