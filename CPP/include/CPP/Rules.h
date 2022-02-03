#ifndef CPP_RULES_H
#define CPP_RULES_H

#include <functional>
#include <string>
#include <vector>
#include "IteratorMatcher.h"
#include <XLog/XLog.h>

#define CPP_Rules_LogCapture1(rule, custom_name) new CPP::Rules::LogCapture(rule, custom_name)
#define CPP_Rules_LogCapture(rule) CPP_Rules_LogCapture1(rule, #rule)
#define Rules_NS_LogCapture1(rule, custom_name) new Rules::LogCapture(rule, custom_name)
#define Rules_NS_LogCapture(rule) Rules_NS_LogCapture1(rule, #rule)

#define CPP_Rules_LogCapture1_A(rule, custom_name, action) new CPP::Rules::LogCapture(rule, custom_name, action)
#define CPP_Rules_LogCapture_A(rule, action) CPP_Rules_LogCapture1(rule, #rule, action)
#define Rules_NS_LogCapture1_A(rule, custom_name, action) new Rules::LogCapture(rule, custom_name, action)
#define Rules_NS_LogCapture_A(rule, action) Rules_NS_LogCapture1(rule, #rule, action)

#define CPP_Rules_LogInput1(rule, custom_name) new CPP::Rules::LogInput(rule, custom_name)
#define CPP_Rules_LogInput(rule) CPP_Rules_LogInput1(rule, #rule)
#define Rules_NS_LogInput1(rule, custom_name) new Rules::LogInput(rule, custom_name)
#define Rules_NS_LogInput(rule) Rules_NS_LogInput1(rule, #rule)

#define CPP_Rules_LogInput1_A(rule, custom_name, action) new CPP::Rules::LogInput(rule, custom_name, action)
#define CPP_Rules_LogInput_A(rule, action) CPP_Rules_LogInput1_A(rule, #rule, action)
#define Rules_NS_LogInput1_A(rule, custom_name, action) new Rules::LogInput(rule, custom_name, action)
#define Rules_NS_LogInput_A(rule, action) Rules_NS_LogInput1_A(rule, #rule, action)



#define CPP_Rules_LogCapture1_NO_ALLOC(rule, custom_name) CPP::Rules::LogCapture(rule, custom_name)
#define CPP_Rules_LogCapture_NO_ALLOC(rule) CPP_Rules_LogCapture1_NO_ALLOC(rule, #rule)
#define Rules_NS_LogCapture1_NO_ALLOC(rule, custom_name) Rules::LogCapture(rule, custom_name)
#define Rules_NS_LogCapture_NO_ALLOC(rule) Rules_NS_LogCapture1_NO_ALLOC(rule, #rule)

#define CPP_Rules_LogCapture1_A_NO_ALLOC(rule, custom_name, action) CPP::Rules::LogCapture(rule, custom_name, action)
#define CPP_Rules_LogCapture_A_NO_ALLOC(rule, action) CPP_Rules_LogCapture1_NO_ALLOC(rule, #rule, action)
#define Rules_NS_LogCapture1_A_NO_ALLOC(rule, custom_name, action) Rules::LogCapture(rule, custom_name, action)
#define Rules_NS_LogCapture_A_NO_ALLOC(rule, action) Rules_NS_LogCapture1_NO_ALLOC(rule, #rule, action)

#define CPP_Rules_LogInput1_NO_ALLOC(rule, custom_name) CPP::Rules::LogInput(rule, custom_name)
#define CPP_Rules_LogInput_NO_ALLOC(rule) CPP_Rules_LogInput1_NO_ALLOC(rule, #rule)
#define Rules_NS_LogInput1_NO_ALLOC(rule, custom_name) Rules::LogInput(rule, custom_name)
#define Rules_NS_LogInput_NO_ALLOC(rule) Rules_NS_LogInput1_NO_ALLOC(rule, #rule)

#define CPP_Rules_LogInput1_A_NO_ALLOC(rule, custom_name, action) CPP::Rules::LogInput(rule, custom_name, action)
#define CPP_Rules_LogInput_A_NO_ALLOC(rule, action) CPP_Rules_LogInput1_A_NO_ALLOC(rule, #rule, action)
#define Rules_NS_LogInput1_A_NO_ALLOC(rule, custom_name, action) Rules::LogInput(rule, custom_name, action)
#define Rules_NS_LogInput_A_NO_ALLOC(rule, action) Rules_NS_LogInput1_A_NO_ALLOC(rule, #rule, action)

