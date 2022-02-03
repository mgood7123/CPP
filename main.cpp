#include "CPP/CPP.h"

std::string input = R"(
//#define X(a) a y
//#define Y(Z) Z
//Y(X)(z)
//#define a b
//#define b c
//a
#define foo(a) a
#define b Y
foo(b)
)";

int main() {
    CPP::CPP cpp;
    cpp.parse(input);
    return 0;
}