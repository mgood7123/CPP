//
// Created by Matthew Good on 28/11/21.
//

#ifndef ASTRID_XLOG_H
#define ASTRID_XLOG_H

#include "XLogOStringStream.h"

# include <inttypes.h>

// include iop manip operators for fun :)
#include <iomanip>

#include <climits>
#include <vector>
#include <algorithm>

#define XLOG_PRINTABLE_CLASS_GLOBAL_DECL(Class) XLog& operator<<(XLog& XLog, const Class & value)
#define XLOG_PRINTABLE_CLASS_DECL(Class) friend XLOG_PRINTABLE_CLASS_GLOBAL_DECL(Class)
#define XLOG_PRINTABLE_CLASS_IMPL(Class) XLOG_PRINTABLE_CLASS_GLOBAL_DECL(Class)

class XLog {
protected:
    XLogOStringStream stream;
    class Abort {};
public:

    template <typename T>
    class Array {
    public:
        T * array;
        size_t count;
        Array(T *array, size_t count) : array(array), count(count) {}
    };

    static Abort Abort;

    static const char * NewLine;

    XLog() = default;

    XLog(const XLog & old);

    XLog & operator=(const XLog & old);

    XLog(XLog && old);

    XLog & operator=(XLog && old);

    XLog(const char *string);
    XLog(const std::string & string);
    XLog(const std::string_view & string);

    template <typename... Args>
    XLog(const char *fmt, const Args&... args) {
        Printf(fmt, args...).parse(*this);
    }

    XLog(std::string * buffer);

    XLog(std::string * buffer, const char *string);

    template <typename... Args>
    XLog(std::string * buffer, const char *fmt, const Args&... args) : XLog(buffer) {
        Printf(fmt, args...).parse(*this);
    }

    std::ios_base::fmtflags flags();

    void flags(std::ios_base::fmtflags flags);

    void flush();

    void operator<<(class Abort);

    XLog& operator<<(bool value);
    XLog& operator<<(short value);
    XLog& operator<<(unsigned short value);
    XLog& operator<<(int value);
    XLog& operator<<(unsigned int value);
    XLog& operator<<(long value);
    XLog& operator<<(unsigned long value);
    XLog& operator<<(long long value);
    XLog& operator<<(unsigned long long value);
    XLog& operator<<(float value);
    XLog& operator<<(double value);
    XLog& operator<<(long double value);
    XLog& operator<<(const char value);
    XLog& operator<<(const char * value);
    XLog& operator<<(const void* value);
    XLog& operator<<(std::streambuf * value);

    explicit operator bool() const;

    XLog& put(char c);
    XLog& write(const char* string, size_t len);

    XLog& operator<<(nullptr_t);

    XLog& operator<<(std::stringstream&);

    // for std::endl and others
    XLog& operator<<(std::ostream& (*value)(std::ostream&));

    // for std::hex and others
    XLog& operator<<(std::ios_base& (*value)(std::ios_base&));

    // for something else i do not yet know
    XLog& operator<<(std::ios& (*value)(std::ios&));

    // extra io functions

    // resetiosflags
    XLog& operator<<(const std::__iom_t1& value);

    // setiosflags
    XLog& operator<<(const std::__iom_t2& value);

    // setbase
    XLog& operator<<(const std::__iom_t3& value);

    // setfill
    template<class _CharT>
    XLog& operator<<(const std::__iom_t4<_CharT>& value) {
        stream << value;
        return *this;
    }

    // setprecision
    XLog& operator<<(const std::__iom_t5& value);

    // setw
    XLog& operator<<(const std::__iom_t6& value);

    // get_money
    template <class _MoneyT>
    XLog& operator<<(const std::__iom_t7<_MoneyT>& value) {
        stream << value;
        return *this;
    }

    // put_money
    template <class _MoneyT>
    XLog& operator<<(const std::__iom_t8<_MoneyT>& value) {
        stream << value;
        return *this;
    }

    // get_time
    template <class _CharT>
    XLog& operator<<(const std::__iom_t9<_CharT>& value) {
        stream << value;
        return *this;
    }

    // put_time
    template <class _CharT>
    XLog& operator<<(const std::__iom_t10<_CharT>& value) {
        stream << value;
        return *this;
    }

    XLog& operator<<(const std::pair<const char*, size_t> & value);

    XLog& operator<<(const std::string & value);
    XLog& operator<<(const std::string_view & value);

    XLog& operator<<(const XLog& value);

    class Printf {
    public:
        const char * fmt;
        size_t len = 0;

        std::function<void(XLog & xlog)> callBack;
        bool haveCallback = false;

        template <typename Func, typename... Args>
        void save(Func f, const Args & ... args) {
            haveCallback = true;
            callBack = [f, args...](XLog & xlog)
            {
                (f)(xlog, args...);
            };
        }

        Printf(const char *fmt);

        template <typename... Args>
        Printf(const char *fmt, const Args&... args) {
            this->fmt = fmt;
            this->len = strlen(fmt);
            save([=](XLog & xlog, const Args&... args) {
                parse(xlog, args...);
            }, args...);
        }

        void expand(XLog *x, ...);

