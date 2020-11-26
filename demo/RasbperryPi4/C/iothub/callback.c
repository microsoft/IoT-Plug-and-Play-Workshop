#include "callback.h"

extern deviceTwin_callback_fn _deviceTwinCallback_fn;
extern deviceMethod_callback_fn _deviceMethodCallback_fn;
extern c2d_callback_fn _c2dCallback_fn;

int set_callback(CALLBACK_ENUM cb, void* fn, void* appContext)
{
    switch(cb)
    {
        case CB_DEVICE_TWIN:
            _deviceTwinCallback_fn = fn;
            break;
        case CB_DEVICE_METHOD:
            _deviceMethodCallback_fn = fn;
            break;
        case CB_C2D_MESSAGE:
            _c2dCallback_fn = fn;
            break;
    }
    return 0;
}
