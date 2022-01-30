//
// Created by Matthew Good on 28/11/21.
//

#include <XLog/XLog.h>

XLog XOut;

XLog::Endian XLog::endian;

class XLog::Abort XLog::Abort;

const char * XLog::NewLine = "\n";

XLog::XLog(const char *string) {
    *this << string << NewLine;
}

XLog::XLog(const std::string & string) {
    *this << string << NewLine;
}

XLog::XLog(const std::string_view & string) {
    *this << string << NewLine;
}

XLog::XLog(std::string * buffer) {
    stream = XLogOStringStream(buffer);
}

XLog::XLog(std::string * buffer, const char *string) {
    stream = XLogOStringStream(buffer);
    *this << string << NewLine;
}

std::ios_base::fmtflags XLog::flags() {
    return stream.flags();
}

void XLog::flags(std::ios_base::fmtflags flags) {
    stream.flags(flags);
}

void XLog::flush() {
    stream.flush();
}

void XLog::operator<<(class XLog::Abort) {
    *this << NewLine;
    flush();
    abort();
}

XLog& XLog::operator<<(bool value) {
    stream << (value ? "true" : "false");
    return *this;
}

XLog& XLog::operator<<(short value) {
    stream << value;
    return *this;
}

XLog& XLog::operator<<(unsigned short value) {
    stream << value;
    return *this;
}

XLog& XLog::operator<<(int value) {
    stream << value;
    return *this;
}

XLog& XLog::operator<<(unsigned int value) {
    stream << value;
    return *this;
}

XLog& XLog::operator<<(long value) {
    stream << value;
    return *this;
}

XLog& XLog::operator<<(unsigned long value) {
    stream << value;
    return *this;
}

XLog& XLog::operator<<(long long value) {
    stream << value;
    return *this;
}

XLog& XLog::operator<<(unsigned long long value) {
    stream << value;
    return *this;
}

XLog& XLog::operator<<(float value){
    stream << value;
    return *this;
}

XLog& XLog::operator<<(double value){
    stream << value;
    return *this;
}

XLog& XLog::operator<<(long double value){
    stream << value;
    return *this;
}

XLog& XLog::operator<<(const char *value) {
    while(*value != '\0') {
        *this << *(value++);
    }
    return *this;
}

XLog &XLog::operator<<(const char value) {
    stream << value;
    if (value == '\n') stream.flush();
    return *this;
}

XLog& XLog::operator<<(const void* value){
    stream << value;
    return *this;
}

XLog& XLog::operator<<(std::streambuf * value){
    stream << value;
    return *this;
}

XLog& XLog::operator<<(nullptr_t) {
    return *this << "nullptr";
}

XLog& XLog::operator<<(std::stringstream& value) {
    return *this << value.str();
}

XLog& XLog::operator<<(std::ios_base &(*value)(std::ios_base &)) {
    stream << value;
    return *this;
}

XLog& XLog::operator<<(std::ios &(*value)(std::ios &)) {
    stream << value;
    return *this;
}

XLog& XLog::operator<<(std::ostream &(*value)(std::ostream &)) {
    stream << value;
    return *this;
}

XLog &XLog::operator<<(const std::pair<const char*, size_t> &value) {
    for (size_t i = 0; i < value.second; i++) {
        *this << value.first[i];
    }
    return *this;
}

XLog& XLog::operator<<(const std::string &value) {
    return *this << value.c_str();
}

XLog &XLog::operator<<(const std::string_view &value) {
    return *this << std::make_pair(value.data(), value.size());
}

XLog& XLog::operator<<(const XLog& value) {
    std::string s = value.stream.log.str();
    return *this << s;
}

XLog::operator bool() const {
    return stream.operator bool();
}

XLog &XLog::put(char c) {
    stream.put(c);
    return *this;
}

XLog &XLog::write(const char *string, size_t len) {
    stream.write(string, len);
    return *this;
}

