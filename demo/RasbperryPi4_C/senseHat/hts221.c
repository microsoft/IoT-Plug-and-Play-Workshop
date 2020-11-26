#include "hts221.h"
#include <math.h>
/* 
** Initializes HTS221 Temperature and Humidity Sensor
** Reference : https://www.st.com/resource/en/datasheet/hts221.pdf
** Reference : https://www.st.com/resource/en/technical_note/dm00208001-interpreting-humidity-and-temperature-readings-in-the-hts221-digital-humidity-sensor-stmicroelectronics.pdf
*/
HTS221_CONFIG* hts221_initialize(int fd)
{
    int ret = 0;
    HTS221_CONFIG* config;

    unsigned char rawData[2];
    uint8_t H0_H_2 = 0;
    uint8_t H1_H_2 = 0;
    uint16_t T0_C_8 = 0;
    uint16_t T1_C_8 = 0;
    int16_t H0_T0_OUT = 0;
    int16_t H1_T0_OUT = 0;
    int16_t T0_OUT = 0;
    int16_t T1_OUT = 0;
    double H0, H1, T0, T1;

    config = (HTS221_CONFIG*)malloc(sizeof(HTS221_CONFIG));

    printf("HTS221 init start\r\n");

    if (config == NULL)
    {
        printf("ERR : Failed to allocate memory for HTS221_CONFIG");
        return config;
    }

    (void)memset(config, 0, sizeof(HTS221_CONFIG));

    config->isInitialized = false;
    config->fd = fd;

    // Search I2C device
    if ((config->i2c_addr = search_i2c_device(config->fd, HTS221_WHO_AM_I, HTS221_I2C_ID)) < 0)
    {
        printf("HTS221 Not Found\r\n");
        return config;
    }

    //
    // 7   : PD (Power Down Control) = 1 = Active
    // 6:3 : Reserved
    // 2   : BDU (Block Data Update) = 1
    // 1:0 : ODR1, ODR0 Output Data Rate Selection = 11 = 12.5hz for Himudity and Temperature
    // b10000111 = 0x87
    if ((ret = write_i2c(config->fd, config->i2c_addr, HTS221_CTRL_REG1, 0x87)) < 0)
    {
        printf("ERR : Failed to set HTS221_CTRL_REG1 : %x %s\r\n", errno, strerror(errno));
        return config;
    }
    //
    // 7:6 : Reserved
    // 5:3 : AVTG2-0 : To select the numbers of averaged temperature samples : b011 = 16
    // 2:0 : AVTH2-0 : To select the numbers of averaged humidy samples : b011 = 32 (Default)
    // b00011011 = 0x1B
    else if ((ret = write_i2c(config->fd, config->i2c_addr, HTS221_AV_CONF, 0x1B)) < 0)
    {
        printf("ERR : Failed to set HTS221_AV_CONF : %x %s\r\n", errno, strerror(errno));
        return config;
    }

    // Get calibration data
    if ((ret = read_i2c(config->fd, config->i2c_addr, HTS221_T1_T0_MSB | HTS221_AUTO_ADVANCE, 1, &rawData[1])) < 0)
    {
        printf("ERR : Failed to read HTS221_T1_T0_MSB : %x %s\r\n", errno, strerror(errno));
        return config;        
    }
    else if ((ret = read_i2c(config->fd, config->i2c_addr, HTS221_T0_DEGC_X8 | HTS221_AUTO_ADVANCE, 1, rawData)) < 0)
    {
        printf("ERR : Failed to read HTS221_T0_DEGC_X8 : %x %s\r\n", errno, strerror(errno));
        return config;        
    }

    T0_C_8 = (((unsigned int)rawData[1] & 0x3 ) << 8) | (unsigned int)rawData[0];
    T0 = (double)T0_C_8 / 8;

    if ((ret = read_i2c(config->fd, config->i2c_addr, HTS221_T1_DEGC_X8 | HTS221_AUTO_ADVANCE, 1, rawData)) < 0)
    {
        printf("ERR : Failed to read HTS221_T1_DEGC_X8 : %x %s\r\n", errno, strerror(errno));
        return config;        
    }
    T1_C_8 = (unsigned int)(((uint16_t)(rawData[1] & 0xC) << 6) | (uint16_t)rawData[0]);
    T1 = (double)T1_C_8 / 8;

    if ((ret = read_i2c(config->fd, config->i2c_addr, HTS221_T0_OUT | HTS221_AUTO_ADVANCE, 2, rawData)) < 0)
    {
        printf("ERR : Failed to read HTS221_T0_OUT : %x %s\r\n", errno, strerror(errno));
        return config;        
    }
    T0_OUT = (int16_t)(((unsigned int)rawData[1]) << 8) | (unsigned int)rawData[0];

    if ((ret = read_i2c(config->fd, config->i2c_addr, HTS221_T1_OUT | HTS221_AUTO_ADVANCE, 2, rawData)) < 0)
    {
        printf("ERR : Failed to read HTS221_T1_OUT : %x %s\r\n", errno, strerror(errno));
        return config;        
    }
    T1_OUT = (int16_t)(((unsigned int)rawData[1]) << 8) | (unsigned int)rawData[0];

    if ((ret = read_i2c(config->fd, config->i2c_addr, HTS221_H0_H_2 | HTS221_AUTO_ADVANCE, 1, &H0_H_2)) < 0)
    {
        printf("ERR : Failed to read HTS221_H0_H_2 : %x %s\r\n", errno, strerror(errno));
        return config;        
    }
    H0 = (double)H0_H_2 / 2;

    if ((ret = read_i2c(config->fd, config->i2c_addr, HTS221_H1_H_2 | HTS221_AUTO_ADVANCE, 1, &H1_H_2)) < 0)
    {
        printf("ERR : Failed to read HTS221_H1_H_2 : %x %s\r\n", errno, strerror(errno));
        return config;        
    }
    H1 = (double)H1_H_2 / 2;

    if ((ret = read_i2c(config->fd, config->i2c_addr, HTS221_H0_T0_OUT | HTS221_AUTO_ADVANCE, 2, rawData)) < 0)
    {
        printf("ERR : Failed to read HTS221_H0_T0_OUT : %x %s\r\n", errno, strerror(errno));
        return config;        
    }
    H0_T0_OUT = (int16_t)(((unsigned int)rawData[1]) << 8) | (unsigned int)rawData[0];


    if ((ret = read_i2c(config->fd, config->i2c_addr, HTS221_H1_T0_OUT | HTS221_AUTO_ADVANCE, 2, rawData)) < 0)
    {
        printf("ERR : Failed to read HTS221_H1_T0_OUT : %x %s\r\n", errno, strerror(errno));
        return config;        
    }
    H1_T0_OUT = (int16_t)(((unsigned int)rawData[1]) << 8) | (unsigned int)rawData[0];

    config->temperature_m = (T1-T0)/(T1_OUT-T0_OUT);
    config->temperature_c = T0-(config->temperature_m*T0_OUT);
    config->humidity_m = (H1-H0)/(H1_T0_OUT-H0_T0_OUT);
    config->humidity_c = (H0)-(config->humidity_m*H0_T0_OUT);    

    if (ret >= 0)
    {
        config->isInitialized = true;
    }

    printf("HTS221 init finish : ID 0x%02x at 0x%x : Sts %d\r\n", HTS221_I2C_ID, config->i2c_addr, ret);

    return config;
}

