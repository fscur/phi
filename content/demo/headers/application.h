#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "form.h"

class application
{
public:
    application(void);
    ~application(void);

    void run(form* form);
};

#endif