namespace CPP {
    namespace Rules {
        class Input {
#ifdef GTEST_API_
        public:
#endif
            Iterator<std::string> &iterator;
            IteratorMatcher::MatchData & match;
            int pops;
            bool executed = false;

        public:
            Input(Iterator<std::string> &iterator, IteratorMatcher::MatchData &match, int pops) : iterator(iterator), match(match), pops(pops) {}

            std::string string() {
                return iterator.substr(match.begin, match.end);
            }

            std::string quotedString(std::string quote = "'") {
                std::string out = quote;
                out += string();
                out += quote;
                return out;
            }

            static std::string quote(const std::string & string, std::string quote = "'") {
                std::string out = quote;
                out += string;
                out += quote;
                return out;
            }

            static std::string quote(const char & character, std::string quote = "'") {
                std::string out = quote;
                out += character;
                out += quote;
                return out;
            }

            std::string stringRemoveCharactersFromEnd(int tail) {
                return iterator.substr(match.begin, match.end - tail);
            }

            std::string quotedStringRemoveCharactersFromEnd(int tail, std::string quote = "'") {
                std::string out = quote;
                out += stringRemoveCharactersFromEnd(tail);
                out += quote;
                return out;
            }

            std::string stringRemoveCharactersFromStart(int head) {
                return iterator.substr(match.begin + head, match.end);
            }

            std::string quotedStringRemoveCharactersFromStart(int head, std::string quote = "'") {
                std::string out = quote;
                out += stringRemoveCharactersFromEnd(head);
                out += quote;
                return out;
            }

            std::string stringRemoveCharactersFromStartAndEnd(int head, int tail) {
                return iterator.substr(match.begin + head, match.end - tail);
            }

            std::string quotedStringRemoveCharactersFromStartAndEnd(int head, int tail, std::string quote = "'") {
                std::string out = quote;
                out += stringRemoveCharactersFromStartAndEnd(head, tail);
                out += quote;
                return out;
            }

            void rescan() {
                iterator.setCurrentToIteratorPops(pops);
            }

            void eraseAndRescan() {
                if (executed) {
                    throw new std::runtime_error("cannot modify input more than once in the same rule");
                }
                executed = true;
                auto savePoint1 = iterator.save();
                auto savePoint2 = iterator.save(match.begin);
                iterator.input.erase(match.begin, match.end);
                iterator.load(savePoint1);
                iterator.load(savePoint2, match.begin);
                match.end = match.begin;
                rescan();
            }

            void replace(const char & character) {
                if (executed) {
                    throw new std::runtime_error("cannot modify input more than once in the same rule");
                }
                executed = true;
                auto savePoint1 = iterator.save();
                auto savePoint2 = iterator.save(match.begin);
                auto savePoint3 = iterator.save(match.end);
                char string[2] = {character, '\0'};
                iterator.input.replace(match.begin, match.end, string);
                iterator.load(savePoint1);
                iterator.load(savePoint2, match.begin);
                iterator.load(savePoint3, match.end);
            }

#ifndef GTEST_API_
        private:
#endif
            void replace_(const std::string & string) {
                if (executed) {
                    throw new std::runtime_error("cannot modify input more than once in the same rule");
                }
                executed = true;
                auto savePoint1 = iterator.save();
                auto savePoint2 = iterator.save(match.begin);
                iterator.input.replace(match.begin, match.end, string);
                iterator.load(savePoint1);
                iterator.load(savePoint2, match.begin);
                match.end = match.begin + string.size();
            }

        public:
            void replace(const std::string & string) {
                replace_(string);
                iterator.setCurrent(match.end);
            }

            void replaceAndRescan(const char & character) {
                replace(character);
                rescan();
            }

            void replaceAndRescan(const std::string & string) {
                replace_(string);
                rescan();
            }

            void insert(const char & character) {
                if (executed) {
                    throw new std::runtime_error("cannot modify input more than once in the same rule");
                }
                executed = true;
                auto savePoint1 = iterator.save();
                auto savePoint2 = iterator.save(match.begin);
                auto savePoint3 = iterator.save(match.end);
                char string[2] = {character, '\0'};
                iterator.input.insert(iterator.currentPosition(match.end), string);
                iterator.load(savePoint1);
                iterator.load(savePoint2, match.begin);
                iterator.load(savePoint3, match.end);
                match.end++;
            }

