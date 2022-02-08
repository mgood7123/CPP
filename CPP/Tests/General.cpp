#include <gtest/gtest.h>

#include <CPP/Rules.h>

#ifdef GTEST_API_
TEST(Rules, success_test_01) {
    std::string a = "Hello World!";
    CPP::Iterator<std::string> b(a);
    EXPECT_TRUE(CPP::Rules::Success().match(b));
    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), 'H');
}

TEST(Rules, success_test_02) {
    std::string a = "Hello World!";
    CPP::Iterator<std::string> b(a);
    bool x = false;
    EXPECT_TRUE(CPP::Rules::Success([&] (CPP::Rules::Input input) { x = true; }).match(b));
    EXPECT_TRUE(x);
    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), 'H');
}

TEST(Rules, fail_test_01) {
    std::string a = "Hello World!";
    CPP::Iterator<std::string> b(a);
    EXPECT_FALSE(CPP::Rules::Fail().match(b));
    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), 'H');
}

TEST(Rules, fail_test_02) {
    std::string a = "Hello World!";
    CPP::Iterator<std::string> b(a);
    bool x = false;
    EXPECT_FALSE(CPP::Rules::Fail([&] (CPP::Rules::Input input) { x = true; }).match(b));
    EXPECT_TRUE(x);
    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), 'H');
}

TEST(Rules, any_test_pass) {
    std::string a = "Hello World!";
    CPP::Iterator<std::string> b(a);
    EXPECT_TRUE(CPP::Rules::Any().match(b));
    EXPECT_EQ(b.currentPosition(), 1);
    EXPECT_EQ(b.peekNext(), 'e');
}

TEST(Rules, any_test_backtrack) {
    std::string a = "Hello World!";
    CPP::Iterator<std::string> b(a);
    auto match = CPP::Rules::Any().match(b);
    EXPECT_TRUE(match);
    b.popIterator(match.matches);
    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), 'H');
}

TEST(Rules, any_test_rescan) {
    std::string a = "Hello World!";
    CPP::Iterator<std::string> b(a);
    auto match = CPP::Rules::Any([] (CPP::Rules::Input input) { input.rescan(); }).match(b);
    EXPECT_TRUE(match);
    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), 'H');
}

TEST(Rules, char_test_pass) {
    std::string a = "Hello World!";
    CPP::Iterator<std::string> b(a);
    EXPECT_TRUE(CPP::Rules::Char('H').match(b));
    EXPECT_EQ(b.currentPosition(), 1);
    EXPECT_EQ(b.peekNext(), 'e');
}

TEST(Rules, char_test_fail) {
    std::string a = "Hello World!";
    CPP::Iterator<std::string> b(a);
    EXPECT_FALSE(CPP::Rules::Char('e').match(b));
    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), 'H');
}

TEST(Rules, char_test_backtrack) {
    std::string a = "Hello World!";
    CPP::Iterator<std::string> b(a);
    auto match = CPP::Rules::Char('H').match(b);
    EXPECT_TRUE(match);
    b.popIterator(match.matches);
    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), 'H');
}

TEST(Rules, char_test_rescan) {
    std::string a = "Hello World!";
    CPP::Iterator<std::string> b(a);
    auto match = CPP::Rules::Char('H', [] (CPP::Rules::Input input) { input.rescan(); }).match(b);
    EXPECT_TRUE(match);
    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), 'H');
}

TEST(Rules, string_test_pass) {
    std::string a = "Hello World!";
    CPP::Iterator<std::string> b(a);
    EXPECT_TRUE(CPP::Rules::String("He").match(b));
    EXPECT_EQ(b.currentPosition(), 2);
    EXPECT_EQ(b.peekNext(), 'l');
}

TEST(Rules, string_test_fail) {
    std::string a = "Hello World!";
    CPP::Iterator<std::string> b(a);
    EXPECT_FALSE(CPP::Rules::String("HH").match(b));
    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), 'H');
}

TEST(Rules, string_test_backtrack) {
    std::string a = "Hello World!";
    CPP::Iterator<std::string> b(a);
    auto match = CPP::Rules::String("He").match(b);
    EXPECT_TRUE(match);
    b.popIterator(match.matches);
    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), 'H');
}

TEST(Rules, string_test_rescan) {
    std::string a = "Hello World!";
    CPP::Iterator<std::string> b(a);
    auto match = CPP::Rules::String("He", [] (CPP::Rules::Input input) { input.rescan(); }).match(b);
    EXPECT_TRUE(match);
    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), 'H');
}

TEST(Rules, range_test_pass) {
    std::string a = "Hello World!";
    CPP::Iterator<std::string> b(a);
    EXPECT_TRUE(CPP::Rules::Range({'H'}).match(b));
    EXPECT_EQ(b.currentPosition(), 1);
    EXPECT_EQ(b.peekNext(), 'e');
}

TEST(Rules, range_test_fail) {
    std::string a = "Hello World!";
    CPP::Iterator<std::string> b(a);
    EXPECT_FALSE(CPP::Rules::Range({'e'}).match(b));
    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), 'H');
}

TEST(Rules, range_test_backtrack) {
    std::string a = "Hello World!";
    CPP::Iterator<std::string> b(a);
    auto match = CPP::Rules::Range({'H'}).match(b);
    EXPECT_TRUE(match);
    b.popIterator(match.matches);
    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), 'H');
}

TEST(Rules, range_test_rescan) {
    std::string a = "Hello World!";
    CPP::Iterator<std::string> b(a);
    auto match = CPP::Rules::Range({'H'}, [] (CPP::Rules::Input input) { input.rescan(); }).match(b);
    EXPECT_TRUE(match);
    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), 'H');
}

TEST(Rules, at_test_pass) {
    std::string a = "Hello World!";
    CPP::Iterator<std::string> b(a);
    EXPECT_TRUE(CPP::Rules::At(new CPP::Rules::Char('H')).match(b));
    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), 'H');
}

TEST(Rules, at_test_fail) {
    std::string a = "Hello World!";
    CPP::Iterator<std::string> b(a);
    EXPECT_FALSE(CPP::Rules::At(new CPP::Rules::Char('e')).match(b));
    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), 'H');
}

TEST(Rules, notat_test_pass) {
    std::string a = "Hello World!";
    CPP::Iterator<std::string> b(a);
    EXPECT_FALSE(CPP::Rules::NotAt(new CPP::Rules::Char('H')).match(b));
    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), 'H');
}

TEST(Rules, notat_test_fail) {
    std::string a = "Hello World!";
    CPP::Iterator<std::string> b(a);
    EXPECT_TRUE(CPP::Rules::NotAt(new CPP::Rules::Char('e')).match(b));
    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), 'H');
}

TEST(Rules, sequence_test_pass) {
    std::string a = "Hello World!";
    CPP::Iterator<std::string> b(a);
    using namespace CPP::Rules;
    EXPECT_TRUE(Sequence({new Char('H'), new Char('e'), new Char('l')}).match(b));
    EXPECT_EQ(b.currentPosition(), 3);
    EXPECT_EQ(b.peekNext(), 'l');
}

