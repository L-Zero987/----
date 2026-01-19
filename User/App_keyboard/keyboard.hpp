#ifndef __Keyboard_HPP
#define __Keyboard_HPP

#ifdef __cplusplus
extern "C"
{
#endif
#include "main.h"
#include "gpio.h"
#ifdef __cplusplus
}
#endif

#define KEY_NUM_MAX     2               // 最多支持2键
#define KEY_HOLD_CNT    200             // 长按检测次数
#define KEY_IDLE_LEVEL  GPIO_PIN_RESET  // 按键未按下电平

namespace Keyboard_n
{
    typedef enum // 按键状态
    {
        KEY_Air   = 0b0000, // 无按压          0
        KEY_Press = 0b0001, // 按压            1
        KEY_Up    = 0b0010, // 抬起            2
        KEY_Down  = 0b0011, // 按下            3
        KEY_Idle  = 0b0100, // 空闲（长时间未按下 4
        KEY_Hold  = 0b0101  // 长按            5
    } Key_State_e;

    typedef enum // 状态标志位
    {
        Press  = 0b0001, // 按压位
        Change = 0b0010, // 状态变化位
        Hold   = 0b0100  // 长按位
    } Key_CheckBit_e;

    typedef struct // 初始化参数
    {
        GPIO_TypeDef* key_gpiox;   // 按键GPIO组
        uint16_t key_gpio_Pin;     // 按键GPIO引脚
        uint16_t counter;          // 计数器(用于长按检测
    } Key_Param_s;

    void Keyboard_Init(uint8_t key_num, Key_Param_s* config);
    void Keyboard_ClearKeyState(uint8_t key_num, Key_CheckBit_e state);
    Key_State_e Keyboard_GetKeyState(uint8_t key_num);
    void Keyboard_Loop(void);
}

#endif //! __Keyboard_HPP
