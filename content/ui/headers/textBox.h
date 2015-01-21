#ifndef _PHI_TEXT_BOX_H_
#define _PHI_TEXT_BOX_H_

#include <string>
#include "globals.h"
#include "control.h"
#include "quadRenderer2D.h"
#include "textRenderer2D.h"
#include "eventArgs.h"

namespace phi
{
    class textBox :
        public control
    {
    private:
        static const unsigned int CURSOR_WIDTH = 1;

        std::string _text;
        font* _font;
        int _textX;
        int _textY;
        int _textOffsetX;
        int _selectionStartIndex;
        int _selectionEndIndex;
        bool _isClickingOver;
        unsigned int _cursorIndex;
        color _backgroundColor;
        color _foregroundColor;
        color _currentColor;
        texture* _texture;
        quadRenderer2D* _backgroundRenderer;
        quadRenderer2D* _cursorRenderer;
        quadRenderer2D* _selectionRenderer;
        textRenderer2D* _textRenderer;
        eventHandler<eventArgs>* _textChanged;

    private:
        void notifyTextChanged(eventArgs e);

        void updateTextLocation();
        void updateCursorLocation();
        void updateSelectionRenderer();

        void deleteSelection();

    public:
        UI_API textBox(size<GLuint> viewportSize);
        UI_API ~textBox();

        UI_API std::string getText() { return _text; }
        UI_API color getBackgroundColor() { return _backgroundColor; }
        UI_API color getForegroundColor() { return _foregroundColor; }
        UI_API font* getFont() { return _font; }
        UI_API eventHandler<eventArgs>* getTextChanged() { return _textChanged; }

        UI_API void setText(std::string value);
        UI_API void setBackgroundColor(color value);
        UI_API void setForegroundColor(color value) { _foregroundColor = value; }
        UI_API void setFont(font* value);

        UI_API void setX(int value) override;
        UI_API void setY(int value) override;
        UI_API void setZIndex(float value) override;
        UI_API void setSize(size<GLuint> value) override;
        UI_API void setViewportSize(size<GLuint> value) override;

        UI_API void onMouseDown(mouseEventArgs e) override;
        UI_API void onMouseUp(mouseEventArgs e) override;
        UI_API void onMouseMove(mouseEventArgs e) override;
        UI_API void onMouseEnter(mouseEventArgs e) override;
        UI_API void onKeyDown(keyboardEventArgs e) override;

        UI_API void render() override;
    };
}

#endif