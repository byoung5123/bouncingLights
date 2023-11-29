#ifdef WIN32
#define SDL_MAIN_HANDLED
#endif
#include <SDL2/SDL.h>
#include <cstdlib>
#include <math.h>
#include <SDL_image.h>
#include <complex.h>

struct point_t {
    int x;
    int y;
    };
struct vec_t {
    float x;
    float y;
    };
struct light_t {
    point_t pos;
    vec_t vel;
    };

vec_t pointSub(point_t a, point_t b)
//subtracts point b from point a
{
    vec_t result = {a.x-b.x, a.y-b.y};
    return result;
}

light_t move(light_t light, int WindowWidth, int WindowHeight)
//move the light using its velocity, invert velocity when "bouncing" off window boundaries
{
    light.pos.x+=light.vel.x;
        if (light.pos.x>WindowWidth || light.pos.x<0)
            light.vel.x*=-1;
    light.pos.y+=light.vel.y;
        if (light.pos.y>WindowHeight || light.pos.y<0)
            light.vel.y*=-1;
    return light;
}

void putPixel(SDL_Renderer *renderer, float x, float y, unsigned char r, unsigned char g, unsigned char b)
{
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_RenderDrawPoint(renderer, x, y);
}

double _Complex getColor(point_t pixel, light_t light, int lambda)
//calculate how much light is affecting a given pixel
{
    vec_t v = pointSub(pixel, light.pos);
    float d2 = v.x*v.x+v.y*v.y;
    float d = sqrt(d2);
    double _Complex a = cexp(2*M_PI*d*I/lambda)/d2; //attenuation
    return a;
}

auto main() -> int
{
    constexpr int WindowWidth = 450;
    constexpr int WindowHeight = 450;
    SDL_Event event;
    SDL_Renderer *renderer;
    SDL_Window *window;
    SDL_Surface* image;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(WindowWidth, WindowHeight, 0, &window, &renderer);
    SDL_SetWindowTitle(window, "Put Pixel");
    // clear to background
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    bool quit = false;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    
    // array of lights, contains starting position and velocity
    int numOfLights = 3;
    light_t lights[numOfLights] = {
        {{WindowWidth/2,WindowHeight/2},{2,2}},
        {{0,0},{2.2,-2}},
        {{WindowWidth,WindowHeight},{2.5,1.8}}
    };
    int lambda = 30;

    //animation loop
    while (!quit)
    {
        //movement and bouncing for each light
        for (int i = 0; i < numOfLights; i++)
            lights[i] = move(lights[i], WindowWidth, WindowHeight);

        //calculate and render each pixel color
        for (int x = 0; x < WindowWidth; x++) {
            for (int y = 0; y < WindowHeight; y++) {
                point_t pixel = {x, y};
                double _Complex totalLight = 0.0;

                //add color from each light to pixel color
                for (int i = 0; i < numOfLights; i++) {
                    double _Complex lightCol = getColor(pixel, lights[i], lambda);
                    totalLight += lightCol;
                }
                int color = cabs(totalLight) * 1000000; //scale up lights

                //ensure color value is normalised
                if (color > 255)
                    color = 255;
                if (color < 0)
                    color = 0;

                //draw pixel
                putPixel(renderer, x, y, color, color, color);
            }
        } 
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