TEST(Rules, sequence_test_fail) {
    std::string a = "Hello World!";
    CPP::Iterator<std::string> b(a);
    using namespace CPP::Rules;
    EXPECT_FALSE(Sequence({new Char('H'), new Char('e'), new Char('x')}).match(b));
    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), 'H');
}

TEST(Rules, sequence_test_backtrack) {
    std::string a = "Hello World!";
    CPP::Iterator<std::string> b(a);
    using namespace CPP::Rules;
    auto match = Sequence({new Char('H'), new Char('e'), new Char('l')}).match(b);
    EXPECT_TRUE(match);
    b.popIterator(match.matches);
    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), 'H');
}

TEST(Rules, sequence_test_rescan) {
    std::string a = "Hello World!";
    CPP::Iterator<std::string> b(a);
    using namespace CPP::Rules;
    auto match = Sequence({new Char('H'), new Char('e'), new Char('l')}, [] (CPP::Rules::Input input) { input.rescan(); }).match(b);
    EXPECT_TRUE(match);
    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), 'H');
}

TEST(Rules, or_test_pass) {
    std::string a = "Hello World!";
    CPP::Iterator<std::string> b(a);
    using namespace CPP::Rules;
    EXPECT_TRUE(Or({new Char('H'), new Char('e'), new Char('l')}).match(b));
    EXPECT_EQ(b.currentPosition(), 1);
    EXPECT_EQ(b.peekNext(), 'e');
}

TEST(Rules, or_test_fail) {
    std::string a = "Hello World!";
    CPP::Iterator<std::string> b(a);
    using namespace CPP::Rules;
    EXPECT_FALSE(Or({new Char('z'), new Char('z'), new Char('z')}).match(b));
    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), 'H');
}

TEST(Rules, or_test_backtrack) {
    std::string a = "Hello World!";
    CPP::Iterator<std::string> b(a);
    using namespace CPP::Rules;
    auto match = Or({new Char('H'), new Char('e'), new Char('l')}).match(b);
    EXPECT_TRUE(match);
    b.popIterator(match.matches);
    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), 'H');
}

TEST(Rules, or_test_rescan) {
    std::string a = "Hello World!";
    CPP::Iterator<std::string> b(a);
    using namespace CPP::Rules;
    auto match = Or({new Char('H'), new Char('e'), new Char('l')}, [] (CPP::Rules::Input input) { input.rescan(); }).match(b);
    EXPECT_TRUE(match);
    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), 'H');
}

TEST(Rules, at_sequence_test_pass) {
    std::string a = "Hello World!";
    CPP::Iterator<std::string> b(a);
    using namespace CPP::Rules;
    EXPECT_TRUE(At(new Sequence({new Char('H'), new Char('e'), new Char('l')})).match(b));
    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), 'H');
}

TEST(Rules, at_sequence_test_fail) {
    std::string a = "Hello World!";
    CPP::Iterator<std::string> b(a);
    using namespace CPP::Rules;
    EXPECT_FALSE(At(new Sequence({new Char('H'), new Char('e'), new Char('x')})).match(b));
    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), 'H');
}

TEST(Rules, optional_test_pass) {
    std::string a = "Hello World!";
    CPP::Iterator<std::string> b(a);
    using namespace CPP::Rules;
    EXPECT_TRUE(Optional(new Sequence({new Char('H'), new Char('e'), new Char('l')})).match(b));
    EXPECT_EQ(b.currentPosition(), 3);
    EXPECT_EQ(b.peekNext(), 'l');
}

TEST(Rules, optional_test_fail) {
    std::string a = "Hello World!";
    CPP::Iterator<std::string> b(a);
    using namespace CPP::Rules;
    EXPECT_TRUE(Optional(new Sequence({new Char('H'), new Char('e'), new Char('x')})).match(b));
    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), 'H');
}

TEST(Rules_Extra, test_01) {
    std::string a = "A(B)";
    CPP::Iterator<std::string> b(a);
    using namespace CPP::Rules;

    auto whitespaces = new OneOrMore(new Char(' '));
    auto optional_whitespaces = new Optional(whitespaces);

    auto identifier = new Sequence({
                new Range({'a', 'z', 'A', 'Z', '_'}),
                new Optional(
                    new OneOrMore(
                        new Range({'a', 'z', 'A', 'Z', '0', '9', '_'})
                    )
                )
            });

    auto parens_open = new Char('(');

    TemporaryAction * function_name = new TemporaryAction(identifier);

    EXPECT_TRUE(At(new Sequence({
                    function_name, optional_whitespaces, parens_open
                })).match(b));

    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), 'A');
}

TEST(Rules_Extra, test_02) {
    std::string a = "A(B)";
    CPP::Iterator<std::string> b(a);
    using namespace CPP;

    auto newline = new Rules::Char('\n');
    auto whitespaces = new Rules::OneOrMore(new Rules::Char(' '));
    auto optional_whitespaces = new Rules::Optional(whitespaces);

    auto identifier = Rules_NS_LogCapture1(new Rules::Sequence({
        new Rules::Range({'a', 'z', 'A', 'Z', '_'}),
        new Rules::Optional(
            new Rules::OneOrMore(
                new Rules::Range({'a', 'z', 'A', 'Z', '0', '9', '_'})
            )
        )
    }), "identifier");

    auto parens_open = new Rules::Char('(');

    Rules::TemporaryAction * function_name = new Rules::TemporaryAction(identifier);

    auto g = Rules_NS_LogInput(new Rules::At(new Rules::Sequence({
                                                                         Rules_NS_LogInput(function_name),
                                                                         Rules_NS_LogInput(optional_whitespaces),
                                                                         Rules_NS_LogInput(parens_open)
                                                                 })));

    EXPECT_TRUE(g->match(b));
    delete g;

    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), 'A');
}

TEST(Rules_Extra, test_03) {
    std::string a = "A(B)";
    CPP::Iterator<std::string> b(a);
    using namespace CPP;

    auto newline = new Rules::Char('\n');
    auto whitespaces = new Rules::OneOrMore(new Rules::Char(' '));
    auto optional_whitespaces = new Rules::Optional(whitespaces);

    auto identifier = Rules_NS_LogCapture1(new Rules::Sequence({
        new Rules::Range({'a', 'z', 'A', 'Z', '_'}),
        new Rules::Optional(
            new Rules::OneOrMore(
                new Rules::Range({'a', 'z', 'A', 'Z', '0', '9', '_'})
            )
        )
    }), "identifier");

    auto parens_open = new Rules::Char('(');

    Rules::TemporaryAction * function_name = new Rules::TemporaryAction(identifier);

    EXPECT_TRUE(Rules::Sequence({
                Rules_NS_LogInput1(new Rules::Success(), "function call sequence start"),
                Rules_NS_LogInput(new Rules::At(new Rules::Sequence({
                    Rules_NS_LogInput(function_name),
                    Rules_NS_LogInput(optional_whitespaces),
                    Rules_NS_LogInput(parens_open)
                })))
    }).match(b));

    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), 'A');
}

