#ifndef _IOTHUB_DIRECT_METHOD
#define _IOTHUB_DIRECT_METHOD

#include "iothub_op.h"

typedef int (*deviceMethod_callback_fn)(const char* methodName, const unsigned char* payload, size_t size, unsigned char** response, size_t* responseSize, void* userContextCallback);

deviceMethod_callback_fn _deviceMethodCallback_fn;

int deviceMethodCallback(const char* methodName, const unsigned char* payload, size_t size, unsigned char** response, size_t* responseSize, void* userContextCallback);

#endif // _IOTHUB_DIRECT_METHOD