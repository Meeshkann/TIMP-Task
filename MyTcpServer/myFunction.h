#ifndef FUNC_HANDLER_H
#define FUNC_HANDLER_H

#include <cmath>

class myFunc {
public:
    myFunc();
    ~myFunc();
    double solveForX(int x, int a, int b, int c) {
        if (x<0) {
            return std::abs(x)*a;
        }
        if (x>=1) {
            return (x*x*2 - 2*x + 1)*b;
        }
        return c/x;
    }

    QString generateFunctionData(int a, int b, int c)
    {
        QStringList points;
        const double start = -10.0;
        const double end = 10.0;
        const double step = 0.1;

        for (double x = start; x <= end + step/2; x += step) {
            double y = solveForX(static_cast<float>(x), a, b, c);
            points.append(QString("%1,%2").arg(x, 0, 'f', 2).arg(y, 0, 'f', 6));
        }
        return points.join(";");
    }
};


#endif // FUNC_HANDLER_H
