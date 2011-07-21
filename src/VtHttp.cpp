
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
* @file     VtHttp.cpp
* @author   R. Picard
* @date     10/12/2009
* 
*****************************************************************************/
#include "VtHttp.h"
#include <string.h>



/**
* @date     10/12/2009
* 
*  Constructor.
*
******************************************************************************/
VtHttp::VtHttp() : Http()
{
   i_AvCount = 0;
}


/**
* @date     10/12/2009
* 
*  Destructor.
*
******************************************************************************/
VtHttp::~VtHttp()
{
   /// @todo delete result list items
}


/**
* @date     10/12/2009
* 
*  Resets the http connection settings.
*
* @return   B_OK if success.
* @return   Error otherwise.
******************************************************************************/
status_t VtHttp::Reset(void)
{
   c_ResultXHtml.SetTo("");
   return(Http::Reset());
}


/**
* @date     10/12/2009
* 
*  Queries VT webserver to know if a hash is already known.
*
* @return   B_OK the has exists in VT database.
* @return   B_NAME_NOT_FOUND if the hash is not in VT database.
* @return   B_ERROR if an error occurs.
******************************************************************************/
status_t VtHttp::ConsultHash(void)
{
   long i_HttpStatus;
   status_t i_Ret;
   BString  c_PostData;
   BString  c_HttpHeader;

   Reset();
   SetUrl("http://www.virustotal.com/search.html");
   c_PostData.SetTo("chain=");
   c_PostData.Append(c_HashString);
   SetPostTextData(c_PostData.String());

   PostHttp();
   i_Ret = B_ERROR;
   i_HttpStatus = GetStatus();
   if(i_HttpStatus == 303)
   {
      c_HttpHeader.SetTo("Location: ");
      GetHeader(c_HttpHeader);
      c_HttpHeader.Remove(0, 10);
      if(c_HttpHeader.Compare("http://www.virustotal.com/search.html?notfound=1") == 0)
      {
         i_Ret = B_NAME_NOT_FOUND;
      }
      else
      {
         c_ResultUrl.SetTo(c_HttpHeader);
         i_Ret = B_OK;
      }
   }
   return(i_Ret);
}


/**
* @date     10/12/2009
* 
*  Sets the hash of the file to analyze.
*
* @param    pc_Hash (in): File hash.
* @return   B_OK
******************************************************************************/
status_t VtHttp::SetHash(const BString &c_Hash)
{
   c_HashString.SetTo(c_Hash);
   return(B_OK);
}


/**
* @date     10/12/2009
* 
*  Uploads the file for analysis.
*
* @param    c_FileName (in): Path of the file to analyze.
* @return   B_OK if success.
******************************************************************************/
status_t VtHttp::UploadFile(const BString &c_FileName)
{
   status_t i_Ret;
   BString c_PartName, c_MimeType, c_Identifier;

   i_Ret = _GetUploadIdentifier(c_Identifier);
   if(i_Ret == B_OK)
   {
      BString UploadUrl;

      Reset();
      UploadUrl.SetTo("http://www.virustotal.com/file-upload/file_upload?");
      UploadUrl.Append(c_Identifier);
      SetUrl(UploadUrl);
      c_PartName.SetTo("file");
      c_MimeType.SetTo("application/octet-stream");
      i_Ret = AddFileFormPart(c_PartName, c_FileName, &c_MimeType);
      if(i_Ret == B_OK)
      {
         long i_HttpStatus;
         BString  c_HttpHeader;

         i_Ret = PostForm();
         i_HttpStatus = GetStatus();

         if(i_HttpStatus == 303)
         {
            c_HttpHeader.SetTo("Location: ");
            GetHeader(c_HttpHeader);
            c_HttpHeader.Remove(0, 10);
            if(c_HttpHeader.Compare("/buscaHash.html?notfound") == 0)
            {
               i_Ret = B_NAME_NOT_FOUND;
            }
            else
            {
               c_ResultUrl.SetTo(c_HttpHeader);
               c_ResultUrl.Prepend("http://www.virustotal.com");
               i_Ret = B_OK;
            }
         }
         else
            i_Ret = B_BAD_DATA;
      }
   }
   return(i_Ret);
}


