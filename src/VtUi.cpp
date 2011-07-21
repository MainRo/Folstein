
/*
*****************************************************************************
*                      ___       _   _    _ _
*                     / _ \ __ _| |_| |__(_) |_ ___
*                    | (_) / _` | / / '_ \ |  _(_-<
*                     \___/\__,_|_\_\_.__/_|\__/__/      
*                          Copyright (c) 2009
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
* @file     VtUi.cpp
* @author   R. Picard
* @date     21/12/2009
* 
*****************************************************************************/

#include <VtUi.h>



/**
* @date     21/12/2009
*
*  Constructor
*
******************************************************************************/
VtUi::VtUi(void)
{
   pc_Uploader = NULL;
   return;
}


/**
* @date     21/12/2009
*
*  Destructor
*
******************************************************************************/
VtUi::~VtUi(void)
{
   return;
}


/**
* @date     03/01/2010
* 
* @param    pc_NewUploader (in): Uploader used for analysis
* @return   Always B_OK
******************************************************************************/
status_t VtUi::SetUploader(VtUploader *pc_NewUploader)
{
   pc_Uploader = pc_NewUploader;
   return(B_OK);
} 


/**
* @date     03/01/2010
* 
*  Starts the ui and the analysis of the current file.
*
* @return   B_OK if success.
* @return   B_BAD_VALUE if the ui is not cofigured.
******************************************************************************/
status_t VtUi::Start(void)
{
   if(pc_Uploader == NULL)
      return(B_BAD_VALUE);

   return(B_OK);
} 


/**
* @date     03/01/2010
* 
*  Stops the ui, and interrupts the analysis if it is still running.
*
* @return   Always B_OK
******************************************************************************/
status_t VtUi::Stop(void)
{
   if(pc_Uploader == NULL)
      return(B_OK);

   return(B_OK);
} 

