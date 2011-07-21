
#ifndef HTTP_H
#define HTTP_H

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
* @file     Http.h
* @author   R. Picard
* @date     21/02/2009
* 
*****************************************************************************/
#include <OS.h>
#include <String.h>
#include <List.h>

extern "C" {
#include <curl/curl.h>
}


/** Base class for http transfers. The class must be subclassed to be used.
 * Http transfers are based on curl. */
class Http
{
   public:
      Http(void);
      virtual ~Http(void);

      status_t SetUrl(const char *sz_Url);
      status_t SetUrl(BString &sz_Url);

      virtual size_t CurlWrite(void *p_Buffer, size_t i_Size, size_t i_Nmemb);
      virtual size_t CurlRead(void *p_Buffer, size_t i_Size, size_t i_Nmemb);
      status_t Reset(void);
      status_t GetHttp(void);
      status_t UploadHttp(size_t i_DataSize);
      status_t PostHttp(void);
      status_t SetPostTextData(const char *sz_Data);
      long     GetStatus(void);
      
      status_t AddFileFormPart(const BString &c_PartName, 
                              const BString &c_FileName, 
                              BString *pc_MimeType);
      status_t AddTextFormPart(const BString &c_PartName, const BString &c_Text);
      status_t PostForm(void);

      status_t AddHeader(const char *sz_Header);
      status_t GetHeader(BString &c_Header);

   private:
      long                    i_HttpStatus;
      CURL                    *p_CurlHandle;
      struct   curl_slist     *CurlHeadersList;
      struct   curl_httppost  *Post;
      struct   curl_httppost  *Last;
      BList HeadersList;

      status_t _EmptyHeaders(void);
};

#endif /* HTTP_H */