        template <typename... Args>
        void parse(XLog & xlog, const Args&... args) {
            char ch[2] = {0, 0};
            ([&] (auto & input)
            {
                while (*fmt != '\0') {
                    if (*fmt == '%') {
                        fmt++;
                        if (*fmt == '\0') {
                            xlog << "illegal format string" << Abort;
                        }
                        switch (*fmt) {
                            case '?':
                                xlog << input;
                                break;
                            default:
                                if (std::is_trivial_v<decltype(input)>) {
                                    // pass by address to avoid the following error:
                                    //   In template: cannot pass object of non-trivial type
                                    //   'const std::string' through variadic method; call will
                                    //   abort at runtime
                                    expand(&xlog, &input);
                                } else {
                                    xlog << input;
                                }
                        }
                        fmt++;
                        break;
                    } else {
                        ch[0] = *fmt;
                        xlog << ch;
                    }
                    fmt++;
                }
            } (args), ...);
            xlog << fmt;
        }

        void parse(XLog & log);
    };

    XLog& operator<<(const Printf & value);

    template<class T>
    XLog& operator<<(const Array<T> & value) {
        for (size_t i = 0; i < value.count; i++) {
            *this << value.array[i];
            if (i+1 != value.count) stream << ", ";
        }
        return *this;
    }

    class Endian {
    public:
        enum Info {
#if XLOG_COMPILE_ENDIAN_IS_BIG_ENDIAN == 1
            little = 0,
            big    = 1,
            native = big
#else
            little = 1,
            big    = 0,
            native = little
#endif
        };
        static const char * string;
    };

    static Endian endian;

    XLog& operator<<(const Endian & value);
    XLog& operator<<(const Endian::Info & value);

    class Binary {
        bool default_reverse_bits = true;
        bool default_reverse_section_bits = true;
        int default_spacing = 8;
    public:
        std::string bin;
        size_t bits;
        size_t bytes;
        int spacing = default_spacing;

        Endian::Info endian;

        Binary(const std::string & x) {
            parseBinary(Endian::native, x);
        }

        Binary(const Endian::Info & endian, const std::string & x) {
            parseBinary(endian, x);
        }

        Binary(const std::string_view & x) {
            parseBinary(Endian::native, x);
        }

        Binary(const Endian::Info & endian, const std::string_view & x) {
            parseBinary(endian, x);
        }

        Binary(const char * x) {
            parseBinary(Endian::native, std::string(x));
        }

        Binary(const Endian::Info & endian, const char * x) {
            parseBinary(endian, std::string(x));
        }

        Binary(const char * x, size_t length) {
            parseBinary(Endian::native, x, length);
        }

        Binary(const Endian::Info & endian, const char * x, size_t length) {
            parseBinary(endian, x, length);
        }

        template<typename T>
        Binary(const T& x) {
            parseBinary(Endian::native, x);
        }

        template<typename T>
        Binary(const Endian::Info & endian, const T& x) {
            parseBinary(endian, x);
        }

    private:

        void parseBinary(const Endian::Info & endian, const std::string & x) {
            parseBinary(endian, x.data(), x.size());
        }

        void parseBinary(const Endian::Info & endian, const std::string_view & x) {
            parseBinary(endian, x.data(), x.size());
        }

        void parseBinary(const Endian::Info & endian, const char * x) {
            parseBinary(endian, x, strlen(x));
        }

        void parseBinary(const Endian::Info & endian, const char * x, size_t length) {
            bytes = length * sizeof(char);
            bits = bytes * CHAR_BIT;
            this->endian = endian;
            parseBinary(endian, reinterpret_cast<const int8_t *>(x));
        }
        
        template<typename T>
        void parseBinary(const Endian::Info & endian, const T& x) {
            bytes = sizeof(T);
            bits = bytes * CHAR_BIT;
            this->endian = endian;
            parseBinary(endian, reinterpret_cast<const int8_t *>(&x));
        }

        void parseBinary(const Endian::Info & endian, const int8_t * data);
    };

    XLog& operator<<(Binary & value);

    class Address {
    public:
        const void * address;

        template <typename T>
        Address(const T* pointer) {
            address = pointer;
        }

        template <typename T>
        Address(const T & reference_to_get_address_of) {
            address = &reference_to_get_address_of;
        }
    };

    XLog& operator<<(Address & value);

    class Number {
    public:

        bool print_as_bool;
        int8_t size;
        bool isInteger;
        bool isUnsigned;

        union number_t {
            int8_t int8;
            uint8_t uint8;
            int16_t int16;
            uint16_t uint16;
            int32_t int32;
            uint32_t uint32;
            int64_t int64;
            uint64_t uint64;
            float float32;
            double float64;
        } number;

        Number(const int8_t & val, bool print_as_bool = false);
        Number(const uint8_t & val, bool print_as_bool = false);
        Number(const int16_t & val, bool print_as_bool = false);
        Number(const uint16_t & val, bool print_as_bool = false);
        Number(const int32_t & val, bool print_as_bool = false);
        Number(const uint32_t & val, bool print_as_bool = false);
        Number(const int64_t & val, bool print_as_bool = false);
        Number(const uint64_t & val, bool print_as_bool = false);
        Number(const ssize_t & val, bool print_as_bool = false);
        Number(const size_t & val, bool print_as_bool = false);
        Number(const float & val, bool print_as_bool = false);
        Number(const double & val, bool print_as_bool = false);
    };

