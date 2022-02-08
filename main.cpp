#include "CPP/CPP.h"

std::string input = R"(
#define X(a) a y
#define Y(Z) Z
Y(X)(z)
//#define a_ b_
//#define b_ c_
//#define Y a
//#define foo(a) a
//#define b Y
//foo(b)
//#define a x
//#define c a
//#define foo(a) a c
//#define b Y
//foo(b)
//#define foo(a) a c foo
//foo(b)
//#define foo(a) a c foo(b)
//foo(b)
//
//#define foo(a) foo(b)
//foo(b)
//
//#define foo bar
//foo(b)
//#define Y G
//#define a x
//#define c a
//#define foo(a) a c
//#define b Y
//foo(b)
//#define foo(a) a c foo
//foo(b)
//#define foo(a) a c foo(b)
//foo(b)
//#define foo(a) foo(b)
//foo(b)
//#define foo bar
//foo(b)
//
//#define foo(x) x(x)
//foo(bar)
//
//#define foo(x) 9 x(x) 9
//1 foo(foo) 2 foo(bar) 3
//
//#define X(z) z b
//#define foo foo X
//foo(z)
//#define X(z) z b
//#define foo(x) foo x
//foo(z)
)";

/*
11:36:30 fizzie: smallville7123: Okay, so for that... we start with the base context conceptually
            containing "foo ( z )". It reads out foo. It's an object-like macro, so we push a
            context that contains its unexpanded tokens, so "foo X"; the base context has "( z )"
            remaining. Then we look at the next token of the topmost context. It's "foo", but in
            this context it's not eligible for expansion, so it's just copied to output. The next
            token is X. It's a function-like macro, so we peek at the next token. Since the top
            context is exhausted, it comes from the base context, and is a '('. We pop off the
            `foo` context, read the '(', and then continue reading tokens up to the matching ')'
            as the macro parameters. This consumes the remaining "( z )" from the input. Then we
            push on a new context that contains X's body after argument substitution, so in this
            case from X(z) -> "z b". We resume reading/rescanning from the top context. The next
            token is z, which is not a macro, so it's copied to output. The token after that is
            "b", which is also not a macro, and is copied. The X's context is now exhausted and
            popped off. The base context is *also* empty. No more output is generated. We've
            produced the token sequence "foo z b", which is what that should expand to.
 */

/*
23:16:48 fizzie: Well, let's say you have `#define foo(x) 9 x(x) 9` and the input tokens
            are `1 foo(foo) 2 foo(bar) 3`. It'd start in a special "base context", which
            means "read tokens from the input source". It'd read and pass through 1. At
            the position "foo(foo) ..." it would detect a function-like macro, so it
            would push on the stack a context that contains the token list
            [9 foo(foo) 9], leaving the rest ("2 foo(bar) 3") in the input. Then it'd
            continue reading tokens from that context: within the context, `foo` is not
            eligible for expansion, so they all pass through. Once the context runs out of
            tokens, it gets popped off the stack, and scanning continues from the input.
            It passes 2 through, encounters `foo(bar)`, and since it's back in the base
            context it is again eligible for expansion.
23:17:35 fizzie: So there's no "rewinding" of the input, or replacing anything in the
            input.
*/

// iterater must have parent iterater to get more tokens from

int main() {
    CPP::CPP cpp;
    cpp.parse(input);
    return 0;
}