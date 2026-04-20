#ifndef FUNC_HANDLER_H
#define FUNC_HANDLER_H

#include <cmath>

class myFunc {
public:
    myFunc();
    ~myFunc();
    float solve(int x, int b, int c) {
        if (x<0) {
            return std::abs(x);
        }
        if (x>=1) {
            return x*x*2 - 2*x + 1;
        }
        return 1.0f/x;
    }
};


#endif // FUNC_HANDLER_H
