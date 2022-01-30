#include "Iterator.h"
namespace CPP {
    namespace IteratorMatcher {

        struct MatchData {
            bool matched;
            int matches = 0;
            std::string::const_iterator begin;
            std::string::const_iterator end;
            MatchData() : matched(false) {}
            MatchData(bool matched) : matched(matched) {}
            operator bool() {
                return matched;
            }
        };

        // match anything
        template<typename T>
        MatchData match(Iterator <T> & i) {
            if (!i.has_next()) {
                // unexpected EOF
                return false;
            }
            MatchData matchData;
            matchData.begin = i.current();
            i.pushIterator();
            i.advance();
            matchData.end = i.current();
            matchData.matched = true;
            return matchData;
        }

        // optimize for single character match
        template<typename T>
        MatchData match(Iterator <T> & i, char value) {
            if (!i.has_next()) {
                // unexpected EOF
                return false;
            }
            MatchData matchData;
            matchData.begin = i.current();
            i.pushIterator();
            if (i.next() == value) {
                matchData.end = i.current();
                matchData.matched = true;
                return matchData;
            }
            // input does not match
            i.popIterator();
            return false;
        }

        template<typename T>
        MatchData match(Iterator <T> & i, const std::string &value) {
            if (value.size() == 0) {
                // match EOF if input is empty
                return !i.has_next();
            }
            if (!i.has_next()) {
                // unexpected EOF
                return false;
            }
            MatchData matchData;
            matchData.begin = i.current();
            i.pushIterator();
            // optimize for single character matches and double character matches
            if (value.size() == 1) {
                if (i.next() == value[0]) {
                    matchData.end = i.current();
                    matchData.matched = true;
                    return matchData;
                }
                // input does not match
                i.popIterator();
                return false;
            }
            if (value.size() == 2) {
                if (i.next() == value[0]) {
                    if (i.next() == value[1]) {
                        matchData.end = i.current();
                        matchData.matched = true;
                        return matchData;
                    }
                }
                // input does not match
                i.popIterator();
                return false;
            }

            // value is longer than 2

            Iterator tmp(value);
            while (true) {
                if (!tmp.has_next()) {
                    // we have matched all input
                    matchData.end = i.current();
                    matchData.matched = true;
                    return matchData;
                }
                if (!i.has_next() && tmp.has_next()) {
                    // unexpected EOF
                    i.popIterator();
                    return false;
                }
                if (i.next() != tmp.next()) {
                    // input does not match
                    i.popIterator();
                    return false;
                }
            }
        }

        // optimize for single character match
        template<typename T>
        MatchData matchUntil(Iterator <T> & i, const char value) {
            MatchData matchData;
            matchData.begin = i.current();
            while (true) {
                auto match2 = IteratorMatcher::match(i, value);
                if (match2.matched) {
                    matchData.end = match2.end;
                    matchData.matched = match2.matched;
                    return matchData;
                } else {
                    if (i.has_next()) {
                        i.next();
                    } else {
                        i.popIterator();
                        return false;
                    }
                }
            }
        }

        template<typename T>
        MatchData matchUntil(Iterator <T> & i, const std::string &value) {
            MatchData matchData;
            matchData.begin = i.current();
            while (true) {
                auto match2 = IteratorMatcher::match(i, value);
                if (match2.matched) {
                    matchData.end = match2.end;
                    matchData.matched = match2.matched;
                    return matchData;
                } else {
                    if (i.has_next()) {
                        i.next();
                    } else {
                        i.popIterator();
                        return false;
                    }
                }
            }
        }

        // optimize for single character match
        template<typename T>
        MatchData matchOneOrMore(Iterator <T> & i, const char value) {
            MatchData matchData;
            matchData.begin = i.current();
            i.pushIterator();
            auto match1 = IteratorMatcher::match(i, value);
            if (match1.matched) {
                int pops = 1;
                matchData.end = match1.end;
                matchData.matched = match1.matched;
                while (true) {
                    auto match2 = IteratorMatcher::match(i, value);
                    if (match2.matched) {
                        pops++;
                        matchData.end = match2.end;
                        matchData.matched = match2.matched;
                    } else {
                        std::string::const_iterator c = i.current();
                        for (int n = 0; n < pops; n++) {
                            i.popIterator();
                        }
                        i.setCurrent(c);
                        return matchData;
                    }
                }
            } else {
                i.popIterator();
                return false;
            }
        }

        template<typename T>
        MatchData matchOneOrMore(Iterator <T> & i, const std::string &value) {
            MatchData matchData;
            matchData.begin = i.current();
            i.pushIterator();
            auto match1 = IteratorMatcher::match(i, value);
            if (match1.matched) {
                int pops = 1;
                matchData.end = match1.end;
                matchData.matched = match1.matched;
                while (true) {
                    auto match2 = IteratorMatcher::match(i, value);
                    if (match2.matched) {
                        pops++;
                        matchData.end = match2.end;
                        matchData.matched = match2.matched;
                    } else {
                        std::string::const_iterator c = i.current();
                        for (int n = 0; n < pops; n++) {
                            i.popIterator();
                        }
                        i.setCurrent(c);
                        return matchData;
                    }
                }
            } else {
                i.popIterator();
                return false;
            }
        }
    }
}