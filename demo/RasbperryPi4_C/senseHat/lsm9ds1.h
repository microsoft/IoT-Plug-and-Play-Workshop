#ifndef _SENSE_HAT_LSM9DS1
#define _SENSE_HAT_LSM9DS1

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

#include "util.h"

typedef void* rtimu;
typedef void* rtimuSettings;

typedef struct {
    bool isInitialized;
    rtimu imu;
    rtimuSettings settings;
} RTIMU_CONFIG;

EXTERNC RTIMU_CONFIG* rtimu_initialize(void);
EXTERNC void rtimu_uninitialize(RTIMU_CONFIG* config);
EXTERNC int lsm9ds1_read(RTIMU_CONFIG* config, ORIANTATION_DATA* accel, ORIANTATION_DATA* gyro, ORIANTATION_DATA* compass, FUSION_DATA* fusion );
EXTERNC int lsm9ds1_get_fusion(RTIMU_CONFIG* config, int *roll, int *pitch, int *yaw);
EXTERNC int lsm9ds1_get_accel(RTIMU_CONFIG* config, ORIANTATION_DATA* data);
EXTERNC int lsm9ds1_get_compass(RTIMU_CONFIG* config, ORIANTATION_DATA* data);
EXTERNC int lsm9ds1_get_gyro(RTIMU_CONFIG* config, ORIANTATION_DATA* data);

#endif //_SENSE_HAT_LSM9DS1