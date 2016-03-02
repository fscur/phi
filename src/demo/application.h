#pragma once
#include <precompiled.h>
#include "form.h"

class application
{
public: 
	static phi::string path;
	static phi::string exePath;
public:
    application();
    ~application(void);

    void run(form* form);
};