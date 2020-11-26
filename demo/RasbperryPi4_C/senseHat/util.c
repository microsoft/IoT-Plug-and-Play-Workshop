#include "util.h"

/*
** Callback to filter I2C devices
*/
static int check_i2c(const struct dirent *dir)
{
    return strncmp("i2c", dir->d_name, 2) ==0 ;
}

/*
** Scan /dev/i2c-x.  Assumption is there is only 1 i2c device (SenseHat)
** To do : Add Multiple I2C device support
*/
I2C_DEVICE* scan_i2c_devices()
{
    struct dirent **scanList;
    I2C_DEVICE* device = NULL;
    int i, scanCount, res;

    scanCount = scandir("/dev", &scanList, check_i2c, versionsort);

    if (scanCount == 0)
    {
        printf("ERR : Did not find I2C devices\r\n");
    }
    else if (scanCount == 1)
    {
        for (i=0; i < scanCount; i++)
        {
            int fd;
            char i2cPath[64];

            snprintf(i2cPath, sizeof(i2cPath),"/dev/%s", scanList[i]->d_name);

            if ((fd = open(i2cPath, O_RDWR)) >=0 )
            {
                device = malloc(sizeof(I2C_DEVICE));
                strcpy(device->name, i2cPath);
                device->fd = fd;
                break;
            }
        }
    }
    else
    {
        printf("ERR : Found multiple I2C devices\r\n");
    }

    if (scanCount > 0)
    {
        for (i=0; i < scanCount; i++)
            free(scanList[i]);

        free(scanList);
    }

    return device;
}


/*
** Callback to filter Frame Buffer devices
*/
static int check_fb(const struct dirent *dir)
{
    return strncmp("fb", dir->d_name, 2) ==0 ;
}

/*
** Scan /dev/fb*.  Look for FB with Name "RPi-Sense FB"
** To do : Add Multiple I2C device support
*/
FB_DEVICE* scan_fb_devices()
{
    struct dirent **scanList;
    FB_DEVICE* device = NULL;
    int i, scanCount, res;
    struct fb_fix_screeninfo fix_info;

    scanCount = scandir("/dev", &scanList, check_fb, versionsort);

    if (scanCount > 0)
    {
        for (i=0; i < scanCount; i++)
        {
            int fd;
            char fbPath[64];

            snprintf(fbPath, sizeof(fbPath),"/dev/%s", scanList[i]->d_name);
            if ((fd = open(fbPath, O_RDWR)) < 0 )
            {
                continue;
            }

            ioctl(fd, FBIOGET_FSCREENINFO, &fix_info);
            if (strcmp("RPi-Sense FB", fix_info.id) == 0)
                device = malloc(sizeof(FB_DEVICE));

                if (device)
                {
                    device->fd = fd;
                    strcpy(device->name, fbPath);
                    break;
                }
        }

        for (i=0; i < scanCount; i++)
            free(scanList[i]);

        free(scanList);
    }

    return device;
}

/*
 * Send WHO_AM_I and look for specified ID
*/
int search_i2c_device(int fd, unsigned char who_am_i, unsigned char i2c_id)
{
    int i;
    int i2c_addr = -1;
    unsigned char data;

    // printf("%s ID 0x%x\r\n", __func__, i2c_id);

    for (i=0; i < 0x100; i++)
    {
        int ret;

        // Set Slave Address
        ret = ioctl(fd, I2C_SLAVE, i);

        if (ret >= 0)
        {
            if ((write(fd, &who_am_i, 1)) != 1)
            {
                if (errno == 0x79)
                {
                    continue;
                }
                printf("write() failed %x %s\r\n", errno, strerror(errno));
            }
            else if ((ret = read(fd, &data, 1)) != 1)
            {
                printf("read() failed %x %s\r\n", errno, strerror(errno));
            }
            else if (ret == 1)
            {
                // printf("Who Am I response : %x\r\n", data);
                if (data == i2c_id) 
                {
                    // printf("Found i2c device ID : 0x%x at 0x%x\r\n", data, i);
                    i2c_addr = i;
                    break;
                }
            }
        }
    }

    return i2c_addr;
}

/*
** Read from I2C.  Sets slave address
** To Do : Remember the current slave address so we don't have to set on every call
*/
int read_i2c(int fd, 
             unsigned char i2c_addr,
             unsigned char regAddr,
             unsigned char length,
             unsigned char *buffer)
{
    if (ioctl(fd, I2C_SLAVE, i2c_addr) < 0)
    {
        printf("ERR : Failed to set slave at 0x%x : %x %s\r\n", i2c_addr, errno, strerror(errno));
        return -1;
    }

    return i2c_smbus_read_i2c_block_data(fd, regAddr, length, buffer);
}

/*
** Write to I2C.  Sets slave address
** To Do : Remember the current slave address so we don't have to set on every call
*/
int write_i2c(int fd, 
              unsigned char i2c_addr,
              __u8 regAddr,
              __u8 value)
{
    if (ioctl(fd, I2C_SLAVE, i2c_addr) < 0)
    {
        printf("ERR : Failed to set slave at 0x%x : %x %s\r\n", i2c_addr, errno, strerror(errno));
        return -1;
    }

    return i2c_smbus_write_byte_data(fd, regAddr, value);
}

/*
** Sleep function
*/
void delay_ms(long ms)
{
	struct timespec tp;

	tp.tv_sec = ms / 1000;
	tp.tv_nsec = (ms % 1000) * 1000000;
	nanosleep(&tp, NULL);
}

int64_t
seload(uint64_t measured, size_t size)
{
	uint64_t mask;

	mask = 1ULL << (size - 1);
	return (measured & (mask - 1)) - (measured & mask);
}