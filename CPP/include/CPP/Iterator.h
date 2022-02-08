#ifndef CPP_ITERATOR
#define CPP_ITERATOR

#include <string>
#include <vector>
#include <XLog/XLog.h>
#include <optional>

// sub iterators
//
// parent: 12345
// child : 987
//
// child sub iterator of parent from '1' to '3'
// parent:                    1          3 4 5
//                            ^          ^ ^
// child :                    ^  9 8 7   ^ ^
//                            ^  ^ ^ ^ ^ ^ ^
// child begin----------------^--^ ^ ^ ^ ^ ^ // '9' from itself, begin returns the same as if next at begin
// child end------------------^--^-^-^-^ ^ ^ // END
// child previous at begin----^  ^ ^ ^   ^ ^ // '1' from parent
// child next at begin-----------^ ^ ^   ^ ^ // '9' from itself, next at begin must return begin
// child next----------------------^ ^   ^ ^ // '8' from itself
// child next------------------------^   ^ ^ // '7' from itself
// child next at end-----------------^---^ ^ // '3' from parent
// child next------------------------^---^-^ // '4' from parent, returns iterator and increases it
// child previous--------------------^---^-^ // '4' from parent, decreases iterator and returns it
// child previous--------------------^---^   // '3' from parent, decreases iterator and returns it
// child previous--------------------^       // '7' from itself, decreases iterator and returns it

namespace CPP {
    template<typename T>
    class Iterator {
#ifdef GTEST_API_
    public:
#endif
        std::string::const_iterator iteratorCurrent;
        std::vector <std::string::const_iterator> iteratorStack;
    public:
        T &input;

        Iterator(T &input) : iteratorCurrent(input.cbegin()), input(input){}

        Iterator copy() {
            Iterator iterator(input);
            iterator.iteratorCurrent = iteratorCurrent;
            // we require a fresh stack
            return iterator;
        };

        bool has_next() {
            return iteratorCurrent < input.cend();
        }

        char next() {
            const char c = iteratorCurrent[0];
            iteratorCurrent++;
            return c;
        }

        bool has_previous() {
            return iteratorCurrent > input.cbegin();
        }

        char previous() {
            if (iteratorCurrent > input.cbegin()) {
                iteratorCurrent--;
            }
            return iteratorCurrent[0];
        }

        char peekPrevious() {
            if (iteratorCurrent > input.cbegin()) {
                return input[(iteratorCurrent - 1) - input.cbegin()];
            }
            return iteratorCurrent[0];
        }

        char peekNext() {
            return iteratorCurrent[0];
        }

        std::string::const_iterator cbegin() const {
            return input.cbegin();
        }

        std::string::const_iterator current() const {
            return iteratorCurrent;
        }

        void setCurrent(std::string::const_iterator current) {
            iteratorCurrent = current;
        }

        uint64_t currentPosition() {
            return iteratorCurrent - input.cbegin();
        }

        uint64_t currentPosition(std::string::const_iterator iterator) {
            return iterator - input.cbegin();
        }

        std::string::const_iterator peekPreviousCurrent() const {
            if (iteratorCurrent > input.cbegin()) {
                return iteratorCurrent - 1;
            }
            return iteratorCurrent;
        }

        std::string::const_iterator peekNextCurrent() const {
            if (iteratorCurrent < input.cend()) {
                return iteratorCurrent + 1;
            }
            return iteratorCurrent;
        }

        std::string::const_iterator cend() const {
            return input.cend();
        }

        void reset() {
            iteratorCurrent = input.cbegin();
            iteratorStack.clear();
        }

        std::string substr(std::string::const_iterator begin, std::string::const_iterator end) {
            return input.substr(begin - input.cbegin(), end - begin);
        }

        void pushIterator() {
            iteratorStack.push_back(iteratorCurrent);
        }

        void pushIterator(std::string::const_iterator iterator) {
            iteratorStack.push_back(iterator);
        }

