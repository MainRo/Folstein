
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
* @file     VtUploader.cpp
* @author   R. Picard
* @date     22/12/2009
* 
*****************************************************************************/
#include <Path.h>
#include "VtUploader.h"
#include "Hash.h"

#define VTU_MSG_ANALYZE 'VtAn'
#define VTU_MSG_POLL_RESULT 'VtPr'

/**
* @date     03/01/2010
* 
*  Constructor.
******************************************************************************/
VtUploader::VtUploader(void) :
   BLooper("VtUploader"),
   c_FileName(), c_HashString(), Http(),
   b_ForceUpload(false), pc_Handler(NULL)
{
   Run();
}

/**
* @date     03/01/2010
* 
*  Destructor.
******************************************************************************/
VtUploader::~VtUploader(void)
{
}


/**
* @date     03/01/2010
* 
*  Sets if the upload must be forced even if the file is already known by
*  VirusTotal database. default value is false.
*
* @param    b_ForceState (in): State of the force upload option.
* @return   B_OK.
******************************************************************************/
status_t VtUploader::ForceUpload(bool b_ForceState)
{
   b_ForceUpload = b_ForceState;
   return(B_OK);
} 


/**
* @date     03/01/2010
* 
*  Starts the analysis of the file. This method is asynchronous. Only a message
*  is sent to the uploader looper.
*
* @return   B_OK if the analysis is started.
* @return   B_BAD_VALUE if the uploader is not correctly configured.
* @return   B_ERROR otherwise.
******************************************************************************/
status_t VtUploader::Analyze(void)
{
   BMessenger  c_Messenger(this);

   if(pc_Handler == NULL)
      return(B_BAD_VALUE);

   c_Messenger.SendMessage(VTU_MSG_ANALYZE);
   return(B_OK);
}


/**
* @date     03/18/2010
* 
*  Cancels the analysis. Any pending request is suspended, and no other request
*  will be possible on this session.
*
* @return   B_OK if the analysis is started.
******************************************************************************/
status_t VtUploader::Cancel(void)
{
   return(B_OK);
}


/**
* @date     03/01/2010
* 
*  Starts the analysis of the file.
*
* @return   B_OK if the analysis is started.
* @return   B_FILE_NOT_FOUND if the file was uploaded for analysis.
* @return   B_ERROR otherwise.
******************************************************************************/
status_t VtUploader::_Analyze(void)
{
   status_t i_Ret;
   BMessenger c_TargetMessenger(pc_Handler);
   BMessage c_ProgressMsg(VTU_MSG_PROGRESS);

   if(b_ForceUpload == false)
      i_Ret = _SearchHash();
   else
      i_Ret = B_NAME_NOT_FOUND;

   if(i_Ret == B_OK)
   {
      c_TargetMessenger.SendMessage(VTU_MSG_DONE);
   }
   else if(i_Ret == B_NAME_NOT_FOUND)
   {
      if(c_FileName.CountChars() > 0)
      {
         c_ProgressMsg.AddInt32(VTU_FIELD_UPLOAD, 0);
         c_TargetMessenger.SendMessage(&c_ProgressMsg);
         i_Ret = _UploadFile();
         if(i_Ret == B_OK)
         {
            c_ProgressMsg.ReplaceInt32(VTU_FIELD_UPLOAD, 100);
            c_TargetMessenger.SendMessage(&c_ProgressMsg);
            i_Ret = B_FILE_NOT_FOUND;
         }
         else
         {
            c_TargetMessenger.SendMessage(VTU_MSG_ERROR);
         }
      }
   }

   if( (i_Ret != B_OK) && (i_Ret != B_FILE_NOT_FOUND) )
      c_TargetMessenger.SendMessage(VTU_MSG_ERROR);
   
   return(i_Ret);
} 


/**
* @date     03/01/2010
* 
*  Message hook to do long actions: Uploader and analysis.
*
* @param    pc_Msg (in): Received message.
******************************************************************************/
void VtUploader::MessageReceived(BMessage *pc_Msg)
{
   status_t i_Ret;
   BMessenger  c_Messenger(this);
   BMessenger c_TargetMessenger(pc_Handler);

   switch(pc_Msg->what)
   {
      case VTU_MSG_ANALYZE:
         if(_Analyze() == B_FILE_NOT_FOUND)
         {
            BMessage c_ProgressMsg(VTU_MSG_PROGRESS);

            c_ProgressMsg.AddInt32(VTU_FIELD_UPLOAD, 100);
            c_ProgressMsg.AddInt32(VTU_FIELD_ANALYSIS, 0);
            c_TargetMessenger.SendMessage(&c_ProgressMsg);
            c_Messenger.SendMessage(VTU_MSG_POLL_RESULT);
         }
         break;
      
      case VTU_MSG_POLL_RESULT:
         /** @todo do not block for such a long time (use rest api for on-going
          * analysis status). */
         sleep(5);
         i_Ret = Http.GetAnalysisStatus();
         if(i_Ret == B_OK)
         {
            if(Http.GetAvCount() > 0)
            {
               BMessage c_ProgressMsg(VTU_MSG_PROGRESS);
               
               c_ProgressMsg.AddInt32(VTU_FIELD_UPLOAD, 100);
               c_ProgressMsg.AddInt32(VTU_FIELD_ANALYSIS, 100);
               c_TargetMessenger.SendMessage(VTU_MSG_DONE);
               break;
            }
            c_Messenger.SendMessage(VTU_MSG_POLL_RESULT);
         }
         else
            c_TargetMessenger.SendMessage(VTU_MSG_ERROR);
         break;

      default:
         BLooper::MessageReceived(pc_Msg);
         break;
   }
   return ;
} 