XLog &XLog::operator<<(const XLog::Printf &value) {
    const_cast<XLog::Printf &>(value).parse(*this);
    return *this;
}

XLog &XLog::operator<<(const XLog::Endian &value) {
    return *this << XLog::Endian::string;
}

XLog &XLog::operator<<(const XLog::Endian::Info &value) {
    if (value == Endian::big) {
        return *this << "Big";
    } else if (value == Endian::little) {
        return *this << "Little";
    } else {
        return *this << Endian::string;
    }
}

XLog &XLog::operator<<(XLog::Binary &value) {
   return *this << "Endian: " << value.endian << ", Bytes: " << value.bytes << ", Bits: " << value.bits << ", Binary data: " << value.bin;
}

XLog &XLog::operator<<(XLog::Address &value) {
   return *this << value.address;
}

XLog &XLog::operator<<(const XLog::Number &value) {
    if (value.print_as_bool) {
        switch (value.size) {
            case 8:
                if (!value.isUnsigned)
                    return *this << (value.number.int8 == 1 ? "true" : "false");
                else
                    return *this << (value.number.uint8 == 1 ? "true" : "false");
            case 16:
                if (!value.isUnsigned)
                    return *this << (value.number.int16 == 1 ? "true" : "false");
                else
                    return *this << (value.number.uint16 == 1 ? "true" : "false");
            case 32:
                if (value.isInteger) {
                    if (!value.isUnsigned)
                        return *this << (value.number.int32 == 1 ? "true" : "false");
                    else
                        return *this << (value.number.uint32 == 1 ? "true" : "false");
                } else {
                    return *this << (value.number.float32 == 1.0f ? "true" : "false");
                }
            case 64:
                if (value.isInteger) {
                    if (!value.isUnsigned)
                        return *this << (value.number.int64 == 1 ? "true" : "false");
                    else
                        return *this << (value.number.uint64 == 1 ? "true" : "false");
                } else {
                    return *this << (value.number.float64 == 1.0f ? "true" : "false");
                }
        }
    } else {
        switch (value.size) {
            case 8:
                if (!value.isUnsigned)
                    return *this << value.number.int8;
                else
                    return *this << value.number.uint8;
            case 16:
                if (!value.isUnsigned)
                    return *this << value.number.int16;
                else
                    return *this << value.number.uint16;
            case 32:
                if (value.isInteger) {
                    if (!value.isUnsigned)
                        return *this << value.number.int32;
                    else
                        return *this << value.number.uint32;
                } else {
                    return *this << value.number.float32;
                }
            case 64:
                if (value.isInteger) {
                    if (!value.isUnsigned)
                        return *this << value.number.int64;
                    else
                        return *this << value.number.uint64;
                } else {
                    return *this << value.number.float64;
                }
        }
    }
    return *this << 0;
}

XLog &XLog::operator<<(const XLog::Bool &value) {
    return *this << value.value;
}

XLog &XLog::operator<<(const XLog::Hex &value) {
    XLogMemory mem;
    mem << std::hex << value.value << std::flush;
    return *this << mem.getMemoryCRef();
}

XLog &XLog::operator<<(const XLog::HexArray &value) {
    XLogMemory mem;
    mem << Array(value.hex.data(), value.hex.size()) << std::flush;
    return *this << mem.getMemoryCRef();
}

XLog &XLog::operator<<(const XLog::Oct &value) {
    XLogMemory mem;
    mem << std::oct << value.value << std::flush;
    return *this << mem.getMemoryCRef();
}

XLog &XLog::operator<<(const std::__iom_t1 &value) {
    stream << value;
    return *this;
}

XLog &XLog::operator<<(const std::__iom_t2 &value) {
    stream << value;
    return *this;
}

XLog &XLog::operator<<(const std::__iom_t3 &value) {
    stream << value;
    return *this;
}

XLog &XLog::operator<<(const std::__iom_t5 &value) {
    stream << value;
    return *this;
}

