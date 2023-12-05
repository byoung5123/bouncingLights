#ifdef WIN32
#define SDL_MAIN_HANDLED
#endif
#include <SDL2/SDL.h>
#include <cstdlib>
#include <math.h>
#include <SDL_image.h>
#include <complex.h>

struct point_t {
//point: contains x and y pixel values
    int x;
    int y;
    };
struct vec_t {
//vector: contains x and y magnitude + direction
    float x;
    float y;
    };
struct light_t {
//light: contains position of light on screen and velocity for movement
    point_t pos;
    vec_t vel;
    };

void putPixel(SDL_Renderer *renderer, float x, float y, unsigned char r, unsigned char g, unsigned char b)
//displays pixel with given color on screen at given x and y coordinates
{
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_RenderDrawPoint(renderer, x, y);
}

light_t move(light_t light, int WindowWidth, int WindowHeight)
//move the light using its velocity, inverts velocity when "bouncing" off window boundaries
{
    //x
    light.pos.x += light.vel.x;
        if (light.pos.x > WindowWidth || light.pos.x < 0)
            light.vel.x *= -1;
    //y
    light.pos.y += light.vel.y;
        if (light.pos.y > WindowHeight || light.pos.y < 0)
            light.vel.y *= -1;

    return light;
}

double _Complex getLight(point_t pixel, light_t light, int lambda)
//calculate how much light is affecting a given pixel
{
    float d2 = pow((pixel.x - light.pos.x), 2) + pow((pixel.y - light.pos.y), 2); //distance squared between light and pixel
    float d = sqrt(d2); //distance between light and pixel
    double _Complex a = cexp(2 * M_PI * d * I / lambda) / d2; //attenuation of light at pixel
    return a;
}

auto main() -> int
{
    constexpr int WindowWidth = 450;
    constexpr int WindowHeight = 450;
    SDL_Event event;
    SDL_Renderer *renderer;
    SDL_Window *window;
    SDL_CreateWindowAndRenderer(WindowWidth, WindowHeight, 0, &window, &renderer);
    SDL_SetWindowTitle(window, "bouncingLights");
    // clear to background
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    bool quit = false;
    SDL_RenderClear(renderer);
    
    // array of lights, contains starting position and velocity
    int numOfLights = 3;
    light_t lights[numOfLights] = {
        {{WindowWidth / 2, WindowHeight / 2}, {2, 2}},
        {{0, 0},{2.2, -2}},
        {{WindowWidth, WindowHeight}, {2.5, 1.8}}
    };
    int lambda = 30;
    
    while (!quit)
    {
    //animation loop

        //movement for each light
        for (int i = 0; i < numOfLights; i++)
            lights[i] = move(lights[i], WindowWidth, WindowHeight);

        //calculate and render each pixel color
        for (int x = 0; x < WindowWidth; x++) {
            for (int y = 0; y < WindowHeight; y++) {

                point_t pixel = {x, y}; //current pixel
                double _Complex totalLight = 0.0; //total light at pixel

                //add color value from each light to pixel color
                for (int i = 0; i < numOfLights; i++)
                    totalLight += getLight(pixel, lights[i], lambda);

                int color = cabs(totalLight) * 1000000; //scale up lights

                //clamp color value
                if (color > 255)
                    color = 255;
                if (color < 0)
                    color = 0;

                //draw pixel
                putPixel(renderer, x, y, color, color, color);
            }
        }
    //events

        while (SDL_PollEvent(&event)) 
        {
            switch (event.type)
            {
            // this is the window x being clicked.
            case SDL_QUIT:
                quit = true;
                break;
            // now we look for a keydown event
            case SDL_KEYDOWN:
            {
                switch (event.key.keysym.sym)
                {
                // if it's the escape key quit
                case SDLK_ESCAPE:
                    quit = true;
                    break;
                default:
                    break;
                } // end of key process
            }     // end of keydown
            break;
            default:
                break;
            } // end of event switch
        }     // end of poll events
        // flip buffers
        SDL_RenderPresent(renderer);
    }
    // clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
