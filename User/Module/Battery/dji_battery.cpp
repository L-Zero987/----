#include "dji_battery.hpp"

extern "C"
{
#include <string.h>
}

const uint8_t DJI_BATTERY_TABLE[11][2] =
        {
                {DJI_BATTERY_WHO_AM_I, 1},
                {DJI_BATTERY_DESIGNED_CAPACITY, 4},
                {DJI_BATTERY_LOOP_TIMES, 2},
                {DJI_BATTERY_PRODUCTION_DATE, 2},
                {DJI_BATTERY_LIFE, 1},
                {DJI_BATTERY_CURRENT_VOLTAGE, 4},
                {DJI_BATTERY_CURRENT_CURRENT, 4},
                {DJI_BATTERY_TEMPERATURE, 2},
                {DJI_BATTERY_CAPACITY_PERCENT, 1},
                {DJI_BATTERY_INTERNAL_STATE, 1},
                {DJI_BATTERY_ERROR_STATE, 1}
        };

Battery_c::Battery_c(I2C_HandleTypeDef* i2c_handle) : i2c_handle_(i2c_handle)
{

}

void Battery_c::Do_Read(AddrTable_e read)
{
    uint8_t data[4] = {0,0,0,0};

    working_status_ = HAL_I2C_Mem_Read(i2c_handle_, (DJI_BATTERY_ADDR << 1), DJI_BATTERY_TABLE[read][0], I2C_MEMADD_SIZE_8BIT, data, DJI_BATTERY_TABLE[read][1], 1000);

    switch (read)
    {
        case WHO_AM_I:
            rx_data_.who_am_i = data[0];
            break;
        case DESIGNED_CAPACITY:
            memcpy(&(rx_data_.designed_capacity), &data[0], 4);
            break;
        case LOOP_TIMES:
            memcpy(&(rx_data_.loop_times), &data[0], 2);
            break;
        case PRODUCTION_DATE:
            memcpy(&(rx_data_.production_date.raw_date), &data[0], 2);
            break;
        case LIFE:
            rx_data_.life = data[0];
            break;
        case CURRENT_VOLTAGE:
            memcpy(&(rx_data_.current_voltage), &data[0], 4);
            break;
        case CURRENT_CURRENT:
            memcpy(&(rx_data_.current_current), &data[0], 4);
            break;
        case TEMPERATURE:
            memcpy(&(rx_data_.temperature), &data[0], 2);
            break;
        case CAPACITY_PERCENT:
            rx_data_.capacity_percent = data[0];
            break;
        case INTERNAL_STATE:
            rx_data_.internal_state.raw_state = data[0];
            break;
        case ERROR_STATE:
            rx_data_.error_state.raw_error = data[0];
            break;
        default:
            break;
    }
}

void Battery_c::Do_Read()
{
    for(uint8_t i = 0; i < 11; i++)
    {
        this->Do_Read((Battery_c::AddrTable_e)i);
    }
}
