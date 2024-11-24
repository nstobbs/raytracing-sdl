#ifndef VEC3_UINT8_HEADER
#define VEC3_UINT8_HEADER

#include <stdio.h>
#include <string>

/*
vec3_uint8.h
Super basic class for storing unsigned int 8s.
Can be used for mainly storing color data.
*/

class vec3_uint8
{
    public:
        vec3_uint8()
        {
            x = 0x00;
            y = 0x00;
            z = 0x00;
        };
        uint8_t x = data[0];
        uint8_t y = data[1];
        uint8_t z = data[1];
    private:
        uint8_t data[3];
        uint32_t pixelFormat; // WIP Placeholder for SDL like Pixel Data Storage 
};

using color_uint8 = vec3_uint8;
#endif