XLog &XLog::operator<<(const std::__iom_t6 &value) {
    stream << value;
    return *this;
}

XLog::XLog(const XLog &old) {
    XLog & o = const_cast<XLog &>(old);
    stream = std::move(o.stream);
}

XLog &XLog::operator=(const XLog &old) {
    XLog & o = const_cast<XLog &>(old);
    stream = std::move(o.stream);
    return *this;
}

XLog::XLog(XLog &&old) {
    stream = std::move(old.stream);
}

XLog &XLog::operator=(XLog &&old) {
    stream = std::move(old.stream);
    return *this;
}

XLogFatal::~XLogFatal() {
    *this << Abort;
}

XLog::Printf::Printf(const char *fmt) {
    this->fmt = fmt;
    this->len = strlen(fmt);
}

void XLog::Printf::expand(XLog *x, ...) {
    va_list args;
    va_start(args, x);

    XLog & xlog = *x;

    switch (*fmt) {
        case '%': {
            xlog << "%";
            break;
        }
        case 'd': {
            int i = va_arg(args, int*)[0];
            xlog << i;
            break;
        }
        case 'c': {
            unsigned char ch[2] = {0, 0};
            ch[0] = static_cast<char>(va_arg(args, int*)[0]);
            xlog << ch;
            break;
        }
        case 's': {
            const char *string = va_arg(args, const char **)[0];
            xlog << string;
            break;
        }
        case 'f': {
            double d = va_arg(args, double*)[0];
            xlog << d;
            break;
        }
    }
    va_end(args);
}

void XLog::Printf::parse(XLog &log) {
    if (haveCallback) {
        callBack(log);
    } else {
        log << fmt;
    }
}

#if XLOG_COMPILE_ENDIAN_IS_BIG_ENDIAN == 1
const char * XLog::Endian::string = "Big";
#else
const char * XLog::Endian::string = "Little";
#endif

void XLog::Binary::parseBinary(const XLog::Endian::Info & endian, const int8_t *data) {
    bin = "";
    size_t count = 0;
    size_t start;
    size_t end;
    size_t plus_one;
    if (Endian::native == endian) {
        start = 0;
        end = bytes;
        plus_one = 1;
    } else {
        start = bytes-1;
        end = -1;
        plus_one = -1;
    }

    for (size_t byte = start; byte != end; byte += plus_one) {
        const int8_t & b = data[byte];
        // true = print bits left-to-right
        bool reverse_bits = default_reverse_bits;
        // true = print individual sections left-to-right
        bool reverse_section_bits = default_reverse_section_bits;
        size_t startb;
        size_t endb;
        size_t plus_oneb;
        if (!reverse_bits) {
            startb = 0;
            endb = bits / bytes;
            plus_oneb = 1;
        } else {
            startb = (bits/bytes)-1;
            endb = -1;
            plus_oneb = -1;
        }

        std::vector<int8_t> binary_data;
        binary_data.reserve(spacing);

        for (size_t bit = startb; bit != endb; bit += plus_oneb) {
            binary_data.push_back((b >> bit) & 0x1 ? 1 : 0);
            count++;
            if (count == spacing) {
                if (!reverse_section_bits) {
                    std::reverse(binary_data.begin(), binary_data.end());
                }

                for (int8_t & b : binary_data) {
                    bin += std::to_string(b);
                }
                bin += " ";
                binary_data.clear();
                binary_data.reserve(spacing);
                count = 0;
            }
        }
        if (!reverse_section_bits) {
            std::reverse(binary_data.begin(), binary_data.end());
        }
        for (int8_t & b : binary_data) {
            bin += std::to_string(b);
        }
    }
}

XLog::Number::Number(const int8_t &val, bool print_as_bool) {
    number.int8 = val;
    isInteger = true;
    isUnsigned = false;
    size = 8;
    this->print_as_bool = print_as_bool;
}

