#ifndef CPP_H
#define CPP_H

#include "CPP_Preprocessor_Data.h"
#include "Rules.h"
#include <stack>

namespace CPP {
    class CPP {
    private:
#ifdef GTEST_API_
        public:
#endif
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
            XOut << "removed line continuations: " << Rules::Input::quote(input) << std::endl;
        }

        void remove_comments(std::string &input) {
            auto grammar = Rules::OneOrMore(
                new Rules::Or({
                    new Rules::Sequence({
                        new Rules::String("//"),
                        new Rules::Until(new Rules::NewlineOrEOF())
                    }, [](Rules::Input in) {
                        in.eraseAndRescan();
                    }),
                    new Rules::Sequence({
                        new Rules::String("/*"),
                        new Rules::ErrorIfNotMatch(
                            new Rules::Until(new Rules::String("*/")),
                            "Unterminated block comment, expected '*/' to match '/*'"
                        )
                    }, [](Rules::Input in) {
                        in.eraseAndRescan();
                    }),
                    new Rules::Any()
                })
            );
            grammar.match(input);
            XOut << "removed comments: " << Rules::Input::quote(input) << std::endl;
        }

        void preprocess(std::string &input) {
            preprocess(input, cpp_data);
        }

    private:
#ifdef GTEST_API_
        public:
#endif
        CPP_Preprocessor_Data cpp_data;