/**
* @date     03/01/2010
* 
*  Sets the target handler that will receive status messages.
*
* @param    pc_Target (in): Target handler for notifications.
******************************************************************************/
void VtUploader::SetTarget(BHandler *pc_Target)
{
   pc_Handler = pc_Target;
}


/**
* @date     03/01/2010
* 
*  Searches for the file hash in the VirusTotal database.
*
* @return   B_OK if success.
* @return   B_NAME_NOT_FOUND if the hash is not available.
* @return   Error otherwise.
******************************************************************************/
status_t VtUploader::_SearchHash(void)
{
   return(_ConsultHash());
}


/**
* @date     22/12/2009
* 
*  uploads the file and start analysis. This function is blocking until the 
* file is uploaded.
*
* @return   B_OK if success.
* @return   B_ERROR otherwise.
******************************************************************************/
status_t VtUploader::_UploadFile(void)
{
   status_t i_Ret;

   if(c_FileName.CountChars() <= 0)
      return(B_BAD_VALUE);

   i_Ret = Http.UploadFile(c_FileName);
   return(i_Ret);
}


/**
* @date     03/01/2010
* 
*  Returns the analysis result list. The list belongs to the VtUploader object.
*
* @param    pp_List (out): Will be filled with result list.
* @return   B_OK if success.
* @return   B_BAD_VALUE if list is NULL.
* @return   B_NAME_NOT_FOUND if the analysis result is not available.
******************************************************************************/
status_t VtUploader::GetAnalysisResult(BList **pp_List, uint32 &i_AvCount)
{
   status_t i_Ret;

   if(pp_List == NULL)
      return(B_BAD_VALUE);

   *pp_List = NULL;
   i_AvCount = 0;

   i_Ret = _ConsultHash();
   if(i_Ret == B_OK)
   {
      // Hash found, get results
      i_Ret = Http.GetAnalysisStatus();
      if(i_Ret == B_OK)
      {
         i_AvCount = Http.GetAvCount();
         *pp_List = Http.GetResult();
      }
   }
   else if(i_Ret == B_NAME_NOT_FOUND)
   {
   }
   return(i_Ret);
}
      

/**
* @date     03/01/2010
* 
*  Returns the full path of the analyzed file.
*
* @param    pc_Name (out): Will be filled with file name.
* @return   B_OK if success.
* @return   B_BAD_VALUE if a parameter is not correct.
******************************************************************************/
status_t VtUploader::GetFileName(BString *pc_Name)
{
   BPath c_Path;

   if(pc_Name == NULL)
      return(B_BAD_VALUE);

   c_Path.SetTo(c_FileName.String());
   pc_Name->SetTo(c_Path.Leaf());
   return(B_OK);
}


/**
* @date     03/01/2010
* 
*  Sets the full path of the file to analyze.
*
* @param    c_File (in): file path.
* @return   B_OK if success.
* @return   Error otherwise.
******************************************************************************/
status_t VtUploader::SetToFile(const BString &c_File)
{
   status_t i_Ret;
   
   c_FileName.SetTo(c_File);
   i_Ret = _ComputeHash();
   return(i_Ret);
}


/**
* @date     03/01/2010
* 
*  Sets the hash of the file to analyze. The hash can be md5, sha1 or sha256.
*  All will be automatically recognized.
*
* @param    c_Hash (in): file hash.
* @return   B_OK.
******************************************************************************/
status_t VtUploader::SetToHash(const BString &c_Hash)
{
   c_HashString.SetTo(c_Hash);
   return(B_OK);
}


/**
* @date     03/01/2010
* 
*  Computes the sha1 hash of the file.
*
* @return   B_OK if success.
* @return   B_ERROR otherwise.
******************************************************************************/
status_t VtUploader::_ComputeHash(void)
{
   status_t i_Ret;
   Hash c_Hash;

   c_Hash.SetTo(c_FileName);
   i_Ret = c_Hash.GetString(&c_HashString);
   return(i_Ret);
}


/**
* @date     03/01/2010
* 
*  Query VT database to check if the hash is already known
*
* @return   B_OK if success.
* @return   B_ERROR otherwise.
******************************************************************************/
status_t VtUploader::_ConsultHash(void)
{
   status_t i_Ret;

   i_Ret = Http.SetHash(c_HashString);
   if(i_Ret == B_OK)
   {
      i_Ret = Http.ConsultHash();
   }
   return(i_Ret);
}