XLog::Number::Number(const uint8_t &val, bool print_as_bool) {
    number.uint8 = val;
    isInteger = true;
    isUnsigned = true;
    size = 8;
    this->print_as_bool = print_as_bool;
}

XLog::Number::Number(const int16_t &val, bool print_as_bool) {
    number.int16 = val;
    isInteger = true;
    isUnsigned = false;
    size = 16;
    this->print_as_bool = print_as_bool;
}

XLog::Number::Number(const uint16_t &val, bool print_as_bool) {
    number.uint16 = val;
    isInteger = true;
    isUnsigned = true;
    size = 16;
    this->print_as_bool = print_as_bool;
}

XLog::Number::Number(const int32_t &val, bool print_as_bool) {
    number.int32 = val;
    isInteger = true;
    isUnsigned = false;
    size = 32;
    this->print_as_bool = print_as_bool;
}

XLog::Number::Number(const uint32_t &val, bool print_as_bool) {
    number.uint32 = val;
    isInteger = true;
    isUnsigned = true;
    size = 32;
    this->print_as_bool = print_as_bool;
}

XLog::Number::Number(const int64_t &val, bool print_as_bool) {
    number.int64 = val;
    isInteger = true;
    isUnsigned = false;
    size = 64;
    this->print_as_bool = print_as_bool;
}

XLog::Number::Number(const uint64_t &val, bool print_as_bool) {
    number.uint64 = val;
    isInteger = true;
    isUnsigned = true;
    size = 64;
    this->print_as_bool = print_as_bool;
}

XLog::Number::Number(const ssize_t &val, bool print_as_bool) {
    if (sizeof(ssize_t) == sizeof(int32_t)) {
        number.int32 = val;
        isInteger = true;
        isUnsigned = false;
        size = 32;
        this->print_as_bool = print_as_bool;
    } else {
        number.int64 = val;
        isInteger = true;
        isUnsigned = false;
        size = 64;
        this->print_as_bool = print_as_bool;
    }
}

XLog::Number::Number(const size_t &val, bool print_as_bool) {
    if (sizeof(size_t) == sizeof(uint32_t)) {
        number.uint32 = val;
        isInteger = true;
        isUnsigned = true;
        size = 32;
        this->print_as_bool = print_as_bool;
    } else {
        number.uint64 = val;
        isInteger = true;
        isUnsigned = true;
        size = 64;
        this->print_as_bool = print_as_bool;
    }
}

XLog::Number::Number(const float &val, bool print_as_bool) {
    number.float32 = val;
    isInteger = false;
    size = 32;
    this->print_as_bool = print_as_bool;
}

XLog::Number::Number(const double &val, bool print_as_bool) {
    number.float64 = val;
    isInteger = false;
    size = 64;
    this->print_as_bool = print_as_bool;
}

XLog::Bool::Bool(const XLog::Number &value) : value(value) {
    const_cast<bool&>(this->value.print_as_bool) = true;
}

XLog::Hex::Hex(const XLog::Number &value) : value(value) {}

void XLog::HexArray::add(const char * data, size_t length) {
    for (size_t i = 0; i < length; ++i) {
        hex.push_back(Hex(static_cast<uint8_t>(data[i])));
    }
}

XLog::HexArray::HexArray(const std::string & value) {
    add(value.data(), value.size());
}

XLog::HexArray::HexArray(const std::string_view & value) {
    add(value.data(), value.size());
}

XLog::Oct::Oct(const XLog::Number &value) : value(value) {}

XLogMemory::XLogMemory() : XLog(&memoryString) {}

XLogMemory::XLogMemory(const char *string) : XLog(&memoryString, string) {}

const std::string *XLogMemory::getMemory() const {
    return &memoryString;
}

const char *XLogMemory::getMemoryCRef() const {
    return memoryString.c_str();
}

XLOG_PRINTABLE_CLASS_IMPL(XLogMemory) {
    XLog << value.getMemoryCRef();
    return XLog;
}