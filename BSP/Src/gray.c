#include "gray.h"
#include <string.h>
#include <stdlib.h>

uint8_t  gray_digital[GRAY_NUM];
uint16_t gray_analog[GRAY_NUM];
volatile uint8_t gray_updated = 0;

static UART_HandleTypeDef *s_huart;//选择串口
static uint8_t  s_rx_byte;
static uint8_t  s_frame[64];//传感器
static uint16_t s_frame_idx;

/* 8 个探头的位置坐标（中心为 0） */
static const float s_pos[GRAY_NUM] =
    {-3.5f, -2.5f, -1.5f, -0.5f, 0.5f, 1.5f, 2.5f, 3.5f};

void Gray_Init(UART_HandleTypeDef *huart)
{
    s_huart = huart;
    s_frame_idx = 0;
    gray_updated = 0;
    memset(gray_digital, 0, sizeof(gray_digital));
    memset(gray_analog,  0, sizeof(gray_analog));
    HAL_UART_Receive_IT(huart, &s_rx_byte, 1);   // 启动逐字节中断接收
}

/* 解析一帧 $D,...# 或 $A,...# */
static void Gray_ParseFrame(uint8_t *buf, uint16_t len)
{
    if (len < 3 || buf[0] != '$') return;

    if (buf[1] == 'D')              // 数字量帧
    {
        for (int i = 0; i < GRAY_NUM; i++)
        {
            uint16_t pos = 6 + i * 5;          // 第 i 路的值位置（同示例协议）
            if (pos < len)
                gray_digital[i] = (buf[pos] == '1') ? 1 : 0;
        }
        gray_updated = 1;
    }
    else if (buf[1] == 'A')         // 模拟量帧
    {
        const char *p = (const char *)buf + 2; // 跳过 "$A"
        for (int i = 0; i < GRAY_NUM; i++)
        {
            char *colon = strchr(p, ':');
            if (colon == NULL) break;
            p = colon + 1;
            gray_analog[i] = (uint16_t)atoi(p);

            char *comma = strchr(p, ',');
            if (comma == NULL) break;
            p = comma + 1;
        }
        gray_updated = 1;
    }
}

/* 接收完成回调：喂入一个字节并维持接收 */
void Gray_UartRxCallback(void)
{
    static uint8_t started = 0;
    uint8_t c = s_rx_byte;

    if (c == '$')
    {
        started = 1;
        s_frame_idx = 0;
        s_frame[s_frame_idx++] = c;
    }
    else if (started)
    {
        if (s_frame_idx < sizeof(s_frame))
            s_frame[s_frame_idx++] = c;

        if (c == '#')
        {
            started = 0;
            Gray_ParseFrame(s_frame, s_frame_idx);
            s_frame_idx = 0;
        }
        else if (s_frame_idx >= sizeof(s_frame))   // 溢出保护
        {
            started = 0;
            s_frame_idx = 0;
        }
    }

    HAL_UART_Receive_IT(s_huart, &s_rx_byte, 1);   // 继续接收下一字节
}

uint8_t Gray_GetDigital(uint8_t ch)
{
    return (ch < GRAY_NUM) ? gray_digital[ch] : 0;
}

uint16_t Gray_GetAnalog(uint8_t ch)
{
    return (ch < GRAY_NUM) ? gray_analog[ch] : 0;
}

/* 数字量加权：0=压线视为权重 */
float Gray_CalcLineError(void)
{
    float num = 0.0f, den = 0.0f;
    for (int i = 0; i < GRAY_NUM; i++)
    {
        float on = (gray_digital[i] == 0) ? 1.0f : 0.0f;
        num += on * s_pos[i];
        den += on;
    }
    if (den < 0.5f) return 0.0f;   // 丢线
    return num / den;
}

/* 模拟量加权：假设白=高值、黑=低值，取反使黑线权重高 */
float Gray_CalcLineErrorAnalog(void)
{
    float num = 0.0f, den = 0.0f;
    for (int i = 0; i < GRAY_NUM; i++)
    {
        float w = 4095.0f - (float)gray_analog[i];
        if (w < 0.0f) w = 0.0f;
        num += w * s_pos[i];
        den += w;
    }
    if (den < 0.5f) return 0.0f;   // 丢线/全白
    return num / den;
}

void Gray_SendConfig(uint8_t analog, uint8_t digital)
{
    uint8_t buf[8] = "$0,0,0#";
    buf[3] = analog  ? '1' : '0';   // 模拟量开关
    buf[5] = digital ? '1' : '0';   // 数字量开关
    HAL_UART_Transmit(s_huart, buf, 7, 100);
}