        void popIterator() {
            if (!iteratorStack.empty()) {
                iteratorCurrent = iteratorStack.back();
                iteratorStack.pop_back();
            }
        }

        void popIterator(int n) {
            for (int i = 0; i < n; ++i) {
                if (iteratorStack.empty()) break;
                iteratorCurrent = iteratorStack.back();
                iteratorStack.pop_back();
            }
        }

        void setCurrentToIteratorPops(int n) {
            if (iteratorStack.size() >= n) {
                iteratorCurrent = *(iteratorStack.cend() - n);
            } else {
                iteratorCurrent = cbegin();
            }
        }

        void advance() {
            iteratorCurrent++;
        }

        void advance(int n) {
            iteratorCurrent += n;
        }

        struct SaveState {
            uint64_t iteratorCurrent = 0;
            std::vector <uint64_t> iteratorStack;
        };

        SaveState save() {
            SaveState saveState;
            saveState.iteratorCurrent = iteratorCurrent - input.cbegin();
            for (auto &&item : iteratorStack) {
                saveState.iteratorStack.push_back(item - input.cbegin());
            }
            return saveState;
        }

        SaveState save(std::string::const_iterator iterator) {
            SaveState saveState;
            saveState.iteratorCurrent = iterator - input.cbegin();
            return saveState;
        }

        void load(SaveState & saveState) {
            iteratorCurrent = input.cbegin() + saveState.iteratorCurrent;
            iteratorStack.clear();
            for (auto &&item : saveState.iteratorStack) {
                iteratorStack.push_back(input.cbegin() + item);
            }
        }

        void load(SaveState & saveState, std::string::const_iterator & iterator) {
            iterator = input.cbegin() + saveState.iteratorCurrent;
        }

        std::string currentString() {
            return input.substr(currentPosition());
        }

        std::string currentString(std::string::const_iterator iterator) {
            return input.substr(currentPosition(iterator));
        }
    };

    template<typename T>
    class Iterator2 {
#ifdef GTEST_API_
    public:
#endif
        std::string::const_iterator iteratorCurrent;
        std::vector <std::string::const_iterator> iteratorStack;
        Iterator2 * parent;

        bool current_has_next() {
            return iteratorCurrent < input.cend();
        }

        bool parent_has_next() {
            return parent == nullptr ? false : parent->has_next();
        }

        bool current_has_previous() {
            return iteratorCurrent > input.cbegin();
        }

        bool parent_has_previous() {
            return parent == nullptr ? false : parent->has_previous();
        }

        int parentCount(int count) {
            if (parent == nullptr) {
                return count;
            }
            return parent->parentCount(count+1);
        }


    public:
        T &input;

        Iterator2(T &input) : iteratorCurrent(input.cbegin()), input(input), parent(nullptr) {}

        Iterator2(Iterator2 & parent, T &input) : iteratorCurrent(input.cbegin()), input(input), parent(parent) {}

        int parentCount() {
            if (parent == nullptr) {
                return 0;
            } else {
                return parent->parentCount(1);
            }
        }

        bool has_previous() {
            bool c = current_has_previous();
            return c ? c : parent_has_previous();
        }

        std::optional<char> previous() {
            if (current_has_previous()) {
                iteratorCurrent--;
                return iteratorCurrent[0];
            } else if (parent_has_previous()) {
                return parent->previous();
            } else return std::nullopt;
        }

        std::optional<char> peekPrevious() {
            if (current_has_previous()) {
                return input[(iteratorCurrent - 1) - input.cbegin()];
            } else if (parent_has_previous()) {
                return parent->peekPrevious();
            } else return std::nullopt;
        }

        bool has_next() {
            bool c = current_has_next();
            return c ? c : parent_has_next();
        }

        std::optional<char> next() {
            if (current_has_next()) {
                const char c = iteratorCurrent[0];
                iteratorCurrent++;
                return c;
            } else if (parent_has_next()) {
                return parent->next();
            } else return std::nullopt;
        }

