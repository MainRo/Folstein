
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
* @file     ResultView.cpp
* @author   R. Picard
* @date     11/02/2010
* 
*****************************************************************************/
#include <new>
#include <alm/ALMLayout.h>
#include <alm/Area.h>

#include <ClassInfo.h>
#include <Window.h>
#include <StringItem.h>

#include "ResultView.h"
#include "VtUploader.h"


/**
* @date     11/02/2010
* 
*  Constructor.
*
******************************************************************************/
ResultView::ResultView(BRect c_Rect)  :
   BView(c_Rect, "ResultView", B_FOLLOW_NONE, B_NAVIGABLE),
   pc_ListView(NULL), pc_ScrollView(NULL)
{
   BRect   c_ListRect;   
  

   c_ListRect = Bounds();
   c_ListRect.InsetBy(5,5);
   c_ListRect.bottom -= B_H_SCROLL_BAR_HEIGHT;
   c_ListRect.right -= B_V_SCROLL_BAR_WIDTH;
   pc_ListView = new (std::nothrow )BListView(c_ListRect, "Rule List View",
         B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
   if(pc_ListView == NULL)
      return;
 
   pc_ScrollView = new (std::nothrow) BScrollView("ScrollView", pc_ListView, 
                                    B_FOLLOW_ALL_SIDES, 0, true, true);
   AddChild(pc_ScrollView);
   /* _UpdateView(); */
   /* ResizeTo(300,100); */

   /* SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR)); */
}


/**
* @date     11/02/2010
* 
*  Destructor.
*
******************************************************************************/
ResultView::~ResultView()
{
}


/**
* @date     11/02/2010
* 
*  Fills the ListView with the Av results.
*
* @param    c_ResultList (in): Av result list.
* @return   B_OK.
******************************************************************************/
status_t ResultView::Fill(const BList &c_ResultList)
{
   int32 i_CurItem;
   BString     c_EntryString;
   BStringItem        *pc_NewItem;
   VtAvEntry   *pc_CurEntry;

   for(i_CurItem=0; i_CurItem<c_ResultList.CountItems(); i_CurItem++)
   {
      pc_CurEntry = (VtAvEntry*)c_ResultList.ItemAt(i_CurItem);
      if(pc_CurEntry != NULL)
      {
         if(pc_CurEntry->Result.CountChars() > 0)
         {
            c_EntryString.SetTo("");
            c_EntryString << pc_CurEntry->Result << " (" << 
                  pc_CurEntry->Name << ")";
            pc_NewItem = new (std::nothrow) BStringItem(c_EntryString.String());
            pc_ListView->AddItem(pc_NewItem);
         }
      }
   }

   _UpdateView();
   Invalidate();
   return(B_OK);
} 


/**
* @date     06/02/2006
* 
*  Updates the scrollview sidebars.
* 
* @return   B_OK if success.
* @return   B_ERROR otherwise.
******************************************************************************/
status_t ResultView::_UpdateView(void)
{
   uint32   i_Index;
   float    i_MaxWidth, i_ViewWidth, i_Width;
   BRect    c_Bounds;
   BScrollBar *pc_ScrollBar;
   BListItem *pc_Item;
   BStringItem *pc_StringItem;
  
   if(pc_ListView == NULL)
      return(B_ERROR);
   
   if(pc_ScrollView == NULL)
      return(B_ERROR);
   
   pc_ScrollBar = pc_ScrollView->ScrollBar(B_HORIZONTAL);
   if(pc_ScrollBar == NULL)
      return(B_ERROR);

   c_Bounds = pc_ScrollView->Bounds();
   i_MaxWidth = c_Bounds.right - c_Bounds.left;
   i_ViewWidth = i_MaxWidth;

   i_Index = 0;
   pc_Item = pc_ListView->ItemAt(i_Index);
   while(pc_Item != NULL)
   {
      /* update the max width of the text to update the horizontal 
       * scrollbar */
      pc_StringItem = cast_as(pc_Item, BStringItem);
      if(pc_StringItem != NULL)
      {
         i_Width = pc_StringItem->Width();
         if(i_Width > i_MaxWidth)
            i_MaxWidth = i_Width;
      }
      i_Index++;
      pc_Item = pc_ListView->ItemAt(i_Index);
   };

   /* update the horizontal scroll range */
   if(i_MaxWidth == i_ViewWidth) /* nothing bigger than the view */
      pc_ScrollBar->SetRange(0, 0); /* no scrolling */
   else
      pc_ScrollBar->SetRange(0, i_MaxWidth - i_ViewWidth + 20);

   return(B_OK);
}

