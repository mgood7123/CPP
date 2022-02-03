#ifndef CPP_ITERATOR_MATCHER_H
#define CPP_ITERATOR_MATCHER_H

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
            operator bool() const noexcept {
                return matched;
            }
        };

        // match anything
        template<typename T>
        MatchData match(Iterator <T> & i) {
            MatchData matchData;
            matchData.begin = i.current();
            matchData.end = i.current();
            matchData.matched = false;
            if (!i.has_next()) {
                // unexpected EOF
                return matchData;
            }
            i.pushIterator();
            i.advance();
            matchData.end = i.current();
            matchData.matched = true;
            matchData.matches++;
            return matchData;
        }

        // optimize for single character match
        template<typename T>
        MatchData match(Iterator <T> & i, char value) {
            MatchData matchData;
            matchData.begin = i.current();
            matchData.end = i.current();
            matchData.matched = false;
            if (!i.has_next()) {
                // unexpected EOF
                return matchData;
            }
            i.pushIterator();
            if (i.next() == value) {
                matchData.end = i.current();
                matchData.matched = true;
                matchData.matches++;
                return matchData;
            }
            // input does not match
            i.popIterator();
            return matchData;
        }

        template<typename T>
        MatchData match(Iterator <T> & i, const std::string &value) {
            MatchData matchData;
            matchData.begin = i.current();
            matchData.end = i.current();
            matchData.matched = false;
            if (value.size() == 0) {
                // match EOF if input is empty
                matchData.matched = !i.has_next();
                return matchData;
            }
            if (!i.has_next()) {
                // unexpected EOF
                return matchData;
            }
            i.pushIterator();
            // optimize for single character matches and double character matches
            if (value.size() == 1) {
                if (i.next() == value[0]) {
                    matchData.end = i.current();
                    matchData.matched = true;
                    matchData.matches++;
                    return matchData;
                }
                // input does not match
                i.popIterator();
                return matchData;
            }
            if (value.size() == 2) {
                if (i.next() == value[0]) {
                    if (i.next() == value[1]) {
                        matchData.end = i.current();
                        matchData.matched = true;
                        matchData.matches++;
                        return matchData;
                    }
                }
                // input does not match
                i.popIterator();
                return matchData;
            }

            // value is longer than 2

            Iterator tmp(value);
            while (true) {
                if (!tmp.has_next()) {
                    // we have matched all input
                    matchData.end = i.current();
                    matchData.matched = true;
                    matchData.matches++;
                    return matchData;
                }
                if (!i.has_next() && tmp.has_next()) {
                    // unexpected EOF
                    i.popIterator();
                    return matchData;
                }
                if (i.next() != tmp.next()) {
                    // input does not match
                    i.popIterator();
                    return matchData;
                }
            }
        }
    }
}

#ifdef GTEST_API_
TEST(Initialization_IteratorMatcher_MatchData, test_initialization) {
    CPP::IteratorMatcher::MatchData matchData;
    EXPECT_FALSE(matchData.matched);
    EXPECT_FALSE(matchData);
    EXPECT_EQ(matchData.begin, std::string::const_iterator());
    EXPECT_EQ(matchData.end, std::string::const_iterator());
    EXPECT_EQ(matchData.matches, 0);
}

TEST(IteratorMatcher, test_01) {
    std::string a = "Hello World!";
    CPP::Iterator<std::string> b(a);
    CPP::IteratorMatcher::MatchData matchData = CPP::IteratorMatcher::match(b);
    EXPECT_TRUE(matchData.matched);
    EXPECT_TRUE(matchData);
    EXPECT_EQ(matchData.begin, b.cbegin());
    EXPECT_EQ(matchData.end, b.cbegin()+1);
    EXPECT_EQ(matchData.matches, 1);
    EXPECT_EQ(b.currentPosition(), 1);
    EXPECT_EQ(b.peekNext(), 'e');
}

TEST(IteratorMatcher, test_02) {
    std::string a = "Hello World!";
    CPP::Iterator<std::string> b(a);
    CPP::IteratorMatcher::MatchData matchData = CPP::IteratorMatcher::match(b, 'H');
    EXPECT_TRUE(matchData.matched);
    EXPECT_TRUE(matchData);
    EXPECT_EQ(matchData.begin, b.cbegin());
    EXPECT_EQ(matchData.end, b.cbegin()+1);
    EXPECT_EQ(matchData.matches, 1);
    EXPECT_EQ(b.currentPosition(), 1);
    EXPECT_EQ(b.peekNext(), 'e');
}

TEST(IteratorMatcher, test_03) {
    std::string a = "Hello World!";
    CPP::Iterator<std::string> b(a);
    CPP::IteratorMatcher::MatchData matchData = CPP::IteratorMatcher::match(b, 'H');
    CPP::IteratorMatcher::MatchData matchData2 = CPP::IteratorMatcher::match(b, 'e');
    EXPECT_TRUE(matchData2.matched);
    EXPECT_TRUE(matchData2);
    EXPECT_EQ(matchData2.begin, b.cbegin()+1);
    EXPECT_EQ(matchData2.end, b.cbegin()+2);
    EXPECT_EQ(matchData2.matches, 1);
    EXPECT_EQ(b.currentPosition(), 2);
    EXPECT_EQ(b.peekNext(), 'l');
}

TEST(IteratorMatcher, test_04) {
    std::string a = "Hello World!";
    CPP::Iterator<std::string> b(a);
    CPP::IteratorMatcher::MatchData matchData = CPP::IteratorMatcher::match(b, 'H');
    CPP::IteratorMatcher::MatchData matchData2 = CPP::IteratorMatcher::match(b, 'r');
    EXPECT_TRUE(matchData.matched);
    EXPECT_TRUE(matchData);
    EXPECT_EQ(matchData.begin, b.cbegin());
    EXPECT_EQ(matchData.end, b.cbegin()+1);
    EXPECT_EQ(matchData.matches, 1);
    EXPECT_FALSE(matchData2.matched);
    EXPECT_FALSE(matchData2);
    EXPECT_EQ(matchData2.begin, b.cbegin()+1);
    EXPECT_EQ(matchData2.end, b.cbegin()+1);
    EXPECT_EQ(matchData2.matches, 0);
    EXPECT_EQ(b.currentPosition(), 1);
    EXPECT_EQ(b.peekNext(), 'e');
}

TEST(IteratorMatcher_backtracking, test_01) {
    std::string a = "Hello World!";
    CPP::Iterator<std::string> b(a);
    b.pushIterator();
    CPP::IteratorMatcher::MatchData matchData = CPP::IteratorMatcher::match(b, 'H');
    CPP::IteratorMatcher::MatchData matchData2 = CPP::IteratorMatcher::match(b, 'r');
    b.popIterator();
    EXPECT_TRUE(matchData.matched);
    EXPECT_TRUE(matchData);
    EXPECT_EQ(matchData.begin, b.cbegin());
    EXPECT_EQ(matchData.end, b.cbegin()+1);
    EXPECT_EQ(matchData.matches, 1);
    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), 'H');
}

TEST(IteratorMatcher_backtracking, test_02) {
    std::string a = "Hello World!";
    CPP::Iterator<std::string> b(a);
    CPP::IteratorMatcher::MatchData matchData = CPP::IteratorMatcher::match(b, 'H');
    b.popIterator(matchData.matches);
    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), 'H');
}

#endif

#endif