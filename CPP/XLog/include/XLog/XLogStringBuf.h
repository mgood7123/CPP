//
// Created by Matthew Good on 28/11/21.
//

#ifndef ASTRID_XLOGSTRINGBUF_H
#define ASTRID_XLOGSTRINGBUF_H

#include <sstream>

class XLogStringBuf : public std::stringbuf
{
    std::string * buffer = nullptr;
public:
    XLogStringBuf() = default;
    XLogStringBuf(std::string *buffer);

    int sync() override;
};

#endif //ASTRID_XLOGSTRINGBUF_H
