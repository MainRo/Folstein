
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
* @file     VtGui.cpp
* @author   R. Picard
* @date     14/01/2010
* 
*****************************************************************************/
#include <new>
#include <alm/Tab.h>
#include <alm/Area.h>
#include <alm/ALMLayout.h>

#include "VtGui.h"

#define VTGUI_MSG_DONE 'VgDn'


/**
* @date     24/01/2010
* 
*  Constructor.
******************************************************************************/
VtGui::VtGui(void) :
  BWindow(BRect(0, 0, 300, 200), "", B_TITLED_WINDOW_LOOK, 
            B_NORMAL_WINDOW_FEEL, 
            B_ASYNCHRONOUS_CONTROLS | B_NOT_RESIZABLE | B_NOT_ZOOMABLE ),
   VtUi(),
   pc_ResultView(NULL), pc_StatusString(NULL)
{
   _Init();
   return;
}


/**
* @date     24/01/2010
* 
*  Destructor.
******************************************************************************/
VtGui::~VtGui()
{
}


/**
* @date     24/01/2010
* 
*  Init code.
*
* @return B_OK.
******************************************************************************/
status_t VtGui::_Init(void)
{
   float i_Width, i_Height;
   BSize c_WindowSize;
   BRect c_Rect;
   BALMLayout  *pc_Layout;
   Area  *pc_BackArea;
   Area  *pc_ResultArea;
   Area  *pc_DoneArea;
   Area  *pc_StatusArea;
   YTab  *pc_StatusYTab, *pc_DoneYTab;
   BView    *pc_MainView;  /**< Background view */
   BButton  *pc_DoneButton;/**< Validation button  */

  /* Layout */
   pc_Layout = new(std::nothrow) BALMLayout();
   /* pc_Layout->SetLayoutStyle(BALM::ADJUST_SIZE); */
   pc_StatusYTab = pc_Layout->AddYTab();
   pc_DoneYTab = pc_Layout->AddYTab();
   SetLayout(pc_Layout);

 
   /* first create a global view in the window */
   pc_MainView = new(std::nothrow) BView("MainView", 0);
   pc_MainView->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
   /* Button */
   pc_DoneButton = new(std::nothrow) BButton("Done", "Done",
         new BMessage(VTGUI_MSG_DONE));
   SetDefaultButton(pc_DoneButton);
   pc_DoneButton->GetPreferredSize(&i_Width,&i_Height);
   pc_DoneButton->ResizeTo(i_Width,i_Height);
   /* pc_MainView->AddChild(pc_DoneButton); */

   /* Result */
   c_Rect = Bounds();
   c_Rect.bottom = c_Rect.top+100;
   pc_ResultView = new(std::nothrow) ResultView(c_Rect);

   /* Status  */
   /** @todo find a clean way to resize the stringview. Spaces at the end are
    * here to let enough space to the other texts. */
   pc_StatusString = new(std::nothrow) BStringView("Status", "Searching known file status...      ");
   pc_StatusString->GetPreferredSize(&i_Width,&i_Height);
   pc_StatusString->ResizeTo(i_Width,i_Height);

   /* Button Layout */
   pc_DoneArea = pc_Layout->AddView(pc_DoneButton, pc_Layout->Left(), pc_DoneYTab,
         pc_Layout->Right(), pc_Layout->Bottom());
   pc_DoneButton->SetExplicitAlignment(BAlignment(B_ALIGN_RIGHT, B_ALIGN_BOTTOM));
   pc_DoneArea->SetRightInset(10);
   pc_DoneArea->SetTopInset(10);
   pc_DoneArea->SetBottomInset(10);

   /* Result layout */
   pc_ResultArea = pc_Layout->AddView(pc_ResultView, pc_Layout->Left(), pc_StatusYTab,
         pc_Layout->Right(), pc_DoneYTab);
   pc_ResultArea->SetRightInset(10);
   pc_ResultArea->SetLeftInset(10);

   /* Status Layout */
   pc_StatusArea = pc_Layout->AddView(pc_StatusString, pc_Layout->Left(), pc_Layout->Top(),
         pc_Layout->Right(), pc_StatusYTab);
   pc_StatusString->SetExplicitAlignment(BAlignment(B_ALIGN_LEFT, B_ALIGN_TOP));
   pc_StatusArea->SetRightInset(10);
   pc_StatusArea->SetLeftInset(10);
   pc_StatusArea->SetTopInset(10);
   
   pc_BackArea = pc_Layout->AddView(pc_MainView, pc_Layout->Left(), pc_Layout->Top(),
         pc_Layout->Right(), pc_Layout->Bottom());

   /* pc_Layout->AddConstraint(1, pc_StatusYTab, -1, pc_DoneYTab, */
         /* OperatorType(LE), 0); */

   /* set window position */
   c_WindowSize = pc_Layout->PreferredSize();
   ResizeTo(c_WindowSize.width, c_WindowSize.height);
   CenterOnScreen();
   /* Show(); */
   return(B_OK);
}