        std::optional<char> peekNext() {
            if (current_has_next()) {
                return iteratorCurrent[0];
            } else if (parent_has_next()) {
                return parent->peekNext();
            } else return std::nullopt;
        }

        std::string::const_iterator cbegin() const {
            return input.cbegin();
        }

        std::string::const_iterator current() const {
            return iteratorCurrent;
        }

        void setCurrent(std::string::const_iterator current) {
            iteratorCurrent = current;
        }

        uint64_t currentPosition() {
            return iteratorCurrent - input.cbegin();
        }

        uint64_t currentPosition(std::string::const_iterator iterator) {
            return iterator - input.cbegin();
        }

        std::optional<std::string::const_iterator> peekPreviousCurrent() const {
            if (current_has_previous()) {
                return iteratorCurrent - 1;
            } else if (parent_has_previous()) {
                return parent->peekPreviousCurrent();
            } else return std::nullopt;
        }

        std::optional<std::string::const_iterator> peekNextCurrent() const {
            if (current_has_next()) {
                return iteratorCurrent + 1;
            } else if (parent_has_next()) {
                return parent->peekNextCurrent();
            } else return std::nullopt;
        }

        std::string::const_iterator cend() const {
            return input.cend();
        }

        void reset() {
            iteratorCurrent = input.cbegin();
            iteratorStack.clear();
        }

        std::string substr(std::string::const_iterator begin, std::string::const_iterator end) {
            return input.substr(begin - input.cbegin(), end - begin);
        }

        void pushIterator() {
            iteratorStack.push_back(iteratorCurrent);
        }

        void pushIterator(std::string::const_iterator iterator) {
            iteratorStack.push_back(iterator);
        }

        void popIterator() {
            if (!iteratorStack.empty()) {
                iteratorCurrent = iteratorStack.back();
                iteratorStack.pop_back();
            }
        }

        void popIterator(int n) {
            for (int i = 0; i < n; ++i) {
                if (iteratorStack.empty()) break;
                iteratorCurrent = iteratorStack.back();
                iteratorStack.pop_back();
            }
        }

        void setCurrentToIteratorPops(int n) {
            if (iteratorStack.size() > n) {
                iteratorCurrent = *(iteratorStack.cend() - n);
            } else {
                iteratorCurrent = cbegin();
            }
        }

        void advance() {
            iteratorCurrent++;
        }

        void advance(int n) {
            iteratorCurrent += n;
        }

        struct SaveState {
            uint64_t iteratorCurrent = 0;
            std::vector <uint64_t> iteratorStack;
        };

        SaveState save() {
            SaveState saveState;
            saveState.iteratorCurrent = iteratorCurrent - input.cbegin();
            for (auto &&item : iteratorStack) {
                saveState.iteratorStack.push_back(item - input.cbegin());
            }
            return saveState;
        }

        SaveState save(std::string::const_iterator iterator) {
            SaveState saveState;
            saveState.iteratorCurrent = iterator - input.cbegin();
            return saveState;
        }

        void load(SaveState & saveState) {
            iteratorCurrent = input.cbegin() + saveState.iteratorCurrent;
            iteratorStack.clear();
            for (auto &&item : saveState.iteratorStack) {
                iteratorStack.push_back(input.cbegin() + item);
            }
        }

        void load(SaveState & saveState, std::string::const_iterator & iterator) {
            iterator = input.cbegin() + saveState.iteratorCurrent;
        }

        std::string currentString() {
            return input.substr(currentPosition());
        }

        std::string currentString(std::string::const_iterator & iterator) {
            return input.substr(currentPosition(iterator));
        }
    };
}