TEST(Rules_Extra, test_04_a) {
    std::string a = "A(B)";
    CPP::Iterator<std::string> b(a);
    using namespace CPP;

    auto newline = new Rules::Char('\n');
    auto whitespaces = new Rules::OneOrMore(new Rules::Char(' '));
    auto optional_whitespaces = new Rules::Optional(whitespaces);

    auto identifier = Rules_NS_LogCapture1(new Rules::Sequence({
        new Rules::Range({'a', 'z', 'A', 'Z', '_'}),
        new Rules::Optional(
            new Rules::OneOrMore(
                new Rules::Range({'a', 'z', 'A', 'Z', '0', '9', '_'})
            )
        )
    }), "identifier");

    auto parens_open = new Rules::Char('(');

    Rules::TemporaryAction * function_name = new Rules::TemporaryAction(identifier);

    EXPECT_TRUE(Rules::Or({
                            new Rules::Sequence({
                                Rules_NS_LogInput1(new Rules::Success(), "function call sequence start"),
                                Rules_NS_LogInput(new Rules::At(new Rules::Sequence({
                                    function_name
                                }))),
                                new Rules::Fail()
                            }),
                            Rules_NS_LogInput(new Rules::Any()),
                        })
                        .match(b));

    EXPECT_EQ(b.currentPosition(), 1);
    EXPECT_EQ(b.peekNext(), '(');
}

TEST(Rules_Extra, test_04_b) {
    std::string a = "A(B)";
    CPP::Iterator<std::string> b(a);
    using namespace CPP;

    auto newline = new Rules::Char('\n');
    auto whitespaces = new Rules::OneOrMore(new Rules::Char(' '));
    auto optional_whitespaces = new Rules::Optional(whitespaces);

    auto identifier = Rules_NS_LogCapture1(new Rules::Sequence({
        new Rules::Range({'a', 'z', 'A', 'Z', '_'}),
        new Rules::Optional(
            new Rules::OneOrMore(
                new Rules::Range({'a', 'z', 'A', 'Z', '0', '9', '_'})
            )
        )
    }), "identifier");

    auto parens_open = new Rules::Char('(');

    Rules::TemporaryAction * function_name = new Rules::TemporaryAction(identifier);

    EXPECT_TRUE(Rules::Or({
                            new Rules::Sequence({
                                Rules_NS_LogInput1(new Rules::Success(), "function call sequence start"),
                                Rules_NS_LogInput(new Rules::At(new Rules::Success())),
                                new Rules::Fail()
                            }),
                            Rules_NS_LogInput(new Rules::Any()),
                        })
                        .match(b));

    EXPECT_EQ(b.currentPosition(), 1);
    EXPECT_EQ(b.peekNext(), '(');
}

TEST(Rules_Extra, test_04_c) {
    std::string a = "A(B)";
    CPP::Iterator<std::string> b(a);
    using namespace CPP;

    auto newline = new Rules::Char('\n');
    auto whitespaces = new Rules::OneOrMore(new Rules::Char(' '));
    auto optional_whitespaces = new Rules::Optional(whitespaces);

    auto identifier = Rules_NS_LogCapture1(new Rules::Sequence({
        new Rules::Range({'a', 'z', 'A', 'Z', '_'}),
        new Rules::Optional(
            new Rules::OneOrMore(
                new Rules::Range({'a', 'z', 'A', 'Z', '0', '9', '_'})
            )
        )
    }), "identifier");

    auto parens_open = new Rules::Char('(');

    Rules::TemporaryAction * function_name = new Rules::TemporaryAction(identifier);

    EXPECT_TRUE(Rules::Or({
                            new Rules::Sequence({
                                Rules_NS_LogInput1(new Rules::Success(), "function call sequence start"),
                                Rules_NS_LogInput(new Rules::At(new Rules::AdvanceInputBy(1))),
                                new Rules::Fail()
                            }),
                            Rules_NS_LogInput(new Rules::Any()),
                        })
                        .match(b));

    EXPECT_EQ(b.currentPosition(), 1);
    EXPECT_EQ(b.peekNext(), '(');
}

TEST(Rules_Extra, test_04_d) {
    std::string a = "A(B)";
    CPP::Iterator<std::string> b(a);
    using namespace CPP;

    auto newline = new Rules::Char('\n');
    auto whitespaces = new Rules::OneOrMore(new Rules::Char(' '));
    auto optional_whitespaces = new Rules::Optional(whitespaces);

    auto identifier = Rules_NS_LogCapture1(new Rules::Sequence({
        new Rules::Range({'a', 'z', 'A', 'Z', '_'}),
        new Rules::Optional(
            new Rules::OneOrMore(
                new Rules::Range({'a', 'z', 'A', 'Z', '0', '9', '_'})
            )
        )
    }), "identifier");

    auto parens_open = new Rules::Char('(');

    Rules::TemporaryAction * function_name = new Rules::TemporaryAction(identifier);

    EXPECT_TRUE(Rules::Or({
                            new Rules::Sequence({
                                new Rules::At(new Rules::AdvanceInputBy(1)),
                                new Rules::Fail()
                            }),
                            new Rules::Any(),
                        })
                        .match(b));

    EXPECT_EQ(b.currentPosition(), 1);
    EXPECT_EQ(b.peekNext(), '(');
}

TEST(Rules_Extra, test_04_e) {
    std::string a = "A(B)";
    CPP::Iterator<std::string> b(a);
    using namespace CPP;

    EXPECT_TRUE(Rules::Or({
                            new Rules::Sequence({
                                new Rules::AdvanceInputBy(1),
                                new Rules::Fail()
                            }),
                            new Rules::Any(),
                        })
                        .match(b));

    EXPECT_EQ(b.currentPosition(), 1);
    EXPECT_EQ(b.peekNext(), '(');
}

TEST(Rules_Extra, test_04_e_a) {
    std::string a = "A(B)";
    CPP::Iterator<std::string> b(a);
    using namespace CPP;

    EXPECT_TRUE(Rules::Or({
                            new Rules::Char('K'),
                            new Rules::Char('A'),
                        })
                        .match(b));

    EXPECT_EQ(b.currentPosition(), 1);
    EXPECT_EQ(b.peekNext(), '(');
}