            void insert(const std::string & string) {
                if (executed) {
                    throw new std::runtime_error("cannot modify input more than once in the same rule");
                }
                executed = true;
                auto savePoint1 = iterator.save();
                auto savePoint2 = iterator.save(match.begin);
                auto savePoint3 = iterator.save(match.end);
                iterator.input.insert(iterator.currentPosition(match.end), string.c_str());
                iterator.load(savePoint1);
                iterator.load(savePoint2, match.begin);
                iterator.load(savePoint3, match.end);
                match.end += string.size();
            }

            void insertAndRescan(const char & character) {
                insert(character);
                rescan();
            }

            void insertAndRescan(const std::string & string) {
                insert(string);
                rescan();
            }

            Iterator<std::string> &getIterator() const {
                return iterator;
            }
        };

        using Action = std::function<void(Input input)>;

        extern Action NO_ACTION;

        // an empty rule, this matches nothing
        struct Rule {
            Action action;

            Rule(Action action = NO_ACTION) : action(action) {}

            IteratorMatcher::MatchData match(std::string & string, bool doAction = true) {
                Iterator<std::string> iterator(string);
                return match(iterator, doAction);
            };

            virtual IteratorMatcher::MatchData match(Iterator<std::string> &iterator, bool doAction = true) {
                IteratorMatcher::MatchData match;
                match.matched = false;
                match.begin = iterator.current();
                match.end = iterator.current();
                return match;
            };

        };

        struct Success : Rule {

            Success(Action action = NO_ACTION) : Rule(action) {}

            using Rule::match;

            virtual IteratorMatcher::MatchData match(Iterator<std::string> &iterator, bool doAction = true) override {
                IteratorMatcher::MatchData match;
                match.matched = true;
                match.begin = iterator.current();
                iterator.pushIterator();
                match.matches++;
                match.end = iterator.current();
                if (doAction) action(Input(iterator, match, match.matches));
                return match;
            }
        };

        class AdvanceInputBy : public Rule {
#ifdef GTEST_API_
            public:
#endif
            int n;

        public:
            AdvanceInputBy(Action action = NO_ACTION) : n(1), Rule(action) {}
            AdvanceInputBy(int n, Action action = NO_ACTION) : n(n < 0 ? 0 : n), Rule(action) {}

            using Rule::match;

            virtual IteratorMatcher::MatchData match(Iterator<std::string> &iterator, bool doAction = true) override {
                IteratorMatcher::MatchData match;
                match.matched = true;
                match.begin = iterator.current();
                iterator.pushIterator();
                match.matches++;
                iterator.advance(n);
                match.end = iterator.current();
                if (doAction) action(Input(iterator, match, match.matches));
                return match;
            }
        };

        struct Fail : Rule {

            Fail(Action action = NO_ACTION) : Rule(action) {}

            using Rule::match;

            virtual IteratorMatcher::MatchData match(Iterator<std::string> &iterator, bool doAction = true) override {
                IteratorMatcher::MatchData match;
                match.matched = false;
                match.begin = iterator.current();
                match.end = iterator.current();
                if (doAction) action(Input(iterator, match, match.matches));
                return match;
            }
        };

        struct Any : Rule {
            Any(Action action = NO_ACTION) : Rule(action) {}

            using Rule::match;

            virtual IteratorMatcher::MatchData match(Iterator<std::string> &iterator, bool doAction = true) override {
                IteratorMatcher::MatchData match = IteratorMatcher::match(iterator);
                if (match && doAction) {
                    action(Input(iterator, match, match.matches));
                }
                return match;
            }
        };

        struct Char : Rule {
            char character;

            Char(char character, Action action = NO_ACTION) : character(character), Rule(action) {}

            using Rule::match;

            virtual IteratorMatcher::MatchData match(Iterator<std::string> &iterator, bool doAction = true) override {
                auto match = IteratorMatcher::match(iterator, character);
                if (match && doAction) {
                    action(Input(iterator, match, match.matches));
                }
                return match;
            }
        };

        struct EndOfFile : Rule {
            EndOfFile(Action action = NO_ACTION) : Rule(action) {}

            using Rule::match;

            virtual IteratorMatcher::MatchData match(Iterator<std::string> &iterator, bool doAction = true) override {
                IteratorMatcher::MatchData match;
                match.begin = iterator.current();
                match.end = iterator.current();
                match.matched = false;
                if (!iterator.has_next()) {
                    match.matched = true;
                    iterator.pushIterator();
                    match.matches++;
                    if (doAction) action(Input(iterator, match, match.matches));
                    return match;
                }

                return match;
            }
        };

