#pragma once

#ifndef TAG
#define TAG "FOGLTLOGLES"
#endif

#define PRINTF_LOG

#ifdef __cplusplus
extern "C" {
#endif

#ifdef PRINTF_LOG
    #define LOGE(...) printf(TAG " E: " __VA_ARGS__), printf("\n")
    #define LOGW(...) printf(TAG " W: " __VA_ARGS__), printf("\n")
    #define LOGI(...) printf(TAG " I: " __VA_ARGS__), printf("\n")
    #define LOGD(...) printf(TAG " D: " __VA_ARGS__), printf("\n")
#else
    #include <android/log.h>
    #define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)
    #define LOGW(...) __android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__)
    #define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
    #define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif
