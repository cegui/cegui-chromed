/***********************************************************************
    filename:   CEGUIChromeWidget.cpp
    created:    7/1/2011
    author:     Martin Preisler
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2011 Martin Preisler
 *
 *   Permission is hereby granted, free of charge, to any person obtaining
 *   a copy of this software and associated documentation files (the
 *   "Software"), to deal in the Software without restriction, including
 *   without limitation the rights to use, copy, modify, merge, publish,
 *   distribute, sublicense, and/or sell copies of the Software, and to
 *   permit persons to whom the Software is furnished to do so, subject to
 *   the following conditions:
 *
 *   The above copyright notice and this permission notice shall be
 *   included in all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 *   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *   OTHER DEALINGS IN THE SOFTWARE.
 ***************************************************************************/

#include "CEGUIChromeWidget.h"
#include "CEGUIChromeSystem.h"

#include "CEGUIGeometryBuffer.h"
#include "CEGUIVertex.h"
#include "CEGUITexture.h"
#include "CEGUICoordConverter.h"

#include <berkelium/Berkelium.hpp>
#include <berkelium/Window.hpp>
#include <berkelium/WindowDelegate.hpp>
#include <berkelium/Rect.hpp>

#include <iostream>

namespace CEGUI
{

// the whole reason for this class is to avoid including Berkelium in the header
class BerkeliumDelegate :
    public Berkelium::WindowDelegate,
    public AllocatedObject<BerkeliumDelegate>
{
public:
    BerkeliumDelegate(ChromeWidget* target):
        d_target(target)
    {}

    ~BerkeliumDelegate()
    {}

    virtual void onPaint(
        Berkelium::Window *win,
        const unsigned char *sourceBuffer,
        const Berkelium::Rect &sourceBufferRect,
        size_t numCopyRects,
        const Berkelium::Rect *copyRects,
        int dx, int dy,
        const Berkelium::Rect &scrollRect)
    {
        d_target->onPaint(win, sourceBuffer, sourceBufferRect, numCopyRects, copyRects, dx, dy, scrollRect);
    }

    virtual void onUnresponsive(Window *win)
    {
        std::cout << "on unresponsive" << std::endl;
    }

    virtual void onResponsive(Window *win)
    {
        std::cout << "on responsive" << std::endl;
    }

private:
    ChromeWidget* d_target;
};

ChromeWidget::ChromeWidget(const String& type, const String& name):
    Window(type, name),

    d_interactionMode(IM_NoInteraction),
    d_renderingDetailRatio(1.0f),
    d_renderingResizeDelay(-1.0f),
    d_renderingCanvasReserve(0.0f),
    d_renderingCanvasMaxOverhead(0.2f),
    d_colourRect(Colour(1, 1, 1, 1)),

    d_renderingResizeTimer(-1.0f),
    d_renderingResizeNeeded(true),