        struct NewlineOrEOF : Rule {
            NewlineOrEOF(Action action = NO_ACTION) : Rule(action) {}

            using Rule::match;

            virtual IteratorMatcher::MatchData match(Iterator<std::string> &iterator, bool doAction = true) override {
                auto match = IteratorMatcher::match(iterator, '\n');

                if (match.matched) {
                    if (doAction) action(Input(iterator, match, match.matches));
                } else if (!iterator.has_next()) {
                    match.matched = true;
                    iterator.pushIterator();
                    match.matches++;
                    if (doAction) action(Input(iterator, match, match.matches));
                }

                return match;
            }
        };

        struct String : Rule {
            std::string string;

            String(const std::string &string, Action action = NO_ACTION) : string(string), Rule(action) {}

            using Rule::match;

            virtual IteratorMatcher::MatchData match(Iterator<std::string> &iterator, bool doAction = true) override {
                auto match = IteratorMatcher::match(iterator, string);

                if (match && doAction) {
                    action(Input(iterator, match, match.matches));
                }
                return match;
            }
        };

        struct Error : Rule {
            std::string message;

            Error(const std::string &message, Action action = NO_ACTION) : message(message), Rule(action) {}

            using Rule::match;

            virtual IteratorMatcher::MatchData match(Iterator<std::string> &iterator, bool doAction = true) override {
                IteratorMatcher::MatchData match;
                match.begin = iterator.current();
                match.end = iterator.current();
                match.matched = false;
                if (doAction) action(Input(iterator, match, 0));
                XOut << message << XLog::Abort;
                return match;
            }
        };

        struct RuleHolder : Rule {
            struct Reference {
                Rule * rule = nullptr;
                int reference = 0;
            };
            static std::vector<Reference*> rules;
            Reference * ref = nullptr;
            Rule * rule;

            RuleHolder(Rule *rule, Action action = NO_ACTION) : Rule(action) {
                bool exists = false;
                for (Reference * reference : rules) {
                    if (reference->rule == rule) {
                        ref = reference;
                        exists = true;
                        break;
                    }
                }
                if (!exists) {
                    ref = new Reference();
                    rules.push_back(ref);
                }
                ref->rule = rule;
                this->rule = ref->rule;
                ref->reference++;
            }

            RuleHolder(const RuleHolder & other) {
                ref = other.ref;
                rule = other.rule;
                ref->reference++;
            };

            RuleHolder & operator=(const RuleHolder & other) {
                ref = other.ref;
                rule = other.rule;
                ref->reference++;
                return *this;
            };

            RuleHolder(RuleHolder && other) noexcept {
                ref = other.ref;
                rule = other.rule;
                ref->reference++;
            }

            RuleHolder & operator=(RuleHolder && other)  noexcept {
                ref = other.ref;
                rule = other.rule;
                ref->reference++;
                return *this;
            }

            using Rule::match;

            virtual IteratorMatcher::MatchData match(Iterator<std::string> &iterator, bool doAction = true) override {
                IteratorMatcher::MatchData match = rule->match(iterator, doAction);
                if (match && doAction) {
                    action(Input(iterator, match, match.matches));
                }
                return match;
            }

            virtual ~RuleHolder() {
                if (ref != nullptr) {
                    if (ref->reference > 0) {
                        ref->reference--;
                        if (ref->reference == 0) {
                            std::vector<Reference*>::iterator r = rules.begin();
                            std::vector<Reference*>::const_iterator e = rules.cend();
                            while (r != e) {
                                if ((*r)->rule == ref->rule) {
                                    break;
                                }
                                r++;
                            }
                            delete ref->rule;
                            delete ref;
                            rules.erase(r);
                        }
                    }
                }
            }
        };

        struct TemporaryAction : RuleHolder {
            using RuleHolder::RuleHolder;

            using Rule::match;

            virtual IteratorMatcher::MatchData match(Iterator<std::string> &iterator, bool doAction = true) override {
                IteratorMatcher::MatchData match = rule->match(iterator, false);
                if (match && doAction) {
                    action(Input(iterator, match, match.matches));
                }
                return match;
            };

        };

        struct LogCurrentCharacter : Rule {

