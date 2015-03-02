#include <iostream>
#include "textBox.h"
#include <iostream>
#include <sstream>
#include <cctype>
#include "uiSystem.h"

namespace phi
{
    textBox::textBox(size<GLuint> viewportSize) : control(viewportSize)
    {
        _text = "";
        _texture = uiRepository::repository->getResource<texture>("button.png");
        _font = uiRepository::repository->getResource<font>("Consola_18");
        _backgroundRenderer = new quadRenderer2D(glm::vec2(0, 0), _zIndex, size<GLuint>(0, 0, 0), viewportSize);
        _cursorRenderer = new quadRenderer2D(glm::vec2(0, 0), _zIndex + 0.03f, size<GLuint>(CURSOR_WIDTH, _font->getLineHeight(), 0), viewportSize);
        _selectionRenderer = new quadRenderer2D(glm::vec2(0, 0), _zIndex + 0.01f, size<GLuint>(0.0f, 0.0f, 0.0f), viewportSize);
        _textRenderer = new textRenderer2D(viewportSize);
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
        DELETE(_backgroundRenderer);
        DELETE(_textRenderer);
    }

    void textBox::notifyTextChanged(eventArgs e)
    {
        if (_textChanged->isBound())
            _textChanged->invoke(e);
    }

    void textBox::updateTextLocation()
    {
        size<unsigned int> textSize = _textRenderer->measureSize(_text, _font);

        _textX = _x;
        _textY = (int)(_y + _size.height * 0.5f - textSize.height * 0.5f);
    }

    void textBox::updateCursorLocation()
    {
        std::string sub = _text.substr(0, _cursorIndex);
        size<unsigned int> subTextSize = _textRenderer->measureSize(sub, _font);

        float x = (float)_x + (float)subTextSize.width + (float)_textOffsetX; // Cast needed probably because of a float rounding problem (?)
        float y = _y + _size.height * 0.5f - _cursorRenderer->getSize().height * 0.5f;
        if (x > _x + _size.width - 1)
        {
            _textOffsetX += ((_x + _size.width -1) - x);
            x = _x + _size.width - 1;
        }

        if (x < _x)
        {
            _textOffsetX -= x - _x;
            x = _x;
        }

        //size<unsigned int> textSize = _textRenderer->measureSize(_text, _font);
        //if (textSize.width + _textOffsetX < _size.width && _textOffsetX < 0)
        //{
        //    _textOffsetX -= (textSize.width + _textOffsetX) - _size.width;
        //    x = _x + _size.width - 1;
        //}

        size<unsigned int> textSize = _textRenderer->measureSize(_text, _font);
        if (textSize.width < _size.width)
        {
            _textOffsetX = 0;
            x = _x + subTextSize.width;
        }
        else if (textSize.width + _textOffsetX < _size.width)
        {
            _textOffsetX -= (textSize.width + _textOffsetX) - _size.width;
            x = _x + _size.width - 1;
        }

        _cursorRenderer->setLocation(glm::vec2(x, y));
        _cursorRenderer->update();
    }

    void textBox::updateSelectionRenderer()
    {
        unsigned int start = glm::min(_selectionStartIndex, _selectionEndIndex);
        unsigned int end = glm::max(_selectionStartIndex, _selectionEndIndex);

        std::string sub = _text.substr(start, end - start);
        size<unsigned int> subTextSize = _textRenderer->measureSize(sub, _font);
        std::string pre = _text.substr(0, start);
        size<unsigned int> preTextSize = _textRenderer->measureSize(pre, _font);

        _selectionRenderer->setSize(size<GLuint>(subTextSize.width, _font->getLineHeight()));
        _selectionRenderer->setLocation(glm::vec2(_x + preTextSize.width + _textOffsetX, _y + _size.height * 0.5f - _font->getLineHeight() * 0.5f));
        _selectionRenderer->update();
    }

    void textBox::setX(int value)
    {
        _x = value;
        _backgroundRenderer->setLocation(glm::vec2(_x, _y));
        _backgroundRenderer->setZIndex(_zIndex);
        _backgroundRenderer->update();
        updateTextLocation();
    }

    void textBox::setY(int value)
    {
        _y = value;
        _backgroundRenderer->setLocation(glm::vec2(_x, _y));
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

    void textBox::setSize(size<GLuint> value)
    {
        _size = value;
        _backgroundRenderer->setSize(value);
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
        _cursorRenderer->setSize(size<GLuint>(CURSOR_WIDTH, _font->getLineHeight()));
        updateTextLocation();
        updateCursorLocation();
    }

    void textBox::setBackgroundColor(color value)
    {
        _backgroundColor = value;
        _currentColor = _backgroundColor;
    }

    void textBox::setViewportSize(size<GLuint> value)
    {
        control::setViewportSize(value);
        _backgroundRenderer->setViewportSize(getViewportSize());
        _backgroundRenderer->update();
        _cursorRenderer->setViewportSize(getViewportSize());
        _cursorRenderer->update();
        _selectionRenderer->setViewportSize(getViewportSize());
        _selectionRenderer->update();
        _textRenderer->setViewportSize(getViewportSize());
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
            _cursorIndex = _textRenderer->measureString(_text, _font, size<unsigned int>(glm::max(e->x - _x - _textOffsetX, 0), _size.height));
            updateCursorLocation();
            _selectionStartIndex = _cursorIndex;
            _selectionEndIndex = _cursorIndex;
            updateSelectionRenderer();
        }
        else
        {
            unsigned int index = _textRenderer->measureString(_text, _font, size<unsigned int>(glm::max(e->x - _x - _textOffsetX, 0), _size.height));
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
            _selectionEndIndex = _cursorIndex = _textRenderer->measureString(_text, _font, size<unsigned int>(glm::max(e->x - _x - _textOffsetX, 0), _size.height));
            updateCursorLocation();
            updateSelectionRenderer();
        }
    }

    void textBox::onMouseEnter(mouseEventArgs* e)
    {
        uiSystem::get()->setCursor(uiRepository::repository->getResource<cursor>("TextCursor"));
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
                _selectionStartIndex = _selectionEndIndex = _cursorIndex = glm::min(_cursorIndex + 1, _text.length());
            else
                _selectionEndIndex = _cursorIndex = glm::min(_cursorIndex + 1, _text.length());
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
                _selectionStartIndex = _selectionEndIndex = _cursorIndex = _text.length();
            else
                _selectionEndIndex = _cursorIndex = _text.length();
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

    void textBox::render()
    {
        glEnable(GL_SCISSOR_TEST);
        glScissor(_x, (_viewportSize.height - _size.height - _y), _size.width, _size.height);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        _backgroundRenderer->render(_texture, _currentColor);
        if (getIsFocused())
            _selectionRenderer->render(_texture, color::fromRGBA(0.0f, 0.0f, 0.9f, 1.0f));
        _textRenderer->render(_text, _font, _foregroundColor, color::transparent, glm::vec2(_textX + _textOffsetX, _textY), _zIndex + 0.02f);
        if (getIsFocused())
            _cursorRenderer->render(_texture, color::fromRGBA(0.3f, 0.3f, 0.3f, 1.0f));
        glDisable(GL_BLEND);
        glDisable(GL_SCISSOR_TEST);
    }
}