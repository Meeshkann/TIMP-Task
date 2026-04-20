#ifndef FUNC_HANDLER_H
#define FUNC_HANDLER_H

#include <cmath>
#include <QString>
#include <QStringList>

class myFunc {
public:
    myFunc() {}   // конструктор определён inline
    ~myFunc() {}  // деструктор определён inline

    // Исправлено: параметр x теперь double
    double solveForX(double x, int a, int b, int c) {
        if (x <= 0) {
            return std::abs(x) * a;
        }
        if (x > 1) {
            // 2*x^2 - 2*x + 1
            return (2.0 * x * x - 2.0 * x + 1.0) * c;
        }
        // 0 <= x < 1
        if (std::fabs(x) < 1e-9) {  // защита от деления на ноль
            return 0.0;
        }
        return b / x;
    }

    QString generateFunctionData(int a, int b, int c) {
        QStringList points;
        const double start = -10.0;
        const double end = 10.0;
        const double step = 0.1;

        for (double x = start; x <= end + step/2; x += step) {
            double y = solveForX(x, a, b, c);
            points.append(QString("%1,%2")
                              .arg(x, 0, 'f', 1)   // x с 1 знаком после запятой
                              .arg(y, 0, 'f', 6)); // y с 6 знаками
        }
        return points.join(";");
    }
};

#endif // FUNC_HANDLER_H