            LogCurrentCharacter(Action action = NO_ACTION) : Rule(action) {}

            using Rule::match;

            virtual IteratorMatcher::MatchData match(Iterator<std::string> &iterator, bool doAction = true) override {
                IteratorMatcher::MatchData match;
                match.begin = iterator.current();
                match.matched = true;
                iterator.pushIterator();
                match.matches++;
                match.end = iterator.current();
                XOut << "current character: " << Input::quote(iterator.peekNext()) << std::endl;
                if (doAction) action(Input(iterator, match, match.matches));
                return match;
            }
        };

        class LogMatchStatus : public RuleHolder {
#ifdef GTEST_API_
            public:
#endif
            std::string ruleName;
        public:
            LogMatchStatus(Rule *rule, const std::string & ruleName, Action action = NO_ACTION) : ruleName(ruleName), RuleHolder(rule, action) {}

            using Rule::match;

            virtual IteratorMatcher::MatchData match(Iterator<std::string> &iterator, bool doAction = true) override {
                IteratorMatcher::MatchData match = rule->match(iterator, doAction);
                XOut << "rule '" << ruleName << "' was " << (match ? "matched" : "not matched") << std::endl;
                if (doAction) action(Input(iterator, match, match.matches));
                return match;
            };

        };

        class LogCapture : public RuleHolder {
#ifdef GTEST_API_
            public:
#endif
            std::string ruleName;
        public:
            LogCapture(Rule *rule, const std::string & ruleName, Action action = NO_ACTION) : ruleName(ruleName), RuleHolder(rule, action) {}

            using Rule::match;

            virtual IteratorMatcher::MatchData match(Iterator<std::string> &iterator, bool doAction = true) override {
                IteratorMatcher::MatchData match = rule->match(iterator, doAction);
                if (match) {
                    XOut << "rule '" << ruleName << "' captured " << Input(iterator, match, 0).quotedString() << std::endl;
                } else {
                    XOut << "rule '" << ruleName << "' did not capture anything because it did not match" << std::endl;
                }
                if (doAction) action(Input(iterator, match, match.matches));
                return match;
            };

        };

        class LogInput : public RuleHolder {
#ifdef GTEST_API_
            public:
#endif
            std::string ruleName;
        public:
            LogInput(Rule *rule, const std::string & ruleName, Action action = NO_ACTION) : ruleName(ruleName), RuleHolder(rule, action) {}

            using Rule::match;

            virtual IteratorMatcher::MatchData match(Iterator<std::string> &iterator, bool doAction = true) override {
                IteratorMatcher::MatchData match = rule->match(iterator, doAction);
                XOut << "input after rule '" << ruleName << "' : " << iterator.currentString() << std::endl;
                if (doAction) action(Input(iterator, match, match.matches));
                return match;
            };

        };

        class ErrorIfMatch : public RuleHolder {
#ifdef GTEST_API_
            public:
#endif
            std::string message;
        public:
            ErrorIfMatch(Rule *rule, const std::string & message, Action action = NO_ACTION) : message(message), RuleHolder(rule, action) {}

            using Rule::match;

            virtual IteratorMatcher::MatchData match(Iterator<std::string> &iterator, bool doAction = true) override {
                IteratorMatcher::MatchData match = rule->match(iterator, doAction);
                if (match) {
                    if (doAction) action(Input(iterator, match, 0));
                    XOut << message << XLog::Abort;
                }
                return match;
            };

        };

        class ErrorIfNotMatch : public RuleHolder {
#ifdef GTEST_API_
            public:
#endif
            std::string message;
        public:
            ErrorIfNotMatch(Rule *rule, const std::string & message, Action action = NO_ACTION) : message(message), RuleHolder(rule, action) {}

            using Rule::match;

            virtual IteratorMatcher::MatchData match(Iterator<std::string> &iterator, bool doAction = true) override {
                IteratorMatcher::MatchData match = rule->match(iterator, doAction);
                if (!match) {
                    if (doAction) action(Input(iterator, match, 0));
                    XOut << message << XLog::Abort;
                }
                return match;
            };

        };

        struct Optional : RuleHolder {

            Optional(Rule * rule, Action action = NO_ACTION) : RuleHolder(rule, action) {}

            using Rule::match;

