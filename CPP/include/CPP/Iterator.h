#include <string>
#include <vector>

namespace CPP {
    template<typename T>
    class Iterator {
        std::string::const_iterator iteratorCurrent;
        std::vector <std::string::const_iterator> iteratorStack;
    public:
        T &input;

        Iterator(T &input) : iteratorCurrent(input.cbegin()), input(input){}

        bool has_next() {
            return iteratorCurrent < input.cend();
        }

        char next() {
            const char c = iteratorCurrent[0];
            iteratorCurrent++;
            return c;
        }

        char previous() {
            if (iteratorCurrent > input.cbegin()) {
                iteratorCurrent--;
            }
            return iteratorCurrent[0];
        }

        char peekPrevious() {
            if (iteratorCurrent > input.cbegin()) {
                return input[iteratorCurrent - 1];
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

        void advance() {
            iteratorCurrent++;
        }

        void advance(int n) {
            iteratorCurrent += n;
        }

        struct SaveState {
            uint64_t iteratorCurrent;
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
    };
}