/* 
** Uninitialize HTS221
*/
void hts221_uninitialize(HTS221_CONFIG* config)
{
    if (config != NULL)
    {
        free(config);
    }
}

/* 
** Reads Temperature and Humidity data from HTS221
** Reference : https://www.st.com/resource/en/datasheet/hts221.pdf
*/
int hts221_read(HTS221_CONFIG* config, HTS221_DATA* data)
{
    unsigned char rawData[2];
    unsigned char status;
    double tempData;
    int ret;

    data->temperature = 0.0;
    data->humidity = 0.0;
    data->isHumidityValid = false;
    data->isTemperatureValid = false;

    // Reference : https://www.st.com/resource/en/datasheet/hts221.pdf
    // Page 24 : 7.6 STATUS_REG
    if ((ret = read_i2c(config->fd, config->i2c_addr, HTS221_STATUS, 1, &status)) < 0)
    {
        printf("ERR : Failed to read HTS221_H1_T0_OUT : %x %s\r\n", errno, strerror(errno));
        return ret;        
    }


    if ((status & 0x3) == 0)
    {
        // Data not ready/available
        return -ENODATA;
    }
    // Reference : https://www.st.com/resource/en/datasheet/hts221.pdf
    // Page 25 : 7.7, 7.8 HUMIDITY_OUT_L, HUMIDITY_OUT_H
    // Page 28 : Step 2: Humidity conversion from ADC_OUT (LSB) to rH %
    if (status & 2) {
        if ((ret = read_i2c(config->fd, config->i2c_addr, HTS221_HUMIDITY_OUT_L | HTS221_AUTO_ADVANCE, 2, rawData)) < 0)
        {
            printf("ERR : Failed to read HTS221_HUMIDITY_OUT_L : %x %s\r\n", errno, strerror(errno));
            return ret;        
        }

        // Read from register and apply calibration data
        tempData = (int16_t)((((unsigned int)rawData[1]) << 8) | (unsigned int)rawData[0]);
        //data->humidity = tempData * config->humidity_m + config->humidity_c;
        data->humidity = tempData * config->humidity_m + config->humidity_c;
        data->isHumidityValid = true;
    }

    // Reference : https://www.st.com/resource/en/datasheet/hts221.pdf
    // Page 25 : 7.9, 7.10 TEMP_OUT_L, TEMP_OUT_H
    // Page 28 : Step 2: Humidity conversion from ADC_OUT (LSB) to rH %
    if (status & 1) {
        if ((ret = read_i2c(config->fd, config->i2c_addr, HTS221_TEMP_OUT_L | HTS221_AUTO_ADVANCE, 2, rawData)) < 0)
        {
            printf("ERR : Failed to read HTS221_HUMIDITY_OUT_L : %x %s\r\n", errno, strerror(errno));
            return ret;        
        }

        tempData = (int16_t)((((unsigned int)rawData[1]) << 8) | (unsigned int)rawData[0]);
        data->temperature = tempData * config->temperature_m + config->temperature_c;
        data->isTemperatureValid = true;
    }

    return ret;
}