            virtual IteratorMatcher::MatchData match(Iterator<std::string> &iterator, bool doAction = true) override {
                IteratorMatcher::MatchData match;
                match.begin = iterator.current();
                match.end = iterator.current();
                IteratorMatcher::MatchData tmp = rule->match(iterator, doAction);
                if (tmp) {
                    match.end = tmp.end;
                    match.matches = tmp.matches;
                }
                match.matched = true;
                iterator.pushIterator();
                match.matches++;
                if (doAction) action(Input(iterator, match, match.matches));
                return match;
            }
        };

        struct OneOrMore : RuleHolder {

            OneOrMore(Rule * rule, Action action = NO_ACTION) : RuleHolder(rule, action) {}

            using Rule::match;

            virtual IteratorMatcher::MatchData match(Iterator<std::string> &iterator, bool doAction = true) override {
                IteratorMatcher::MatchData match = rule->match(iterator, doAction);
                if (match) {
                    while (true) {
                        IteratorMatcher::MatchData tmp = rule->match(iterator, doAction);
                        if (!tmp) break;
                        match.end = tmp.end;
                        match.matches += tmp.matches;
                    }
                    if (doAction) action(Input(iterator, match, match.matches));
                }

                return match;
            }
        };

        struct ZeroOrMore : RuleHolder {

            ZeroOrMore(Rule * rule, Action action = NO_ACTION) : RuleHolder(new Optional(new OneOrMore(rule)), action) {}

            using Rule::match;

            virtual IteratorMatcher::MatchData match(Iterator<std::string> &iterator, bool doAction = true) override {
                IteratorMatcher::MatchData match = rule->match(iterator, doAction);
                if (match && doAction) {
                    action(Input(iterator, match, match.matches));
                }
                return match;
            }
        };

        struct MatchBUntilA : Rule {
            RuleHolder A;
            RuleHolder B;

            MatchBUntilA(Rule * A, Rule * B, Action action = NO_ACTION) : A(A), B(B), Rule(action) {}

            using Rule::match;

            virtual IteratorMatcher::MatchData match(Iterator<std::string> &iterator, bool doAction = true) override {
                // until A matches, match B
                IteratorMatcher::MatchData match;
                match.begin = iterator.current();
                match.end = iterator.current();
                match.matched = false;
                while (true) {
                    IteratorMatcher::MatchData tmp = A.match(iterator, doAction);
                    if (!tmp) {
                        tmp = B.match(iterator, doAction);
                        if (!tmp) {
                            return match;
                        }
                        match.end = tmp.end;
                        match.matches += tmp.matches;
                    } else {
                        match.matched = true;
                        match.end = tmp.end;
                        match.matches += tmp.matches;
                        if (doAction) action(Input(iterator, match, match.matches));
                        return match;
                    }
                }
            }
        };

        struct Or : Rule {
            std::vector<RuleHolder> rules;

            Or(std::initializer_list<Rule*> rules, Action action = NO_ACTION) : Rule(action) {
                for (Rule * rule : rules) {
                    this->rules.push_back(rule);
                }
            }

            using Rule::match;

            virtual IteratorMatcher::MatchData match(Iterator<std::string> &iterator, bool doAction = true) override {
                IteratorMatcher::MatchData match;
                match.begin = iterator.current();
                match.end = iterator.current();
                match.matched = false;
                if (rules.size() == 0) {
                    match.matched = true;
                    iterator.pushIterator();
                    match.matches++;
                    if (doAction) action(Input(iterator, match, match.matches));
                    return match;
                }
                for (Rule & rule : rules) {
                    match = rule.match(iterator, doAction);
                    if (match) {
                        if (doAction) action(Input(iterator, match, match.matches));
                        return match;
                    }
                }
                return match;
            }
        };

        struct Sequence : Rule {
            std::vector<RuleHolder> rules;

            Sequence(std::initializer_list<Rule*> rules, Action action = NO_ACTION) : Rule(action) {
                for (Rule * rule : rules) {
                    this->rules.push_back(rule);
                }
            }

            using Rule::match;

