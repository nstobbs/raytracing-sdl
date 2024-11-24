#ifndef CAMERA_H
#define CAMERA_H

#include <vector>

#include "../vec3_uint8.h"

#include "hittable.h"
#include "material.h"

class camera
{
    public:
        int imagePlaneWidth = 100;
        int imagePlaneHeight= 100;
        int samplesPerPixel = 10;
        int maxDepth = 10;

        double viewFov = 90;
        point3 lookFrom = point3(0,0,0);
        point3 lookAt = point3(0,0,-1);
        vec3 vUp = vec3(0,1,0);

        double defocusAngle = 0;
        double focusDist = 10;

        void calPixelColor(int x, int y, const hittable &world, std::vector<std::vector<color_uint8>> &framebuffer)
        {
            color pixelColor (0,0,0);
            for (int sampleID = 0; sampleID < samplesPerPixel; sampleID++)
            {
                ray r = getRay(x, y);
                pixelColor += rayColor(r, maxDepth, world);
            }
            auto finalPixel = pixelSampleScale * pixelColor;
            framebuffer[y][x].x = int(finalPixel[0] * 255.0);
            framebuffer[y][x].y = int(finalPixel[1] * 255.0);
            framebuffer[y][x].z = int(finalPixel[2] * 255.0);
        };

        void initializeCamera()
        {
            initialize();
        }

    private:
        double pixelSampleScale;
        point3 cameraCenter;
        point3 pixel_00_loc;
        vec3 pixelDeltaU;
        vec3 pixelDeltaV;
        vec3 u, v, w;
        vec3 defocusDiskU;
        vec3 defocusDiskV;

        void initialize()
        {
            pixelSampleScale = 1.0 / samplesPerPixel;
            cameraCenter = lookFrom;

            auto theta = degreesToRadians(viewFov);
            auto h = std::tan(theta/2);
            auto viewportHeight = 2 * h * focusDist;
            auto viewportWidth = viewportHeight * (float(imagePlaneWidth)/imagePlaneHeight);

            w = unitVector(lookFrom - lookAt);
            u = unitVector(cross(vUp, w));
            v = cross(w, u);

            auto viewportU = viewportWidth * u;
            auto viewportV = viewportHeight * -v;

            pixelDeltaU = viewportU / imagePlaneWidth;
            pixelDeltaV = viewportV / imagePlaneHeight;

            auto viewportUpperLeft = cameraCenter - (focusDist * w) - viewportU/2 - viewportV/2;
            pixel_00_loc = viewportUpperLeft + 0.5 * (pixelDeltaU + pixelDeltaV);

            auto defocusRadius = focusDist * std::tan(degreesToRadians(defocusAngle / 2));
            defocusDiskU = u * defocusRadius;
            defocusDiskV = v * defocusRadius;
        };

        ray getRay(int x, int y) const
        {
            auto offset = sampleSquare();
            auto pixelSample = pixel_00_loc + ((x + offset.x()) * pixelDeltaU) + ((y + offset.y()) * pixelDeltaV);
            auto rayOrigin = (defocusAngle <= 0) ? cameraCenter : defocusDiskSample();
            auto rayDirection = pixelSample - rayOrigin;

            return ray(rayOrigin, rayDirection);
        }

        vec3 sampleSquare() const
        {
            return vec3(randomDouble() - 0.5, randomDouble() - 0.5, 0);
        }

        point3 defocusDiskSample() const
        {
            auto p = randomInUnitDisk();
            return cameraCenter + (p[0] * defocusDiskU) + (p[1] * defocusDiskV);
        }

        color rayColor(const ray& r, int maxDepth, const hittable& world)
        {
            if (maxDepth <= 0)
            {
                return color(0,0,0);
            }
            hitRecord rec;
            if(world.hit(r, interval(0, infinity), rec))
            {
                ray scattered;
                color attenuation;
                if(rec.mat->scatter(r, rec, attenuation, scattered))
                {
                    return attenuation * rayColor(scattered, maxDepth - 1, world);
                }
                return color(0,0,0);
            }

            vec3 unitDirection = unitVector(r.direction());
            auto a = 0.5*(unitDirection.y() + 1.0);
            return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
        };
};

#endif