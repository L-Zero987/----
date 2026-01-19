#include "my_main.hpp"
#include "dji_battery.hpp"
#include "keyboard.hpp"

extern "C"
{
#include "oled.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
}

Keyboard_n::Key_State_e key_state[2];
Battery_c* dji_battery;
Keyboard_n::Key_Param_s config[2] =
    {
        {GPIOB, GPIO_PIN_9}, // 左
        {GPIOA, GPIO_PIN_3}  // 右
    };
uint8_t now_page = 0;
bool is_details = false;
const char *SHOW_TABLE[4] = {"Info","Life","Detail","State"};

bool is_refresh = false;

// 页面切换与详情切换
inline void Main_ChangePage()
{
    if(key_state[0] == Keyboard_n::KEY_Down)
    {
        Keyboard_n::Keyboard_ClearKeyState(0, Keyboard_n::Change); // 清除状态变化位
        if(is_details)
        {
            return;
        }

        if(now_page > 2)
        {
            now_page = 0;
        }
        else
        {
            now_page++;
        }
        is_refresh = false;
    }
    if(key_state[1] == Keyboard_n::KEY_Down)
    {
        Keyboard_n::Keyboard_ClearKeyState(1, Keyboard_n::Change); // 清除状态变化位
        is_details = !is_details;
    }

}

// 读取电池数据
void Main_ReadBattery()
{
    if(!is_details) // 非详情页不读取数据
    {
        return;
    }

    switch (now_page)
    {
        case 0: // 设计容量 生产日期
            dji_battery->Do_Read(Battery_c::DESIGNED_CAPACITY);
            dji_battery->Do_Read(Battery_c::PRODUCTION_DATE);
            break;
        case 1: // 循环次数 寿命 当前容量
            dji_battery->Do_Read(Battery_c::LOOP_TIMES);
            dji_battery->Do_Read(Battery_c::LIFE);
            dji_battery->Do_Read(Battery_c::CAPACITY_PERCENT);
            break;
        case 2: // 当前电压 当前电流 温度
            dji_battery->Do_Read(Battery_c::CURRENT_VOLTAGE);
            dji_battery->Do_Read(Battery_c::CURRENT_CURRENT);
            dji_battery->Do_Read(Battery_c::TEMPERATURE);
            break;
        case 3: // 内部状态 错误状态
            dji_battery->Do_Read(Battery_c::INTERNAL_STATE);
            dji_battery->Do_Read(Battery_c::ERROR_STATE);
            break;
        default:
            break;
    }
    is_refresh = true; // 标记数据已刷新
}

// 页面详情显示
void Main_PageDetails()
{
    if(is_details)
    {
        // 读取数据失败不刷新显示
        if(dji_battery->working_status() != HAL_OK)
        {
            OLED_Clear();
            OLED_ShowString(0,0,(char *)"not connect...",OLED_8X16);
            OLED_Update();
            return;
        }

        // 无刷新标记不刷新显示
        if(!is_refresh)
        {
            return;
        }

        OLED_Clear();
        switch (now_page)
        {
            case 0: // 设计容量 生产日期
                OLED_ShowString(0,0,(char *)"design capacity",OLED_8X16);
                OLED_ShowNum(0,16,dji_battery->rx_data().designed_capacity,5,OLED_8X16);
                OLED_ShowString(40,16,(char *)"mAh",OLED_8X16);
                OLED_ShowString(0,32,(char *)"produced date",OLED_8X16);
                OLED_ShowNum(0,48,1980 + dji_battery->rx_data().production_date.date.year,4,OLED_8X16);
                OLED_ShowNum(40,48,dji_battery->rx_data().production_date.date.month,2,OLED_8X16);
                OLED_ShowNum(64,48,dji_battery->rx_data().production_date.date.day,2,OLED_8X16);
                break;
            case 1: // 循环次数 寿命 当前容量
                OLED_ShowString(0,4,(char *)"loop times",OLED_8X16);
                OLED_ShowNum(96,4,dji_battery->rx_data().loop_times,4,OLED_8X16);
                OLED_ShowString(0,24,(char *)"life",OLED_8X16);
                OLED_ShowNum(112,24,dji_battery->rx_data().life,2,OLED_8X16);
                OLED_ShowString(0,44,(char *)"now capacity",OLED_8X16);
                OLED_ShowNum(112,44,dji_battery->rx_data().capacity_percent,2,OLED_8X16);
                break;
            case 2: // 当前电压 当前电流 温度
                OLED_ShowSignedNum(0,4,dji_battery->rx_data().current_voltage,10,OLED_8X16);
                OLED_ShowString(88,4,(char *)"mV",OLED_8X16);
                OLED_ShowSignedNum(0,24,dji_battery->rx_data().current_current,10,OLED_8X16);
                OLED_ShowString(88,24,(char *)"mA",OLED_8X16);
                OLED_ShowSignedNum(0,44,dji_battery->rx_data().temperature / 10,4,OLED_8X16);
                OLED_ShowChar(40,44,'.',OLED_8X16);
                OLED_ShowNum(48,44,dji_battery->rx_data().temperature % 10,1,OLED_8X16);
                OLED_ShowChar(56,44,'C',OLED_8X16);
                break;
            case 3: // 内部状态 错误状态
                OLED_ShowString(0,0,(char *)"internal state",OLED_8X16);
                OLED_ShowBinNum(0,16,dji_battery->rx_data().internal_state.raw_state,8,OLED_8X16);
                OLED_ShowString(0,32,(char *)"error state",OLED_8X16);
                OLED_ShowBinNum(0,48,dji_battery->rx_data().error_state.raw_error,8,OLED_8X16);
                break;
            default:
                break;
        }
        is_refresh = false; // 重置刷新标记
    }
    else
    {
        OLED_Clear();
        OLED_ShowString(0,0,(char *)SHOW_TABLE[0],OLED_8X16);
        OLED_ShowString(0,16,(char *)SHOW_TABLE[1],OLED_8X16);
        OLED_ShowString(0,32,(char *)SHOW_TABLE[2],OLED_8X16);
        OLED_ShowString(0,48,(char *)SHOW_TABLE[3],OLED_8X16);
        OLED_ReverseArea(0,now_page * 16,128,16);
    }
    OLED_Update();
}

void Main_Init(void)
{
    HAL_Delay(100);
    dji_battery = new Battery_c(&hi2c1);
    OLED_Init(&hi2c1);
    HAL_Delay(100);
    OLED_Clear();
    OLED_Update();
    HAL_Delay(100);
    Keyboard_n::Keyboard_Init(2,config);
}

void MainLoop(void const * argument)
{
    while (1)
    {
        static uint8_t cnt = 0;
        cnt++;
        if(cnt == 101)
        {
            Main_ReadBattery();
        }
        else if (cnt > 200)
        {
            Main_PageDetails();
            cnt = 0;
        }
        vTaskDelay(1);
    }
}

void KeyLoop(void const * argument)
{
    while (1)
    {
        Keyboard_n::Keyboard_Loop();
        key_state[0] = Keyboard_n::Keyboard_GetKeyState(0);
        key_state[1] = Keyboard_n::Keyboard_GetKeyState(1);
        Main_ChangePage();
        vTaskDelay(1);
    }
}
