#ifndef _PHI_DRAW_ELEMENTS_INDIRECT_CMD_H_
#define _PHI_DRAW_ELEMENTS_INDIRECT_CMD_H_

#include <GL\glew.h>

struct drawElementsIndirectCmd
{
    GLuint indicesCount;
    GLuint instanceCount;
    GLuint firstIndex;
    GLuint baseVertex;
    GLuint baseInstance;
};

#endif // !_PHI_DRAW_ELEMENTS_INDIRECT_CMD_H_

