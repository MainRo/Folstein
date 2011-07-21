
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
* @file     Http.cpp
* @author   R. Picard
* @date     02/02/2009
* 
*****************************************************************************/
#include <stdlib.h>
#include "Http.h"

static size_t Http_CurlWrite(void *p_Buffer, size_t i_Size, 
                                 size_t i_Nmemb, void *p_Tag);
static size_t Http_CurlRead(void *p_Buffer, size_t i_Size, 
                           size_t i_Nmemb, void *p_Tag);
static size_t Http_CurlWriteHeader(void *p_Buffer, size_t i_Size, 
                           size_t i_Nmemb, void *p_Tag);


/**
* @date     25/02/2009
* 
*  Curl write callback for http get requests. Forward the request to the 
* class write method.
*
* @param    p_Buffer (in): Buffer received.
* @param    i_Size (in): Size of an element.
* @param    i_Nmemb (in): Number of elements received.
* @param    p_Tag (in): Tag that was set to the Http object.
* @return   Number of elements written.
******************************************************************************/
size_t Http_CurlWrite(void *p_Buffer, size_t i_Size, 
                           size_t i_Nmemb, void *p_Tag)
{
   Http  *p_This = (Http*)p_Tag;
   return(p_This->CurlWrite(p_Buffer, i_Size, i_Nmemb));
}


/**
* @date     21/12/2009
* 
*  Curl read callback for http post requests. Forward the request to the 
* class read method.
*
* @param    p_Buffer (OUT): Buffer to fill.
* @param    i_Size (in): Size of an element.
* @param    i_Nmemb (in): Number of elements in the buffer.
* @param    p_Tag (in): Tag that was set to the Http object.
* @return   Number of elements read.
******************************************************************************/
size_t Http_CurlRead(void *p_Buffer, size_t i_Size, 
                           size_t i_Nmemb, void *p_Tag)
{
   Http  *p_This = (Http*)p_Tag;
   return(p_This->CurlRead(p_Buffer, i_Size, i_Nmemb));
}


/**
* @date     21/12/2009
* 
*  Curl write callback for http get requests. Forward the request to the 
* class write method.
*
* @param    p_Buffer (in): Buffer to fill.
* @param    i_Size (in): Size of an element.
* @param    i_Nmemb (in): Number of elements in the buffer.
* @param    p_Tag (in): Tag that was set to the Http object.
* @return   Number of elements written.
******************************************************************************/
size_t Http_CurlWriteHeader(void *p_Buffer, size_t i_Size, 
                           size_t i_Nmemb, void *p_Tag)
{
   Http  *p_This = (Http*)p_Tag;

   if(p_This != NULL)
      p_This->AddHeader((const char*)p_Buffer);
   return(i_Nmemb);
}


/**
* @date     25/02/2009
* 
*  Http constructor.
*
******************************************************************************/
Http::Http(void):
   i_HttpStatus(400), 
   p_CurlHandle(NULL), CurlHeadersList(NULL), Post(NULL), Last(NULL), 
   HeadersList()
{
   p_CurlHandle = curl_easy_init();
   return ;
}


/**
* @date     25/02/2009
* 
*  Http destructor.
*
******************************************************************************/
Http::~Http(void )
{
   _EmptyHeaders();
   if(p_CurlHandle != NULL)
      curl_easy_cleanup(p_CurlHandle);
   return ;
} 


/**
* @date     25/02/2009
* 
*  Sets the url to work on.
*
* @param    sz_Url (in): Url.
* @return   B_OK if success.
* @return   B_ERROR otherwise.
******************************************************************************/
int32 Http::SetUrl(const char *sz_Url)
{
   if(p_CurlHandle != NULL)
   {
      if(curl_easy_setopt(p_CurlHandle, CURLOPT_URL, sz_Url) == 0)
         return(B_OK);
   }
   return(B_ERROR);
} 


/**
* @date     14/07/2011
* 
*  Sets the url to work on.
*
* @param    sz_Url (in): Url.
* @return   B_OK if success.
* @return   B_ERROR otherwise.
******************************************************************************/
int32 Http::SetUrl(BString &sz_Url)
{
   return(SetUrl(sz_Url.String()));
} 


/**
* @date     22/12/2009
* 
*  Returns the http status of the last request.
*
* @return   Http status of the last request.
******************************************************************************/
long  Http::GetStatus(void)
{
   return(i_HttpStatus);
}


