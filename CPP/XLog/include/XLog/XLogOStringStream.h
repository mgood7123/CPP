//
// Created by Matthew Good on 28/11/21.
//

#ifndef ASTRID_XLOGOSTRINGSTREAM_H
#define ASTRID_XLOGOSTRINGSTREAM_H

#include "XLogStringBuf.h"

class XLogOStringStream : public std::ostringstream {
public:
    XLogOStringStream(std::string *buffer);

    XLogStringBuf log;
    XLogOStringStream();
};


#endif //ASTRID_XLOGOSTRINGSTREAM_H
