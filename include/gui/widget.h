#ifndef __MYOS__GUI__WIDGET__H
#define __MYOS__GUI__WIDGET__H

#include <common/types.h>
#include <common/graphicscontext.h>
#include <drivers/keyboard.h>

namespace myos
{
    namespace gui
    {
        class Widget : public myos::drivers::KeyboardEventHandler
        {
            protected:
                Widget* parent;
                myos::common::uint32_t x;
                myos::common::uint32_t y;
                myos::common::uint32_t w;
                myos::common::uint32_t h;

                myos::common::uint8_t r;
                myos::common::uint8_t g;
                myos::common::uint8_t b;

                bool Focussable;

            public:
                Widget();
                Widget(Widget* parent, myos::common::uint32_t x, myos::common::uint32_t y, myos::common::uint32_t w, myos::common::uint32_t h, myos::common::uint8_t r, myos::common::uint8_t g, myos::common::uint8_t b);
                ~Widget();

                virtual void GetFocus(Widget* widget);
                virtual void ModelToScreen(myos::common::int32_t &x, myos::common::int32_t &y);

                virtual void Draw(myos::common::GraphicsContext* gc);
                virtual void OnMouseDown(myos::common::int32_t x, myos::common::int32_t y, myos::common::uint8_t button);
                virtual void OnMouseUp(myos::common::int32_t x, myos::common::int32_t y, myos::common::uint8_t button);
                virtual void OnMouseMove(myos::common::int32_t oldx, myos::common::int32_t oldy, myos::common::int32_t newx, myos::common::int32_t newy);

                virtual bool ContainsCoordinate(myos::common::int32_t x, myos::common::int32_t y);
        };

        class CompositeWidget : public Widget
        {
            private:
                Widget* children[100];
                int numChildren;
                Widget* focussedChild;

            public:
                CompositeWidget(Widget* parent, myos::common::uint32_t x, myos::common::uint32_t y, myos::common::uint32_t w, myos::common::uint32_t h, myos::common::uint8_t r, myos::common::uint8_t g, myos::common::uint8_t b);
                ~CompositeWidget();

                virtual void GetFocus(Widget* widget);
                virtual bool AddChild(Widget* child);

                virtual void Draw(myos::common::GraphicsContext* gc);
                virtual void OnMouseDown(myos::common::int32_t x, myos::common::int32_t y, myos::common::uint8_t button);
                virtual void OnMouseUp(myos::common::int32_t x, myos::common::int32_t y, myos::common::uint8_t button);
                virtual void OnMouseMove(myos::common::int32_t oldx, myos::common::int32_t oldy, myos::common::int32_t newx, myos::common::int32_t newy);

                void OnKeyDown(char character);
                void OnKeyUp(char character);

        };
    }
}

#endif