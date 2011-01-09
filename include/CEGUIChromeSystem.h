/***********************************************************************
    filename:   CEGUIChromeSystem.h
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

#ifndef _CEGUIChromeSystem_h_
#define _CEGUIChromeSystem_h_

#include "CEGUIChromePrerequisites.h"

namespace Berkelium
{
    class Context;
}

namespace CEGUI
{

/*!
\brief Central class of the module
*/
class CHROMED_CEGUI_API ChromeSystem
{
public:
    //! if the system wasn't initialised already, this throws exception!
    static void ensureInitialised();

    //! initialises the system, if it was initialised, exception is thrown
    static void initialise();

    //! finalises the system, you have to do this manually if you don't want leaks to occur!
    static void finalise();

    //! checks whether the system was initialised already
    static bool isInitialised();

    //! returns the shared Berkelium context, we use one context for all windows but it seems the windows clone it anyways
    static Berkelium::Context* getContext();

    //! needs to be called every frame
    static void update();

private:
    //! internal member variable, if true the system was initialised already
    static bool ds_initialised;
    //! holds Berkelium context that all Berkelium windows share
    static Berkelium::Context* ds_context;
};

}

#endif