        void preprocess(std::string &input, CPP_Preprocessor_Data & data) {

            auto newline = new Rules::Char('\n');
            auto whitespaces = new Rules::OneOrMore(new Rules::Char(' '));
            auto optional_whitespaces = new Rules::Optional(whitespaces);

            auto identifier = new Rules::Sequence({
                new Rules::Range({'a', 'z', 'A', 'Z', '_'}),
                new Rules::ZeroOrMore(
                    new Rules::Range({'a', 'z', 'A', 'Z', '0', '9', '_'})
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
                XOut << "definition function-macro argument: " << Rules::Input::quote(macro) << std::endl;
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

            auto preprocessor_directive_replacement = new Rules::Until(
                new Rules::Or({
                    newline,
                    new Rules::EndOfFile
                }), [&data](Rules::Input in) {
                std::string content = in.stringRemoveCharactersFromEnd(1);
                std::string x = in.quote(content);
                XOut << "definition content: " << x << std::endl;
                data.definitions[data.current_id].content = content;
                data.current_id = "";
            });

            auto define = new Rules::Sequence({
                new Rules::String("define", [&data](Rules::Input) {
                    data.preprocessor_state = CPP_Preprocessor_Data::define;
                }),
                whitespaces,
                new Rules::TemporaryAction(identifier, [&data](Rules::Input in) {
                    XOut << "definition id: " << in.quotedString() << std::endl;
                    CPP_Preprocessor_Data::Macro macro;
                    macro.id = in.string();
                    if (macro.id == "defined") {
                        XOut << "defined is a reserved preprocessor keyword" << XLog::Abort;
                    }
                    data.current_id = macro.id;
                    data.definitions[data.current_id] = macro;
                }),
                optional_whitespaces,
                new Rules::Optional(macro_function_decl),
                optional_whitespaces,
                preprocessor_directive_replacement
            });

            auto reset_preprocessor_state = new Rules::Success([&data](Rules::Input) {
                data.preprocessor_state = CPP_Preprocessor_Data::none;
            });

            auto preprocessor_directive = new Rules::Sequence({
                new Rules::Char('#'),
                optional_whitespaces,
                new Rules::Or({
                    define
                }),
                reset_preprocessor_state
            }, [](Rules::Input in) {
                XOut << "erasing preprocessor statement: " << in.quotedStringRemoveCharactersFromEnd(1) << std::endl;
                in.eraseAndRescan();
            });

            Rules::Sequence * balanced_parens = new Rules::Sequence({}, [](Rules::Input in) {
                XOut << "rescanning balanced parens" << std::endl;
                in.rescan();
            });
            balanced_parens->rules = {
                parens_open,
                new Rules::MatchBUntilA(
                    parens_close,
                    new Rules::Or({
                        balanced_parens,
                        new Rules::ErrorIfMatch(new Rules::EndOfFile, "Unterminated parenthesis, expected ')' to match '('"),
                        new Rules::AdvanceInputBy(1)
                    })
                )
            };

            auto function_begin = new Rules::TemporaryAction(parens_open, [&](Rules::Input in) {
                auto * var = data.getValueIfKeyExists(data.definitions, data.current_id);
                if (var != nullptr) {
                    if (var->type == CPP_Preprocessor_Data::Macro::Function) {
                        XOut << "scanning arguments for function-like macro : " << Rules::Input::quote(data.current_id) << std::endl;
                        var->count = 0;
                    }
                }
            });

            auto function_end = new Rules::TemporaryAction(parens_close, [&](Rules::Input in) {
                auto * var = data.getValueIfKeyExists(data.definitions, data.current_id);
                if (var != nullptr) {
                    if (var->type == CPP_Preprocessor_Data::Macro::Function) {
                        XOut << "scanned arguments for function-like macro : " << Rules::Input::quote(data.current_id) << std::endl;
                        // check argument count

                        XOut << "argument count: " << var->count << '\n';
                        size_t argc = var->args.size();
                        XOut << "required argument count: " << argc << '\n';
                        if (var->count > argc) {
                            XOut << "macro " << in.quote(data.current_id) << " passed " << var->count << " arguments, but takes just " << argc << XLog::Abort;
                        }
                        else if (var->count < argc) {
                            XOut << "macro " << in.quote(data.current_id) << " requires " << argc << " arguments, but only " << var->count << " given" << XLog::Abort;
                        }
                    }
                }
            });

            Rules::TemporaryAction * function_name = new Rules::TemporaryAction(identifier);
            function_name->action = [&](Rules::Input in) {
                data.current_id = in.string();
                XOut << "function name: " << Rules::Input::quote(data.current_id) << std::endl;
                auto * var = data.getValueIfKeyExists(data.definitions, data.current_id);
                if (var != nullptr) {
                    XOut << "macro found" << std::endl;
                    if (var->type == CPP_Preprocessor_Data::Macro::Object) {
                        XOut << "expanding object-like macro: " << Rules::Input::quote(data.current_id) << std::endl;


                        CPP_Preprocessor_Data old = data;

                        CPP_Preprocessor_Data tmp;
                        tmp.definitions = data.definitions;
                        tmp.do_not_expand = data.do_not_expand;
                        std::string mac = data.current_id;
                        XOut << "adding macro to do-not-expand list: " << Rules::Input::quote(mac) << std::endl;
                        tmp.do_not_expand.push_back(var->id);

                        data = tmp;

                        std::string replacement = std::string(var->content);
                        function_name->match(replacement);

                        XOut << "removing macro from do-not-expand list: " << Rules::Input::quote(mac) << std::endl;
                        data = old;

                        XOut << "expanded object-like macro: " << Rules::Input::quote(data.current_id) << " to " << Rules::Input::quote(replacement) << std::endl;
                        in.replace(replacement);
                    } else {
                        XOut << "expanding function-like macro: " << Rules::Input::quote(data.current_id) << std::endl;
                    }
                } else {
                    XOut << "macro not found" << std::endl;
                }
            };

            Rules::TemporaryAction * non_function_name = new Rules::TemporaryAction(identifier);
            non_function_name->action = [&](Rules::Input in) {
                data.current_id = in.string();
                XOut << "non function name: " << Rules::Input::quote(data.current_id) << std::endl;
                auto * var = data.getValueIfKeyExists(data.definitions, data.current_id);
                if (var != nullptr) {
                    XOut << "macro found" << std::endl;
                    if (var->type == CPP_Preprocessor_Data::Macro::Object) {
                        XOut << "expanding object-like macro: " << Rules::Input::quote(data.current_id) << std::endl;


                        CPP_Preprocessor_Data old = data;

                        CPP_Preprocessor_Data tmp;
                        tmp.definitions = data.definitions;
                        tmp.do_not_expand = data.do_not_expand;
                        XOut << "adding macro to do-not-expand list: " << Rules::Input::quote(data.current_id) << std::endl;
                        tmp.do_not_expand.push_back(var->id);

                        data = tmp;

                        std::string replacement = std::string(var->content);
                        function_name->match(replacement);

                        XOut << "removing macro from do-not-expand list: " << Rules::Input::quote(data.current_id) << std::endl;
                        data = old;

                        XOut << "expanded object-like macro: " << Rules::Input::quote(data.current_id) << " to " << Rules::Input::quote(replacement) << std::endl;
                        in.replaceAndRescan(replacement);
                    } else {
                        XOut << "expanding non function-like macro: " << Rules::Input::quote(data.current_id) << std::endl;
                    }
                } else {
                    XOut << "macro not found" << std::endl;
                }
            };

            // arguments are separated by comma, and can have whitespaces in between
            auto function_call_arg = new Rules::MatchBUntilA(
                // an empty argument is a valid argument and is replaced by an empty token
                new Rules::At(
                    new Rules::Or({
                        comma,
                        function_end
                    })
                ),
                new Rules::Or({
                    new Rules::ErrorIfMatch(new Rules::EndOfFile, "Unterminated function parenthesis, expected ')' to match '('"),
                    balanced_parens,
                    new Rules::AdvanceInputBy(1)
                })
            , [&](Rules::Input in) {
                auto arg = in.string();
                auto * var = data.getValueIfKeyExists(data.definitions, data.current_id);
                if (var != nullptr) {
                    if (var->type == CPP_Preprocessor_Data::Macro::Function) {
                        XOut << "found function argument for function-like macro : " << Rules::Input::quote(data.current_id) << std::endl;
                        var->count++;
                    }
                }
                XOut << "function argument: " << in.quote(arg) << std::endl;
            });

            auto function_call_arg_and_optional_comma = new Rules::Sequence({
                function_call_arg,
                new Rules::Optional(comma)
            });

            auto function_call = new Rules::Sequence({
                new Rules::At(new Rules::Sequence({
                    function_name,
                    optional_whitespaces,
                    parens_open
                })),
                function_name,
                optional_whitespaces,
                balanced_parens,
                function_begin,
                new Rules::MatchBUntilA(
                        function_end,
                        function_call_arg_and_optional_comma
                )
            }, [&](Rules::Input in) {
                XOut << "function: " << in.quotedString() << std::endl;
                std::string function_name = Rules::Input::quote(data.current_id);
                auto * var = data.getValueIfKeyExists(data.definitions, data.current_id);
                if (var == nullptr) {
                    XOut << "no macro defined for function: " << function_name << std::endl;
                    // expand and replace arguments
                } else {
                    if (var->type == CPP_Preprocessor_Data::Macro::Object) {
                        XOut << "object-like macro defined for function as result of macro-expansion but no function-like macro defined: " << function_name << std::endl;
                    } else {
                        XOut << "function-like macro defined for function: " << function_name << std::endl;
                    }
                }

            });

            auto grammar = Rules::OneOrMore(
                new Rules::Or({
                    newline,
                    whitespaces,
                    preprocessor_directive,
                    function_call,
                    non_function_name,
                    new Rules::Any()
                })
            );
            grammar.match(input);
            XOut << "preprocessed: " << Rules::Input::quote(input) << std::endl;
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

#endif