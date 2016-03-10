#include <precompiled.h>
#include "application.h"

phi::string application::path = "";
phi::string application::exePath = "";

application::application()
{
    phi::debug("Starting application.");

    if(SDL_Init(SDL_INIT_VIDEO) < 0)
        phi::logError("SDL could not initialize! SDL_Error: " + phi::string(SDL_GetError()));

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