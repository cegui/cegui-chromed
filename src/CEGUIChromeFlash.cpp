/***********************************************************************
    filename:   CEGUIChromeFlash.cpp
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

#include "CEGUIChromeFlash.h"

#include <berkelium/Berkelium.hpp>
#include <berkelium/Window.hpp>

#include <fstream>

namespace CEGUI
{

const String ChromeFlash::WidgetTypeName("ChromeFlash");

ChromeFlash::ChromeFlash(const String& type, const String& name):
    ChromeWidget(type, name)
{}

ChromeFlash::~ChromeFlash()
{}

void ChromeFlash::fetchFlash(const String& URI)
{
    const char* data = URI.c_str();

    d_chromeWindow->navigateTo(data, strlen(data));
}

void ChromeFlash::loadFromFile(const String& filename, const String& resourceGroup)
{
    RawDataContainer file;
    System::getSingleton().getResourceProvider()->
        loadRawDataContainer(filename, file, resourceGroup);

    String base64_swf("data:application/x-shockwave-flash;base64,");
    base64_encode(base64_swf, file.getDataPtr(), file.getSize());

    CEGUI::System::getSingleton().getResourceProvider()->
        unloadRawDataContainer(file);

    String wrapper(String("<object width=\"550\" height=\"400\"><embed width=\"550\" height=\"400\" src=\"") + base64_swf + String("\"></embed></object>\n"));
    
    String base64("data:text/html;charset=utf8;base64,");
    const char* dataToEncode = wrapper.c_str();
    base64_encode(base64, reinterpret_cast<const uint8*>(dataToEncode), strlen(dataToEncode));

    const char* dataToPass = base64.c_str();

    std::ofstream("output.html") << dataToEncode << std::endl;

    d_chromeWindow->navigateTo(dataToPass, strlen(dataToPass));
}

}
