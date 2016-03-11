#pragma once
#include <phi.h>

#include "core\eventArgs.h"
#include "rendering\quadRenderer2D.h"
#include "rendering\textRenderer2D.h"

#include "control.h"

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
        unsigned int _selectionStartIndex;
        unsigned int _selectionEndIndex;
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
        textBox(sizeui viewportSize);
        ~textBox();

        std::string getText() { return _text; }
        color getBackgroundColor() { return _backgroundColor; }
        color getForegroundColor() { return _foregroundColor; }
        font* getFont() { return _font; }
        eventHandler<eventArgs>* getTextChanged() { return _textChanged; }

        void setText(std::string value);
        void setBackgroundColor(color value);
        void setForegroundColor(color value) { _foregroundColor = value; }
        void setFont(font* value);

        void setX(int value) override;
        void setY(int value) override;
        void setZIndex(float value) override;
        void setSize(sizeui value) override;
        void setViewportSize(sizeui value) override;

        void onMouseDown(mouseEventArgs* e) override;
        void onMouseUp(mouseEventArgs* e) override;
        void onMouseMove(mouseEventArgs* e) override;
        void onMouseEnter(mouseEventArgs* e) override;
        void onKeyDown(keyboardEventArgs e) override;

        void onRender() override;
    };
}