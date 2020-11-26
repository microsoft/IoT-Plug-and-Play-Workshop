#include "sensehat.h"


/* 
** Initializes SenseHat
** Search and initialize 
** HTS221
** LPS25H
** LSM9DS1
** LED
*/
SENSEHAT_DATA* initialize_senseHat(void)
{
    SENSEHAT_DATA* senseHat;

#ifdef HAVE_RTIMULIB_H

#else
    LSM9DS1_CONFIG lsm9ds1;

#endif
    senseHat = malloc(sizeof(SENSEHAT_DATA));

    if (senseHat == NULL)
    {
        return NULL;
    }

    // search I2C device    
    if ((senseHat->i2c_dev = scan_i2c_devices()) == NULL)
    {
        printf("ERR : Did not find i2c device\r\n");
        return NULL;
    }

    printf("i2c device found : %s\r\n", senseHat->i2c_dev->name);

    // Initialize HTS221
    // To Do : We should open separate FD for each sensor
    if ((senseHat->hts221 = hts221_initialize(senseHat->i2c_dev->fd)) == NULL)
    {
        printf("ERR : Failed to initialize HTS221\r\n");
    }
    else if (senseHat->hts221->isInitialized != true)
    {
        printf("ERR : Failed to initialize HTS221\r\n");
        hts221_uninitialize(senseHat->hts221);
        senseHat->hts221 = NULL;
    }

    // Initialize LPS25H
    // To Do : We should open separate FD for each sensor
    if ((senseHat->lps25h = lps25h_initialize(senseHat->i2c_dev->fd)) == NULL)
    {
        printf("ERR : Failed to initialize LPS25H\r\n");
    }
    else if (senseHat->lps25h->isInitialized != true)
    {
        printf("ERR : Failed to initialize LPS25H\r\n");
        lps25h_uninitialize(senseHat->lps25h);
        senseHat->lps25h = NULL;
    }
    // Initialize LSM9DS1
#ifdef HAVE_RTIMULIB_H
    // Use RTIMULib
    if ((senseHat->rtimu = rtimu_initialize()) == NULL)
    {
        printf("ERR : Failed to initialize RTIMULib\r\n");
    }
    else if (senseHat->rtimu->isInitialized != true)
    {
        printf("ERR : Failed to initialize RTIMULib\r\n");
        rtimu_uninitialize(senseHat->rtimu);
        senseHat->rtimu = NULL;
    }
#else
    // lsm9ds1.fd = i2c_dev->fd;
    // lsm9ds1_initialize(&lsm9ds1);
#endif

    // Initlialize LED
    if ((senseHat->led = led_initialize()) == NULL)
    {
        printf("ERR : Failed to initialize LED\r\n");
    }
    else if (senseHat->led->isInitialized != true)
    {
        printf("ERR : Failed to initialize LED\r\n");
        led_uninitialize(senseHat->led);
        senseHat->led = NULL;
    }

    return senseHat;
}

int uninitialize_senseHat(SENSEHAT_DATA* senseHat)
{
    if (senseHat)
    {
        if (senseHat->hts221)
        {
            hts221_uninitialize(senseHat->hts221);
        }

        if (senseHat->lps25h)
        {
            lps25h_uninitialize(senseHat->lps25h);
        }

        if (senseHat->rtimu)
        {
            rtimu_uninitialize(senseHat->rtimu);
        }

        if (senseHat->led)
        {
            led_uninitialize(senseHat->led);
        }
    }
}

