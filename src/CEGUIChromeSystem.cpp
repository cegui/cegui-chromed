/***********************************************************************
    filename:   CEGUIChromeSystem.cpp
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

#include "CEGUIChromeSystem.h"

#include "CEGUIChromeHTML.h"
#include "CEGUIChromeImage.h"
#include "CEGUIChromeFlash.h"

#include "CEGUIExceptions.h"
#include "CEGUIWindowFactoryManager.h"
#include "CEGUITplWindowFactory.h"

#include <berkelium/Berkelium.hpp>
#include <berkelium/Context.hpp>

namespace CEGUI
{

bool ChromeSystem::ds_initialised = false;
Berkelium::Context* ChromeSystem::ds_context = 0;

void ChromeSystem::ensureInitialised()
{
    if (!ds_initialised)
    {
        CEGUI_THROW(InvalidRequestException(
            "ChromeSystem::ensureInitialised - System wasn't initialised, initialise it using CEGUI::ChromeSystem::initialise()!."));
    }
}

void ChromeSystem::initialise()
{
    if (ds_initialised)
    {
        CEGUI_THROW(InvalidRequestException(
            "ChromeSystem::finalise - System was already initialised!."));
    }

    Berkelium::init(Berkelium::FileString::empty());
    ds_context = Berkelium::Context::create();

    // lets register our precious widgets
    WindowFactoryManager::addFactory< TplWindowFactory<ChromeHTML> >();
    WindowFactoryManager::addFactory< TplWindowFactory<ChromeImage> >();
    WindowFactoryManager::addFactory< TplWindowFactory<ChromeFlash> >();

    ds_initialised = true;
}

void ChromeSystem::finalise()
{
    if (!ds_initialised)
    {
        CEGUI_THROW(InvalidRequestException(
            "ChromeSystem::finalise - System isn't currently initialised!."));
    }

    ds_context->destroy();
    Berkelium::destroy();

    ds_initialised = false;
}

bool ChromeSystem::isInitialised()
{
    return ds_initialised;
}


Berkelium::Context* ChromeSystem::getContext()
{
    return ds_context;
}

void ChromeSystem::update()
{
    Berkelium::update();
}

}
