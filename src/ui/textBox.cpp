#include <precompiled.h>
#include "textBox.h"

#include "uiSystem.h"

namespace phi
{
    textBox::textBox(sizef viewportSize) : control(viewportSize)
    {
        _text = "";
        _texture = uiRepository::textureButton;
        _font = uiRepository::fontConsolas18;
        auto size = sizeui(viewportSize.w, viewportSize.h, viewportSize.d);
        _backgroundRenderer = new quadRenderer2D(vec2(), _zIndex, sizeui(), size);
        _cursorRenderer = new quadRenderer2D(vec2(), _zIndex + 0.03f, sizeui(CURSOR_WIDTH, _font->getLineHeight()), size);
        _selectionRenderer = new quadRenderer2D(vec2(), _zIndex + 0.01f, sizeui(), size);
        _textRenderer = new textRenderer2D(size);
        _textX = 0;
        _textY = 0;
        _textOffsetX = 0;
        _cursorIndex = 0;
        _selectionStartIndex = 0;
        _selectionEndIndex = 0;
        _isClickingOver = false;
        _textChanged = new eventHandler<eventArgs>();
    }

    textBox::~textBox()
    {
        safeDelete(_backgroundRenderer);
        safeDelete(_textRenderer);
    }

    void textBox::notifyTextChanged(eventArgs e)
    {
        _textChanged->raise(e);
    }

    void textBox::updateTextLocation()
    {
        sizef textSize = _textRenderer->measureSize(_text, _font);

        _textX = _x;
        _textY = (int)(_y + _size.h * 0.5f - textSize.h * 0.5f);
    }

    void textBox::updateCursorLocation()
    {
        std::string sub = _text.substr(0, _cursorIndex);
        sizef subTextSize = _textRenderer->measureSize(sub, _font);

        float x = (float)_x + (float)subTextSize.w + (float)_textOffsetX; // Cast needed probably because of a float rounding problem (?)
        float y = _y + _size.h * 0.5f - _cursorRenderer->getSize().h * 0.5f;
        if (x > _x + _size.w - 1)
        {
            _textOffsetX += (int)round(((_x + _size.w - 1) - x));
            x = (float)(_x + _size.w - 1.0f);
        }

        if (x < _x)
        {
            _textOffsetX -= (int)round(x - _x);
            x = (float)_x;
        }

        //sizef textSize = _textRenderer->measureSize(_text, _font);
        //if (textSize.w + _textOffsetX < _size.w && _textOffsetX < 0)
        //{
        //    _textOffsetX -= (textSize.w + _textOffsetX) - _size.w;
        //    x = _x + _size.w - 1;
        //}

        sizef textSize = _textRenderer->measureSize(_text, _font);
        if (textSize.w < _size.w)
        {
            _textOffsetX = 0;
            x = (float)(_x + subTextSize.w);
        }
        else if (textSize.w + _textOffsetX < _size.w)
        {
            _textOffsetX -= (textSize.w + _textOffsetX) - _size.w;
            x = (float)(_x + _size.w - 1.0f);
        }

        _cursorRenderer->setLocation(vec2(x, y));
        _cursorRenderer->update();
    }

    void textBox::updateSelectionRenderer()
    {
        unsigned int start = glm::min(_selectionStartIndex, _selectionEndIndex);
        unsigned int end = glm::max(_selectionStartIndex, _selectionEndIndex);

        std::string sub = _text.substr(start, end - start);
        sizef subTextSize = _textRenderer->measureSize(sub, _font);
        std::string pre = _text.substr(0, start);
        sizef preTextSize = _textRenderer->measureSize(pre, _font);

        _selectionRenderer->setSize(sizeui(subTextSize.w, _font->getLineHeight()));
        _selectionRenderer->setLocation(vec2(_x + preTextSize.w + _textOffsetX, _y + _size.h * 0.5f - _font->getLineHeight() * 0.5f));
        _selectionRenderer->update();
    }

    void textBox::setX(int value)
    {
        _x = value;
        _backgroundRenderer->setLocation(vec2(_x, _y));
        _backgroundRenderer->setZIndex(_zIndex);
        _backgroundRenderer->update();
        updateTextLocation();
    }

