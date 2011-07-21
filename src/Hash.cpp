
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
* @file     Hash.cpp
* @author   R. Picard
* @date     17/11/2007
* 
*****************************************************************************/
#include <stdio.h>
#include "Hash.h"

#define BUFSIZE  (32*1024)

/**
* @date     17/11/2007
* 
*  Constructor.
* 
* @param    sz_Filename (in): File to compute the hash
******************************************************************************/
Hash::Hash(const char *sz_Filename):
   b_Computed(false), pc_File(NULL)
{
   SetTo(sz_Filename);
}


/**
* @date     17/11/2007
* 
*  Constructor.
* 
******************************************************************************/
Hash::Hash(void):
   b_Computed(false), pc_File(NULL)
{
}


/**
* @date     17/11/2007
* 
*  Destructor.
* 
******************************************************************************/
Hash::~Hash(void)
{
   if(pc_File != NULL)
      delete pc_File;
}


/**
* @date     17/11/2007
* 
*  Sets the file name and path on which the hash will be computed.
* 
* @param    sz_Filename (in): File on which the hash will be computed.
* @return   B_OK if success.
* @return   B_BAD_VALUE if sz_Filename is NULL
* @return   B_ERROR otherwise.
******************************************************************************/
status_t Hash::SetTo(const char *sz_Filename)
{
   if(sz_Filename == NULL)
      return(B_BAD_VALUE);
   
   if(pc_File != NULL)
      delete pc_File;

   pc_File = new BFile(sz_Filename, B_READ_ONLY);
   if( (pc_File == NULL) || (pc_File->InitCheck() != B_OK) )
         return(B_ERROR);
   return(B_OK);
} 


/**
* @date     17/11/2007
* 
*  Sets the entry on which the hash will be computed.
* 
* @param    ps_Ref (in): Entry on which the hash will be computed.
* @return   B_OK if success.
* @return   B_BAD_VALUE if ps_Ref is NULL.
* @return   B_ERROR otherwise.
******************************************************************************/
status_t Hash::SetTo(entry_ref *ps_Ref)
{
   if(ps_Ref == NULL)
      return(B_BAD_VALUE);

   if(pc_File != NULL)
      delete pc_File;
   
   pc_File = new BFile(ps_Ref, B_READ_ONLY);
   if( (pc_File == NULL) || (pc_File->InitCheck() != B_OK) )
         return(B_ERROR);
   return(B_OK);
} 


/**
* @date     17/11/2007
* 
*  Computes the hash and returns the result as binary data.
* 
* @param    pp_HashData (out): Will be filled with hash value.
* @return   B_OK if success.
* @return   B_BAD_VALUE if pp_HashData is NULL.
* @return   B_ERROR otherwise.
******************************************************************************/
status_t Hash::GetData(unsigned char **pp_HashData)
{   
   if(pp_HashData == NULL)
      return(B_BAD_VALUE);

   if(_Compute() != B_OK)
      return(B_ERROR);

   *pp_HashData = a_Hash;
   return(B_OK);
}


/**
* @date     17/11/2007
* 
*  Computes the hash and returns the result as a string.
* 
* @param    ps_String (out): Will be filled with hash value.
* @return   B_OK if success.
* @return   B_BAD_VALUE if ps_String is NULL.
* @return   B_ERROR otherwise.
******************************************************************************/
status_t Hash::GetString(BString *ps_String)
{   
   int i;
   char  sz_Val[4];

   if(ps_String == NULL)
      return(B_BAD_VALUE);

   if(_Compute() != B_OK)
      return(B_ERROR);

   for (i=0; i<SHA_DIGEST_LENGTH; i++)
   {
      sprintf(sz_Val, "%.2X", a_Hash[i]);
      *ps_String << sz_Val;
   }
   return(B_OK);
}


/**
* @date     13/01/2008
* 
*  Returns the size of the hash.
* 
* @return   Hash size.
******************************************************************************/
uint32 Hash::GetSize(void)
{
   return(SHA_DIGEST_LENGTH);
} 


/**
* @date     13/01/2008
* 
*  Computes the has hof the file.
* 
* @return   B_OK if success.
* @return   B_BAD_VALUE if no file/entry was set.
* @return   B_ERROR otherwise.
******************************************************************************/
status_t Hash::_Compute(void)
{   
   SHA_CTX s_Context;
   int i;
   /** @bug : computing several hash at the same time will fail */
   static unsigned char ai_Buf[BUFSIZE];

   if(pc_File == NULL)
      return(B_BAD_VALUE);

   if(b_Computed == true)
      return(B_OK);
   
   pc_File->Seek(0, SEEK_SET);
   SHA1_Init(&s_Context);
   while(1)
   {
      i = pc_File->Read(ai_Buf, BUFSIZE);
      if (i <= 0) 
         break;
      SHA1_Update(&s_Context, ai_Buf, (unsigned long)i);
   };
   SHA1_Final(&(a_Hash[0]),&s_Context);
   b_Computed = true;
   return(B_OK);
}


