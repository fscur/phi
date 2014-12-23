#include "screen.h"
#include "application.h"

int main(int argc, char* args[])
{   
    application *app = new application();

    screen *mainScreen = new screen();
    mainScreen->initialize();
    
    app->run(mainScreen);

    delete mainScreen;
    delete app;

	return 0;
}