    void textBox::setY(int value)
    {
        _y = value;
        _backgroundRenderer->setLocation(vec2(_x, _y));
        _backgroundRenderer->update();
        updateTextLocation();
    }

    void textBox::setZIndex(float value)
    {
        _zIndex = value;
        _backgroundRenderer->setZIndex(_zIndex);
        _backgroundRenderer->update();
        _cursorRenderer->setZIndex(_zIndex + 0.03f);
        _selectionRenderer->setZIndex(_zIndex + 0.01f);
        _cursorRenderer->update();
    }

    void textBox::setSize(sizef value)
    {
        _size = value;
        _backgroundRenderer->setSize(sizeui(value.w, value.h, value.d));
        _backgroundRenderer->update();
        updateTextLocation();
    }

    void textBox::setText(std::string value)
    {
        _text = value;
        updateTextLocation();
        notifyTextChanged(eventArgs());
    }

    void textBox::setFont(font* value)
    {
        _font = value;
        _cursorRenderer->setSize(sizeui(CURSOR_WIDTH, _font->getLineHeight()));
        updateTextLocation();
        updateCursorLocation();
    }

    void textBox::setBackgroundColor(color value)
    {
        _backgroundColor = value;
        _currentColor = _backgroundColor;
    }

    void textBox::setViewportSize(sizef value)
    {
        control::setViewportSize(value);
        auto size = sizeui(value.w, value.h, value.d);
        _backgroundRenderer->setViewportSize(size);
        _backgroundRenderer->update();
        _cursorRenderer->setViewportSize(size);
        _cursorRenderer->update();
        _selectionRenderer->setViewportSize(size);
        _selectionRenderer->update();
        _textRenderer->setViewportSize(size);
        _textRenderer->update();
    }

    void textBox::onMouseDown(mouseEventArgs* e)
    {
        if (!getIsMouseOver())
            return;

        if (!e->leftButtonPressed)
            return;

        e->handled = true;

        if (e->clicks < 2)
        {
            _isClickingOver = true;
            _cursorIndex = _textRenderer->measureString(_text, _font, sizeui(glm::max(e->x - _x - _textOffsetX, 0), _size.h));
            updateCursorLocation();
            _selectionStartIndex = _cursorIndex;
            _selectionEndIndex = _cursorIndex;
            updateSelectionRenderer();
        }
        else
        {
            unsigned int index = _textRenderer->measureString(_text, _font, sizeui(glm::max(e->x - _x - _textOffsetX, 0), _size.h));
            std::vector<char> v(_text.begin(), _text.end());
            v.push_back('\0');
            char* str = &v[0];

            if (str == nullptr)
                return;

            bool inSpaces = true;
            unsigned int start = 0;
            unsigned int i = 0;
            bool findEnd = false;

            while (*str != '\0')
            {
                if (std::isspace(*str) && !inSpaces)
                {
                    inSpaces = true;
                    if (findEnd)
                        break;
                    else
                        start = i;
                }
                else if (!std::isspace(*str) && inSpaces)
                {
                    inSpaces = false;
                    if (findEnd)
                        break;
                    else
                        start = i;
                }

                if (i >= index)
                    findEnd = true;

                str++;
                i++;
            }

            _selectionStartIndex = start;
            _selectionEndIndex = _cursorIndex = i;

            updateCursorLocation();
            updateSelectionRenderer();
        }
    }

    void textBox::onMouseUp(mouseEventArgs* e)
    {
        if (_isClickingOver && e->leftButtonPressed)
            _isClickingOver = false;
    }

    void textBox::onMouseMove(mouseEventArgs* e)
    {
        if (_isClickingOver)
        {
            _selectionEndIndex = _cursorIndex = _textRenderer->measureString(_text, _font, sizeui(glm::max(e->x - _x - _textOffsetX, 0), _size.h));
            updateCursorLocation();
            updateSelectionRenderer();
        }
    }

