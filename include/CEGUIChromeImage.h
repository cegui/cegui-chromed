/***********************************************************************
    filename:   CEGUIChromeImage.h
    created:    8/1/2011
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

#ifndef _CEGUIChromeImage_h_
#define _CEGUIChromeImage_h_

#include "CEGUIChromeWidget.h"

namespace CEGUI
{

class CHROMED_CEGUI_API ChromeImage : public ChromeWidget
{
public:
    // type name for this widget
    static const String WidgetTypeName; //!< The unique typename of this widget

    /*!
    \brief Constructor
    */
    ChromeImage(const String& type, const String& name);

    /*!
    \brief Destructor
    */
    virtual ~ChromeImage();

    /*!
    \brief
        Fetches image off the interwebz

    \param URI
        where is the content located (for example: http://www.domain.com/someImage.png)
    */
    void fetchImage(const String& URI);

    /*!
    \brief
        Loads content Image from given file

    This method tries to guess the mime type based on image extension.

    \param filename
        name of the source file
    \param resourceGroup
        Resource group identifier to be passed to the resource manager when
        loading the image file.
    */
    void loadFromFile(const String& filename, const String& resourceGroup = "");

    /*!
    \brief
        Loads content Image from given file

    \param mimeSubtype
        the second part of the whole mime type, for example "svg+xml", "jpeg", "png"
    \param filename
        name of the source file
    \param resourceGroup
        Resource group identifier to be passed to the resource manager when
        loading the image file.
    */
    void loadFromFile(const String& mimeSubtype, const String& filename, const String& resourceGroup = "");
    
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
		if (class_name=="ChromeImage")
            return true;

		return ChromeWidget::testClassName_impl(class_name);
	}
};

}

#endif