/**
* @date     24/01/2010
* 
*  Returns the handler of the GUI, meaning the object itself since it is a
*  BWindow.
*
* @return this.
******************************************************************************/
BHandler *VtGui::GetHandler(void)
{
   return(this);
}


/**
* @date     24/01/2010
* 
*  Message handler for ui events.
*
* @param    pc_Msg (in): Message received.
******************************************************************************/
void VtGui::MessageReceived(BMessage *pc_Msg)
{
   int32 i_Progress;

   if(pc_Msg == NULL)
      return;

   switch(pc_Msg->what)
   {
      case VTGUI_MSG_DONE:
         be_app->PostMessage(B_QUIT_REQUESTED);
         break;

      case VTU_MSG_ERROR:
         pc_StatusString->SetText("Error");
         break;
      
      case VTU_MSG_PROGRESS:
         if(pc_Msg->FindInt32(VTU_FIELD_ANALYSIS, &i_Progress) == B_OK)
         {
            if(i_Progress == 0)
               pc_StatusString->SetText("Analysing file (This can be long)...");
         }
         else if(pc_Msg->FindInt32(VTU_FIELD_UPLOAD, &i_Progress) == B_OK)
         {
            if(i_Progress == 0)
               pc_StatusString->SetText("Uploading file...");
            else if(i_Progress == 100)
               pc_StatusString->SetText("File uploaded.");
         }
         break;
      
      case VTU_MSG_DONE:
         pc_StatusString->SetText("Analysis completed.");
         _PrintResult();
         break;

      default:
         BWindow::MessageReceived(pc_Msg);
         break;
   }

}


/**
* @date     24/01/2010
* 
*  Starts the GUI. The window is shown.
*
* @return   B_OK.
******************************************************************************/
status_t VtGui::Start(void)
{
   BString c_TitleName;

   pc_Uploader->GetFileName(&c_TitleName);
   c_TitleName.Prepend("Folstein - ");
   SetTitle(c_TitleName.String());
   Show();
   return(B_OK);
}


/**
* @date     24/01/2010
* 
*  Stop the GUI. The window is closed.
*
* @return   B_OK.
******************************************************************************/
status_t VtGui::Stop(void)
{
   pc_Uploader->Cancel();
   Lock();
   Quit();
   return(B_OK);
}


/**
* @date     07/02/2010
* 
*  Fills the resultview with the result of the uploader.
*
* @return   B_OK
******************************************************************************/
status_t VtGui::_PrintResult(void)
{
   status_t i_Ret;
   BList *pc_ResultList;
   uint32   i_AvCount;
   BString  ResultText;

   i_Ret = pc_Uploader->GetAnalysisResult(&pc_ResultList, i_AvCount);
   if( (i_Ret == B_OK) && (pc_ResultList != NULL) )
   {
      if(pc_ResultList->CountItems() > 0)
      {
         ResultText.SetTo("File is infected (");
         ResultText << pc_ResultList->CountItems() << "/" << i_AvCount << ").";
         pc_StatusString->SetText(ResultText.String());
         /* Add and fill Result view */
         pc_ResultView->Fill(*pc_ResultList);
      }
      else
      {
         ResultText.SetTo("File is sane (");
         ResultText << pc_ResultList->CountItems() << "/" << i_AvCount << ").";
         pc_StatusString->SetText(ResultText.String());
      }
   }
   else
      pc_StatusString->SetText("Error while getting analysis result.");

   return(B_OK);
} 
