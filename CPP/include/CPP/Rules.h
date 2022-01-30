#include <functional>
#include <string>
#include <vector>
#include "IteratorMatcher.h"
#include <XLog/XLog.h>

namespace CPP {
    namespace Rules {
        class Input {
            Iterator<std::string> &iterator;
            IteratorMatcher::MatchData & match;
            int pops;
            bool executed = false;

        public:
            Input(Iterator<std::string> &iterator, IteratorMatcher::MatchData &match, int pops) : iterator(iterator), match(match), pops(pops) {}

            std::string string() {
                return iterator.substr(match.begin, match.end);
            }

            std::string stringRemoveCharactersFromEnd(int tail) {
                return iterator.substr(match.begin, match.end - tail);
            }

            std::string stringRemoveCharactersFromStart(int head) {
                return iterator.substr(match.begin + head, match.end);
            }

            std::string stringRemoveCharactersFromStartAndEnd(int head, int tail) {
                return iterator.substr(match.begin + head, match.end - tail);
            }

            void eraseAndRescan() {
                if (executed) {
                    throw new std::runtime_error("cannot modify input more than once in the same rule");
                }
                executed = true;
                auto savePoint1 = iterator.save();
                auto savePoint2 = iterator.save(match.begin);
                auto savePoint3 = iterator.save(match.end);
                iterator.input.erase(match.begin, match.end);
                iterator.load(savePoint1);
                iterator.load(savePoint2, match.begin);
                iterator.load(savePoint3, match.end);
                iterator.popIterator(pops);
            }

            void replaceAndRescan(const char & character) {
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
                iterator.popIterator(pops);
            }

            void replaceAndRescan(const std::string & string) {
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
                iterator.popIterator(pops);
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
        };

        using Action = std::function<void(Input input)>;

        extern Action NO_ACTION;

        // an empty rule, this matches nothing
        struct Rule {
        private:
            Rule * copy = nullptr;
        public:
            Action action;

            Rule(Action action = NO_ACTION) : action(action) {}
            Rule(Rule * copy, Action action = NO_ACTION) : copy(copy), action(action) {}

            IteratorMatcher::MatchData match(std::string & string, bool doAction = true) {
                Iterator<std::string> iterator(string);
                return match(iterator, doAction);
            };

            virtual IteratorMatcher::MatchData match(Iterator<std::string> &iterator, bool doAction = true) {
                IteratorMatcher::MatchData match;
                match.matched = false;
                match.begin = iterator.current();
                match.end = iterator.current();
                if (copy != nullptr) {
                    IteratorMatcher::MatchData tmp = copy->match(iterator, doAction);
                    if (tmp) {
                        match.matched = true;
                        match.end = tmp.end;
                        match.matches = tmp.matches;
                        if (doAction) action(Input(iterator, match, match.matches));
                        return match;
                    }
                    iterator.popIterator(tmp.matches);
                }
                if (doAction) action(Input(iterator, match, match.matches));
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
                match.end = iterator.current();
                match.matches = 1;
                if (doAction) action(Input(iterator, match, match.matches));
                return match;
            }
        };

        struct AdvanceInputBy : Rule {
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
                iterator.advance(n);
                match.end = iterator.current();
                match.matches = 1;
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
                IteratorMatcher::MatchData match;
                match.matched = false;
                match.begin = iterator.current();
                match.end = iterator.current();
                auto tmp = IteratorMatcher::match(iterator);
                if (tmp.matched) {
                    match.matched = true;
                    match.end = tmp.end;
                    match.matches++;
                    if (doAction) action(Input(iterator, match, match.matches));
                    return match;
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
                if (match.matched) {
                    match.matches++;
                    if (doAction) action(Input(iterator, match, match.matches));
                    return match;
                }
                return false;
            }
        };

        struct String : Rule {
            std::string string;

            String(const std::string &string, Action action = NO_ACTION) : string(string), Rule(action) {}

            using Rule::match;

