#ifndef COLORMAP_H
#define COLORMAP_H
#include <cstdlib>

class ColorMap
{
private:
    static const float MATRIX[][3];

    static const int MAX_VALUE=30;
    int index;
public:
    ColorMap();
    const float * getColor( int i );
};

#endif // COLORMAP_H