TEST(Rules_Extra, test_04_f) {
    std::string a = "A(B)";
    CPP::Iterator<std::string> b(a);
    using namespace CPP;

    auto newline = new Rules::Char('\n');
    auto whitespaces = new Rules::OneOrMore(new Rules::Char(' '));
    auto optional_whitespaces = new Rules::Optional(whitespaces);

    auto identifier = Rules_NS_LogCapture1(new Rules::Sequence({
        new Rules::Range({'a', 'z', 'A', 'Z', '_'}),
        new Rules::Optional(
            new Rules::OneOrMore(
                new Rules::Range({'a', 'z', 'A', 'Z', '0', '9', '_'})
            )
        )
    }), "identifier");

    auto parens_open = new Rules::Char('(');

    Rules::TemporaryAction * function_name = new Rules::TemporaryAction(identifier);

    EXPECT_FALSE(Rules::Sequence({
                    new Rules::AdvanceInputBy(1),
                    new Rules::Fail()
    }).match(b));

    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), 'A');
}

TEST(Rules_Extra, test_04_g) {
    std::string a = "A(B)";
    CPP::Iterator<std::string> b(a);
    using namespace CPP;

    auto newline = new Rules::Char('\n');
    auto whitespaces = new Rules::OneOrMore(new Rules::Char(' '));
    auto optional_whitespaces = new Rules::Optional(whitespaces);

    auto identifier = Rules_NS_LogCapture1(new Rules::Sequence({
        new Rules::Range({'a', 'z', 'A', 'Z', '_'}),
        new Rules::Optional(
            new Rules::OneOrMore(
                new Rules::Range({'a', 'z', 'A', 'Z', '0', '9', '_'})
            )
        )
    }), "identifier");

    auto parens_open = new Rules::Char('(');

    Rules::TemporaryAction * function_name = new Rules::TemporaryAction(identifier);

    EXPECT_FALSE(Rules::Sequence({
                    new Rules::Char('H'),
                    new Rules::Char('H')
    }).match(b));

    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), 'A');
}

TEST(Rules_Extra, test_04_h) {
    std::string a = "A(B)";
    CPP::Iterator<std::string> b(a);
    using namespace CPP;

    auto newline = new Rules::Char('\n');
    auto whitespaces = new Rules::OneOrMore(new Rules::Char(' '));
    auto optional_whitespaces = new Rules::Optional(whitespaces);

    auto identifier = Rules_NS_LogCapture1(new Rules::Sequence({
        new Rules::Range({'a', 'z', 'A', 'Z', '_'}),
        new Rules::Optional(
            new Rules::OneOrMore(
                new Rules::Range({'a', 'z', 'A', 'Z', '0', '9', '_'})
            )
        )
    }), "identifier");

    auto parens_open = new Rules::Char('(');

    Rules::TemporaryAction * function_name = new Rules::TemporaryAction(identifier);

    EXPECT_FALSE(Rules::Sequence({
                    new Rules::Char('H'),
                    new Rules::Fail()
    }).match(b));

    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), 'A');
}

TEST(Rules_Extra, test_04_i) {
    std::string a = "A(B)";
    CPP::Iterator<std::string> b(a);
    using namespace CPP;

    EXPECT_FALSE(Rules::Sequence({
                    new Rules::Char('H'),
                    new Rules::Fail()
    }).match(b));

    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), 'A');
}

TEST(Rules_Extra, test_04_j) {
    std::string a = "A(B)";
    CPP::Iterator<std::string> b(a);
    using namespace CPP;
    EXPECT_FALSE(Rules::Sequence({new Rules::Char('H'), new Rules::Char('e')}).match(b));
    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), 'A');
}

TEST(Rules_Extra, test_04_k) {
    std::string a = "A(B)";
    CPP::Iterator<std::string> b(a);
    using namespace CPP;
    EXPECT_FALSE(Rules::Sequence({new Rules::Char('H'), new Rules::Fail()}).match(b));
    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), 'A');
}

TEST(Rules_Extra, test_04_l) {
    std::string a = "AAAB";
    CPP::Iterator<std::string> b(a);
    using namespace CPP;

    EXPECT_TRUE(Rules::OneOrMore(new Rules::Char('A')).match(b));

    EXPECT_EQ(b.currentPosition(), 3);
    EXPECT_EQ(b.peekNext(), 'B');
}

TEST(Rules_Extra, test_04_m) {
    std::string a = "AAAB";
    CPP::Iterator<std::string> b(a);
    using namespace CPP;

    EXPECT_FALSE(Rules::OneOrMore(new Rules::Char('B')).match(b));

    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), 'A');
}

TEST(Rules_Extra, test_04_n) {
    std::string a = "A(B)";
    CPP::Iterator<std::string> b(a);
    using namespace CPP;

    EXPECT_TRUE(Rules::OneOrMore(new Rules::Or({
        new Rules::Sequence({
            new Rules::Char('H'),
            new Rules::Fail()
        }),
        new Rules::Any()
    })).match(b));

    EXPECT_EQ(b.currentPosition(), 4);
    EXPECT_EQ(b.peekNext(), '\0');
}

TEST(Rules_Extra, test_04) {
    std::string a = "A(B)";
    CPP::Iterator<std::string> b(a);
    using namespace CPP;

    auto newline = new Rules::Char('\n');
    auto whitespaces = new Rules::OneOrMore(new Rules::Char(' '));
    auto optional_whitespaces = new Rules::Optional(whitespaces);

    auto identifier = Rules_NS_LogCapture1(new Rules::Sequence({
        new Rules::Range({'a', 'z', 'A', 'Z', '_'}),
        new Rules::Optional(
            new Rules::OneOrMore(
                new Rules::Range({'a', 'z', 'A', 'Z', '0', '9', '_'})
            )
        )
    }), "identifier");

    auto parens_open = new Rules::Char('(');

    Rules::TemporaryAction * function_name = new Rules::TemporaryAction(identifier);

    EXPECT_TRUE(Rules::Or({
                            new Rules::Sequence({
                                Rules_NS_LogInput1(new Rules::Success(), "function call sequence start"),
                                Rules_NS_LogInput(new Rules::At(new Rules::Sequence({
                                    function_name,
                                    optional_whitespaces,
                                    parens_open
                                }))),
                                new Rules::Fail()
                            }),
                            Rules_NS_LogInput(new Rules::Any()),
                        })
                        .match(b));

    EXPECT_EQ(b.currentPosition(), 1);
    EXPECT_EQ(b.peekNext(), '(');
}

TEST(Rules_Extra, test_05) {
    std::string a = "A(B)";
    CPP::Iterator<std::string> b(a);
    using namespace CPP;

    auto newline = new Rules::Char('\n');
    auto whitespaces = new Rules::OneOrMore(new Rules::Char(' '));
    auto optional_whitespaces = new Rules::Optional(whitespaces);

    auto identifier = Rules_NS_LogCapture1(new Rules::Sequence({
        new Rules::Range({'a', 'z', 'A', 'Z', '_'}),
        new Rules::Optional(
            new Rules::OneOrMore(
                new Rules::Range({'a', 'z', 'A', 'Z', '0', '9', '_'})
            )
        )
    }), "identifier");

    auto parens_open = new Rules::Char('(');

    Rules::TemporaryAction * function_name = new Rules::TemporaryAction(identifier);

    EXPECT_TRUE(Rules::RuleHolder(new Rules::Or({
                            new Rules::Sequence({
                                Rules_NS_LogInput1(new Rules::Success(), "function call sequence start"),
                                Rules_NS_LogInput(new Rules::At(new Rules::Sequence({
                                    function_name,
                                    optional_whitespaces,
                                    parens_open
                                }))),
                                new Rules::Fail()
                            }),
                            Rules_NS_LogInput(new Rules::Any()),
                        }))
                        .match(b));

    EXPECT_EQ(b.currentPosition(), 1);
    EXPECT_EQ(b.peekNext(), '(');
}

