#include "phi/ui/scissorStack.h"

namespace phi
{
    scissorStack::scissorStack(size<GLuint> viewportSize)
    {
        _viewportSize = viewportSize;
        pushScissor(0.0f, 0.0f, _viewportSize.width, _viewportSize.height);
    }

    void scissorStack::calculateCurrentScissor()
    {
        _currentScissor = _scissors[0];

        for (int i = 1; i < _scissors.size(); i++)
            _currentScissor = _currentScissor.intersection(_scissors[i]);
    }

    void scissorStack::pushScissor(scissor s)
    {
        _scissors.push_back(s);
        _currentScissor = _currentScissor.intersection(s);
    }

    void scissorStack::pushScissor(float x, float y, float width, float height)
    {
        pushScissor(scissor(x, y, width, height));
    }

    void scissorStack::popScissor()
    {
        if (_scissors.size() > 1)
        {
            _scissors.pop_back();
            calculateCurrentScissor();
        }
    }

    void scissorStack::enable()
    {
        glEnable(GL_SCISSOR_TEST);
        glScissor(_currentScissor.x, (_viewportSize.height - _currentScissor.height - _currentScissor.y), _currentScissor.width, _currentScissor.height);
    }

    void scissorStack::disable()
    {
        glDisable(GL_SCISSOR_TEST);
    }
}