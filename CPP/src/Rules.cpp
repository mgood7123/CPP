#include "../include/CPP/Rules.h"
CPP::Rules::Action CPP::Rules::NO_ACTION = [](CPP::Rules::Input) {};
std::vector<CPP::Rules::RuleHolder::Reference*> CPP::Rules::RuleHolder::rules;