TEST(Rules_Extra, test_06) {
    std::string a = "A(B)";
    CPP::Iterator<std::string> b(a);
    using namespace CPP;

    auto newline = new Rules::Char('\n');
    auto whitespaces = new Rules::OneOrMore(new Rules::Char(' '));
    auto optional_whitespaces = new Rules::Optional(whitespaces);

    auto identifier = Rules_NS_LogCapture1(new Rules::Sequence({
        new Rules::Range({'a', 'z', 'A', 'Z', '_'}),
        new Rules::Optional(
            new Rules::OneOrMore(
                new Rules::Range({'a', 'z', 'A', 'Z', '0', '9', '_'})
            )
        )
    }), "identifier");

    auto parens_open = new Rules::Char('(');

    Rules::TemporaryAction * function_name = new Rules::TemporaryAction(identifier);

    EXPECT_TRUE(Rules::LogInput(new Rules::Or({
                            new Rules::Sequence({
                                Rules_NS_LogInput1(new Rules::Success(), "function call sequence start"),
                                Rules_NS_LogInput(new Rules::At(new Rules::Sequence({
                                    function_name,
                                    optional_whitespaces,
                                    parens_open
                                }))),
                                new Rules::Fail()
                            }),
                            Rules_NS_LogInput(new Rules::Any()),
                        }), "CUSTOM OR")
                        .match(b));

    EXPECT_EQ(b.currentPosition(), 1);
    EXPECT_EQ(b.peekNext(), '(');
}


TEST(Rules_Extra, test_07) {
    std::string a = "A(B)";
    CPP::Iterator<std::string> b(a);
    using namespace CPP;

    auto newline = new Rules::Char('\n');
    auto whitespaces = new Rules::OneOrMore(new Rules::Char(' '));
    auto optional_whitespaces = new Rules::Optional(whitespaces);

    auto identifier = Rules_NS_LogCapture1(new Rules::Sequence({
        new Rules::Range({'a', 'z', 'A', 'Z', '_'}),
        new Rules::Optional(
            new Rules::OneOrMore(
                new Rules::Range({'a', 'z', 'A', 'Z', '0', '9', '_'})
            )
        )
    }), "identifier");

    auto parens_open = new Rules::Char('(');

    Rules::TemporaryAction * function_name = new Rules::TemporaryAction(identifier);

    EXPECT_TRUE(
            Rules::OneOrMore(
                        Rules_NS_LogInput1(new Rules::Or({
                            new Rules::Sequence({
                                Rules_NS_LogInput1(new Rules::Success(), "function call sequence start"),
                                Rules_NS_LogInput(new Rules::At(new Rules::Sequence({
                                    function_name,
                                    optional_whitespaces,
                                    parens_open
                                }))),
                                new Rules::Fail()
                            }),
                            Rules_NS_LogInput(new Rules::Any()),
                        }), "CUSTOM OR")
                        )
                        .match(b));

    EXPECT_EQ(b.currentPosition(), 4);
    EXPECT_EQ(b.peekNext(), '\0');
}

TEST(Rules, rescan_test_01) {
    std::string a = "12345";
    CPP::Iterator<std::string> b(a);
    using namespace CPP;

    EXPECT_TRUE(
            Rules::Sequence({
                new Rules::Char('1'),
                new Rules::Or({
                    new Rules::Sequence({
                        new Rules::Char('2'),
                        new Rules::Char('3'),
                        new Rules::Sequence({
                                new Rules::Char('4'),
                                new Rules::Char('5')
                            }, [] (auto x) { x.rescan(); }
                        ),
                        new Rules::Char('8')
                    }),
                    new Rules::Char('2'),
                })
            })
    .match(b));

    EXPECT_EQ(b.currentPosition(), 2);
    EXPECT_EQ(b.peekNext(), '3');
}

TEST(Rules, erase_test_01) {
    std::string a = "1234";
    CPP::Iterator<std::string> b(a);
    using namespace CPP;

    auto match = Rules_NS_LogInput_NO_ALLOC(
            Rules_NS_LogCapture(
                new Rules::String("12", [] (auto x) { x.eraseAndRescan(); })
            )
    ).match(b);

    EXPECT_TRUE(
        match
    );

    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), '3');
}

TEST(Rules, erase_test_02) {
    std::string a = "1234";
    CPP::Iterator<std::string> b(a);
    using namespace CPP;

    auto match = Rules_NS_LogInput_NO_ALLOC(
            Rules_NS_LogCapture(
                    new Rules::Sequence({
                        Rules_NS_LogInput(
                            Rules_NS_LogCapture(
                                new Rules::String("12", [] (auto x) { x.eraseAndRescan(); })
                            )
                        ),
                        Rules_NS_LogInput(
                            Rules_NS_LogCapture(
                                new Rules::Char('3')
                            )
                        )
                    })
            )
    ).match(b);

    EXPECT_TRUE(
        match
    );

    EXPECT_EQ(b.currentPosition(), 1);
    EXPECT_EQ(b.peekNext(), '4');
}

TEST(Rules, erase_test_03) {
    std::string a = "1234";
    CPP::Iterator<std::string> b(a);
    using namespace CPP;

    auto match = Rules_NS_LogInput_NO_ALLOC(
            Rules_NS_LogCapture(
                    new Rules::Sequence({
                        Rules_NS_LogInput(
                            Rules_NS_LogCapture(
                                new Rules::Char('1')
                            )
                        ),
                        Rules_NS_LogInput(
                            Rules_NS_LogCapture(
                                new Rules::String("23", [] (auto x) { x.eraseAndRescan(); })
                            )
                        ),
                        Rules_NS_LogInput(
                            Rules_NS_LogCapture(
                                new Rules::Char('4')
                            )
                        )
                    })
            )
    ).match(b);

    EXPECT_TRUE(
        match
    );

    EXPECT_EQ(b.currentPosition(), 2);
    EXPECT_EQ(b.peekNext(), '\0');
}

TEST(Rules, replace_test_01) {
    std::string a = "1234";
    CPP::Iterator<std::string> b(a);
    using namespace CPP;

    auto match = Rules_NS_LogInput_NO_ALLOC(
            Rules_NS_LogCapture(
                new Rules::String("12", [] (auto x) { x.replaceAndRescan(""); })
            )
    ).match(b);

    EXPECT_TRUE(
        match
    );

    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), '3');
}