#ifdef GTEST_API_
TEST(Initialization_Iterator, test_01) {
    std::string a = "Hello World!";
    CPP::Iterator<std::string> b(a);
    EXPECT_EQ(b.input, a);
    EXPECT_EQ(b.input.data(), a.data());
    EXPECT_EQ(b.input.c_str(), a.c_str());
    EXPECT_STREQ(b.input.c_str(), a.c_str());
    EXPECT_STRCASEEQ(b.input.c_str(), a.c_str());
    EXPECT_EQ(b.current(), a.cbegin());
    EXPECT_EQ(b.cbegin(), b.input.cbegin());
    EXPECT_EQ(b.cbegin(), a.cbegin());
    EXPECT_EQ(b.cend(), b.input.cend());
    EXPECT_EQ(b.cend(), a.cend());
    EXPECT_EQ(b.iteratorCurrent, b.input.cbegin());
    EXPECT_EQ(b.iteratorStack.size(), 0);
    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_TRUE(b.has_next());
    EXPECT_FALSE(b.has_previous());
}

TEST(Initialization_Iterator, test_02) {
    std::string a;
    CPP::Iterator<std::string> b(a);
    EXPECT_EQ(b.input, a);
    EXPECT_EQ(b.input.data(), a.data());
    EXPECT_EQ(b.input.c_str(), a.c_str());
    EXPECT_STREQ(b.input.c_str(), a.c_str());
    EXPECT_STRCASEEQ(b.input.c_str(), a.c_str());
    EXPECT_EQ(b.current(), a.cbegin());
    EXPECT_EQ(b.cbegin(), b.input.cbegin());
    EXPECT_EQ(b.cbegin(), a.cbegin());
    EXPECT_EQ(b.cend(), b.input.cend());
    EXPECT_EQ(b.cend(), a.cend());
    EXPECT_EQ(b.iteratorCurrent, b.input.cbegin());
    EXPECT_EQ(b.iteratorStack.size(), 0);
    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_FALSE(b.has_next());
    EXPECT_FALSE(b.has_previous());
}

TEST(Initialization_Iterator, test_03) {
    std::string a = "";
    CPP::Iterator<std::string> b(a);
    EXPECT_EQ(b.input, a);
    EXPECT_EQ(b.input.data(), a.data());
    EXPECT_EQ(b.input.c_str(), a.c_str());
    EXPECT_STREQ(b.input.c_str(), a.c_str());
    EXPECT_STRCASEEQ(b.input.c_str(), a.c_str());
    EXPECT_EQ(b.current(), a.cbegin());
    EXPECT_EQ(b.cbegin(), b.input.cbegin());
    EXPECT_EQ(b.cbegin(), a.cbegin());
    EXPECT_EQ(b.cend(), b.input.cend());
    EXPECT_EQ(b.cend(), a.cend());
    EXPECT_EQ(b.iteratorCurrent, b.input.cbegin());
    EXPECT_EQ(b.iteratorStack.size(), 0);
    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_FALSE(b.has_next());
    EXPECT_FALSE(b.has_previous());
}

TEST(Iterator, basic_test_01) {
    std::string a = "Hello World!";
    CPP::Iterator<std::string> b(a);
    EXPECT_EQ(b.current(), a.cbegin());
    EXPECT_FALSE(b.has_previous());
    EXPECT_TRUE(b.has_next());
    EXPECT_EQ(b.next(), 'H');
    EXPECT_EQ(b.current(), a.cbegin()+1);
    EXPECT_TRUE(b.has_previous());
    EXPECT_TRUE(b.has_next());
}

TEST(Iterator, basic_test_02) {
    std::string a;
    CPP::Iterator<std::string> b(a);
    EXPECT_FALSE(b.has_next());
    EXPECT_FALSE(b.has_previous());
    EXPECT_EQ(b.next(), '\0');
    EXPECT_EQ(b.current(), a.cbegin()+1);
    EXPECT_TRUE(b.has_previous());
    EXPECT_FALSE(b.has_next());
}

