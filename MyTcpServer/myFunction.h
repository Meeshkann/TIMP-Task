#ifndef FUNC_HANDLER_H
#define FUNC_HANDLER_H

#include <cmath>

class myFunc {
public:
    myFunc();
    ~myFunc();
    float solveForX(int x, int a, int b, int c) {
        if (x<0) {
            return std::abs(x)*a;
        }
        if (x>=1) {
            return (x*x*2 - 2*x + 1)*b;
        }
        return c/x;
    }


};


#endif // FUNC_HANDLER_H
