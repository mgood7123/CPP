//
// Created by Matthew Good on 28/11/21.
//

#include <XLog/XLogStringBuf.h>

#include <cstdio>

#if (__ANDROID__)
#include <android/log.h>
#endif

XLogStringBuf::XLogStringBuf(std::string *buffer) {
    this->buffer = buffer;
}

int XLogStringBuf::sync() {
    std::string s = str();
    if (s.length() == 0) return 0;
    const char * message = s.c_str();

    if (buffer == nullptr) {
#if (__ANDROID__)
        __android_log_write(ANDROID_LOG_INFO, "XLog", message);
#else
        std::printf("%s", message);
#endif
    } else {
        buffer->append(message);
    }

    str("");
    return 0;
}