            virtual IteratorMatcher::MatchData match(Iterator<std::string> &iterator, bool doAction = true) override {
                IteratorMatcher::MatchData match;
                match.begin = iterator.current();
                match.end = iterator.current();
                match.matched = false;
                if (rules.size() == 0) {
                    match.matched = true;
                    iterator.pushIterator();
                    match.matches++;
                    if (doAction) action(Input(iterator, match, match.matches));
                    return match;
                }
                for (Rule & rule : rules) {
                    IteratorMatcher::MatchData tmp = rule.match(iterator, doAction);
                    if (!tmp) {
                        iterator.popIterator(match.matches);
                        match.matches = 0;
                        return match;
                    }
//                    XOut << "match begin: " << iterator.currentPosition(match.begin) << std::endl;
//                    XOut << "match end: " << iterator.currentPosition(match.end) << std::endl;
//                    XOut << "tmp begin: " << iterator.currentPosition(tmp.begin) << std::endl;
//                    XOut << "tmp end: " << iterator.currentPosition(tmp.end) << std::endl;
                    match.end = tmp.end;
//                    XOut << "adding matches: " << tmp.matches << std::endl;
                    match.matches += tmp.matches;
                }
                match.matched = true;
                iterator.pushIterator();
                match.matches++;
                if (doAction) action(Input(iterator, match, match.matches));
                return match;
            }
        };

        struct Until : RuleHolder {

            Until(Rule * rule, Action action = NO_ACTION) : RuleHolder(rule, action) {}

            using Rule::match;

            virtual IteratorMatcher::MatchData match(Iterator<std::string> &iterator, bool doAction = true) override {
                IteratorMatcher::MatchData match;
                match.begin = iterator.current();
                match.end = iterator.current();
                match.matched = false;
                while(iterator.has_next()) {
                    IteratorMatcher::MatchData tmp = rule->match(iterator, doAction);
                    if (tmp) {
                        match.matched = true;
                        match.end = tmp.end;
                        match.matches = tmp.matches;
                        if (doAction) action(Input(iterator, match, match.matches));
                        return match;
                    } else {
                        iterator.advance();
                    }
                }
                iterator.setCurrent(match.begin);
                return match;
            }
        };

        struct Range : Rule {
            std::vector<char> letters;

            Range(std::initializer_list<char> letters, Action action = NO_ACTION) : Rule(action) {
                    for (char letter : letters) {
                        this->letters.push_back(letter);
                    }
            }

            using Rule::match;

            virtual IteratorMatcher::MatchData match(Iterator<std::string> &iterator, bool doAction = true) override {
                if (!iterator.has_next()) {
                    // unexpected EOF
                    return false;
                }
                IteratorMatcher::MatchData match;
                match.begin = iterator.current();
                iterator.pushIterator();
                char ch = iterator.next();
                Iterator l(letters);
                while (l.has_next()) {
                    char low = l.next();
                    if (ch >= low) {
                        if (l.has_next()) {
                            char high = l.next();
                            if (low != high && ch > high) {
                                continue;
                            }
                        }
                    } else {
                        l.advance();
                        continue;
                    }
                    match.end = iterator.current();
                    match.matched = true;
                    match.matches++;
                    if (doAction) action(Input(iterator, match, match.matches));
                    return match;
                }
                // input does not match
                iterator.popIterator();
                match.matches = 0;
                return match;
            }
        };

        struct At : RuleHolder {

            At(Rule * rule, Action action = NO_ACTION) : RuleHolder(rule, action) {}

            using Rule::match;

            virtual IteratorMatcher::MatchData match(Iterator<std::string> &iterator, bool doAction = true) override {
                IteratorMatcher::MatchData match;
                match.begin = iterator.current();
                match.end = iterator.current();
                IteratorMatcher::MatchData tmp = rule->match(iterator, false);
                iterator.popIterator(tmp.matches);
                tmp.matches = 0;
                if (tmp) {
                    match.matched = true;
                    iterator.pushIterator();
                    match.matches++;
                    if (doAction) action(Input(iterator, match, match.matches));
                    return match;
                }
                match.matched = false;
                return match;
            }
        };

        struct NotAt : RuleHolder {

            NotAt(Rule * rule, Action action = NO_ACTION) : RuleHolder(rule, action) {}

            using Rule::match;

            virtual IteratorMatcher::MatchData match(Iterator<std::string> &iterator, bool doAction = true) override {
                IteratorMatcher::MatchData match;
                match.begin = iterator.current();
                match.end = iterator.current();
                IteratorMatcher::MatchData tmp = rule->match(iterator, false);
                iterator.popIterator(tmp.matches);
                tmp.matches = 0;
                if (!tmp) {
                    match.matched = true;
                    iterator.pushIterator();
                    match.matches++;
                    if (doAction) action(Input(iterator, match, match.matches));
                    return match;
                }
                match.matched = false;
                return match;
            }
        };
    }
}

#endif