
#ifndef VTUPLOADER_H
#define VTUPLOADER_H

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
* @file     VtUploader.h
* @author   R. Picard
* @date     03/01/2010
* 
*****************************************************************************/
#include <Application.h>
#include <SupportDefs.h>
#include <String.h>
#include "VtHttp.h"

#define VTU_MSG_DONE       'VtDn'
#define VTU_MSG_ERROR      'VtEr'
#define VTU_MSG_PROGRESS   'VtPg'

#define VTU_FIELD_UPLOAD   "vito:upload"
#define VTU_FIELD_ANALYSIS "vito:analysis"

/** Uploader class. This is the API to use for using VT's services. The client can
 * register as an observer to receive status/analysis results as BMessages. */
class VtUploader : public BLooper
{ 
   public:
      VtUploader();
      virtual ~VtUploader();

      status_t SetToFile(const BString &c_File);
      status_t SetToHash(const BString &c_Hash);
      status_t GetAnalysisResult(BList **pp_List, uint32 &i_AvCount);
      status_t GetFileName(BString *pc_Name);
      status_t Analyze(void);
      status_t Cancel(void);
      status_t ForceUpload(bool b_ForceState);

      virtual void MessageReceived(BMessage *pc_Msg);
      void     SetTarget(BHandler *pc_Target);

   private:
      BString  c_FileName;
      BString  c_HashString;
      VtHttp   Http;
      bool     b_ForceUpload;
      BHandler *pc_Handler;

      status_t _Analyze(void);
      status_t _SearchHash(void);
      status_t _UploadFile(void);

      status_t _ComputeHash(void);
      status_t _ConsultHash(void);
};

#endif /* VTUPLOADER_H */