/**
* @date     22/12/2009
* 
*  Resets the current settings so that another command can be sent. This does
* not close the tcp connection when pipelining is used.
*
* @return   B_OK.
******************************************************************************/
status_t Http::Reset(void)
{
   _EmptyHeaders();
   if(Post != NULL)
   {
      curl_formfree(Post);
      Post = NULL;
   }
   if(Last != NULL)
      Last = NULL;

   if(CurlHeadersList != NULL)
   {
      curl_slist_free_all(CurlHeadersList);
      CurlHeadersList = NULL;
   }

   curl_easy_reset(p_CurlHandle);
   return(B_OK);
}


/**
* @date     25/02/2009
* 
*  Issues an http get request to the current url.
*
* @return   B_OK if success.
* @return   Error otherwise.
******************************************************************************/
status_t Http::GetHttp(void)
{
   CURLcode i_CurlRet;
   
   if(p_CurlHandle == NULL)
      return(B_BAD_VALUE);

   i_CurlRet = curl_easy_setopt(p_CurlHandle, CURLOPT_WRITEFUNCTION, Http_CurlWrite);
   if(i_CurlRet == 0)
      i_CurlRet = curl_easy_setopt(p_CurlHandle, CURLOPT_HTTPGET, 1L);
   if(i_CurlRet == 0)
      i_CurlRet = curl_easy_setopt(p_CurlHandle, CURLOPT_WRITEDATA, this);
   if(i_CurlRet == 0)
      i_CurlRet = curl_easy_setopt(p_CurlHandle, CURLOPT_USERAGENT, "Curl");
   if(i_CurlRet == 0)
      i_CurlRet = curl_easy_perform(p_CurlHandle);

   if(i_CurlRet == 0)
   {
      /* check http status */
      curl_easy_getinfo(p_CurlHandle, CURLINFO_RESPONSE_CODE, &i_HttpStatus);
      if(i_HttpStatus != 200)
         i_CurlRet = CURLE_HTTP_RETURNED_ERROR;
   }


   if(i_CurlRet == 0)
      return(B_OK);
   else
      return(B_ERROR);
} 


/**
* @date     18/12/2009
* 
*  Sets the text data to send on a POST request.
*
* @param    sz_Data (in): Text data to send.
* @return   Number of elements written.
******************************************************************************/
status_t Http::SetPostTextData(const char *sz_Data)
{
   CURLcode i_CurlRet;
   
   if( (p_CurlHandle == NULL) || (sz_Data == NULL) )
      return(B_BAD_VALUE);

   i_CurlRet = curl_easy_setopt(p_CurlHandle, CURLOPT_POSTFIELDS, sz_Data);

   if(i_CurlRet == 0)
      return(B_OK);
   else
      return(B_ERROR);
}


/**
* @date     21/12/2009
* 
*  Starts an http upload. The data will be requested on the Read method.
*
* @param    i_DataSize (in): Size of the data to upload. 0 if unknown.
* @return   Number of elements written.
******************************************************************************/
status_t Http::UploadHttp(size_t i_DataSize = 0)
{
   CURLcode i_CurlRet;
   int32 i_Ret;
   
   if(p_CurlHandle == NULL)
      return(B_BAD_VALUE);

   /* Set specific callback for upload */
   i_CurlRet = curl_easy_setopt(p_CurlHandle, CURLOPT_READFUNCTION, Http_CurlRead);
   if(i_CurlRet == 0)
      i_CurlRet = curl_easy_setopt(p_CurlHandle, CURLOPT_READDATA, this);
   if(i_CurlRet == 0)
      i_CurlRet = curl_easy_setopt(p_CurlHandle, CURLOPT_UPLOAD, 1L);
   if(i_CurlRet == 0)
      i_CurlRet = curl_easy_setopt(p_CurlHandle, CURLOPT_POST, 1L);
   if( (i_CurlRet == 0) && (i_DataSize != 0) )
      i_CurlRet = curl_easy_setopt(p_CurlHandle, CURLOPT_POSTFIELDSIZE, i_DataSize);
 
   i_Ret = PostHttp();
   return(i_Ret);
}


