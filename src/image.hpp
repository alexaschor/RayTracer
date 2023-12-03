#ifndef IMAGE_H
#define IMAGE_H

#include "SETTINGS.hpp"
#include "color.hpp"
#include <string>

class Image {
protected:
    int width, height;
    Color* data;

public:
    Image(int width, int height);
    Image(const string& filename);
    Image(const Image& other);
    ~Image();

    int getWidth() const;
    int getHeight() const;
    int getNumPixels() const;

    Color getMinColor() const;
    Color getMaxColor() const;

    Color* at(int x, int y) const;
    Color* at(int i) const;

    void savePPM(std::string filename) const;
    void normalize();

    void boxBlurPixel(int x, int y, int radius);
    Color getBoxBlurredColor(int x, int y, int radius);
    void boxBlurInPlace(int radius);

    void deepCopyFromImage(const Image& other);
};


#endif