    d_renderOutputTexture(0),
    d_scrollBuffer(CEGUI_NEW_ARRAY_PT(char, 1 * (1 + 1) * 4, AllocatorConfig<ChromeWidget>::Allocator)),
    d_ignorePartialPaint(true)
{
    ChromeSystem::ensureInitialised();

    d_chromeWindow = Berkelium::Window::create(ChromeSystem::getContext());
    d_berkeliumDelegate = CEGUI_NEW_AO BerkeliumDelegate(this);
    d_chromeWindow->setDelegate(d_berkeliumDelegate);
    // this has to be done to ensure Berkelium paints at all
    // (ever, if you do this after navigate/loadContent, it won't work)
    d_chromeWindow->resize(1, 1);

    const String propertyOrigin("ChromeWidget");
    
    // property definition
    CEGUI_DEFINE_PROPERTY(ChromeWidget, InteractionMode, "InteractionMode", 
        "Allows to set whether the widget is interactive (reacts to mouse and keyboard input) or just visual.",
        &ChromeWidget::setInteractionMode,
        &ChromeWidget::getInteractionMode,
        IM_NoInteraction
    );

    CEGUI_DEFINE_PROPERTY(ChromeWidget, float, "RenderingDetailRatio",
        "Rendering detail ratio describes how big the chrome rendering canvas should be related "
        "to the widget's size. For example for ratio 2.0 the canvas will be 2x bigger than the actual widget's "
        "pixel size (this is not really practical), however with ratio 0.5 the canvase's sides will be 2x "
        "smaller, resulting in 4x smaller area which can speed things up considerably",
        &ChromeWidget::setRenderingDetailRatio,
        &ChromeWidget::getRenderingDetailRatio,
        1.0f
    );

    CEGUI_DEFINE_PROPERTY(ChromeWidget, float, "RenderingResizeDelay",
        "Higher values will speed things like drag resizing but rendering can be distorted/of low quality at times. "
        "The default behaviour is to size the rendering the first time update is called and leave it (-1.0f value)",
        &ChromeWidget::setRenderingResizeDelay,
        &ChromeWidget::getRenderingResizeDelay,
        -1.0f
    );

    CEGUI_DEFINE_PROPERTY(ChromeWidget, float, "RenderingCanvasReserve",
        "You want to leave this at 0.0 for widgets that will never resize, it helps to have reserve with often resizing widgets. "
        "Be aware that this must play well with RenderingCanvasOverhead because it his ratio is bigger than the allowed overhead, "
        "you will trigger an infinite loop! And you have to make sure that doesn't happen, there are no checks for this!!",
        &ChromeWidget::setRenderingCanvasReserve,
        &ChromeWidget::getRenderingCanvasReserve,
        0.0f
    );

    CEGUI_DEFINE_PROPERTY(ChromeWidget, float, "RenderingCanvasMaxOverhead",
        "Default is 0.2, meaning if the texture is 20% bigger than it should be (any of its sizes), we recreate it with a smaller size. "
        "Be aware that this must play well with RenderingCanvasOverhead because it his ratio is bigger than the allowed overhead, "
        "you will trigger an infinite loop! And you have to make sure that doesn't happen, there are no checks for this!!",
        &ChromeWidget::setRenderingCanvasMaxOverhead,
        &ChromeWidget::getRenderingCanvasMaxOverhead,
        0.2f
    );

    CEGUI_DEFINE_PROPERTY(ChromeWidget, ColourRect, "ColourRect",
        "sets the colour rect that will affect the rendering (just like any other CEGUI widget)",
        &ChromeWidget::setColourRect,
        &ChromeWidget::getColourRect,
        ColourRect(Colour(1, 1, 1, 1))
    );
}

ChromeWidget::~ChromeWidget()
{
    if (d_renderOutputTexture)
    {
        System::getSingleton().getRenderer()->destroyTexture(*d_renderOutputTexture);
        d_renderOutputTexture = 0;
    }

    d_chromeWindow->setDelegate(0);
    CEGUI_DELETE_AO d_berkeliumDelegate;
    d_berkeliumDelegate = 0;

    CEGUI_DELETE_AO d_chromeWindow;
    d_chromeWindow = 0;

    CEGUI_DELETE_AO [] d_scrollBuffer;
    d_scrollBuffer = 0;
}

void ChromeWidget::setInteractionMode(InteractionMode mode)
{
    d_interactionMode = mode;
}

ChromeWidget::InteractionMode ChromeWidget::getInteractionMode() const
{
    return d_interactionMode;
}

void ChromeWidget::setTransparencyEnabled(bool enabled)
{
    d_chromeWindow->setTransparent(enabled);
}

void ChromeWidget::setRenderingDetailRatio(float ratio)
{
    d_renderingDetailRatio = ratio;

    resizeRenderingCanvas();
}

float ChromeWidget::getRenderingDetailRatio() const
{
    return d_renderingDetailRatio;
}

void ChromeWidget::setRenderingResizeDelay(float seconds)
{
    d_renderingResizeDelay = seconds;
}

float ChromeWidget::getRenderingResizeDelay() const
{
    return d_renderingResizeDelay;
}

void ChromeWidget::setRenderingCanvasReserve(float ratio)
{
    d_renderingCanvasReserve = ratio;
    // process it when we next render
    d_renderingResizeNeeded = true;
}

float ChromeWidget::getRenderingCanvasReserve() const
{
    return d_renderingCanvasReserve;
}

void ChromeWidget::setRenderingCanvasMaxOverhead(float ratio)
{
    d_renderingCanvasMaxOverhead = ratio;
    // process it when we next render
    d_renderingResizeNeeded = true;
}

float ChromeWidget::getRenderingCanvasMaxOverhead() const
{
    return d_renderingCanvasMaxOverhead;
}

void ChromeWidget::setColourRect(const ColourRect& rect)
{
    d_colourRect = rect;

    invalidate();
}

ColourRect ChromeWidget::getColourRect() const
{
    return d_colourRect;
}

void ChromeWidget::populateGeometryBuffer()
{
    if (!d_renderOutputTexture)
    {
        resizeRenderingCanvas();
    }

    Vertex vbuffer[6];

    Sizef pixelSize = getPixelSize();
    const Sizef alteredPixelSize = pixelSize * d_renderingDetailRatio;

    ColourRect colourRect(d_colourRect);
    colourRect.modulateAlpha(getEffectiveAlpha());

    if (alteredPixelSize.d_width * alteredPixelSize.d_height == 0)
    {
        d_geometry->reset();
        return; // guard from division by zero, also it doesn't really make sense to render anyways
    }

    const float rightUV = alteredPixelSize.d_width / d_renderOutputTexture->getSize().d_width;
    const float bottomUV = alteredPixelSize.d_height / d_renderOutputTexture->getSize().d_height;

    // vertex 0 - top left
    vbuffer[0].position   = Vector3f(0.0f, 0.0f, 0.0f);
    vbuffer[0].colour_val = colourRect.d_top_left;
    vbuffer[0].tex_coords = Vector2f(0.0f, 0.0f);

    // vertex 1 - bottom left
    vbuffer[1].position   = Vector3f(0.0f, pixelSize.d_height, 0.0f);
    vbuffer[1].colour_val = colourRect.d_bottom_left;
    vbuffer[1].tex_coords = Vector2f(0.0f, bottomUV);

    // vertex 2 - bottom right
    vbuffer[2].position   = Vector3f(pixelSize.d_width, pixelSize.d_height, 0.0f);
    vbuffer[2].colour_val = colourRect.d_bottom_right;
    vbuffer[2].tex_coords = Vector2f(rightUV, bottomUV);

    // vertex 3 - top right
    vbuffer[3].position   = Vector3f(pixelSize.d_width, 0.0f, 0.0f);
    vbuffer[3].colour_val = colourRect.d_top_right;
    vbuffer[3].tex_coords = Vector2f(rightUV, 0.0f);

    // vertex 4 - top left
    vbuffer[4].position   = Vector3f(0.0f, 0.0f, 0.0f);
    vbuffer[4].colour_val = colourRect.d_top_left;
    vbuffer[4].tex_coords = Vector2f(0.0f, 0.0f);

    // vertex 5 - bottom right
    vbuffer[5].position   = Vector3f(pixelSize.d_width, pixelSize.d_height, 0.0f);
    vbuffer[5].colour_val = colourRect.d_bottom_right;
    vbuffer[5].tex_coords = Vector2f(rightUV, bottomUV);

    d_geometry->reset();
    d_geometry->setActiveTexture(d_renderOutputTexture);
    d_geometry->appendGeometry(vbuffer, 6);
}

void ChromeWidget::onPaint(
        Berkelium::Window *win,
        const unsigned char *sourceBuffer,
        const Berkelium::Rect &sourceBufferRect,
        size_t numCopyRects,
        const Berkelium::Rect *copyRects,
        int dx, int dy,
        const Berkelium::Rect &scrollRect)
{
    const int bytesPerPixel = 4;

    if (!d_renderOutputTexture)
    {
        resizeRenderingCanvas();
    }

    const Sizef alteredPixelSize = getPixelSize() * d_renderingDetailRatio;
    const Sizef textureSize = d_renderOutputTexture->getSize();

    // modified from the GLUT demo from Berkelium source

    if (d_ignorePartialPaint)
    {
        if (sourceBufferRect.left() == 0 &&
            sourceBufferRect.top() == 0 &&
            sourceBufferRect.right() == static_cast<size_t>(floor(alteredPixelSize.d_width)) &&
            sourceBufferRect.bottom() == static_cast<size_t>(floor(alteredPixelSize.d_height)))
        {
            d_renderOutputTexture->blitFromMemory(const_cast<unsigned char*>(sourceBuffer),
                Rectf(sourceBufferRect.left(), sourceBufferRect.top(), sourceBufferRect.right(), sourceBufferRect.bottom()));
            d_ignorePartialPaint = false;
        }

        return;
    }

    if (dx != 0 || dy != 0)
    {
        // scroll_rect contains the Rect we need to move
        // First we figure out where the the data is moved to by translating it
        Berkelium::Rect scrolledRect = scrollRect.translate(-dx, -dy);
        // Next we figure out where they intersect, giving the scrolled
        // region
        Berkelium::Rect scrolledSharedRect = scrollRect.intersect(scrolledRect);
        // Only do scrolling if they have non-zero intersection
        if (scrolledSharedRect.width() > 0 && scrolledSharedRect.height() > 0)
        {
            // And the scroll is performed by moving shared_rect by (dx,dy)
            Berkelium::Rect sharedRect = scrolledSharedRect.translate(dx, dy);

            const int wid = scrolledSharedRect.width();
            const int hig = scrolledSharedRect.height();
            
            int inc = 1;
            char* outputBuffer = d_scrollBuffer;
            // source data is offset by 1 line to prevent memcpy aliasing
            // In this case, it can happen if dy == 0 and dx != 0.
            char* inputBuffer = d_scrollBuffer + (static_cast<size_t>(textureSize.d_width) * 1 * bytesPerPixel);
            int jj = 0;
            if (dy > 0)
            {
                // Here, we need to shift the buffer around so that we start in the
                // extra row at the end, and then copy in reverse so that we
                // don't clobber source data before copying it.
                outputBuffer = d_scrollBuffer + (
                    (scrolledSharedRect.top() + hig + 1) * static_cast<size_t>(textureSize.d_width)
                    - hig * wid) * bytesPerPixel;
                inputBuffer = d_scrollBuffer;
                inc = -1;
                jj = hig-1;
            }

            d_renderOutputTexture->blitToMemory(static_cast<char*>(inputBuffer));

            // Annoyingly, OpenGL doesn't provide convenient primitives, so
            // we manually copy out the region to the beginning of the
            // buffer
            for(; jj < hig && jj >= 0; jj += inc) {
                memcpy(
                    outputBuffer + (jj * wid) * bytesPerPixel,
                    inputBuffer +
                        ((scrolledSharedRect.top() + jj) * static_cast<size_t>(textureSize.d_width)
                        + scrolledSharedRect.left()) * bytesPerPixel,
                    wid * bytesPerPixel
                );
            }

            d_renderOutputTexture->blitFromMemory(outputBuffer,
                Rectf(sharedRect.left(), sharedRect.top(), sharedRect.right(), sharedRect.bottom()));
        }
    }
    
    for (size_t i = 0; i < numCopyRects; i++) {
        int wid = copyRects[i].width();
        int hig = copyRects[i].height();
        int top = copyRects[i].top() - sourceBufferRect.top();
        int left = copyRects[i].left() - sourceBufferRect.left();
        
        for(int jj = 0; jj < hig; jj++) {
            memcpy(
                d_scrollBuffer + jj * wid * bytesPerPixel,
                sourceBuffer + (left + (jj + top) * sourceBufferRect.width()) * bytesPerPixel,
                wid * bytesPerPixel
            );
        }

        d_renderOutputTexture->blitFromMemory(d_scrollBuffer,
            Rectf(copyRects[i].left(), copyRects[i].top(), copyRects[i].right(), copyRects[i].bottom()));
    }

    d_ignorePartialPaint = false;
}

void ChromeWidget::onActivated(ActivationEventArgs& e)
{
    Window::onActivated(e);

    d_chromeWindow->focus();
}

void ChromeWidget::onDeactivated(ActivationEventArgs& e)
{
    Window::onDeactivated(e);

    d_chromeWindow->unfocus();
}

void ChromeWidget::onSized(WindowEventArgs& e)
{
    Window::onSized(e);

    if (d_renderingResizeDelay > 0)
    {
        d_renderingResizeTimer = 0.0f;
    }
    else if (d_renderingResizeDelay == 0.0f)
    {
        resizeRenderingCanvas();
    }
    else
    {
        // rendering resizing isn't enabled
    }
}

void ChromeWidget::onMouseMove(MouseEventArgs& e)
{
    Window::onMouseMove(e);

    if (d_interactionMode == IM_MouseOnlyInteraction ||
        d_interactionMode == IM_FullInteraction)
    {
        // we have to substract the absolute widget position from the absolute mouse position
        // to get the relative mouse position
        Vector2f mousePosition(e.position - getUnclippedInnerRect().getPosition());

        // fix up the position if render output size and real widget size differ
        mousePosition.d_x *= d_renderingDetailRatio;
        mousePosition.d_y *= d_renderingDetailRatio;

        d_chromeWindow->mouseMoved(mousePosition.d_x, mousePosition.d_y);
    }
}

void ChromeWidget::onMouseButtonDown(MouseEventArgs& e)
{
    Window::onMouseButtonDown(e);

    if (d_interactionMode == IM_MouseOnlyInteraction ||
        d_interactionMode == IM_FullInteraction)
    {
        switch (e.button)
        {
        case LeftButton:
            d_chromeWindow->mouseButton(0, true);
            break;
        case MiddleButton:
            d_chromeWindow->mouseButton(1, true);
            break;
        case RightButton:
            d_chromeWindow->mouseButton(2, true);
            break;
        }
    }
}

void ChromeWidget::onMouseButtonUp(MouseEventArgs& e)
{
    Window::onMouseButtonUp(e);

    if (d_interactionMode == IM_MouseOnlyInteraction ||
        d_interactionMode == IM_FullInteraction)
    {
        switch (e.button)
        {
        case LeftButton:
            d_chromeWindow->mouseButton(0, false);
            break;
        case MiddleButton:
            d_chromeWindow->mouseButton(1, false);
            break;
        case RightButton:
            d_chromeWindow->mouseButton(2, false);
            break;
        }
    }
}

void ChromeWidget::updateSelf(float elapsed)
{
    Window::updateSelf(elapsed);

    // sync Berkelium processes
    ChromeSystem::update();

    if (d_renderingResizeDelay > 0.0 && d_renderingResizeTimer >= 0.0)
    {
        d_renderingResizeTimer += elapsed;

        if (d_renderingResizeTimer >= d_renderingResizeDelay)
        {
            d_renderingResizeTimer = -1.0f;
            d_renderingResizeNeeded = true;
        }
    }
}

void ChromeWidget::drawSelf(const RenderingContext& ctx)
{
    if (d_renderingResizeNeeded)
    {
        resizeRenderingCanvas();
    }

    Window::drawSelf(ctx);
}

void ChromeWidget::resizeRenderingCanvas()
{
    //const Size pixelSize = getPixelSize();
    const Sizef alteredPixelSize = getPixelSize() * d_renderingDetailRatio;
    size_t oldScrollBufferSize = 1 * (1 + 1) * 4;

    Renderer* renderer = System::getSingleton().getRenderer();

    if (d_renderOutputTexture)
    {
        const Sizef size = d_renderOutputTexture->getSize();
        oldScrollBufferSize = size.d_width * (size.d_height + 1) * 4;

        if (size.d_width < alteredPixelSize.d_width ||
            size.d_height < alteredPixelSize.d_height)
        {
            // we will have to recreate the texture because the canvas won't fit anymore
            renderer->destroyTexture(*d_renderOutputTexture);
            d_renderOutputTexture = 0;
        }
        else if (size.d_width > alteredPixelSize.d_width * (1.0f + d_renderingCanvasMaxOverhead) ||
                 size.d_height > alteredPixelSize.d_height * (1.0f + d_renderingCanvasMaxOverhead))
        {
            // this time the texture is too big, max overhead has been surpassed
            renderer->destroyTexture(*d_renderOutputTexture);
            d_renderOutputTexture = 0;
        }
    }

    if (!d_renderOutputTexture)
    {
        const Sizef texSize = alteredPixelSize * (1.0f + d_renderingCanvasReserve);
        d_renderOutputTexture = &renderer->createTexture(getName() + "/Texture", texSize);

        CEGUI_DELETE_ARRAY_PT(d_scrollBuffer, char, oldScrollBufferSize, AllocatorConfig<ChromeWidget>::Allocator);
        // FIXME: Size<int>
        d_scrollBuffer = CEGUI_NEW_ARRAY_PT(char, static_cast<unsigned int>(texSize.d_width * (texSize.d_height + 1) * 4), AllocatorConfig<ChromeWidget>::Allocator);
    }

    // 1, 1 to force a full redraw (we destroyed the old texture, so we lost all data)
    d_chromeWindow->resize(1, 1);

    // I do floor(..) to ensure we never ever overflow our target texture
    d_chromeWindow->resize(floor(alteredPixelSize.d_width),
                           floor(alteredPixelSize.d_height));

    invalidate();

    // reset timer and the flag
    d_renderingResizeTimer = -1.0f;
    d_renderingResizeNeeded = false;
}

void ChromeWidget::base64_encode(String& ret, uint8 const* bytes_to_encode, size_t in_len)
{
/* 
    base64.cpp and base64.h

    Copyright (C) 2004-2008 René Nyffenegger

    This source code is provided 'as-is', without any express or implied
    warranty. In no event will the author be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this source code must not be misrepresented; you must not
        claim that you wrote the original source code. If you use this source code
        in a product, an acknowledgment in the product documentation would be
        appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
        misrepresented as being the original source code.

    3. This notice may not be removed or altered from any source distribution.

    René Nyffenegger rene.nyffenegger@adp-gmbh.ch

    With very minor changes by Martin Preisler
*/

    static const String base64_chars("ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                     "abcdefghijklmnopqrstuvwxyz"
                                     "0123456789+/");

    // we don't clear since we want the headers to stay in ret
    //ret.clear();
    // the size will definitely be at least size of the data,
    // furthermore we are aiming for the average case overhead
    // which is 37%
    ret.reserve(static_cast<String::size_type>(ceil(in_len * 1.37f)));

    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    while (in_len--)
    {
        char_array_3[i++] = *(bytes_to_encode++);
        if (i == 3)
        {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for(i = 0; (i <4) ; i++)
                ret += base64_chars[char_array_4[i]];

            i = 0;
        }
    }

    if (i)
    {
        for(j = i; j < 3; j++)
            char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (j = 0; (j < i + 1); j++)
            ret += base64_chars[char_array_4[j]];

        while((i++ < 3))
            ret += '=';
    }
}

}