            virtual IteratorMatcher::MatchData match(Iterator<std::string> &iterator, bool doAction = true) override {
                auto match = IteratorMatcher::match(iterator, string);
                if (match.matched) {
                    match.matches++;
                    if (doAction) action(Input(iterator, match, match.matches));
                    return match;
                }
                return false;
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
                return rule->match(iterator, doAction);
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
        public:
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

        struct OneOrMore : RuleHolder {

            OneOrMore(Rule * rule, Action action = NO_ACTION) : RuleHolder(rule, action) {}

            using Rule::match;

            virtual IteratorMatcher::MatchData match(Iterator<std::string> &iterator, bool doAction = true) override {
                IteratorMatcher::MatchData match = rule->match(iterator, doAction);
                if (match) {
                    match.matches++;
                    while (true) {
                        IteratorMatcher::MatchData tmp = rule->match(iterator, doAction);
                        if (!tmp) break;
                        match.end = tmp.end;
                        match.matches += tmp.matches;
                    }
                    if (doAction) action(Input(iterator, match, match.matches));
                    return true;
                }

                iterator.popIterator(match.matches);
                return false;
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
                for (Rule & rule : rules) {
                    IteratorMatcher::MatchData match = rule.match(iterator, doAction);
                    if (match) {
                        if (doAction) action(Input(iterator, match, match.matches));
                        return match;
                    }
                }
                return false;
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
                bool first = true;
                for (Rule & rule : rules) {
                    IteratorMatcher::MatchData tmp = rule.match(iterator, doAction);
                    if (!tmp) {
                        iterator.popIterator(match.matches);
                        return false;
                    }
                    if (first) {
                        first = false;
                        match = tmp;
                    }
                    match.end = tmp.end;
                    match.matches += tmp.matches;
                }
                if (doAction) action(Input(iterator, match, match.matches));
                return match;
            }
        };

        struct Until : RuleHolder {

            Until(Rule * rule, Action action = NO_ACTION) : RuleHolder(rule, action) {}

            using Rule::match;

            virtual IteratorMatcher::MatchData match(Iterator<std::string> &iterator, bool doAction = true) override {
                IteratorMatcher::MatchData match;
                match.matched = true;
                match.begin = iterator.current();
                match.end = iterator.current();
                while(iterator.has_next()) {
                    IteratorMatcher::MatchData tmp = rule->match(iterator, doAction);
                    if (tmp) {
                        match.end = tmp.end;
                        match.matches += tmp.matches;
                        if (doAction) action(Input(iterator, match, match.matches));
                        return match;
                    } else {
                        iterator.advance();
                    }
                }
                iterator.setCurrent(match.begin);
                return false;
            }
        };

        struct Optional : RuleHolder {

            Optional(Rule * rule, Action action = NO_ACTION) : RuleHolder(rule, action) {}

            using Rule::match;

            virtual IteratorMatcher::MatchData match(Iterator<std::string> &iterator, bool doAction = true) override {
                IteratorMatcher::MatchData match;
                match.matched = true;
                match.begin = iterator.current();
                match.end = iterator.current();
                IteratorMatcher::MatchData tmp = rule->match(iterator, doAction);
                if (tmp) {
                    match.end = tmp.end;
                    match.matches = tmp.matches;
                    if (doAction) action(Input(iterator, match, match.matches));
                    return match;
                }
                iterator.popIterator(tmp.matches);
                if (doAction) action(Input(iterator, match, match.matches));
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
                IteratorMatcher::MatchData matchData;
                matchData.begin = iterator.current();
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
                    matchData.end = iterator.current();
                    matchData.matched = true;
                    matchData.matches++;
                    return matchData;
                }
                // input does not match
                iterator.popIterator();
                if (doAction) action(Input(iterator, matchData, matchData.matches));
                return matchData;
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
                if (tmp) {
                    match.matched = true;
                    iterator.setCurrent(match.begin);
                    if (doAction) action(Input(iterator, match, match.matches));
                    return match;
                }
                match.matched = false;
                match.end = tmp.end;
                match.matches = tmp.matches;
                iterator.popIterator();
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
                if (tmp) {
                    match.matched = false;
                    match.end = tmp.end;
                    match.matches = tmp.matches;
                    iterator.popIterator();
                    return match;
                }
                match.matched = true;
                iterator.setCurrent(match.begin);
                if (doAction) action(Input(iterator, match, match.matches));
                return match;
            }
        };
    }
}