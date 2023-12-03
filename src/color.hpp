#ifndef COLOR_H
#define COLOR_H

#include "SETTINGS.hpp"
#include <string.h>
#include <assert.h>
#include <assert.h>

using namespace std;

class Color : public Vector {
public:
    using Matrix<Real, 3, 1>::Matrix;

    static Color fromHex(string hexCode) {
       Real r, g, b;
        assert(hexCode.length() == 6 || hexCode.length() == 7);
        if(hexCode[0] == '#') hexCode.erase(0,1);
        r = (Real) stoi(hexCode.substr(0, 2), nullptr, 16) / 255;
        g = (Real) stoi(hexCode.substr(2, 2), nullptr, 16) / 255;
        b = (Real) stoi(hexCode.substr(4, 2), nullptr, 16) / 255;

        return Color(r,g,b);
    }

    Color *clamp(Real min = 0.0, Real max = 1.0) {
        for (int i = 0; i < 3; ++i) {
            if ((*this)[i] < min) (*this)[i] = min;
            if ((*this)[i] > max) (*this)[i] = max;
        }
        return this;
    }

};

#endif