TEST(Rules, replace_test_02) {
    std::string a = "1234";
    CPP::Iterator<std::string> b(a);
    using namespace CPP;

    auto match = Rules_NS_LogInput_NO_ALLOC(
            Rules_NS_LogCapture(
                    new Rules::Sequence({
                        Rules_NS_LogInput(
                            Rules_NS_LogCapture(
                                new Rules::String("12", [] (auto x) { x.replaceAndRescan(""); })
                            )
                        ),
                        Rules_NS_LogInput(
                            Rules_NS_LogCapture(
                                new Rules::Char('3')
                            )
                        )
                    })
            )
    ).match(b);

    EXPECT_TRUE(
        match
    );

    EXPECT_EQ(b.currentPosition(), 1);
    EXPECT_EQ(b.peekNext(), '4');
}

TEST(Rules, replace_test_03) {
    std::string a = "1234";
    CPP::Iterator<std::string> b(a);
    using namespace CPP;

    auto match = Rules_NS_LogInput_NO_ALLOC(
            Rules_NS_LogCapture(
                new Rules::String("12", [] (auto x) { x.replaceAndRescan("5"); })
            )
    ).match(b);

    EXPECT_TRUE(
        match
    );

    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), '5');
}

TEST(Rules, replace_test_04) {
    std::string a = "1234";
    CPP::Iterator<std::string> b(a);
    using namespace CPP;

    auto match = Rules_NS_LogInput_NO_ALLOC(
            Rules_NS_LogCapture(
                    new Rules::Sequence({
                        Rules_NS_LogInput(
                            Rules_NS_LogCapture(
                                new Rules::String("12", [] (auto x) { x.replaceAndRescan("5"); })
                            )
                        ),
                        Rules_NS_LogInput(
                            Rules_NS_LogCapture(
                                new Rules::Char('5')
                            )
                        ),
                        Rules_NS_LogInput(
                            Rules_NS_LogCapture(
                                new Rules::Char('3')
                            )
                        )
                    })
            )
    ).match(b);

    EXPECT_TRUE(
        match
    );

    EXPECT_EQ(b.currentPosition(), 2);
    EXPECT_EQ(b.peekNext(), '4');
}

TEST(Rules, replace_test_05) {
    std::string a = "1234";
    CPP::Iterator<std::string> b(a);
    using namespace CPP;

    auto match = Rules_NS_LogInput_NO_ALLOC(
            Rules_NS_LogCapture(
                    new Rules::Sequence({
                        Rules_NS_LogInput(
                            Rules_NS_LogCapture(
                                new Rules::String("12", [] (auto x) { x.replaceAndRescan("5"); })
                            )
                        ),
                    })
            )
    ).match(b);

    EXPECT_TRUE(
        match
    );

    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), '5');
}

TEST(Rules, replace_test_06) {
    std::string a = "1234";
    CPP::Iterator<std::string> b(a);
    using namespace CPP;

    auto match = Rules_NS_LogInput_NO_ALLOC(
            Rules_NS_LogCapture(
                    new Rules::Sequence({
                        Rules_NS_LogInput(
                            Rules_NS_LogCapture(
                                new Rules::String("12", [] (auto x) { x.replaceAndRescan("5"); })
                            )
                        ),
                        Rules_NS_LogInput(
                            Rules_NS_LogCapture(
                                new Rules::Char('5')
                            )
                        ),
                    })
            )
    ).match(b);

    EXPECT_TRUE(
        match
    );

    EXPECT_EQ(b.currentPosition(), 1);
    EXPECT_EQ(b.peekNext(), '3');
}

#endif

TEST(Initialization, test_01) {
    std::string a;
    CPP::Iterator<std::string> string_iterator(a);
}

TEST(BackTracking, test_01) {
    CPP::Rules::Rule rule;
}

