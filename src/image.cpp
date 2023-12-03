#include "image.hpp"
#include "SETTINGS.hpp"

#include <iostream>
#include <string>
#include <vector>

using namespace std;

void writePPM(const string& filename, const int& xRes, const int& yRes, float*& values) {
    int totalCells = xRes * yRes;
    unsigned char* pixels = new unsigned char[3 * totalCells];
    for (int i = 0; i < 3 * totalCells; i++)
        pixels[i] = values[i];

    FILE *fp;
    fp = fopen(filename.c_str(), "wb");
    if (fp == NULL)
    {
        cout << " Could not open file \"" << filename.c_str() << "\" for writing." << endl;
        cout << " Make sure you're not trying to write from a weird location or with a " << endl;
        cout << " strange filename. Bailing ... " << endl;
        exit(0);
    }

    fprintf(fp, "P6\n%d %d\n255\n", xRes, yRes);
    fwrite(pixels, 1, totalCells * 3, fp);
    fclose(fp);
    delete[] pixels;
}

void readPPM(const string& filename, int& xRes, int& yRes, float*& values) {
    // try to open the file
    FILE *fp;
    fp = fopen(filename.c_str(), "rb");
    if (fp == NULL)
    {
        cout << " Could not open file \"" << filename.c_str() << "\" for reading." << endl;
        cout << " Make sure you're not trying to read from a weird location or with a " << endl;
        cout << " strange filename. Bailing ... " << endl;
        exit(0);
    }

    // get the dimensions
    unsigned char newline;
    fscanf(fp, "P6\n%d %d\n255%c", &xRes, &yRes, &newline);
    if (newline != '\n') {
        cout << " The header of " << filename.c_str() << " may be improperly formatted." << endl;
        cout << " The program will continue, but you may want to check your input. " << endl;
    }
    int totalCells = xRes * yRes;

    // grab the pixel values
    unsigned char* pixels = new unsigned char[3 * totalCells];
    fread(pixels, 1, totalCells * 3, fp);

    // copy to a nicer data type
    values = new float[3 * totalCells];
    for (int i = 0; i < 3 * totalCells; i++)
        values[i] = pixels[i];

    // clean up
    delete[] pixels;
    fclose(fp);
    cout << "Read in file " << filename.c_str() << endl;
}

Image::Image(int width, int height): width(width), height(height){
    data = new Color[width * height];
}

Image::Image(const string& filename) {
    float* values;
    readPPM(filename, width, height, values);
    data = new Color[width * height];

    for (int i = 0, c = 0; c < width*height; i+=3, c++) {
        Real r = (float) values[i] / 255;
        Real g = (float) values[i+1] / 255;
        Real b = (float) values[i+2] / 255;
        data[c] = Color(r, g, b);
    }

    delete[] values;
}

Image::Image(const Image& other): Image(1,1) {
    deepCopyFromImage(other);
}


Image::~Image(){
    delete[] data;
}

int Image::getWidth() const{
    return width;
}

int Image::getHeight() const{
    return height;
}

int Image::getNumPixels() const {
    return width * height;
}

Color* Image::at(int x, int y) const {
    return data + (x + y * width);
}

Color* Image::at(int i) const {
    return &data[i];
}

void Image::savePPM(std::string filename) const {
    // Convert bitmap to RGB
    int numPixels = width * height;
    float* values = new float[numPixels * 3];

    for (int i = 0; i < numPixels; i++) {
        data[i].clamp();
        values[i * 3 + 0] = data[i][0] * 255.0;
        values[i * 3 + 1] = data[i][1] * 255.0;
        values[i * 3 + 2] = data[i][2] * 255.0;
    }

    writePPM(filename, width, height, values);
    delete[] values;
}

Color Image::getMinColor() const {
    Color min(INT_MAX,INT_MAX,INT_MAX);
    for (int i = 0; i < width * height; ++i) {
        min = min.cwiseMin(data[i]);
    }
    return min;
}

Color Image::getMaxColor() const {
    Color max(INT_MIN,INT_MIN,INT_MIN);
    for (int i = 0; i < width * height; ++i) {
        max = max.cwiseMax(data[i]);
    }
    return max;
}

void Image::normalize() {

    Color min = getMinColor();
    Color max = getMaxColor();

    // Normalize
    for (int i = 0; i < width * height; ++i) {
        data[i] -= min;
        data[i] = data[i].cwiseQuotient((max - min));
    }

}

void Image::boxBlurPixel(int x, int y, int radius) {

    if (radius <= 1) return;

    int min = -(radius / 2);
    int max = -min;

    int count = 0;

    Color sum(0,0,0);

    for (int ox = min; ox < max; ++ox) {
        if ((x + ox) > 0 && (x + ox) < width) {
            for (int oy = min; oy < max; ++oy) {
                if ((y + oy) > 0 && (y + oy) < height) {
                    count++;
                    sum += *this->at(x + ox, y + oy);
                }
            }
        }
    }

    *this->at(x,y) = sum / count;
}

Color Image::getBoxBlurredColor(int x, int y, int radius) {

    if (radius <= 1) return *this->at(x,y);

    int min = -(radius / 2);
    int max = -min;

    int count = 0;

    Color sum(0,0,0);

    for (int ox = min; ox < max; ++ox) {
        if ((x + ox) > 0 && (x + ox) < width) {
            for (int oy = min; oy < max; ++oy) {
                if ((y + oy) > 0 && (y + oy) < height) {
                    count++;
                    sum += *this->at(x + ox, y + oy);
                }
            }
        }
    }

    return sum / count;
}

void Image::boxBlurInPlace(int radius) {
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            boxBlurPixel(x, y, radius);
        }
    }
}

void Image::deepCopyFromImage(const Image& other) {
    width = other.width;
    height = other.height;
    delete data;

    data = new Color[width * height];
    for (int i = 0; i < other.getNumPixels(); ++i) {
        data[i] = *other.at(i);
    }
}
