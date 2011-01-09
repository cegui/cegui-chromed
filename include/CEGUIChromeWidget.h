/***********************************************************************
    filename:   CEGUIChromeWidget.h
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

#ifndef _CEGUIChromeWidget_h_
#define _CEGUIChromeWidget_h_

#include "CEGUIChromePrerequisites.h"
#include "CEGUIWindow.h"

namespace Berkelium
{
    class Window;
    struct Rect;
}

namespace CEGUI
{

class BerkeliumDelegate;

/*!
\brief
    This is the base class of all widgets using Chrome rendering engine
*/
class CHROMED_CEGUI_API ChromeWidget : public Window
{
public:
    enum InteractionMode
    {
        IM_NoInteraction, //!< No interaction at all, the widget will be visual only
        IM_MouseOnlyInteraction, //!< Just mouse interaction, no keyboard events will be passed to Chrome
        IM_KeyboardOnlyInteraction, //!< You still have to click the widget to gain focus!
        IM_FullInteraction //!< Mouse and Keyboard interaction combined
    };

    /*!
    \brief Constructor
    */
    ChromeWidget(const String& type, const String& name);

    /*!
    \brief Destructor
    */
    virtual ~ChromeWidget();

    /*!
    \brief Allows to set whether the widget is interactive (reacts to mouse and keyboard input) or just visual

    \param mode
        which interaction mode should be used
    */
    virtual void setInteractionMode(InteractionMode mode);

    /*!
    \brief Checks which interaction mode is current active with this widget

    \see ChromeWidget::setInteractionMode
    */
    InteractionMode getInteractionMode() const;

    /*!
    \brief Enables/Disables transparent backgrounds for any Chrome content

    \param enabled
        if true, transparent background is enabled
        if false, transparent background is disabled (default)
    */
    virtual void setTransparencyEnabled(bool enabled);

    /*!
    \brief sets the rendering detail ratio

    \par
        Rendering detail ratio describes how big the chrome rendering canvas should be related
        to the widget's size. For example for ratio 2.0 the canvas will be 2x bigger than the actual widget's
        pixel size (this is not really practical), however with ratio 0.5 the canvase's sides will be 2x
        smaller, resulting in 4x smaller area which can speed things up considerably

    \par
        The default value is 1.0 = 100%
    */
    virtual void setRenderingDetailRatio(float ratio);

    /*!
    \brief gets the current rendering detail ratio
    */
    float getRenderingDetailRatio() const;

    /*!
    \brief Sets the time in which the rendering will be resized to match widget's size

    \param seconds Time in seconds that describes the delay in resizing (0 is immediate, negative means never resize)

    Higher values will speed things like drag resizing but rendering can be distorted/of low quality at times.
    The default behaviour is to size the rendering the first time update is called and leave it (seconds is negative)
    */
    virtual void setRenderingResizeDelay(float seconds);

    /*!
    \brief gets the render resize delay

    \return the delay in seconds
    \see ChromeWidget::setRenderResizeDelay
    */
    float getRenderingResizeDelay() const;

    /*!
    \brief how big a reserve should be allocated as a ratio

    You want to leave this at 0.0 for widgets that will never resize, it helps to have reserve with often resizing widgets.

    Be aware that this must play well with RenderingCanvasOverhead because it his ratio is bigger than the allowed overhead,
    you will trigger an infinite loop! And you have to make sure that doesn't happen, there are no checks for this!!

    \param ratio
        The actual reserve allocated is pixel size * ratio. If this is 0.1, 10% more than necessary is allocated. Defaults to 0.0, no reserve.
    */
    virtual void setRenderingCanvasReserve(float ratio);

    /*!
    \brief retrieves the rendering canvas reserve ratio
    */
    float getRenderingCanvasReserve() const;

    /*!
    \brief how big an overhead is tolerated before our canvas texture will be recreated to shrink it

    Default is 0.2, meaning if the texture is 20% bigger than it should be (any of its sizes), we recreate it with a smaller size

    Be aware that this must play well with RenderingCanvasOverhead because it his ratio is bigger than the allowed overhead,
    you will trigger an infinite loop! And you have to make sure that doesn't happen, there are no checks for this!!

    \param ratio
        The actual max overhead allowed is pixel size * ratio. If this is 0.2, 20% more than necessary is still allowed, any more and we will shrink the texture.
    */
    virtual void setRenderingCanvasMaxOverhead(float ratio);

    /*!
    \brief retrieves the rendering canvas max overhead
    */
    float getRenderingCanvasMaxOverhead() const;

