/***********************************************************************
    filename:   CEGUIChromeHTML.h
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

#ifndef _CEGUIChromeHTML_h_
#define _CEGUIChromeHTML_h_

#include "CEGUIChromePrerequisites.h"
#include "CEGUIChromeWidget.h"

namespace CEGUI
{

class CHROMED_CEGUI_API ChromeHTML : public ChromeWidget
{
public:
    // type name for this widget
    static const String WidgetTypeName; //!< The unique typename of this widget

    /*!
    \brief Constructor
    */
    ChromeHTML(const String& type, const String& name);

    /*!
    \brief Destructor
    */
    virtual ~ChromeHTML();
    
    /*!
    \brief
        Fetches content off the interwebz

    \param URI
        where is the content located (for example: http://www.google.com, about:blank, ...)
    */
    void fetchContent(const String& URI);

    /*!
    \brief
        Loads content XHTML/HTML code from given file

    \param filename
        name of the source file
    \param resourceGroup
        Resource group identifier to be passed to the resource manager when
        loading the image file.
    */
    void loadContentFromFile(const String& filename, const String& resourceGroup = "");

    /*!
    \brief
        Sets the content XHTML/HTML code directly

    \param markupCode
        Entire XHTML/HTML markup to set as content of this widget
    */
    void setContent(const String& markupCode);

protected:
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
		if (class_name=="ChromeHTML")
            return true;

		return ChromeWidget::testClassName_impl(class_name);
	}
};

}

#endif
