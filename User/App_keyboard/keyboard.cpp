#include "keyboard.hpp"

namespace Keyboard_n
{
    Key_Param_s _key_table[KEY_NUM_MAX];
    Key_State_e _key_state[KEY_NUM_MAX];

    // 初始化按键
    void Keyboard_Init(uint8_t key_num, Key_Param_s* config)
    {
        for (uint8_t i = 0; i < key_num; i++)
        {
            _key_state[i]              = KEY_Idle;
            _key_table[i].key_gpiox    = config[i].key_gpiox;
            _key_table[i].key_gpio_Pin = config[i].key_gpio_Pin;
            _key_table[i].counter      = 0;
        }
    }

    // 清除按键状态位
    void Keyboard_ClearKeyState(uint8_t key_num, Key_CheckBit_e state)
    {
        _key_state[key_num] = (Key_State_e)(_key_state[key_num] & (~state));
    }

    // 获取按键状态
    Key_State_e Keyboard_GetKeyState(uint8_t key_num)
    {
        return _key_state[key_num];
    }

    // 按键状态更新循环
    void Keyboard_Loop(void)
    {
        for (uint8_t i = 0; i < KEY_NUM_MAX; i++)
        {
            // 局部变量，方便看
            uint8_t key_state       = _key_state[i];
            GPIO_TypeDef* key_GPIOx = _key_table[i].key_gpiox;
            uint16_t key_GPIO_Pin   = _key_table[i].key_gpio_Pin;
            uint16_t counter        = _key_table[i].counter + 1;
            GPIO_PinState key_level = HAL_GPIO_ReadPin(key_GPIOx, key_GPIO_Pin); // 只读一次
            if(key_state == KEY_Air) // 实现按键冷却
            {
                key_level = KEY_IDLE_LEVEL;
            }

            // 状态更新
            if(key_level != KEY_IDLE_LEVEL) // 按键按下
            {
                if((key_state & Press) == 0) // 上次未按下
                {
                    key_state |= Change; // 记录状态变化
                    key_state &= ~Hold;  // 清除长按标志
                    counter = 0;         // 重置计数器
                }
                key_state |= Press; // 记录按压位
            }
            else
            {
                if((key_state & Press) == 1) // 上次按下
                {
                    key_state |= Change; // 记录状态变化
                    key_state &= ~Hold;  // 清除长按标志
                    counter = 0;         // 重置计数器
                }
                key_state &= ~Press; // 清除按压位
            }

            if(counter > KEY_HOLD_CNT)
            {
                key_state |= Hold;    // 记录长按
                key_state &= ~Change; // 清除状态变化 (特殊情况，状态变化位未被外部擦除)
                counter = 0;
            }

            // 实现状态更新
            _key_state[i] = (Key_State_e)key_state;
            _key_table[i].counter = counter;
        }
    }
}