TEST(CPP_TESTS, General) {
    std::string a = "a";
    EXPECT_TRUE(CPP::Rules::Range({'a'}).match(a));
    a = "b";
    EXPECT_TRUE(CPP::Rules::Range({'A', 'C', 'a', 'c'}).match(a));
    a = "A";
    EXPECT_TRUE(CPP::Rules::Range({'a', 'c', 'A', 'C'}).match(a));
    a = "X";
    EXPECT_TRUE(CPP::Rules::Range({'a', 'c', 'A', 'C', 'X'}).match(a));

    std::string b = "abcd";
    CPP::Iterator A(b);
    auto s = new CPP::Rules::Sequence(
            {
                    new CPP::Rules::Char('a'), new CPP::Rules::Char('b'),
                    new CPP::Rules::Char('c'), new CPP::Rules::Char('d')
            });
    auto s1 = new CPP::Rules::RuleHolder(s);
    EXPECT_TRUE(!CPP::Rules::NotAt(s1).match(A));
    A.reset();
    auto m = new CPP::Rules::NotAt(new CPP::Rules::Sequence(
            {
                    new CPP::Rules::Char('a'), new CPP::Rules::Char('b'),
                    new CPP::Rules::Char('c'), new CPP::Rules::Char('e')
            }));
    auto m1 = new CPP::Rules::At(s);
    EXPECT_TRUE(CPP::Rules::At(s).match(A));
    EXPECT_TRUE(CPP::Rules::Sequence({m1, m, m1, m, s}).match(A));


    std::string c = "a";
    CPP::Iterator ci(c);
    auto anyA = new CPP::Rules::Any([](CPP::Rules::Input) { XOut << "A" << std::endl; });
    auto anyB = new CPP::Rules::RuleHolder(new CPP::Rules::TemporaryAction(anyA), [](CPP::Rules::Input) { XOut << "B" << std::endl; });
    EXPECT_TRUE(anyA->match(ci));
    ci.reset();
    EXPECT_TRUE(anyB->match(ci));

    EXPECT_TRUE(CPP::Rules::Success().match(c));
    EXPECT_TRUE(!CPP::Rules::Fail().match(c));
    EXPECT_TRUE(CPP::Rules::AdvanceInputBy().match(c));
    EXPECT_TRUE(CPP::Rules::AdvanceInputBy(1).match(c));
    EXPECT_TRUE(CPP::Rules::Sequence({new CPP::Rules::Success(), new CPP::Rules::Char('a')}).match(c));
    EXPECT_TRUE(CPP::Rules::Or({new CPP::Rules::Char('b'), new CPP::Rules::Char('a')}).match(c));
    EXPECT_TRUE(CPP::Rules::Or({new CPP::Rules::Sequence({new CPP::Rules::Char('b')}), new CPP::Rules::Char('a')}).match(c));
    EXPECT_TRUE(CPP::Rules::Or({new CPP::Rules::Sequence({new CPP::Rules::Char('a'), new CPP::Rules::Char('b')}), new CPP::Rules::Char('a')}).match(c));
    EXPECT_TRUE(CPP::Rules::Or({new CPP::Rules::Sequence({new CPP::Rules::Char('b'), new CPP::Rules::Char('a')}), new CPP::Rules::Char('a')}).match(c));
    EXPECT_TRUE(CPP::Rules::Or({new CPP::Rules::Sequence({new CPP::Rules::Success(), new CPP::Rules::Char('b')}), new CPP::Rules::Char('a')}).match(c));
    EXPECT_TRUE(CPP::Rules::Or({new CPP::Rules::Sequence({new CPP::Rules::Success(), new CPP::Rules::Fail()}), new CPP::Rules::Char('a')}).match(c));
    EXPECT_TRUE(CPP::Rules::Or({new CPP::Rules::Sequence({new CPP::Rules::Fail(), new CPP::Rules::Char('b')}), new CPP::Rules::Char('a')}).match(c));
    EXPECT_TRUE(CPP::Rules::Or({new CPP::Rules::Sequence({new CPP::Rules::Any(), new CPP::Rules::Char('b')}), new CPP::Rules::Char('a')}).match(c));
    EXPECT_TRUE(CPP::Rules::Or({new CPP::Rules::Sequence({new CPP::Rules::AdvanceInputBy(5), new CPP::Rules::Fail()}), new CPP::Rules::Char('a')}).match(c));
    EXPECT_TRUE(CPP::Rules::Or({new CPP::Rules::Sequence({new CPP::Rules::RuleHolder(new CPP::Rules::AdvanceInputBy(5)), new CPP::Rules::Fail()}), new CPP::Rules::Char('a')}).match(c));
    EXPECT_TRUE(CPP::Rules::Or({new CPP::Rules::Sequence({new CPP::Rules::Success(), new CPP::Rules::Fail()}), new CPP::Rules::Char('a')}).match(c));
    EXPECT_TRUE(CPP::Rules::Or({new CPP::Rules::TemporaryAction(
        new CPP::Rules::Or({
            new CPP::Rules::Sequence({
                new CPP::Rules::Success(),
                new CPP::Rules::Fail()
            }),
            new CPP::Rules::Fail([](auto x) {
                XOut << "fail" << std::endl;
            }),
        }), [](auto x) {
            XOut << "or success" << std::endl;
        }),
        new CPP::Rules::Char('a')
    }).match(c));

    EXPECT_TRUE(CPP::Rules::OneOrMore(
        new CPP::Rules::Or({
            new CPP::Rules::Sequence({
                new CPP::Rules::Success(),
                new CPP::Rules::At(new CPP::Rules::Char('f')),
                new CPP::Rules::Char('f')
            }, [](auto x) {
                XOut << "sequence success" << std::endl;
            }),
            new CPP::Rules::Any([](CPP::Rules::Input x) {
                XOut << "any success: " << x.string() << std::endl;
            })
        }, [](CPP::Rules::Input x) {
            XOut << "or success: " << x.string() << std::endl;
        })
    ).match(a));
}

// #define foo(x) 9 x(x) 9
// 1 foo(foo) 2 foo(bar) 3

TEST(Macro_Tests, A) {
    std::string a = "1 foo(foo) 2 foo(bar) 3";

    XOut << "a: " << a << std::endl;

    auto grammar = new CPP::Rules::Sequence({
        CPP_Rules_LogMatchStatus(new CPP::Rules::String("1 ")),
        CPP_Rules_LogMatchStatus(new CPP::Rules::String("foo(foo)", [] (CPP::Rules::Input in) {
            XOut << "capture: " << in.string() << std::endl;
            in.replace("9 x(x) 9");
        })),
        CPP_Rules_LogMatchStatus(new CPP::Rules::String(" 2 ")),
        CPP_Rules_LogMatchStatus(new CPP::Rules::String("foo(bar)", [] (CPP::Rules::Input in) {
            XOut << "capture: " << in.string() << std::endl;
            in.replace("9 x(x) 9");
        })),
        CPP_Rules_LogMatchStatus(new CPP::Rules::String(" 3"))
    });

    EXPECT_TRUE(grammar->match(a));

    XOut << "a: " << a << std::endl;

    delete grammar;
}

TEST(Macro_Tests, B) {
    std::string a = "1 foo(foo) 2 foo(bar) 3";

    XOut << "a: " << a << std::endl;

    auto grammar = new CPP::Rules::Sequence({
        CPP_Rules_LogMatchStatus(new CPP::Rules::String("1 ")),
        CPP_Rules_LogMatchStatus(new CPP::Rules::String("foo(foo)", [] (CPP::Rules::Input in) {
            XOut << "capture: " << in.string() << std::endl;
            in.replace("9 foo(foo) 9");
        })),
        CPP_Rules_LogMatchStatus(new CPP::Rules::String(" 2 ")),
        CPP_Rules_LogMatchStatus(new CPP::Rules::String("foo(bar)", [] (CPP::Rules::Input in) {
            XOut << "capture: " << in.string() << std::endl;
            in.replace("9 bar(bar) 9");
        })),
        CPP_Rules_LogMatchStatus(new CPP::Rules::String(" 3"))
    });

    EXPECT_TRUE(grammar->match(a));

    XOut << "a: " << a << std::endl;

    delete grammar;
}

TEST(Macro_Tests, C) {
    std::string a = "1 foo(foo) 2 foo(bar) 3";

    XOut << "a: " << a << std::endl;

    auto identifier = new CPP::Rules::Sequence({
        new CPP::Rules::Range({'a', 'z', 'A', 'Z', '_'}),
        new CPP::Rules::ZeroOrMore(
            new CPP::Rules::Range({'a', 'z', 'A', 'Z', '0', '9', '_'})
        )
    });

    auto parens_open = new CPP::Rules::Char('(');
    auto parens_close = new CPP::Rules::Char(')');

    auto function = new CPP::Rules::Sequence({
        identifier, parens_open, identifier, parens_close
    });

    function->action = [] (CPP::Rules::Input in) {
        XOut << "capture: " << in.string() << std::endl;
        in.replace("9 x(x) 9");
    };

    auto grammar = new CPP::Rules::Sequence({
        new CPP::Rules::String("1 "),
        function,
        new CPP::Rules::String(" 2 "),
        function,
        new CPP::Rules::String(" 3")
    });

    EXPECT_TRUE(grammar->match(a));

    XOut << "a: " << a << std::endl;

    delete grammar;
}