TEST(Iterator, basic_test_03) {
    std::string a = "";
    CPP::Iterator<std::string> b(a);
    EXPECT_FALSE(b.has_next());
    EXPECT_FALSE(b.has_previous());
    EXPECT_EQ(b.next(), '\0');
    EXPECT_EQ(b.current(), a.cbegin()+1);
    EXPECT_TRUE(b.has_previous());
    EXPECT_FALSE(b.has_next());
}

TEST(Iterator, basic_test_04) {
    std::string a = "Hello World!";
    CPP::Iterator<std::string> b(a);
    EXPECT_FALSE(b.has_previous());
    EXPECT_TRUE(b.has_next());
    EXPECT_EQ(b.next(), 'H');
    EXPECT_EQ(b.current(), a.cbegin()+1);
    EXPECT_TRUE(b.has_previous());
    EXPECT_EQ(b.previous(), 'H');
    EXPECT_EQ(b.current(), a.cbegin());
    EXPECT_TRUE(b.has_next());
}

TEST(Iterator, basic_test_05) {
    std::string a;
    CPP::Iterator<std::string> b(a);
    EXPECT_FALSE(b.has_next());
    EXPECT_FALSE(b.has_previous());
    EXPECT_EQ(b.next(), '\0');
    EXPECT_EQ(b.current(), a.cbegin()+1);
    EXPECT_TRUE(b.has_previous());
    EXPECT_EQ(b.previous(), '\0');
    EXPECT_EQ(b.current(), a.cbegin());
    EXPECT_FALSE(b.has_next());
}

TEST(Iterator, basic_test_06) {
    std::string a = "";
    CPP::Iterator<std::string> b(a);
    EXPECT_FALSE(b.has_next());
    EXPECT_FALSE(b.has_previous());
    EXPECT_EQ(b.next(), '\0');
    EXPECT_EQ(b.current(), a.cbegin()+1);
    EXPECT_TRUE(b.has_previous());
    EXPECT_EQ(b.previous(), '\0');
    EXPECT_EQ(b.current(), a.cbegin());
    EXPECT_FALSE(b.has_next());
}

TEST(Iterator, test_01) {
    std::string a = "12";
    CPP::Iterator<std::string> b(a);
    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), '1');
    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.next(), '1');

    EXPECT_EQ(b.currentPosition(), 1);
    EXPECT_EQ(b.peekNext(), '2');
    EXPECT_EQ(b.currentPosition(), 1);
    EXPECT_EQ(b.next(), '2');

    EXPECT_EQ(b.currentPosition(), 2);
    EXPECT_EQ(b.peekNext(), '\0');
    EXPECT_EQ(b.currentPosition(), 2);
    EXPECT_EQ(b.peekPrevious(), '2');
    EXPECT_EQ(b.currentPosition(), 2);
    EXPECT_EQ(b.previous(), '2');
    EXPECT_EQ(b.currentPosition(), 1);
    EXPECT_EQ(b.peekNext(), '2');
    EXPECT_EQ(b.currentPosition(), 1);
    EXPECT_EQ(b.peekPrevious(), '1');
    EXPECT_EQ(b.currentPosition(), 1);
    EXPECT_EQ(b.previous(), '1');
    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.peekNext(), '1');
    EXPECT_EQ(b.currentPosition(), 0);
}

TEST(Iterator_Stack, test_01) {
    std::string a = "12";
    CPP::Iterator<std::string> b(a);
    b.pushIterator();
    EXPECT_EQ(b.iteratorStack.size(), 1);
    EXPECT_EQ(b.currentPosition(), 0);
    b.advance();
    EXPECT_EQ(b.currentPosition(), 1);
    EXPECT_EQ(b.iteratorStack.size(), 1);
    b.popIterator();
    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.iteratorStack.size(), 0);
}

TEST(Iterator_SaveState, test_initialization_01) {
    CPP::Iterator<std::string>::SaveState saveState;
    EXPECT_EQ(saveState.iteratorCurrent, 0);
    EXPECT_EQ(saveState.iteratorStack.size(), 0);
}