    /*!
    \brief sets the colour rect that will affect the rendering (just like any other CEGUI widget)
    */
    virtual void setColourRect(const ColourRect& rect);

    /*!
    \brief retrieves the colour rect

    \see ChromeWidget::setColourRect
    */
    ColourRect getColourRect() const;

    //! \copydoc Window::populateGeometryBuffer
    virtual void populateGeometryBuffer();

    /*!
    \brief Internal, don't use!
    */
    void onPaint(
        Berkelium::Window *win,
        const unsigned char *sourceBuffer,
        const Berkelium::Rect &sourceBufferRect,
        size_t numCopyRects,
        const Berkelium::Rect *copyRects,
        int dx, int dy,
        const Berkelium::Rect &scrollRect);

protected:
    //! \copydoc Window::onSized
    virtual void onSized(WindowEventArgs& e);

    //! \copydoc Window::onMouseMove
    virtual void onMouseMove(MouseEventArgs& e);

    //! \copydoc Window::onMouseButtonDown
    virtual void onMouseButtonDown(MouseEventArgs& e);

    //! \copydoc Window::onMouseButtonUp
    virtual void onMouseButtonUp(MouseEventArgs& e);

    //! \copydoc Window::updateSelf
    virtual void updateSelf(float elapsed);

    //! \copydoc Window::drawSelf
    virtual void drawSelf(const RenderingContext& ctx);

    //! which interaction mode is currently used
    InteractionMode d_interactionMode;
    //! rendering detail ratio
    float d_renderingDetailRatio;
    //! rendering resize delay in seconds
    float d_renderingResizeDelay;
    //! rendering canvas size reserve
    float d_renderingCanvasReserve;
    //! rendering canvas size max overhead
    float d_renderingCanvasMaxOverhead;
    //! colour rect affecting the output
    ColourRect d_colourRect;

    //! timer to handle rendering resizes
    float d_renderingResizeTimer;
    //! if true the rendering resize will happen next render call
    bool d_renderingResizeNeeded;

    //! where should chrome output to
    Texture* d_renderOutputTexture;
    //! Berkelium window that does all the dirty (and hard) work
    Berkelium::Window* d_chromeWindow;
    //! the delegate that blits the texture (basically pimpl)
    BerkeliumDelegate* d_berkeliumDelegate;
    //! a buffer we use to store scroll data when painting the canvas
    char* d_scrollBuffer;
    //! if true, we will ignore partial canvas painting and only let full repaint through
    bool d_ignorePartialPaint;

    /*!
    \brief
        Internal method, immediately resizes the rendering to match widget's size
    */
    virtual void resizeRenderingCanvas();

    //! internal method used for the data URI
    static void base64_encode(String& ret, uint8 const* bytes_to_encode, size_t in_len);

	/*!
	\brief
		Return whether this window was inherited from the given class name at some point in the inheritance hierarchy.

	\param class_name
		The class name that is to be checked.

	\return
		true if this window was inherited from \a class_name. false if not.
	*/
	virtual bool testClassName_impl(const String& class_name) const
	{
		if (class_name=="ChromeWidget")
            return true;

		return Window::testClassName_impl(class_name);
	}
};

template<>
class PropertyHelper<ChromeWidget::InteractionMode>
{
public:
    typedef ChromeWidget::InteractionMode return_type;
    typedef ChromeWidget::InteractionMode pass_type;
    typedef String string_return_type;

    static const String& getDataTypeName()
    {
        static String type("ChromeWidget_InteractionMode");

        return type;
    }

    static return_type fromString(const String& str)
    {
        if (str == "MouseOnlyInteraction")
        {
            return ChromeWidget::IM_MouseOnlyInteraction;
        }
        else if (str == "KeyboardOnlyInteraction")
        {
            return ChromeWidget::IM_KeyboardOnlyInteraction;
        }
        else if (str == "FullInteraction")
        {
            return ChromeWidget::IM_FullInteraction;
        }
        else
        {
            return ChromeWidget::IM_NoInteraction;
        }
    }

    static string_return_type toString(pass_type val)
    {
        switch (val)
        {
        case ChromeWidget::IM_NoInteraction:
            return "NoInteraction";
            break;
        case ChromeWidget::IM_MouseOnlyInteraction:
            return "MouseOnlyInteraction";
            break;
        case ChromeWidget::IM_KeyboardOnlyInteraction:
            return "KeyboardOnlyInteraction";
            break;
        case ChromeWidget::IM_FullInteraction:
            return "FullInteraction";
            break;

        default:
            return "NoInteraction";
            break;
        }
    }
};

}

#endif
