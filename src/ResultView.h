
#ifndef RESULTVIEW_H
#define RESULTVIEW_H

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
* @file     ResultView.h
* @author   R. Picard
* @date     11/02/2010
* 
*****************************************************************************/
#include <ListView.h>
#include <ScrollView.h>

#define RL_MSG_SEL  'RlSl'
#define RL_MSG_INV  'RlIv'

/** ResultView contains the list of positive results from the AVs. */
class ResultView : public BView
{
   public:
      ResultView(BRect c_Rect);
      virtual ~ResultView(void);
      status_t Fill(const BList &c_ResultList);

   private:
      BListView      *pc_ListView;
      BScrollView    *pc_ScrollView;

      status_t _UpdateView(void);
};


#endif /* RESULTVIEW_H */
