#include "vofa.h"
#include <stdio.h>  // 用于 sprintf
#include <string.h> // 用于 strlen


static UART_HandleTypeDef* vofa_huart = NULL;//接收串口句柄
static uint8_t vofa_tx_buffer[100]; // 发送缓冲区 100字节

/**
 * @brief 初始化
 * @param USART_HandleTypeDef* huart: 指向USART句柄的指针
 * @return 成功返回0，失败返回非0值
 */
void VOFA_Init(UART_HandleTypeDef* huart)
{
    vofa_huart = huart;
}

/**
 *  @brief 发送数据
 *  @param  data: 指向要发送的数据数组的指针
 *  @param  count: 要发送的数据数量
 */ 
void VOFA_SendData(float*data,uint8_t count)
{
    if(vofa_huart==NULL)
    {
        return;
    }
    int current_position=0;
    for(int i=0;i<count;i++)
    {
        //snprintf用来防止缓冲区溢出
        //int chars_written=snprintf((char*)vofa_tx_buffer+current_position,sizeof(vofa_tx_buffer-current_position),"%.2f,",data[i]);
				int chars_written=snprintf((char*)vofa_tx_buffer+current_position,sizeof(vofa_tx_buffer),"%.2f,",data[i]);
        //判断是否写入成功
        if(chars_written>0)
        {
            current_position+=chars_written;
        }
        else
        {
            break;
        }
    }
    if(current_position>0)
    {
        vofa_tx_buffer[current_position-1]='\r';
        vofa_tx_buffer[current_position]='\n';
        HAL_UART_Transmit(vofa_huart,(uint8_t*)vofa_tx_buffer,current_position+1,100);
    }
}