    void textBox::onMouseEnter(mouseEventArgs* e)
    {
        //uiSystem::get()->setCursor(uiRepository::cursorsRepository->getResource("TextCursor"));
    }

    void textBox::onKeyDown(keyboardEventArgs e)
    {
        switch (e.key)
        {
            case PHIK_LEFT:
                if (!e.isShiftPressed)
                    _selectionStartIndex = _selectionEndIndex = _cursorIndex = glm::max((int)_cursorIndex - 1, 0);
                else
                    _selectionEndIndex = _cursorIndex = glm::max((int)_cursorIndex - 1, 0);
                updateCursorLocation();
                updateSelectionRenderer();
                break;
            case PHIK_RIGHT:
                if (!e.isShiftPressed)
                    _selectionStartIndex = _selectionEndIndex = _cursorIndex = glm::min(_cursorIndex + 1, (unsigned int)_text.length());
                else
                    _selectionEndIndex = _cursorIndex = glm::min(_cursorIndex + 1, (unsigned int)_text.length());
                updateCursorLocation();
                updateSelectionRenderer();
                break;
            case PHIK_BACKSPACE:
                if (_selectionStartIndex != _selectionEndIndex)
                    deleteSelection();
                else if (_cursorIndex > 0)
                {
                    setText(_text.erase(--_cursorIndex, 1));
                    _selectionStartIndex = _selectionEndIndex = _cursorIndex;
                    updateCursorLocation();
                    updateSelectionRenderer();
                }
                break;
            case PHIK_DELETE:
                if (_selectionStartIndex != _selectionEndIndex)
                    deleteSelection();
                else if (_cursorIndex < _text.length())
                {
                    setText(_text.erase(_cursorIndex, 1));
                    updateCursorLocation();
                }
                break;
            case PHIK_HOME:
                if (!e.isShiftPressed)
                    _selectionStartIndex = _selectionEndIndex = _cursorIndex = 0;
                else
                    _selectionEndIndex = _cursorIndex = 0;
                updateCursorLocation();
                updateSelectionRenderer();
                break;
            case PHIK_END:
                if (!e.isShiftPressed)
                    _selectionStartIndex = _selectionEndIndex = _cursorIndex = (unsigned int)_text.length();
                else
                    _selectionEndIndex = _cursorIndex = (unsigned int)_text.length();
                updateCursorLocation();
                updateSelectionRenderer();
                break;
            default:
                if (_selectionStartIndex != _selectionEndIndex)
                    deleteSelection();

                char c = (char)e.key;
                setText(_text.insert(_cursorIndex++, 1, c));
                _selectionStartIndex = _selectionEndIndex = _cursorIndex;
                updateCursorLocation();
                break;
        }
    }

    void textBox::deleteSelection()
    {
        unsigned int start = glm::min(_selectionStartIndex, _selectionEndIndex);
        unsigned int end = glm::max(_selectionStartIndex, _selectionEndIndex);

        setText(_text.erase(start, end - start));
        _selectionStartIndex = _selectionEndIndex = _cursorIndex = start;
        updateCursorLocation();
        updateSelectionRenderer();
    }

    void textBox::onRender()
    {
        //glEnable(GL_SCISSOR_TEST);
        //glScissor(_x, (_viewportSize.h - _size.h - _y), _size.w, _size.h);
        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        control::controlsScissors->pushScissor((float)_x, (float)_y, (float)_size.w, (float)_size.h);
        control::controlsScissors->enable();

        _backgroundRenderer->render(_texture, _currentColor);
        if (getIsFocused())
            _selectionRenderer->render(_texture, color::fromRGBA(0.0f, 0.0f, 0.9f, 1.0f));
        _textRenderer->render(_text, _font, _foregroundColor, color::transparent, vec2(_textX + _textOffsetX, _textY), _zIndex + 0.02f);
        if (getIsFocused())
            _cursorRenderer->render(_texture, color::fromRGBA(0.3f, 0.3f, 0.3f, 1.0f));

        control::controlsScissors->popScissor();
        control::controlsScissors->disable();
        //glDisable(GL_BLEND);
        //glDisable(GL_SCISSOR_TEST);
    }
}