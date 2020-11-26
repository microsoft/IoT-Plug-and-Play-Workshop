#ifndef _IOTHUB_C2D
#define _IOTHUB_C2D

#include "iothub_op.h"

typedef IOTHUBMESSAGE_DISPOSITION_RESULT (*c2d_callback_fn)(IOTHUB_MESSAGE_HANDLE message, void* userContextCallback);

c2d_callback_fn _c2dCallback_fn;

IOTHUBMESSAGE_DISPOSITION_RESULT cloudMessageCallback(IOTHUB_MESSAGE_HANDLE message, void* userContextCallback);

#endif // _IOTHUB_C2D


