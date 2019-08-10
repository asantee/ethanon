#ifndef ANDROID_LOG_H_
#define ANDROID_LOG_H_

#include <android/log.h>

#define STRINGIFY(x) #x
// #define LOG_TAG    __FILE__ ":" STRINGIFY(__LINE__)
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO,  "GS2DInfo",  __VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, "GS2DError", __VA_ARGS__)

#endif