TEST(Iterator_SaveState, test_initialization_02) {
    std::string a;
    CPP::Iterator<std::string> b(a);
    auto saveState = b.save();
    EXPECT_EQ(saveState.iteratorCurrent, 0);
    EXPECT_EQ(saveState.iteratorStack.size(), 0);
}

TEST(Iterator_SaveState, test_save) {
    std::string a;
    CPP::Iterator<std::string> b(a);
    auto saveState = b.save();
    EXPECT_EQ(saveState.iteratorCurrent, 0);
    EXPECT_EQ(saveState.iteratorStack.size(), 0);
    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.iteratorStack.size(), 0);
    b.pushIterator();
    b.advance();
    EXPECT_EQ(b.currentPosition(), 1);
    EXPECT_EQ(b.iteratorStack.size(), 1);
    EXPECT_EQ(b.iteratorStack[0], a.cbegin());
    b.pushIterator();
    b.advance();
    EXPECT_EQ(b.currentPosition(), 2);
    EXPECT_EQ(b.iteratorStack.size(), 2);
    EXPECT_EQ(b.iteratorStack[0], a.cbegin());
    EXPECT_EQ(b.iteratorStack[1], a.cbegin()+1);
    auto saveState2 = b.save();
    EXPECT_EQ(b.currentPosition(), 2);
    EXPECT_EQ(b.iteratorStack.size(), 2);
    EXPECT_EQ(b.iteratorStack[0], a.cbegin());
    EXPECT_EQ(b.iteratorStack[1], a.cbegin()+1);
    EXPECT_EQ(saveState.iteratorCurrent, 0);
    EXPECT_EQ(saveState.iteratorStack.size(), 0);
    EXPECT_EQ(saveState2.iteratorCurrent, 2);
    EXPECT_EQ(saveState2.iteratorStack.size(), 2);
    EXPECT_EQ(saveState2.iteratorStack[0], 0);
    EXPECT_EQ(saveState2.iteratorStack[1], 1);
    b.load(saveState);
    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.iteratorStack.size(), 0);
    EXPECT_EQ(saveState.iteratorCurrent, 0);
    EXPECT_EQ(saveState.iteratorStack.size(), 0);
    EXPECT_EQ(saveState2.iteratorCurrent, 2);
    EXPECT_EQ(saveState2.iteratorStack.size(), 2);
    EXPECT_EQ(saveState2.iteratorStack[0], 0);
    EXPECT_EQ(saveState2.iteratorStack[1], 1);
    b.load(saveState2);
    EXPECT_EQ(b.currentPosition(), 2);
    EXPECT_EQ(b.iteratorStack.size(), 2);
    EXPECT_EQ(b.iteratorStack[0], a.cbegin());
    EXPECT_EQ(b.iteratorStack[1], a.cbegin()+1);
    EXPECT_EQ(saveState.iteratorCurrent, 0);
    EXPECT_EQ(saveState.iteratorStack.size(), 0);
    EXPECT_EQ(saveState2.iteratorCurrent, 2);
    EXPECT_EQ(saveState2.iteratorStack.size(), 2);
    EXPECT_EQ(saveState2.iteratorStack[0], 0);
    EXPECT_EQ(saveState2.iteratorStack[1], 1);
    b.load(saveState);
    EXPECT_EQ(b.currentPosition(), 0);
    EXPECT_EQ(b.iteratorStack.size(), 0);
    EXPECT_EQ(saveState.iteratorCurrent, 0);
    EXPECT_EQ(saveState.iteratorStack.size(), 0);
    EXPECT_EQ(saveState2.iteratorCurrent, 2);
    EXPECT_EQ(saveState2.iteratorStack.size(), 2);
    EXPECT_EQ(saveState2.iteratorStack[0], 0);
    EXPECT_EQ(saveState2.iteratorStack[1], 1);
}

#endif

#endif