/**
* @date     18/12/2009
* 
*  Issues a POST request.
*
* @return   B_OK if success.
* @return   B_ERROR otherwise.
******************************************************************************/
int32 Http::PostHttp(void)
{
   CURLcode i_CurlRet;
   
   if(p_CurlHandle == NULL)
      return(B_BAD_VALUE);

   /* Comon callbacks */
   i_CurlRet = curl_easy_setopt(p_CurlHandle, CURLOPT_WRITEFUNCTION, Http_CurlWrite);
   if(i_CurlRet == 0)
      i_CurlRet = curl_easy_setopt(p_CurlHandle, CURLOPT_HEADERFUNCTION, Http_CurlWriteHeader);
   if(i_CurlRet == 0)
      i_CurlRet = curl_easy_setopt(p_CurlHandle, CURLOPT_WRITEDATA, this);
   if(i_CurlRet == 0)
      i_CurlRet = curl_easy_setopt(p_CurlHandle, CURLOPT_HEADERDATA, this);

   /* Then start the request */
   if(i_CurlRet == 0)
      i_CurlRet = curl_easy_perform(p_CurlHandle);

   if(i_CurlRet == 0)
   {
      /* check http status */
      curl_easy_getinfo(p_CurlHandle, CURLINFO_RESPONSE_CODE, &i_HttpStatus);
      if(i_HttpStatus != 200)
         i_CurlRet = CURLE_HTTP_RETURNED_ERROR;
   }


   if(i_CurlRet == 0)
      return(B_OK);
   else
      return(B_ERROR);
} 


/**
* @date     21/02/2009
* 
*  Curl write callback for http get requests. This method must be overloaded by
*  child classes.
*
* @param    p_Buffer (in): Buffer received.
* @param    i_Size (in): Size of an element.
* @param    i_Nmemb (in): Number of elements received.
* @return   Number of elements written.
******************************************************************************/
size_t Http::CurlWrite(void *p_Buffer, size_t i_Size, size_t i_Nmemb)
{
   return(0);
} 


/**
* @date     21/12/2009
* 
*  Curl read callback for http post requests. This method must be overloaded by
*  child classes.
*
* @param    p_Buffer (in): Buffer fill before sending.
* @param    i_Size (in): Size of an element.
* @param    i_Nmemb (in): Number of elements in the buffer.
* @return   Number of elements written.
******************************************************************************/
size_t Http::CurlRead(void *p_Buffer, size_t i_Size, size_t i_Nmemb)
{
   return(0);
} 


/**
* @date     22/12/2009
* 
*  Adds a new part to multipart form. This new part is a file that will be
*  uploaded as the part data. Mime type is optional.
*
* @param    c_PartName (in): Name of the part.
* @param    c_FileName (in): File to upload.
* @param    pc_MimeType (in): Mime type of the file (Optional).
* @return   B_OK if success.
* @return   B_ERROR otherwise.
******************************************************************************/
status_t Http::AddFileFormPart(const BString &c_PartName,
                              const BString &c_FileName, 
                              BString *pc_MimeType = NULL)
{

   CURLFORMcode   i_CurlRet;

   if(pc_MimeType == NULL)
   {
      i_CurlRet = curl_formadd(&Post, &Last,
            CURLFORM_COPYNAME, c_PartName.String(),
            CURLFORM_FILE, c_FileName.String(),
            CURLFORM_END);
   }
   else
   {
      i_CurlRet = curl_formadd(&Post, &Last,
            CURLFORM_COPYNAME, c_PartName.String(),
            CURLFORM_FILE, c_FileName.String(),
            CURLFORM_CONTENTTYPE, pc_MimeType->String(),
            CURLFORM_END);
   }

   if(i_CurlRet == 0)
      return(B_OK);
   else
      return(B_ERROR);
} 


/**
* @date     22/12/2009
* 
*  Adds a new part to multipart form. This new part is text data.
*
* @param    c_PartName (in): File to upload.
* @param    pc_Text (in): Text to add.
* @return   B_OK if success.
* @return   B_ERROR otherwise.
******************************************************************************/
status_t Http::AddTextFormPart(const BString &c_PartName, 
                              const BString &c_Text)
{

   CURLFORMcode   i_CurlFormRet = CURL_FORMADD_OK;

   i_CurlFormRet = curl_formadd(&Post, &Last,
         CURLFORM_COPYNAME, c_PartName.String(),
         CURLFORM_COPYCONTENTS, c_Text.String(),
         CURLFORM_END);

   if( i_CurlFormRet == CURL_FORMADD_OK)
      return(B_OK);
   else
      return(B_ERROR);
} 


