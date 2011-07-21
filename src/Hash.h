
#ifndef HASH_H
#define HASH_H

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
* @file     Hash.h
* @author   R. Picard
* @date     17/11/2007
* 
*****************************************************************************/
extern "C"
{
#include <openssl/sha.h>
}

#include <Entry.h>
#include <File.h>
#include <String.h>

/** Hash computes the hash value of an input file. Only SHA1hash is generated
 * for the moment. This shoudl be extended to other algorithms later. The
 * computation is done with openssl. */
class Hash
{
   public:
      Hash(void);
      Hash(const char *sz_Filename);
      ~Hash(void);
     
      status_t SetTo(const char *sz_Filename);
      status_t SetTo(entry_ref *ps_Ref);
      status_t GetString(BString *ps_String);
      status_t GetData(unsigned char **pp_HashData);
      uint32   GetSize(void);

   private:
      status_t _Compute(void);

      bool  b_Computed;
      BFile  *pc_File;
      unsigned char  a_Hash[SHA_DIGEST_LENGTH];
};
#endif /* HASH_H */


