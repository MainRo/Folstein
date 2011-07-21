
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
* @file     main.cpp
* @author   R. Picard
* @date     23/12/2009
* 
*****************************************************************************/
#include <Mime.h>
#include <Roster.h>
#include <StorageKit.h>
#include <add-ons/tracker/TrackerAddOn.h>

/**
* @date     19/03/2010
* 
*  This code should be removed from here. Registering the mime type should be
*  done in the installer. This current code prevents from installing the
*  software on another path.
*
*  @todo: register only once
*
******************************************************************************/
void RegisterMimeTypes(void)
{
   update_mime_info("/boot/common/add-ons/Tracker/Folstein", false, true, 2);
   update_mime_info("/boot/common/bin/Folstein", false, true, 2);
   create_app_meta_mime("/boot/common/add-ons/Tracker/Folstein", false, true, 2);
   create_app_meta_mime("/boot/common/bin/Folstein", false, true, 2);
}


void process_refs(entry_ref dir_ref, BMessage *p_Msg, void *reserved)
{
   char  *a_Argv[2];
   int i_Refs;
   entry_ref s_FileRef;
   BEntry   c_Entry;
   BPath    c_Path;

   RegisterMimeTypes();
   a_Argv[0] = (char*) "--gui";
   for(i_Refs = 0; p_Msg->FindRef("refs", i_Refs, &s_FileRef) == B_OK; i_Refs++)
   {
      c_Entry.SetTo(&s_FileRef);
      c_Entry.GetPath(&c_Path);
      a_Argv[1] = (char*)c_Path.Path();
      be_roster->Launch("application/x-vnd.Oakbits.Folstein", 2, a_Argv);
   }
   return;
}
