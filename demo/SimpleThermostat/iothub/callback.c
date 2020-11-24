// Licensed under the MIT license. See LICENSE file in the project root for full license information.
// Sample IoT Plug and Play device app for X.509 certificate attestation
//
// Daisuke Nakahara 
//

#include "callback.h"

extern deviceTwin_callback_fn _deviceTwinCallback_fn;
extern deviceMethod_callback_fn _deviceMethodCallback_fn;

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
        default:
            break;
    }
    return 0;
}
