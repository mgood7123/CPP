#ifndef CPP_CPP_PREPROCESSOR_DATA_H
#define CPP_CPP_PREPROCESSOR_DATA_H

#include <deque>
#include <unordered_map>
#include <vector>

#include <XLog/XLog.h>

namespace CPP {
    struct CPP_Preprocessor_Data {
        template<typename K>
        static bool keyExists(const std::vector<K> &map, const K &key) {
            for (auto &k : map) {
                if (k == key) return true;
            }
            return false;
        }

        template<typename K>
        static bool keyExists(const std::deque<K> &map, const K &key) {
            for (auto &k : map) {
                if (k == key) return true;
            }
            return false;
        }

        template<typename K, typename V>
        static bool keyExists(const std::unordered_map<K, V> &map, const K &key) {
            return map.find(key) != map.end();
        }

        template<typename K>
        static K *getValueIfKeyExists(std::vector<K> &map, const K &key) {
            for (auto &k : map) {
                if (k == key) return &k;
            }
            return nullptr;
        }

        template<typename K>
        static K *getValueIfKeyExists(std::deque<K> &map, const K &key) {
            for (auto &k : map) {
                if (k == key) return &k;
            }
            return nullptr;
        }

        template<typename K, typename V>
        static V *getValueIfKeyExists(std::unordered_map<K, V> &map, const K &key) {
            auto s = map.find(key);
            if (s != map.end()) {
                return &s->second;
            }
            return nullptr;
        }

        enum preprocessor_state_t {
            no_preprocessor_state,
            define,
            undef
        };

        enum preprocessor_expansion_state_t {
            no_expansion_state,
            argument_count,
            expansion
        };

        preprocessor_state_t preprocessor_state = no_preprocessor_state;

        preprocessor_expansion_state_t expansion_state = no_expansion_state;

        struct Macro {
            enum Type {
                Object,
                Function
            };

            Type type = Object;
            std::string id;
            std::string content;
            std::vector<std::string> args;
            std::vector<std::string> args_content;
            size_t count = 0;
        };

        std::unordered_map<std::string, Macro> function_definitions;
        std::unordered_map<std::string, Macro> definitions;

        bool expanding_function = false;

//        std::stack<std::string> function_being_expanded;

        std::string current_id;

        std::deque<std::string> do_not_expand;
    };
}

#endif