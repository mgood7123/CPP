//
// Created by Matthew Good on 28/11/21.
//

#include <XLog/XLogOStringStream.h>

XLogOStringStream::XLogOStringStream() {
    init(&log);
}

XLogOStringStream::XLogOStringStream(std::string *buffer) {
    log = XLogStringBuf(buffer);
    init(&log);
}
