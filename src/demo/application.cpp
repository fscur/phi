#include "application.h"

#include <core\globals.h>

#include <SDL\SDL_ttf.h>

std::string application::path = "";
std::string application::exePath = "";

application::application()
{
    phi::debug("Starting application.");

    if(SDL_Init(SDL_INIT_VIDEO) < 0)
        phi::log("SDL could not initialize! SDL_Error: " + std::string(SDL_GetError()));

    //TTF_Init();
}

application::~application(void)
{
    phi::debug("Closing application.");
    //TTF_Quit();
    SDL_Quit();
}

void application::run(form* form)
{
    form->show();
    while (form->loop());
}