/**
* @date     10/12/2009
* 
*  Returns the status of the file analysis. This is done is issuing a GET
*  request to the result url, and parsing the result html file. There is no rest
*  api to do this more easily.
*
* @return   B_OK if success.
* @return   B_BAD_VALUE if analysis is not completed.
******************************************************************************/
status_t VtHttp::GetAnalysisStatus(void)
{
   status_t i_Ret;

   if(c_ResultUrl.CountChars() == 0)
   {
      return(B_BAD_VALUE);
   }

   i_AvCount = 0;
   Reset();
   i_Ret = SetUrl(c_ResultUrl.String());
   if(i_Ret == B_OK)
   {
      i_Ret = GetHttp();
      if(i_Ret == B_OK)
      {
         _FixupXHtml();
         _GetAvListFromXHtml();
      }
   }
   return(B_OK);
}


/**
* @date     10/12/2009
* 
*  Returns analysis result list. There are entries only for positive matches.
*  The returned pointer belongs to the VtHttp object.
*
* @return   Pointer to the result list.
******************************************************************************/
BList *VtHttp::GetResult(void)
{
   return(&c_Result);
}


/**
* @date     10/12/2009
* 
*  Returns number of antivirus that analyzed the file.
*
* @return   Antivirus count.
******************************************************************************/
int32 VtHttp::GetAvCount()
{
   return(i_AvCount);
}


/**
* @date     11/07/2011
* 
*  Retrieves an upload identifier so that a file can be sent to the server.
*
* @return   B_OK.
******************************************************************************/
status_t VtHttp::_GetUploadIdentifier(BString &sz_Identifier)
{
   status_t i_Ret;

   Reset();
   sz_Identifier.SetTo("");
   i_Ret = SetUrl("http://www.virustotal.com/file-scan/get_identifier.json");
   if(i_Ret == B_OK)
   {
      i_Ret = GetHttp();
      if(i_Ret == B_OK)
      {
         sz_Identifier.SetTo(c_ResultXHtml);
         sz_Identifier.RemoveAll("\"");
      }
   }
   return(i_Ret);
}


/**
* @date     10/12/2009
* 
*  replaces html escape sequences. these ones disturb the xml parser.
*
* @return   B_OK.
******************************************************************************/
status_t VtHttp::_FixupXHtml(void)
{
   c_ResultXHtml.ReplaceAll("&nbsp;", " ");
   c_ResultXHtml.ReplaceAll("&copy;", " ");
   c_ResultXHtml.ReplaceAll("&", " ");
   return(B_OK);
}


/**
* @date     10/12/2009
* 
*  Adds an entry to the result list, from an entry xml "tr" node.
*
* @param    p_Node (in): xml node.
* @return   B_OK if success.
* @return   B_BAD_VALUE if p_Node is NULL.
******************************************************************************/
status_t VtHttp::_GetAvEntryFromNode(xmlNode *p_Node)
{
   uint32 i_CurEntry;
   xmlNode  *p_TextNode;
   VtAvEntry   *pc_AvEntry = NULL;

   if(p_Node == NULL)
      return(B_BAD_VALUE);

   /* At this point we must have a row entry */
   if(strcmp((char*)p_Node->name, "tr") == 0)
   {
      p_Node = p_Node->children;
      i_CurEntry = 0;
      while(p_Node != NULL)
      {
         if(p_Node == NULL)
            break;

         /* Check that entry is a data to avoid array header and footer.
          * The data content is parsed as a "text" child node of the "td" node.
          * */
         if(strcmp((char*)p_Node->name, "td") == 0)
         {
            p_TextNode = p_Node->children;
            if( (p_TextNode != NULL) && (p_TextNode->type == XML_TEXT_NODE) )
            {
               i_CurEntry++;
               if(pc_AvEntry == NULL)
                  pc_AvEntry = new VtAvEntry;

               if(i_CurEntry == 1)
                  pc_AvEntry->Name.SetTo((const char*)p_TextNode->content);
               else if(i_CurEntry == 2)
                  pc_AvEntry->Version.SetTo((const char*)p_TextNode->content);
               else if(i_CurEntry == 3)
                  pc_AvEntry->LastUpdate.SetTo((const char*)p_TextNode->content);
               else if(i_CurEntry == 4)
               {
                  i_AvCount++;
                  if( (strlen((char*)p_TextNode->content) > 0) &&
                     (strcmp((char*)p_TextNode->content, "-") != 0) ) 
                  {
                     pc_AvEntry->Result.SetTo((const char*)p_TextNode->content);
                  }
               }
            }
         }
         p_Node = p_Node->next;
      }

      /* Entries are added only if a malware was identified */
      if(pc_AvEntry != NULL)
      {
         if(pc_AvEntry->Result.CountChars() > 0)
            c_Result.AddItem(pc_AvEntry);
         else
            delete pc_AvEntry;
      }
   }
   return(B_OK);
}


