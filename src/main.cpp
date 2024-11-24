#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>

#include <vector>
#include "vec3_uint8.h"
#include "render.h"

const int HEIGHT = 200;
const int WIDTH = 300;
const int THREADS_NUM = 10;

SDL_Window* window = NULL;
SDL_Texture* framebufferTexture = NULL;
SDL_Renderer* gRenderer = NULL;

void SetPixel(SDL_Surface* surface, int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
    SDL_LockSurface(surface);
    uint8_t* pixelArray = (uint8_t*)surface->pixels;
    pixelArray[y*surface->pitch +x*surface->format->BytesPerPixel+0] = b;
    pixelArray[y*surface->pitch +x*surface->format->BytesPerPixel+1] = g;
    pixelArray[y*surface->pitch +x*surface->format->BytesPerPixel+2] = r;
    SDL_UnlockSurface(surface);
}

SDL_Texture* CopyFramebufferToTexture(std::vector<std::vector<color_uint8>> frameBuffer)
{
    SDL_Texture* newTexture = NULL;

    SDL_Surface* loadedSurface = SDL_GetWindowSurface(window);
    if(loadedSurface == NULL)
    {
        printf("SDL_IMG_Load Failed on import: %s\n", IMG_GetError());
    }
    else
    {
        // TODO: REMOVE this loop when RENDER() outputs a matching data struct of surface pixels array and just do a memcpy here instead.
        for (int y = 0; y < HEIGHT; y++)
        {
            for (int x = 0; x < WIDTH; x++)
            {
                color_uint8 currentPixel = frameBuffer[y][x];
                SetPixel(loadedSurface, x, y, currentPixel.x,  currentPixel.y, currentPixel.z);
            }
        }
        newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if(newTexture == NULL)
        {
            printf("Failed to create texture from surface: %s\n", SDL_GetError());
        }
        SDL_FreeSurface(loadedSurface);
    }
    return newTexture;
}

bool LoadFramebufferTexture(std::vector<std::vector<color_uint8>> frameBuffer) // I should remove this function
{
    bool success = true;
    framebufferTexture = CopyFramebufferToTexture(frameBuffer);
    if(framebufferTexture == NULL)
    {
        printf("framebufferTexture still NULL after copy!\n");
    }
    return success;
};

void close()
{
    SDL_DestroyTexture(framebufferTexture);
    framebufferTexture = NULL;
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char* args[])
{
    // Prep CPU Render
    camera cam;
    cam.imagePlaneWidth = WIDTH;
    cam.imagePlaneHeight = HEIGHT;
    cam.samplesPerPixel = 50;
    cam.maxDepth = 10;
    
    cam.viewFov = 20;
    cam.lookFrom = point3(13, 2, 3);
    cam.lookAt = point3(0,0,0);
    cam.vUp = vec3(0,1,0);

    cam.defocusAngle = 0.6;
    cam.focusDist = 10.0;

    hittable_list world = createWorld();
    cam.initializeCamera();

    // CPU Rendering to Framebuffer
    std::vector<std::vector<color_uint8>> framebuffer;
    createBlank2DVectorColor(framebuffer, WIDTH, HEIGHT);
    render(framebuffer, cam, world, WIDTH, HEIGHT, THREADS_NUM);


    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL ERROR: %s\n", SDL_GetError());
        return -1;
    }
    else
    {
        window = SDL_CreateWindow("raytracing-SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
        if (window == NULL)
        {
            printf("SDL WINDOW ERROR: %s\n", SDL_GetError());
            return 2;
        }
        gRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (gRenderer == NULL)
        {
            printf("SDL RENDERER ERROR: %s\n", SDL_GetError());
            return 4;
        }
        SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);

        int imgFlags = IMG_INIT_PNG;
        if(!(IMG_Init(imgFlags)&imgFlags))
        {
            printf("SDL_Image Fail: %s\n", IMG_GetError());
        }

        if(!LoadFramebufferTexture(framebuffer))
        {
            printf("Failed to Copy Framebuffer To Texture\n");
            return 3;
        }

        // Windows Event Loop 🤢
        bool quit = false;
        SDL_Event e;
        while(!quit)
        {
            while(SDL_PollEvent(&e) != 0)
            {
                if(e.type == SDL_QUIT)
                {
                    quit = true;
                }
            }
            
            // Copying the Framebuffer to the Texture -> GPURenderer
            SDL_RenderClear(gRenderer);
            if(!LoadFramebufferTexture(framebuffer))
            {
                printf("Failed to Copy Framebuffer To Texture\n");
                return 3;
            }
            SDL_RenderCopy(gRenderer, framebufferTexture, NULL, NULL);
            SDL_RenderPresent(gRenderer);
        }
    }

    close(); 
    return 0;
}