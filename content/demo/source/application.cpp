#include "application.h"
#include "globals.h"
#include <SDL\SDL_ttf.h>

std::string application::path = "";
std::string application::exePath = "";

application::application()
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
        LOG("SDL could not initialize! SDL_Error: " << SDL_GetError());

    TTF_Init();
}

application::~application(void)
{
    TTF_Quit();
    SDL_Quit();
}

void application::run(form* form)
{
    while (form->loop());
}