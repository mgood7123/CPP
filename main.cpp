#include "CPP/CPP.h"

std::string input = R"(
#define X(a) a y
#define Y(Z) Z
Y(X)(z)
)";

int main() {
    std::string a = "a";
    assert(CPP::Rules::Range({'a'}).match(a));
    a = "b";
    assert(CPP::Rules::Range({'A', 'C', 'a', 'c'}).match(a));
    a = "A";
    assert(CPP::Rules::Range({'a', 'c', 'A', 'C'}).match(a));
    a = "X";
    assert(CPP::Rules::Range({'a', 'c', 'A', 'C', 'X'}).match(a));

    std::string b = "abcd";
    CPP::Iterator A(b);
    auto s = new CPP::Rules::Sequence(
            {
                    new CPP::Rules::Char('a'), new CPP::Rules::Char('b'),
                    new CPP::Rules::Char('c'), new CPP::Rules::Char('d')
            });
    auto s1 = new CPP::Rules::Rule(s);
    assert(!CPP::Rules::NotAt(s1).match(A));
    A.reset();
    auto m = new CPP::Rules::NotAt(new CPP::Rules::Sequence(
            {
                    new CPP::Rules::Char('a'), new CPP::Rules::Char('b'),
                    new CPP::Rules::Char('c'), new CPP::Rules::Char('e')
            }));
    auto m1 = new CPP::Rules::At(s);
    assert(CPP::Rules::At(s).match(A));
    assert(CPP::Rules::Sequence({m1, m, m1, m, s}).match(A));


    std::string c = "a";
    CPP::Iterator ci(c);
    auto anyA = new CPP::Rules::Any([](CPP::Rules::Input) { XOut << "A" << std::endl; });
    auto anyB = new CPP::Rules::Rule(new CPP::Rules::TemporaryAction(anyA), [](CPP::Rules::Input) { XOut << "B" << std::endl; });
    assert(anyA->match(ci));
    ci.reset();
    assert(anyB->match(ci));

    assert(CPP::Rules::Success().match(c));
    assert(!CPP::Rules::Fail().match(c));
    assert(CPP::Rules::AdvanceInputBy().match(c));
    assert(CPP::Rules::AdvanceInputBy(1).match(c));
    assert(CPP::Rules::Sequence({new CPP::Rules::Success(), new CPP::Rules::Char('a')}).match(c));
    assert(CPP::Rules::Or({new CPP::Rules::Char('b'), new CPP::Rules::Char('a')}).match(c));
    assert(CPP::Rules::Or({new CPP::Rules::Sequence({new CPP::Rules::Char('b')}), new CPP::Rules::Char('a')}).match(c));
    assert(CPP::Rules::Or({new CPP::Rules::Sequence({new CPP::Rules::Char('a'), new CPP::Rules::Char('b')}), new CPP::Rules::Char('a')}).match(c));
    assert(CPP::Rules::Or({new CPP::Rules::Sequence({new CPP::Rules::Char('b'), new CPP::Rules::Char('a')}), new CPP::Rules::Char('a')}).match(c));
    assert(CPP::Rules::Or({new CPP::Rules::Sequence({new CPP::Rules::Success(), new CPP::Rules::Char('b')}), new CPP::Rules::Char('a')}).match(c));
    assert(CPP::Rules::Or({new CPP::Rules::Sequence({new CPP::Rules::Fail(), new CPP::Rules::Char('b')}), new CPP::Rules::Char('a')}).match(c));
    assert(CPP::Rules::Or({new CPP::Rules::Sequence({new CPP::Rules::Any(), new CPP::Rules::Char('b')}), new CPP::Rules::Char('a')}).match(c));
    assert(CPP::Rules::Or({new CPP::Rules::Sequence({new CPP::Rules::AdvanceInputBy(5), new CPP::Rules::Fail()}), new CPP::Rules::Char('a')}).match(c));
    assert(CPP::Rules::Or({new CPP::Rules::Sequence({new CPP::Rules::Rule(new CPP::Rules::AdvanceInputBy(5)), new CPP::Rules::Fail()}), new CPP::Rules::Char('a')}).match(c));
    assert(CPP::Rules::Or({new CPP::Rules::Sequence({new CPP::Rules::Success(), new CPP::Rules::Fail()}), new CPP::Rules::Char('a')}).match(c));

    CPP::CPP cpp;
    cpp.parse(input);

    return 0;
}