#ifndef DJI_NEWBATTERY_DJI_BATTERY_HPP
#define DJI_NEWBATTERY_DJI_BATTERY_HPP

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"
#include "i2c.h"

#ifdef __cplusplus
};
#endif


#define DJI_BATTERY_ADDR 0x41
#define DJI_BATTERY_WHO_AM_I 0x1F
#define DJI_BATTERY_DESIGNED_CAPACITY 0x20
#define DJI_BATTERY_LOOP_TIMES 0x24
#define DJI_BATTERY_PRODUCTION_DATE 0x26
#define DJI_BATTERY_LIFE 0x28
#define DJI_BATTERY_CURRENT_VOLTAGE 0x29
#define DJI_BATTERY_CURRENT_CURRENT 0x2D
#define DJI_BATTERY_TEMPERATURE 0x31
#define DJI_BATTERY_CAPACITY_PERCENT 0x33
#define DJI_BATTERY_INTERNAL_STATE 0x34
#define DJI_BATTERY_ERROR_STATE 0x35

class Battery_c
{
public:
    typedef enum : uint8_t
    {
        WHO_AM_I = 0,
        DESIGNED_CAPACITY, // 设计容量
        LOOP_TIMES,        // 循环次数
        PRODUCTION_DATE,   // 生产日期
        LIFE,              // 寿命
        CURRENT_VOLTAGE,   // 当前电压
        CURRENT_CURRENT,   // 当前电流
        TEMPERATURE,       // 温度
        CAPACITY_PERCENT,  // 容量百分比
        INTERNAL_STATE,    // 内部状态
        ERROR_STATE        // 错误状态
    } AddrTable_e;

    typedef union
    {
        uint16_t raw_date;
        struct
        {
            uint16_t day : 5;
            uint16_t month : 4;
            uint16_t year : 7;
        } date;
    } ProductionDate_u;

    typedef union
    {
        uint8_t raw_state;
        struct
        {
            uint8_t output_state : 1;
            uint8_t connect_state : 1;
            uint8_t air : 6;
        } state;
    } InternalState_u;

    typedef union
    {
        uint8_t raw_error;
        struct
        {
            uint8_t short_circuit : 1;
            uint8_t overload : 1;
            uint8_t over_current : 1;
            uint8_t over_temperature : 1;
            uint8_t under_voltage : 1;
            uint8_t cell_error : 1;
            uint8_t other_error : 1;
            uint8_t air : 1;
        } error;
    } ErrorState_u;

    typedef struct
    {
        uint8_t who_am_i;
        uint32_t designed_capacity;
        uint16_t loop_times;
        ProductionDate_u production_date;
        uint8_t life;
        int32_t current_voltage;
        int32_t current_current;
        int16_t temperature;
        uint8_t capacity_percent;
        InternalState_u internal_state;
        ErrorState_u error_state;
    } Data_t;

    Battery_c(I2C_HandleTypeDef* i2c_handle);
    void Do_Read(AddrTable_e read);
    void Do_Read(void);
    Data_t &rx_data() { return rx_data_; }
    HAL_StatusTypeDef working_status() { return working_status_; }

private:
    I2C_HandleTypeDef* i2c_handle_ = &hi2c1;
    Data_t rx_data_;
    HAL_StatusTypeDef working_status_;

};


#endif //DJI_NEWBATTERY_DJI_BATTERY_HPP
