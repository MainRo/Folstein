
#ifndef VTGUI_H
#define VTGUI_H

/*
*****************************************************************************
*                      ___       _   _    _ _
*                     / _ \ __ _| |_| |__(_) |_ ___
*                    | (_) / _` | / / '_ \ |  _(_-<
*                     \___/\__,_|_\_\_.__/_|\__/__/      
*                          Copyright (c) 2010
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*****************************************************************************/
/**
* @file     VtGui.h
* @author   R. Picard
* @date     14/01/2010
* 
*****************************************************************************/
#include <Button.h>
#include <StringView.h>
#include <View.h>
#include <Window.h>

#include "VtUi.h"
#include "ResultView.h"

/** Graphical UI */
class VtGui: public BWindow, public VtUi
{
   public:
      VtGui(void);
      virtual ~VtGui(void);
      virtual void MessageReceived(BMessage *pc_Msg);      
      virtual status_t Start(void);
      virtual status_t Stop(void);
      virtual BHandler *GetHandler(void);

   private:
      ResultView  *pc_ResultView;
      BStringView *pc_StatusString;

      status_t _Init();
      status_t _PrintResult(void);
};

#endif /* VTGUI_H */
