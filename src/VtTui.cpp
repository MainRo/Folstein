
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
* @file     VtTui.cpp
* @author   R. Picard
* @date     03/01/2010
* 
*****************************************************************************/
#include "VtTui.h"


/**
* @date     03/01/2010
* 
*  Constructor.
*
******************************************************************************/
VtTui::VtTui():
   VtUi(),
   BHandler()
{
   /* Attach myself to the app looper */
   be_app->AddHandler(this);
   return ;
} 

/**
* @date     03/01/2010
* 
*  Destructor.
*
******************************************************************************/
VtTui::~VtTui()
{
   be_app->RemoveHandler(this);
   return ;
} 


/**
* @date     03/01/2010
* 
*  Returns the handler used to receive ui messages.
*
* @return   Pointer to the Tui handle.
******************************************************************************/
BHandler *VtTui::GetHandler(void)
{
   return(this);
}


/**
* @date     03/01/2010
* 
*  Message reception hook.
*
* @param    pc_Msg (in): Message received.
******************************************************************************/
void VtTui::MessageReceived(BMessage *pc_Msg)
{
   int32 i_Progress;

   switch(pc_Msg->what)
   {
      case VTU_MSG_ERROR:
         printf("Error\n");
         be_app->PostMessage(B_QUIT_REQUESTED);
         break;
      
      case VTU_MSG_PROGRESS:
         if(pc_Msg->FindInt32(VTU_FIELD_ANALYSIS, &i_Progress) == B_OK)
         {
            if(i_Progress == 0)
               printf("Analysing file...\n");
         }
         else if(pc_Msg->FindInt32(VTU_FIELD_UPLOAD, &i_Progress) == B_OK)
         {
            if(i_Progress == 0)
               printf("Uploading file...\n");
            else if(i_Progress == 100)
               printf("File uploaded.\n");
         }
         break;
      
      case VTU_MSG_DONE:
         _PrintResult();
         be_app->PostMessage(B_QUIT_REQUESTED);
         break;

      default:
         return(BHandler::MessageReceived(pc_Msg));
         break;
   }
}


/**
* @date     04/01/2010
* 
*Stop the TUI: simply delete ourself.
*
* @return B_OK.
******************************************************************************/
status_t VtTui::Stop(void)
{
   pc_Uploader->Cancel();
   delete this;
   return(B_OK);
}


/**
* @date     04/01/2010
* 
*  Prints the result of the analysis.
******************************************************************************/
void VtTui::_PrintResult(void)
{
   status_t i_Ret;
   int32 i_CurItem;
   uint32   i_AvCount;
   BList *pc_ResultList;
   VtAvEntry   *pc_CurEntry;

   i_Ret = pc_Uploader->GetAnalysisResult(&pc_ResultList, i_AvCount);
   if( (i_Ret == B_OK) && (pc_ResultList != NULL) )
   {
      if(pc_ResultList->CountItems() > 0)
      {
         printf("File is infected (%d/%d):\n", 
               (int)pc_ResultList->CountItems(),
               (int)i_AvCount);
         for(i_CurItem=0; i_CurItem<pc_ResultList->CountItems(); i_CurItem++)
         {
            pc_CurEntry = (VtAvEntry*)pc_ResultList->ItemAt(i_CurItem);
            if(pc_CurEntry != NULL)
            {
               if(pc_CurEntry->Result.CountChars() > 0)
               {
                  printf("%20s | %40s\n", 
                        pc_CurEntry->Name.String(),
                        pc_CurEntry->Result.String());
               }
            }
         }
      }
      else
         printf("File is sane (%d/%d).\n", 
               (int)pc_ResultList->CountItems(),
               (int)i_AvCount);
   }
   else
      printf("Error while getting analysis result.\n");

   return;
} 
