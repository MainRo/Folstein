
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
* @file     HttpFile.cpp
* @author   R. Picard
* @date     25/02/2009
* 
*****************************************************************************/
#include <stdio.h>

#include "HttpFile.h"


/**
* @date     25/02/2009
* 
*  Constructor.
*
******************************************************************************/
HttpFile::HttpFile(void) : Http(),
   c_FilePath(), f_File(NULL)
{
   return ;
}


/**
* @date     25/02/2009
* 
*  Destructor.
*
******************************************************************************/
HttpFile::~HttpFile(void )
{
   return ;
} 


/**
* @date     25/02/2009
* 
*  Sets the file where the get data will be stored.
*
* @param    sz_File (in): File full pathname):
* @return   B_OK if success.
* @return   B_BAD_VALUE if file path is NULL.
******************************************************************************/
int32 HttpFile::SetFile(const char *sz_File)
{
   if(sz_File == NULL)
      return(B_BAD_VALUE);
   
   c_FilePath.SetTo(sz_File);
   return(B_OK);
} 


/**
* @date     25/02/2009
* 
*  Issue an http get request, and put the result in a file.
*
* @return   B_OK if success.
* @return   B_ERROR otherwise.
******************************************************************************/
int32 HttpFile::Download(void)
{
   int32 i_Ret;
  
   f_File = fopen(c_FilePath.String(), "w");
   if(f_File == NULL)
      return(B_ERROR);

   i_Ret = GetHttp();
   fclose(f_File);
   return(i_Ret);
} 


/**
* @date     25/02/2009
* 
*  Curl write callback for http get requests.
*
* @param    p_Buffer (in): Buffer received.
* @param    i_Size (in): Size of an element.
* @param    i_Nmemb (in): Number of elements received.
* @return   Number of elements written.
******************************************************************************/
size_t HttpFile::CurlGetWrite(void *p_Buffer, size_t i_Size, size_t i_Nmemb)
{
   return(fwrite(p_Buffer, i_Size, i_Nmemb, f_File));
} 