/**
* @date     22/12/2009
* 
*  Posts the form to the server. The form must have previousloy been configured
*  via @ref Http::AddFileFormPart and @ref Http::AddTextFormPart.
*
* @return   B_OK if success.
* @return   B_ERROR otherwise.
******************************************************************************/
status_t Http::PostForm(void)
{
   CURLcode i_CurlRet;
   
   i_CurlRet = curl_easy_setopt(p_CurlHandle, CURLOPT_HTTPPOST, Post);
   if(i_CurlRet == 0)
      i_CurlRet = curl_easy_setopt(p_CurlHandle, CURLOPT_WRITEFUNCTION, Http_CurlWrite);
   if(i_CurlRet == 0)
      i_CurlRet = curl_easy_setopt(p_CurlHandle, CURLOPT_WRITEDATA, this);
   if(i_CurlRet == 0)
      i_CurlRet = curl_easy_setopt(p_CurlHandle, CURLOPT_HEADERFUNCTION, Http_CurlWriteHeader);
   if(i_CurlRet == 0)
      i_CurlRet = curl_easy_setopt(p_CurlHandle, CURLOPT_HEADERDATA, this);
   if(i_CurlRet == 0)
   {
      CurlHeadersList = curl_slist_append(CurlHeadersList, "Expect:");
      i_CurlRet = curl_easy_setopt(p_CurlHandle, CURLOPT_HTTPHEADER, CurlHeadersList);
   }

   if(i_CurlRet == 0)
      i_CurlRet = curl_easy_perform(p_CurlHandle);
   if(i_CurlRet == 0)
   {
      /* check http status */
      curl_easy_getinfo(p_CurlHandle, CURLINFO_RESPONSE_CODE, &i_HttpStatus);
      if(i_HttpStatus != 200)
         i_CurlRet = CURLE_HTTP_RETURNED_ERROR;
   }

   if(Post != NULL)
   {
      curl_formfree(Post);
      Post = NULL;
   }
   Last = NULL;
   if(i_CurlRet == 0)
      return(B_OK);
   else
      return(B_ERROR);
}


/**
* @date     22/12/2009
* 
*  Adds a header for the next request.
*
* @param    sz_Header (in): Header to add.
* @return   B_OK if success.
* @return   B_BAD_VALUE if header is NULL.
* @return   B_ERROR otherwise.
******************************************************************************/
status_t Http::AddHeader(const char *sz_Header)
{
   BString *pc_Header;

   if(sz_Header == NULL)
      return(B_BAD_VALUE);

   pc_Header = new BString(sz_Header);
   if(pc_Header != NULL)
   {
      pc_Header->RemoveAll("\r\n");
      if(HeadersList.AddItem(pc_Header) == false)
      {
         delete pc_Header;
         pc_Header = NULL;
      }
   }

   if(pc_Header == NULL)
      return(B_ERROR);
   return(B_OK);
}


/**
* @date     22/12/2009
* 
*  Empties the header list.
*
* @return   Always B_OK.
******************************************************************************/
status_t Http::_EmptyHeaders(void)
{
   BString *pc_Header;


   pc_Header = (BString*)HeadersList.ItemAt((int32)0);
   while(pc_Header != NULL)
   {
      delete pc_Header;
      HeadersList.RemoveItem((int32)0);
      pc_Header = (BString*)HeadersList.ItemAt((int32)0);
   };

   return(B_OK);
}


/**
* @date     22/12/2009
* 
*  Returns the first header that matches the provided header type. The header is
*  searched in the headers of the last received answer.
*
* @param    c_Header (in/out): Header type to search.
* @return   B_OK if success.
* @return   B_NAME_NOT_FOUND if the header is not found.
******************************************************************************/
status_t Http::GetHeader(BString &c_Header)
{
   int32 i_Index;
   BString *pc_CurHeader;

   for(i_Index=0; i_Index<HeadersList.CountItems(); i_Index++)
   {
      pc_CurHeader = (BString*)HeadersList.ItemAt(i_Index);
      if(pc_CurHeader != NULL)
      {
         if(pc_CurHeader->Compare(c_Header, c_Header.CountChars()) == 0)
         {
            c_Header.SetTo(*pc_CurHeader);
            return(B_OK);
         }
      }
   };

   return(B_NAME_NOT_FOUND);
}
