#ifndef CPP_H
#define CPP_H

#include "CPP_Preprocessor_Data.h"
#include "Rules.h"
#include <stack>

namespace CPP {
    class CPP {
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
        constexpr static const char * TAG_NONE =                      "[       NONE       ]";
        constexpr static const char * TAG_DEFINE =                    "[      DEFINE      ]";
        constexpr static const char * TAG_FUNCTION_ARGUMENT_SCAN =    "[FUNCTION ARG CHECK]";
        constexpr static const char * TAG_FUNCTION_EXPANSION =        "[FUNCTION EXPANSION]";
        constexpr static const char * TAG_MACRO_EXPANSION =           "[  MACRO EXPANSION ]";

        CPP_Preprocessor_Data cpp_data;

        static const char * getTag(CPP_Preprocessor_Data & cpp_data, bool is_function_macro = false) {
            switch (cpp_data.preprocessor_state) {
                case CPP_Preprocessor_Data::no_preprocessor_state:
                        switch(cpp_data.expansion_state) {
                            case CPP_Preprocessor_Data::no_expansion_state:
                                return TAG_NONE;
                            case CPP_Preprocessor_Data::argument_count:
                                return TAG_FUNCTION_ARGUMENT_SCAN;
                            case CPP_Preprocessor_Data::expansion:
                                return is_function_macro ? TAG_FUNCTION_EXPANSION : TAG_MACRO_EXPANSION;
                        }
                case CPP_Preprocessor_Data::define:
                    return TAG_DEFINE;
                case CPP_Preprocessor_Data::undef:
                    return TAG_NONE;
            }
        }

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
                XOut << getTag(data) << ' ' << "definition function-macro argument: " << Rules::Input::quote(macro) << std::endl;
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
                XOut << getTag(data) << ' ' << "definition content: " << x << std::endl;
                data.definitions[data.current_id].content = content;
                data.current_id = "";
            });

            auto define = new Rules::Sequence({
                new Rules::String("define", [&data](Rules::Input) {
                    data.preprocessor_state = CPP_Preprocessor_Data::define;
                }),
                whitespaces,
                new Rules::TemporaryAction(identifier, [&data](Rules::Input in) {
                    XOut << getTag(data) << ' ' << "definition id: " << in.quotedString() << std::endl;
                    CPP_Preprocessor_Data::Macro macro;
                    macro.id = in.string();
                    if (macro.id == "defined") {
                        XOut << getTag(data) << ' ' << "defined is a reserved preprocessor keyword" << XLog::Abort;
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
                data.preprocessor_state = CPP_Preprocessor_Data::no_preprocessor_state;
            });

            auto preprocessor_directive = new Rules::Sequence({
                new Rules::Sequence({
                    new Rules::Char('#'),
                    optional_whitespaces,
                    new Rules::Or({
                        define
                        })
                }, [&data](Rules::Input in) {
                    XOut << getTag(data) << ' ' << "erasing preprocessor statement: " << in.quotedStringRemoveCharactersFromEnd(1) << std::endl;
                    in.eraseAndRescan();
                }),
                reset_preprocessor_state
            });

            Rules::Sequence * balanced_parens = new Rules::Sequence({}, [&data](Rules::Input in) {
                XOut << getTag(data) << ' ' << "rescanning balanced parens" << std::endl;
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

            Rules::TemporaryAction * function_name = new Rules::TemporaryAction(identifier);
            function_name->action = [&data, &function_name](Rules::Input in) {
                auto macro_name = in.string();
                for (auto &item : data.do_not_expand) {
                    if (item == macro_name) {
                        XOut << getTag(data) << ' ' << "not expanding macro: " << Rules::Input::quote(macro_name) << std::endl;
                        return;
                    }
                }
                data.current_id = macro_name;
                CPP_Preprocessor_Data::Macro * var = nullptr;
                if (data.expanding_function) {
                    XOut << getTag(data, true) << ' ' << "expanding_function true" << std::endl;
                    var = data.getValueIfKeyExists(data.function_definitions, data.current_id);
                    if (var == nullptr) {
                        var = data.getValueIfKeyExists(data.definitions, data.current_id);
                        if (var != nullptr) {
                            XOut << getTag(data) << ' ' << "found definition for: " << Rules::Input::quote(data.current_id) << std::endl;
                        }
                    } else {
                        XOut << getTag(data) << ' ' << "found function argument definition for: " << Rules::Input::quote(data.current_id) << std::endl;
                    }
                } else {
                    XOut << getTag(data) << ' ' << "expanding_function false" << std::endl;
                    var = data.getValueIfKeyExists(data.definitions, data.current_id);
                    if (var != nullptr) {
                        XOut << getTag(data) << ' ' << "found definition for: " << Rules::Input::quote(data.current_id) << std::endl;
                    }
                }
                if (var != nullptr) {
                    if (var->type == CPP_Preprocessor_Data::Macro::Object) {
                        XOut << getTag(data) << ' ' << "macro name: " << Rules::Input::quote(data.current_id) << std::endl;
                        XOut << getTag(data) << ' ' << "object-macro found" << std::endl;
                        if (data.expansion_state == CPP_Preprocessor_Data::preprocessor_expansion_state_t::no_expansion_state) {
                            data.expansion_state = CPP_Preprocessor_Data::preprocessor_expansion_state_t::expansion;
                        }
                        XOut << getTag(data, false) << ' ' << "expanding object-like macro: " << Rules::Input::quote(data.current_id) << std::endl;


                        CPP_Preprocessor_Data old = data;
                        XOut << getTag(data) << ' ' << "adding macro to do-not-expand list: " << Rules::Input::quote(old.current_id) << std::endl;

                        data.do_not_expand.push_back(var->id);
                        std::string replacement = std::string(var->content);

                        XOut << getTag(data) << ' ' << "calling function_name->match(replacement)" << std::endl;
                        bool old_ = data.expanding_function;
                        data.expanding_function = false;
                        function_name->match(replacement);
                        data.expanding_function = old_;

                        data = old;
                        XOut << getTag(data) << ' ' << "called function_name->match(replacement)" << std::endl;
                        XOut << getTag(data) << ' ' << "removed macro from do-not-expand list: " << Rules::Input::quote(old.current_id) << std::endl;

                        XOut << getTag(data) << ' ' << "expanded object-like macro: " << Rules::Input::quote(data.current_id) << " to " << Rules::Input::quote(replacement) << std::endl;
                        in.replace(replacement);
                        data.expansion_state = CPP_Preprocessor_Data::preprocessor_expansion_state_t::no_expansion_state;
                    } else {
                        XOut << getTag(data) << ' ' << "macro name: " << Rules::Input::quote(data.current_id) << std::endl;
                        XOut << getTag(data, true) << ' ' << "function-macro found" << std::endl;
                        if (data.expansion_state == CPP_Preprocessor_Data::preprocessor_expansion_state_t::no_expansion_state) {
                            data.expansion_state = CPP_Preprocessor_Data::preprocessor_expansion_state_t::argument_count;
                            XOut << getTag(data, true) << ' ' << "scanning function-like macro: " << Rules::Input::quote(data.current_id) << std::endl;
                        } else if (data.expansion_state == CPP_Preprocessor_Data::preprocessor_expansion_state_t::expansion) {
                            XOut << getTag(data, true) << ' ' << "expanding function-like macro: " << Rules::Input::quote(data.current_id) << std::endl;
                        }
//                        if (!in.getIterator().has_next()) {
//                            XOut << getTag(data, true) << ' ' << "NO MORE TOKENS" << ", input: " << in.getIterator().currentString(in.getIterator().cbegin()) << XLog::Abort;
//                        }
                    }
                } else {
                    XOut << getTag(data) << ' ' << "identifier name: " << Rules::Input::quote(data.current_id) << std::endl;
                    XOut << getTag(data) << ' ' << "macro not found" << std::endl;
                }
            };

            auto function_begin = new Rules::TemporaryAction(parens_open, [&](Rules::Input in) {
                auto * var = data.getValueIfKeyExists(data.definitions, data.current_id);
                if (var != nullptr) {
                    if (var->type == CPP_Preprocessor_Data::Macro::Function) {
                        if (data.expansion_state == CPP_Preprocessor_Data::preprocessor_expansion_state_t::argument_count) {
                            XOut << getTag(data, true) << ' ' << "scanning arguments for function-like macro : " << Rules::Input::quote(data.current_id) << std::endl;
                            var->count = 0;
                        } else if (data.expansion_state == CPP_Preprocessor_Data::preprocessor_expansion_state_t::expansion) {
                            XOut << getTag(data, true) << ' ' << "expanding arguments for function-like macro : " << Rules::Input::quote(data.current_id) << std::endl;
                            var->count = 0;
                            var->args_content.clear();
                        }
                    }
                }
            });

            auto function_end = new Rules::TemporaryAction(parens_close, [&](Rules::Input in) {
                auto * var = data.getValueIfKeyExists(data.definitions, data.current_id);
                if (var != nullptr) {
                    if (var->type == CPP_Preprocessor_Data::Macro::Function) {
                        if (data.expansion_state == CPP_Preprocessor_Data::preprocessor_expansion_state_t::argument_count) {
                            XOut << getTag(data, true) << ' ' << "scanned arguments for function-like macro : " << Rules::Input::quote(data.current_id) << std::endl;
                            // check argument count

                            XOut << getTag(data, true) << ' ' << "argument count: " << var->count << '\n';
                            size_t argc = var->args.size();
                            XOut << getTag(data, true) << ' ' << "required argument count: " << argc << '\n';
                            if (var->count > argc) {
                                XOut << getTag(data, true) << ' ' << "macro " << in.quote(data.current_id) << " passed " << var->count << " arguments, but takes just " << argc << XLog::Abort;
                            }
                            else if (var->count < argc) {
                                XOut << getTag(data, true) << ' ' << "macro " << in.quote(data.current_id) << " requires " << argc << " arguments, but only " << var->count << " given" << XLog::Abort;
                            }
                        } else if (data.expansion_state == CPP_Preprocessor_Data::preprocessor_expansion_state_t::expansion) {
                        }
                    }
                }
            });

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
                auto arg = std::string(in.string());
                auto var = data.getValueIfKeyExists(data.definitions, data.current_id);
                XOut << getTag(data, true) << ' ' << "found function argument " << Rules::Input::quote(arg) << std::endl;
//                funcVar = data.getValueIfKeyExists(data.definitions, arg);
                CPP_Preprocessor_Data::Macro * funcVar = nullptr;
                if (data.expanding_function) {
                    XOut << getTag(data, true) << ' ' << "expanding_function true" << std::endl;
                    funcVar = data.getValueIfKeyExists(data.function_definitions, arg);
                    if (funcVar == nullptr) {
                        funcVar = data.getValueIfKeyExists(data.definitions, arg);
                        if (funcVar != nullptr) {
                            XOut << getTag(data) << ' ' << "found definition for: " << Rules::Input::quote(data.current_id) << std::endl;
                        }
                    } else {
                        XOut << getTag(data) << ' ' << "found function argument definition for: " << Rules::Input::quote(data.current_id) << std::endl;
                    }
                } else {
                    XOut << getTag(data) << ' ' << "expanding_function false" << std::endl;
                    funcVar = data.getValueIfKeyExists(data.definitions, arg);
                    if (funcVar != nullptr) {
                        XOut << getTag(data) << ' ' << "found definition for: " << Rules::Input::quote(data.current_id) << std::endl;
                    }
                }
                if (funcVar == nullptr) {
                    XOut << getTag(data, true) << ' ' << "could not find definition for function argument " << Rules::Input::quote(arg) << std::endl;
                } else {
                    XOut << getTag(data, true) << ' ' << "calling function_name->match(replacement)" << std::endl;
                    CPP_Preprocessor_Data old = data;
                    std::string replacement = std::string(arg);
                    function_name->match(replacement);
                    data = old;
                    XOut << getTag(data, true) << ' ' << "called function_name->match(replacement)" << std::endl;
                    funcVar = &data.definitions[arg];
                    var = var == nullptr ? nullptr : &data.definitions[data.current_id];
                    XOut << getTag(data, true) << ' ' << "function argument: " << in.quote(arg) << " for macro : " << Rules::Input::quote(data.current_id) << std::endl;
                    XOut << getTag(data, true) << ' ' << "function argument replacement: " << Rules::Input::quote(replacement) << " for macro : " << Rules::Input::quote(data.current_id) << std::endl;
                    if (var == nullptr) {
                        XOut << getTag(data) << ' ' << "macro is not being expanded: " << Rules::Input::quote(data.current_id) << std::endl;
                        in.replace(replacement);
                    } else if (var->type == var->Object) {
                        XOut << getTag(data) << ' ' << "object-like macro is being expanded: " << Rules::Input::quote(data.current_id) << std::endl;
                        in.replace(replacement);
                    } else {
                        for (auto &item : data.do_not_expand) {
                            if (item == data.current_id) {
                                XOut << getTag(data) << ' ' << "function-like macro is not being expanded: " << Rules::Input::quote(data.current_id) << std::endl;
                                in.replace(replacement);
                                return;
                            }
                        }
                        if (data.expanding_function) {
                            XOut << getTag(data) << ' ' << "expanding_function true, function-like macro is being expanded: " << Rules::Input::quote(data.current_id) << std::endl;
                        }
                        XOut << getTag(data) << ' ' << "expanding_function false, function-like macro is being expanded: " << Rules::Input::quote(data.current_id) << std::endl;
                    }
                }
                if (var != nullptr) {
                    if (var->type == CPP_Preprocessor_Data::Macro::Function) {
                        XOut << getTag(data, true) << ' ' << "found function argument " << Rules::Input::quote(arg) << " for function-like macro : " << Rules::Input::quote(data.current_id) << std::endl;
                        if (data.expansion_state == CPP_Preprocessor_Data::preprocessor_expansion_state_t::argument_count) {
                            var->count++;
                        } else if (data.expansion_state == CPP_Preprocessor_Data::preprocessor_expansion_state_t::expansion) {
                            XOut << getTag(data, true) << ' ' << "var id     : " << in.quote(var->id) << std::endl;
                            XOut << getTag(data, true) << ' ' << "var content: " << in.quote(var->content) << std::endl;
                            XOut << getTag(data, true) << ' ' << "calling function_name->match(replacement)" << std::endl;

                            CPP_Preprocessor_Data old = data;
                            std::string replacement = std::string(in.string());
                            function_name->match(replacement);
                            data = old;
                            var = &data.definitions[data.current_id];
                            XOut << getTag(data, true) << ' ' << "called function_name->match(replacement)" << std::endl;
                            XOut << getTag(data, true) << ' ' << "function argument: " << in.quote(var->args[var->count]) << " for function-like macro : " << Rules::Input::quote(data.current_id) << std::endl;
                            XOut << getTag(data, true) << ' ' << "function argument replacement: " << Rules::Input::quote(replacement) << " for function-like macro : " << Rules::Input::quote(data.current_id) << std::endl;
                            var->args_content.push_back(replacement);
                            var->count++;
                        }
                    }
                }
            });

            auto function_call_arg_and_optional_comma = new Rules::Sequence({
                function_call_arg,
                new Rules::Optional(comma)
            });

            // use rescan to alternate between action modes without duplicating grammar

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
            });

            auto statement = new Rules::Or({
                newline,
                whitespaces,
                function_call,
                function_name,
                new Rules::Any()
            });

            auto statements = new Rules::OneOrMore(statement);

            function_call->action = [&](Rules::Input in) {
                XOut << getTag(data, true) << ' ' << "function: " << in.quotedString() << std::endl;
                std::string function_name = Rules::Input::quote(data.current_id);
                auto * var = data.getValueIfKeyExists(data.definitions, data.current_id);
                if (var == nullptr) {
                    XOut << getTag(data) << ' ' << "no macro defined for function: " << function_name << std::endl;
                } else {
                    if (var->type == CPP_Preprocessor_Data::Macro::Object) {
                        XOut << getTag(data) << ' ' << "object-like macro defined for function as result of macro-expansion but no function-like macro defined: " << function_name << std::endl;
                        if (data.expansion_state == CPP_Preprocessor_Data::preprocessor_expansion_state_t::argument_count) {
                            data.expansion_state = CPP_Preprocessor_Data::preprocessor_expansion_state_t::expansion;
                            XOut << getTag(data, false) << ' ' << "rescanning for expansion phase" << std::endl;
                            in.rescan();
                        } else if (data.expansion_state == CPP_Preprocessor_Data::preprocessor_expansion_state_t::expansion) {
                        }
                    } else {
                        if (data.expansion_state == CPP_Preprocessor_Data::preprocessor_expansion_state_t::argument_count) {
                            XOut << getTag(data, var->type == var->Function) << ' ' << "rescanning for expansion phase" << std::endl;
                            data.expansion_state = CPP_Preprocessor_Data::preprocessor_expansion_state_t::expansion;
                            in.rescan();
                        } else if (data.expansion_state == CPP_Preprocessor_Data::preprocessor_expansion_state_t::expansion) {
                            XOut << getTag(data, true) << ' ' << "function-like macro defined for function name: " << function_name << std::endl;
                            XOut << getTag(data, true) << ' ' << "expanding function body with function parameters and defines" << std::endl;
                            CPP_Preprocessor_Data old = data;
                            std::string replacement = std::string(var->content);
                            for (int i = 0; i < var->args.size(); ++i) {
                                auto &arg = var->args[i];
                                auto & m = data.function_definitions[arg];
                                m.id = arg;
                                m.content = var->args_content[i];
                                m.type = m.Object;
                            }
                            data.expanding_function = true;

                            XOut << getTag(data, true) << ' ' << "adding macro to do-not-expand list: " << function_name << std::endl;
                            XOut << getTag(data, true) << ' ' << "scanning function body: " << Rules::Input::quote(replacement) << std::endl;
                            XOut << getTag(data, true) << ' ' << "calling statements->match(replacement)" << std::endl;


                            data.do_not_expand.push_back(var->id);

                            data.expansion_state = data.no_expansion_state;
                            statements->match(replacement);

                            data = old;
                            XOut << getTag(data, true) << ' ' << "called statements->match(replacement)" << std::endl;
                            XOut << getTag(data, true) << ' ' << "removed macro from do-not-expand list: " << function_name << std::endl;
                            XOut << getTag(data, true) << ' ' << "appending expanded function body: " << Rules::Input::quote(replacement) << std::endl;
                            data.expansion_state = data.no_expansion_state;
                            in.replaceAndRescan(replacement);
//                            in.replace(replacement);
                        }
                    }
                }
            };

            auto grammar = Rules::OneOrMore(
                new Rules::Or({
                    preprocessor_directive,
                    statement,
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