/***********************************************************************
    filename:   CEGUIChromeImage.cpp
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

#include "CEGUIChromeImage.h"

#include <berkelium/Berkelium.hpp>
#include <berkelium/Window.hpp>

namespace CEGUI
{

const String ChromeImage::WidgetTypeName("ChromeImage");

ChromeImage::ChromeImage(const String& type, const String& name):
    ChromeWidget(type, name)
{}

ChromeImage::~ChromeImage()
{}

void ChromeImage::fetchImage(const String& URI)
{
    const char* data = URI.c_str();

    d_chromeWindow->navigateTo(data, strlen(data));
}

void ChromeImage::loadFromFile(const String& filename, const String& resourceGroup)
{
    if (filename.substr(filename.length() - 4) == ".svg")
    {
        loadFromFile("svg+xml", filename, resourceGroup);
    }
    else if (filename.substr(filename.length() - 4) == ".jpg" || filename.substr(filename.length() - 5) == ".jpeg")
    {
        loadFromFile("jpeg", filename, resourceGroup);
    }
    else if (filename.substr(filename.length() - 4) == ".png")
    {
        loadFromFile("png", filename, resourceGroup);
    }
    else if (filename.substr(filename.length() - 4) == ".gif")
    {
        loadFromFile("gif", filename, resourceGroup);
    }
    else
    {
        CEGUI_THROW(InvalidRequestException(
            "ChromeImage::loadFromFile - can't guess the mime type from file extension, use the other overloaded variant and specify mime type yourself!."));
    }
}

void ChromeImage::loadFromFile(const String& mimeSubtype, const String& filename, const String& resourceGroup)
{
    RawDataContainer file;
    System::getSingleton().getResourceProvider()->
        loadRawDataContainer(filename, file, resourceGroup);

    String base64("data:image/" + mimeSubtype + ";base64,");
    base64_encode(base64, file.getDataPtr(), file.getSize());

    CEGUI::System::getSingleton().getResourceProvider()->
        unloadRawDataContainer(file);

    // now lets send it to chrome
    const char* data = base64.c_str();
    d_chromeWindow->navigateTo(data, strlen(data));
}

}