TEST(Macro_Tests, D) {
    std::string a = "1 foo(foo) 2 foo(bar) 3";

    XOut << "a: " << a << std::endl;

    auto identifier = new CPP::Rules::Sequence({
        new CPP::Rules::Range({'a', 'z', 'A', 'Z', '_'}),
        new CPP::Rules::ZeroOrMore(
            new CPP::Rules::Range({'a', 'z', 'A', 'Z', '0', '9', '_'})
        )
    });

    auto parens_open = new CPP::Rules::Char('(');
    auto parens_close = new CPP::Rules::Char(')');

    auto function_tmp = new CPP::Rules::Sequence({
        identifier, parens_open, identifier, parens_close
    });

    auto function = new CPP::Rules::Sequence({
        new CPP::Rules::At(function_tmp), function_tmp
    });

    function->action = [] (CPP::Rules::Input in) {
        XOut << "capture: " << in.string() << std::endl;
        in.replace("9 x(x) 9");
    };

    auto grammar = new CPP::Rules::OneOrMore(
        new CPP::Rules::Or({
           function,
           new CPP::Rules::Any()
       })
    );

    EXPECT_TRUE(grammar->match(a));

    XOut << "a: " << a << std::endl;

    delete grammar;
}

// #define x x y
// #define y x y
// x

/*
[ RUN      ] Macro_Tests.E
a: 1 x 2 x
a: 1 x x y 2 x x y
[       OK ] Macro_Tests.E (0 ms)
*/
TEST(Macro_Tests, E) {
    std::string a = "1 x 2 x";

    XOut << "a: " << a << std::endl;

    /*
14:48:18 smallville7123: where the grammar mutation comes in, is in the case of
14:48:19 smallville7123: "1 x 2 x" -> "1 x y 2 x" // x must not be re-expanded but "2 x" must also allow x to be expanded
14:49:11 smallville7123: in here, we would instead do "1 x 2 x" -> "1 x y 2 x" // a rule would be inserted in order to match "x y" and expand it as needed
14:49:49 smallville7123: and this is where the stack comes in
14:50:20 smallville7123: if say "y" expands to "x y" then we need to insert another rule to expand "x y" (which will not expand further)
14:50:56 smallville7123: so "x" -> "x y" -> "x x y" -> no more expansions, pop to initial rule
     */


    auto x_0 = new CPP::Rules::RuleHolder(new CPP::Rules::Char('x'));

    CPP::Rules::Action base_0;
    CPP::Rules::Action base_1;
    CPP::Rules::Action base_2;
    base_0 = [&](CPP::Rules::Input in) {
        delete x_0->rule;
        // #define x x y
        x_0->rule = new CPP::Rules::String("x y");
        x_0->action = base_1;
        in.replaceAndRescan("x y");
    };
    base_1 = [&](CPP::Rules::Input in) {
        delete x_0->rule;
        // #define y x y
        x_0->rule = new CPP::Rules::String("x x y");
        x_0->action = base_2;
        in.replaceAndRescan("x x y");
    };
    base_2 = [&](CPP::Rules::Input in) {
        delete x_0->rule;
        x_0->rule = new CPP::Rules::Char('x');
        x_0->action = base_0;
        // no more replacements
    };

    x_0->action = base_0;

    auto grammar = new CPP::Rules::OneOrMore(
        new CPP::Rules::Or({
           x_0,
           new CPP::Rules::Any()
       })
    );

    EXPECT_TRUE(grammar->match(a));

    XOut << "a: " << a << std::endl;

    delete grammar;
}

/*
[ RUN      ] Macro_Tests.F
a: 1 x 2 x
base
base + 1
base + 2
pop to base
base
base + 1
base + 2
pop to base
a: 1 x x y 2 x x y
[       OK ] Macro_Tests.F (0 ms)
*/
TEST(Macro_Tests, F) {
    std::string a = "1 x 2 x";

    XOut << "a: " << a << std::endl;

    /*
14:48:18 smallville7123: where the grammar mutation comes in, is in the case of
14:48:19 smallville7123: "1 x 2 x" -> "1 x y 2 x" // x must not be re-expanded but "2 x" must also allow x to be expanded
14:49:11 smallville7123: in here, we would instead do "1 x 2 x" -> "1 x y 2 x" // a rule would be inserted in order to match "x y" and expand it as needed
14:49:49 smallville7123: and this is where the stack comes in
14:50:20 smallville7123: if say "y" expands to "x y" then we need to insert another rule to expand "x y" (which will not expand further)
14:50:56 smallville7123: so "x" -> "x y" -> "x x y" -> no more expansions, pop to initial rule
     */

    auto x_stack = new CPP::Rules::Stack();

    x_stack->setBase(new CPP::Rules::Char('x'), [&](CPP::Rules::Input in) {
        // #define x x y
        XOut << "base" << std::endl;
        x_stack->push(new CPP::Rules::String("x y"),[&](CPP::Rules::Input in) {
            // #define y x y
            XOut << "base + 1" << std::endl;
            x_stack->push(new CPP::Rules::String("x x y"), [&](CPP::Rules::Input in) {
                // no more replacements
                XOut << "base + 2" << std::endl;
                XOut << "pop to base" << std::endl;
                x_stack->popAll();
                // no more replacements
            });
            // #define y x y
            in.replaceAndRescan("x x y");
        });
        // #define x x y
        in.replaceAndRescan("x y");
    });

    auto grammar = new CPP::Rules::OneOrMore(
        new CPP::Rules::Or({
           x_stack,
           new CPP::Rules::Any()
       })
    );

    EXPECT_TRUE(grammar->match(a));

    XOut << "a: " << a << std::endl;

    delete grammar;
}

TEST(Macro_Tests, G) {
    std::string a = "1 foo(foo) 2 foo(bar) 3";

    XOut << "a: " << a << std::endl;

    auto identifier = new CPP::Rules::Sequence({
        new CPP::Rules::Range({'a', 'z', 'A', 'Z', '_'}),
        new CPP::Rules::ZeroOrMore(
            new CPP::Rules::Range({'a', 'z', 'A', 'Z', '0', '9', '_'})
        )
    });

    auto parens_open = new CPP::Rules::Char('(');
    auto parens_close = new CPP::Rules::Char(')');

    auto function_tmp = new CPP::Rules::Sequence({
        identifier, parens_open, identifier, parens_close
    });









    auto function_stack = new CPP::Rules::Stack();

    function_stack->setBase(
        new CPP::Rules::Sequence({
            new CPP::Rules::At(function_tmp), function_tmp
        }),
        [&] (CPP::Rules::Input in) {
            XOut << "capture: " << in.string() << std::endl;
            in.replace("9 x(x) 9");
            function_stack->push(
                new CPP::Rules::OneOrMore(
                    new CPP::Rules::Or({
                        new CPP::Rules::Sequence({
                            new CPP::Rules::At(function_tmp), function_tmp
                        }),
                        new CPP::Rules::Any()
                    })
                ),
                [&](CPP::Rules::Input in) {

                }
            );
        }
    );

    auto grammar = new CPP::Rules::OneOrMore(
        new CPP::Rules::Or({
           function_stack,
           new CPP::Rules::Any()
       })
    );

    EXPECT_TRUE(grammar->match(a));

    XOut << "a: " << a << std::endl;

    delete grammar;
}
