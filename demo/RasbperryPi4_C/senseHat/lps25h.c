#include "lps25h.h"

/* 
** Initializes LPS25H Temperature and Pressure Sensor
** Reference : https://www.st.com/resource/en/datasheet/lps25hb.pdf
*/
LPS25H_CONFIG* lps25h_initialize(int fd)
{
    int ret = 0;
    LPS25H_CONFIG* config;

    printf("LPS25H init start\r\n");

    config = (LPS25H_CONFIG*)malloc(sizeof(LPS25H_CONFIG));

    if (config == NULL)
    {
        printf("ERR : Failed to allocate memory for LPS25H_CONFIG");
        return config;
    }

    (void)memset(config, 0, sizeof(LPS25H_CONFIG));

    config->isInitialized = false;
    config->fd = fd;

    if ((config->i2c_addr = search_i2c_device(config->fd, LPS25H_WHO_AM_I, LPS25H_I2C_ID)) < 0)
    {
        printf("ERR : Failed to find LPS25H\r\n");
        return config;
    }

    // Page 35 8.6 CTRL_REG1 (20h)
    // 7   : PD (Power Down Control) = 1 = Active
    // 6:4 : ORD : b100 Pressure 25Hz, Temperature 25Hz
    // 3   : DIFF_EN : Interrupt Enabled 0
    // 2   : BDU : Block Data Update 1
    // 1   : RESET_AZ : Reset Auto Zero 0
    // 0   : SIM : SPI Serial Interface Mode 0
    // b11000100 = 0xC4
    if ((ret = write_i2c(config->fd, config->i2c_addr, LPS25H_CTRL_REG1, 0xC4)) < 0)
    {
        printf("ERR : Failed to set LPS25H_CTRL_REG1 : %x %s\r\n", errno, strerror(errno));
        return config;
    }

    // Page 34 8.5 RES_CONF (10h)
    // 7:4 : Reserved
    // 3:2 : AVGT : Temperature Internal Average b01 16
    // 1:0 : AVGP : Pressure Internal Average b01 16
    // b00000101 = 0x05
    else if ((ret = write_i2c(config->fd, config->i2c_addr, LPS25H_RES_CONF, 0x05)) < 0)
    {
        printf("ERR : Failed to set LPS25H_RES_CONF : %x %s\r\n", errno, strerror(errno));
        return config;
    }

    // Page 43 8.18 FIFO_CTRL (2Eh)
    // 7:5 : FIFO mode b110 FIFO Mean Mode
    // 4:0 : FIFO threshold level selection 0
    // b11000000 = 0xC0
    // FIFO Mean mode: The FIFO can be used for implementing a HW moving average on the
    // pressure measurements. The number of samples of the moving average can be 2, 4, 8, 16
    // or 32 samples by selecting the FIFO Mean mode sample size as per Table 23
    else if ((ret = write_i2c(config->fd, config->i2c_addr, LPS25H_FIFO_CTRL, 0xC0)) < 0)
    {
        printf("ERR : Failed to set LPS25H_FIFO_CTRL : %x %s\r\n", errno, strerror(errno));
        return config;
    }

    // Page 36 8.7 CTRL_REG2 (21h)
    // 7   : BOOT 0
    // 5   : FIFO_EN Enable FIFO 1
    // 6   : STOP_ON_FTH 0
    // 4   : FIFO_MEAN_DEC 0
    // 3   : I2C_DIS 0
    // 2   : SWRESET 0
    // 1   : AUTOZERO 0
    // 0   : ONE_SHOT 0
    // b01000000 = 0x40
    else if ((ret = write_i2c(config->fd, config->i2c_addr, LPS25H_CTRL_REG2, 0x40)) < 0)
    {
        printf("ERR : Failed to set LPS25H_CTRL_REG2 : %x %s\r\n", errno, strerror(errno));
        return config;
    }

    if (ret >= 0)
    {
        config->isInitialized = true;
    }

    printf("LPS25H init finish : ID 0x%02x at 0x%x : Sts %d\r\n", LPS25H_I2C_ID, config->i2c_addr, ret);

    return config;
}

/* 
** Uninitialize HTS221
*/
void lps25h_uninitialize(LPS25H_CONFIG* config)
{
    if (config != NULL)
    {
        free(config);
    }
}

/* 
** Reads Temperature and Pressure data from LPS25H
** Reference : https://www.st.com/resource/en/datasheet/hts221.pdf
*/
int lps25h_read(LPS25H_CONFIG* config, LPS25H_DATA* data)
{
    unsigned char rawData[3];
    unsigned char status;
    int ret;

    data->temperature = 0.0;
    data->pressure = 0.0;
    data->isPressureValid = false;
    data->isTemperatureValid = false;

    // Page 41 8.12 STATUS_REG (27h)
    if ((ret = read_i2c(config->fd, config->i2c_addr, LPS25H_STATUS, 1, &status)) < 0)
    {
        printf("ERR : Failed to read HTS221_H1_T0_OUT : %x %s\r\n", errno, strerror(errno));
        return ret;        
    }

    // Page 41 8.13 PRESS_OUT_XL (28h), 8.14 PRESS_OUT_L (29h), 8.15 PRESS_OUT_H (2Ah)
    // Reference : https://www.st.com/resource/en/technical_note/dm00242307-how-to-interpret-pressure-and-temperature-readings-in-the-lps25hb-pressure-sensor-stmicroelectronics.pdf
    // Page 4 3.4 How to interpret pressure readings
    if (status & 2) {
        if ((ret = read_i2c(config->fd, config->i2c_addr, LPS25H_PRESS_OUT_XL | LPS25H_AUTO_ADVANCE, 3, rawData)) < 0)
        {
            printf("ERR : Failed to read LPS25H_PRESS_OUT_XL : %x %s\r\n", errno, strerror(errno));
            return ret;        
        }

        data->pressure = (double)((((unsigned int)rawData[2]) << 16) | (((unsigned int)rawData[1]) << 8) | (unsigned int)rawData[0]) / (double)4096;
        data->isPressureValid = true;
    }

    // Page 42 8.16 TEMP_OUT_L (2Bh), 8.17 TEMP_OUT_H (2Ch)
    // Reference : https://www.st.com/resource/en/technical_note/dm00242307-how-to-interpret-pressure-and-temperature-readings-in-the-lps25hb-pressure-sensor-stmicroelectronics.pdf
    // Page 5 How to obtain temperature values in °C
    if (status & 1) {
        if ((ret = read_i2c(config->fd, config->i2c_addr, LPS25H_TEMP_OUT_L | LPS25H_AUTO_ADVANCE, 2, rawData)) < 0)
        {
            printf("ERR : Failed to read LPS25H_TEMP_OUT_L : %x %s\r\n", errno, strerror(errno));
            return ret;        
        }

        data->temperature = (int16_t)((((unsigned int)rawData[1]) << 8) | (unsigned int)rawData[0]) / (double)480 + (double)42.5;
        data->isTemperatureValid = true;
    }

    return 0;
}