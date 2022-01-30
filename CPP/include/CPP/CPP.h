#include "CPP_Preprocessor_Data.h"
#include "Rules.h"
#include <stack>

namespace CPP {
    class CPP {
    private:
        void remove_line_continuations(std::string &input) {
            auto grammar = Rules::OneOrMore(
                    new Rules::Or({
                        new Rules::String("\\\n", [](Rules::Input in) {
                            in.eraseAndRescan();
                        }),
                        new Rules::Any()
                    })
            );
            grammar.match(input);
            XOut << "removed line continuations: " << "'" << input << "'" << std::endl;
        }

        void remove_comments(std::string &input) {
            auto grammar = Rules::OneOrMore(
                new Rules::Or({
                    new Rules::Sequence({
                        new Rules::String("//"),
                        new Rules::Until(new Rules::Char('\n'))
                    }, [](Rules::Input in) {
                        in.eraseAndRescan();
                    }),
                    new Rules::Sequence({
                        new Rules::String("/*"),
                        new Rules::Until(new Rules::String("*/"))
                    }, [](Rules::Input in) {
                        in.eraseAndRescan();
                    }),
                    new Rules::Any()
                })
            );
            grammar.match(input);
            XOut << "removed comments: " << "'" << input << "'" << std::endl;
        }

        void preprocess(std::string &input) {
            preprocess(input, cpp_data);
        }

    private:
        CPP_Preprocessor_Data cpp_data;

        void preprocess(std::string &input, CPP_Preprocessor_Data & data) {

            auto newline = new Rules::Char('\n');
            auto whitespaces = new Rules::OneOrMore(new Rules::Char(' '));
            auto optional_whitespaces = new Rules::Optional(whitespaces);

            auto identifier = new Rules::Sequence({
                new Rules::Range({'a', 'z', 'A', 'Z', '_'}),
                new Rules::Optional(
                    new Rules::OneOrMore(
                        new Rules::Range({'a', 'z', 'A', 'Z', '0', '9', '_'})
                    )
                )
            });

            auto parens_open = new Rules::Char('(');
            auto parens_close = new Rules::Char(')');
            auto comma = new Rules::Char(',');


            auto macro_function_decl_begin = new Rules::TemporaryAction(parens_open, [&data](Rules::Input in) {
                data.definitions[data.current_id].type = CPP_Preprocessor_Data::Macro::Function;
            });
            auto macro_function_decl_arg = new Rules::TemporaryAction(identifier, [&data](Rules::Input in) {
                auto macro = in.string();
                XOut << "definition function-macro argument: " << "'" << macro << "'" << std::endl;
                data.definitions[data.current_id].args.push_back(macro);
            });
            auto macro_function_decl_end = new Rules::TemporaryAction(parens_close, [&data](Rules::Input in) {
                auto & macro = data.definitions[data.current_id];
                if (macro.args.size() == 0) macro.args.push_back("");
            });

            auto macro_function_decl = new Rules::Sequence({
                macro_function_decl_begin,
                optional_whitespaces,
                new Rules::Optional(
                    new Rules::Sequence({
                        new Rules::NotAt(
                            macro_function_decl_end
                        ),
                        macro_function_decl_arg,
                        optional_whitespaces,
                        new Rules::Optional(
                            new Rules::Sequence({
                                new Rules::NotAt(
                                    macro_function_decl_end
                                ),
                                new Rules::OneOrMore(
                                    new Rules::Sequence({
                                        comma,
                                        optional_whitespaces,
                                        macro_function_decl_arg,
                                        optional_whitespaces
                                    })
                                )
                            })
                        )
                    })
                ),
                macro_function_decl_end
            });

            auto preprocessor_directive_replacement = new Rules::Until(newline,
                                                                       [&data](Rules::Input in) {
               XOut << "definition content: " << in.stringRemoveCharactersFromEnd(1) << std::endl;
               data.definitions[data.current_id].content = in.stringRemoveCharactersFromEnd(1);
               data.current_id = "";
            });

            auto define = new Rules::Sequence({
                new Rules::String("define", [&data](Rules::Input) {
                    data.preprocessor_state = CPP_Preprocessor_Data::define;
                }),
                whitespaces,
                new Rules::TemporaryAction(identifier, [&data](Rules::Input in) {
                    XOut << "definition id: " << in.string() << std::endl;
                    CPP_Preprocessor_Data::Macro macro;
                    macro.id = in.string();
                    if (macro.id == "defined") {
                        throw new std::runtime_error("defined is a reserved preprocessor keyword");
                    }
                    data.current_id = macro.id;
                    data.definitions[data.current_id] = macro;
                }),
                optional_whitespaces,
                new Rules::Optional(macro_function_decl),
                optional_whitespaces,
                new Rules::Optional(preprocessor_directive_replacement)
            });

            auto reset_preprocessor_state = new Rules::Success([&data](Rules::Input) {
                data.preprocessor_state = CPP_Preprocessor_Data::none;
            });

            auto grammar = Rules::OneOrMore(
                new Rules::Or({
                    new Rules::Sequence({
                        new Rules::Char('#'),
                        optional_whitespaces,
                        new Rules::Or({
                            define
                        }),
                        reset_preprocessor_state
                    }, [](Rules::Input in) {
                        XOut << "erasing preprocessor statement: '" << in.stringRemoveCharactersFromEnd(1) << "'" << std::endl;
                        in.eraseAndRescan();
                    }),
                    new Rules::Any()
                })
            );
            grammar.match(input);
            XOut << "preprocessed: " << "'" << input << "'" << std::endl;
        }

    public:
        void parse(std::string input) {
            // 1. remove line continuations
            remove_line_continuations(input);
            // 2. remove comments
            remove_comments(input);
            // 3. preprocess
            preprocess(input);
        }
    };
}