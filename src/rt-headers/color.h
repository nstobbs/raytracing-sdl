#ifndef COLOR_H
#define COLOR_H

#include <iostream>

#include "rtweekend.h"
#include "interval.h"
#include "vec3.h"

using color = vec3;

// Will be removed soon as it's not needed anymore 
void writeColor(std::ostream& out, const color& pixelColor)
{
    auto red = pixelColor.x();
    auto green = pixelColor.y();
    auto blue = pixelColor.z();

    static const interval intensity(0.000, 0.999);
    int redByte = int(256 * intensity.clamp(red));
    int greenByte = int(256 * intensity.clamp(green));
    int blueByte = int(256 * intensity.clamp(blue));

    out << redByte << ' ' << greenByte << ' ' << blueByte << '\n';
}

#endif