    XLog& operator<<(const Number & value);

    class Bool {
    public:
        const Number & value;
        Bool(const Number & value);
    };

    XLog& operator<<(const Bool & value);

    class Hex {
    public:
        Number value;
        bool isNumber;
        Hex(const Number & value);
    };

    XLog& operator<<(const Hex & value);

    class HexArray {
        public:
        std::vector<Hex> hex;

        void add(const char * data, size_t length);

        HexArray(const std::string & value);

        HexArray(const std::string_view & value);
    };
    
    XLog& operator<<(const HexArray & value);

    class Oct {
    public:
        const Number & value;
        Oct(const Number & value);
    };

    XLog& operator<<(const Oct & value);





    #ifndef STRACE_PRINT_UTILS_H
    # define STRACE_PRINT_UTILS_H

    /* Hexadecimal output utils */

    static constexpr char hex_chars[17] = "0123456789abcdef";

    /**
     * Character array representing hexadecimal encoding of a character value.
     *
     * @param b_ Byte to provide representation for.
     */
    # define BYTE_HEX_CHARS(b_) \
        hex_chars[((uint8_t) (b_)) >> 4], hex_chars[((uint8_t) (b_)) & 0xf]

    /* Character classification utils */

    static inline bool
    is_print(uint8_t c)
    {
        return (c >= ' ') && (c < 0x7f);
    }

    /* Character printing functions */

    /** @param unabbrev Whether to always print \ooo instead of \[[o]o]o. */
    static inline char *
    sprint_byte_oct(char *s, uint8_t c, bool unabbrev)
    {
        if (unabbrev) {
            /* Print \ooo */
            *s++ = '0' + (c >> 6);
            *s++ = '0' + ((c >> 3) & 0x7);
        } else {
            /* Print \[[o]o]o */
            if ((c >> 3) != 0) {
                if ((c >> 6) != 0)
                    *s++ = '0' + (c >> 6);
                *s++ = '0' + ((c >> 3) & 0x7);
            }
        }
        *s++ = '0' + (c & 0x7);

        return s;
    }

    static inline char *
    sprint_byte_hex(char *buf, uint8_t val)
    {
        *buf++ = hex_chars[val >> 4];
        *buf++ = hex_chars[val & 0xf];

        return buf;
    }

    /** Maximum number of characters emitted by sprint_char */
    # define SPRINT_CHAR_BUFSZ 7

    enum sprint_char_flag_bits {
        SCF_QUOTES_BIT,
        SCF_NUL_BIT,
        SCF_ESC_WS_BIT,
    };

    enum sprint_char_flags {
        SCF_QUOTES = SCF_QUOTES_BIT, /**< Whether to emit quotes */
        SCF_NUL = SCF_NUL_BIT,       /**< Whether to terminate output with \0 */
        SCF_ESC_WS = SCF_ESC_WS_BIT, /**< Whether to print \t\n\v\f\r in symbolic form */
    };

    /** Emits a character into buf (SPRINT_CHAR_BUFSZ max), returns new position. */
    static inline char *
    sprint_char(char *buf, const unsigned char c, const enum sprint_char_flags f)
    {
        if (f & SCF_QUOTES)
            *buf++ = '\'';

        if (is_print(c)) {
            if (c == '\'' || c == '\\')
                *buf++ = '\\';
            *buf++ = c;
        } else if ((f & SCF_ESC_WS) && (c >= '\t') && (c <= '\r')) {
            static const char ws_chars[] = "tnvfr";

            *buf++ = '\\';
            *buf++ = ws_chars[c - '\t'];
        } else {
            *buf++ = '\\';
            *buf++ = 'x';
            buf = sprint_byte_hex(buf, c);
        }

        if (f & SCF_QUOTES)
            *buf++ = '\'';
        if (f & SCF_NUL)
            *buf++ = '\0';

        return buf;
    }

    # define print_char(c_, flags_)					\
        do {							\
            char buf[SPRINT_CHAR_BUFSZ];			\
                                    \
            sprint_char(buf, (c_), (flags_) | SCF_NUL);	\
            tprints(buf);					\
        } while (0)

    #endif /* STRACE_PRINT_UTILS_H */
};

extern XLog XOut;

class XLogMemory : public XLog {
    std::string memoryString;
public:
    XLogMemory();

    XLogMemory(const char *string);

    template <typename... Args>
    XLogMemory(const char *fmt, const Args&... args) : XLog(&memoryString, fmt, args...) {}

    const std::string * getMemory() const;

    const char * getMemoryCRef() const;
};

XLOG_PRINTABLE_CLASS_GLOBAL_DECL(XLogMemory);

class XLogFatal : public XLog {
public:
    using XLog::XLog; // inherit constructors
    ~XLogFatal();
};

#endif //ASTRID_XLOG_H