/**
* @date     10/12/2009
* 
*  Parses the xml table containing the av result list. Each array entry has 4
* elements.
*
* @param    NodeSet (in): xml node.
* @return   B_OK if success.
* @return   B_BAD_VALUE if p_Node is NULL.
******************************************************************************/
status_t VtHttp::_GetAvListFromNode(const xmlNodeSet &NodeSet)
{
   xmlNode *p_Node;

   if(NodeSet.nodeNr < 1)
      return(B_BAD_VALUE);

   if(NodeSet.nodeTab[0]->type == XML_ELEMENT_NODE) 
   {
      p_Node = NodeSet.nodeTab[0];
      if(p_Node != NULL)
      {
         p_Node = p_Node->children;
         while(p_Node != NULL)
         {
            _GetAvEntryFromNode(p_Node);
            p_Node = p_Node->next;
         }
      }
   }
   return(B_OK);
}


/**
* @date     10/12/2009
* 
*  Parses the xhtml file to extract the av analysis results.
*
* @return   B_OK.
******************************************************************************/
status_t VtHttp::_GetAvListFromXHtml(void)
{
   htmlDocPtr   p_XmlDoc;

   p_XmlDoc = htmlReadMemory(c_ResultXHtml.String(), c_ResultXHtml.Length(), 
         NULL, "utf-8", 
         XML_PARSE_RECOVER|XML_PARSE_NOERROR|XML_PARSE_NOWARNING);
   if(p_XmlDoc != NULL)
   {
      xmlXPathContextPtr   p_XPathContext;
      xmlXPathObjectPtr  p_XPathObj;

      p_XPathContext = xmlXPathNewContext(p_XmlDoc);
      if(p_XPathContext != NULL)
      {
         p_XPathObj = xmlXPathEvalExpression((const xmlChar*)"//table[@id='tablaMotores']", p_XPathContext);
         if(p_XPathObj != NULL)
         {
            _GetAvListFromNode(*p_XPathObj->nodesetval);
            xmlXPathFreeObject(p_XPathObj);
         }
         xmlXPathFreeContext(p_XPathContext);
      }
      xmlFreeDoc(p_XmlDoc);
   }
   return(B_OK);

}


/**
* @date     10/12/2009
* 
*  Curl write callback. it is assumed that the data is the xhtml result.
*
* @param    p_Buffer (in): Received data.
* @param    i_Size (in): Size of a data element.
* @param    i_Nmemb (in): Number of elements in the buffer.
* @return   number of written elements.
******************************************************************************/
size_t VtHttp::CurlWrite(void *p_Buffer, size_t i_Size, size_t i_Nmemb)
{
   /* printf("%s\n", (char*)p_Buffer); */
   c_ResultXHtml.Append((const char *)p_Buffer);
   return(i_Nmemb);
} 

