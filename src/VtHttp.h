
#ifndef VTHTTP_H
#define VTHTTP_H

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
* @file     VtHttp.h
* @author   R. Picard
* @date     10/12/2009
* 
*****************************************************************************/
#include <String.h>
#include <libxml/HTMLparser.h>
#include <libxml/xpath.h>
#include "Http.h"

struct VtAvEntry
{
   BString  Name;
   BString  Version;
   BString  LastUpdate;
   BString  Result;
};

/** Virus Total http requests. Provides a facility to used VT's more or less
 * REST APIs. */
class VtHttp : public Http
{
   public:
      VtHttp(void);
      virtual ~VtHttp(void);
      status_t Reset(void);
      status_t ConsultHash(void);
      status_t SetHash(const BString &c_Hash);
      status_t UploadFile(const BString &c_FileName);
      status_t GetAnalysisStatus(void);
      BList    *GetResult(void);
      int32    GetAvCount(void);

      virtual size_t CurlWrite(void *p_Buffer, size_t i_Size, size_t i_Nmemb);

   private:
      BString  c_ResultXHtml;
      BString  c_HashString;
      BString  c_ResultUrl;
      BList    c_Result;
      int32    i_AvCount;
      
      status_t _GetUploadIdentifier(BString &sz_Identifier);
      status_t _FixupXHtml(void);
      status_t _GetAvEntryFromNode(xmlNode *p_Node);
      status_t _GetAvListFromNode(const xmlNodeSet &NodeSet);
      status_t _GetAvListFromXHtml(void);
};

#endif /* VTHTTP_H */

