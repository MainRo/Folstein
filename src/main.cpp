
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
* @file     main.cpp
* @author   R. Picard
* @date     24/02/2010
* 
*****************************************************************************/
#include <openssl/evp.h>
#include <new>
#include <Application.h>
#include "VtUploader.h"
#include "VtTui.h"
#include "VtGui.h"

#define APP_SIG  "application/x-vnd.Oakbits.Folstein"
#define LINE_SIZE 1024

#define APP_MSG_ANALYZE_FILE  'AnFi'
#define APP_MSG_ANALYZE_HASH  'AnHa'

class VtApp : public BApplication
{
   public :
      VtApp(void);
      virtual ~VtApp(void);
      virtual void ArgvReceived(int32 argc, char **argv);
      virtual void MessageReceived(BMessage *p_Msg);
      virtual bool QuitRequested(void);

   private:
      BString     c_FileName;
      BString     c_Hash;
      VtUploader  *pc_Uploader;
      VtUi        *pc_Ui;
};


static void PrintHelp(void);

/**
* @date     24/02/2010
* 
*  program entry point.
*
* @param    argc (in): argument count.
* @param    argv (in): arguments.
* @return   0 if success.
* @return   -1 otherwise.
******************************************************************************/
int main(int argc, char *argv[])
{
   VtApp c_MainApp;

   if(argc < 2)      
   {
      PrintHelp();
      return(-1);
   }
   OpenSSL_add_all_algorithms();

   c_MainApp.Run();
   EVP_cleanup();
   return(0);
} 


/**
* @date     24/02/2010
* 
*  prints the help of the program.
*
******************************************************************************/
void PrintHelp(void)
{
   printf("Usage: Folstein [--hash 'hash value'] [--force_upload] [--gui] [file to check]\n");
   return;
}


/**
* @date     24/02/2010
* 
*  Application constructor.
*
******************************************************************************/
VtApp::VtApp(): BApplication(APP_SIG),
      c_FileName(), c_Hash(),
      pc_Uploader(NULL), pc_Ui(NULL)
{
   pc_Uploader = new(std::nothrow) VtUploader;
}


/**
* @date     24/02/2010
* 
*  Application destructor.
*
******************************************************************************/
VtApp::~VtApp(void)
{
}


/**
* @date     24/02/2010
* 
*  arguments handler. Here we parse the arguments, and instanciate the ui.
*
* @param    argc (in): argument count.
* @param    argv (in): arguments.
******************************************************************************/
void VtApp::ArgvReceived(int32 argc, char **argv)
{
   bool     b_Gui = false;
   bool     b_ActionFound = false;
   int32 i_CurArg;

   if(argc > 1)      
   {
      for(i_CurArg=1; i_CurArg<argc; i_CurArg++)
      {
         if(strcmp(argv[i_CurArg], "--help") == 0)
         {
            break;
         }
         else if(strcmp(argv[i_CurArg], "--hash") == 0)
         {
            i_CurArg++;
            if(i_CurArg < argc)
            {
               c_Hash.SetTo(argv[i_CurArg]);
               be_app->PostMessage(APP_MSG_ANALYZE_HASH);
               b_ActionFound = true;
               break;
            }
         }
         else if(strcmp(argv[i_CurArg], "--gui") == 0)
         {
            b_Gui = true;
         }
         else if(strcmp(argv[i_CurArg], "--force_upload") == 0)
         {
            pc_Uploader->ForceUpload(true);
         }
         else if(i_CurArg == argc-1)
         {
            c_FileName.SetTo(argv[i_CurArg]);
            be_app->PostMessage(APP_MSG_ANALYZE_FILE);
            b_ActionFound = true;
            break;
         }
      }
   }
   if(b_ActionFound == false)      
   {
      PrintHelp();
      be_app->PostMessage(B_QUIT_REQUESTED);
   }
   else
   {
      if(b_Gui == false)
         pc_Ui = new(std::nothrow) VtTui();
      else
         pc_Ui = new(std::nothrow) VtGui();
      if(pc_Ui != NULL)
      {
         BHandler *p_Handler;

         pc_Ui->SetUploader(pc_Uploader);
         p_Handler = pc_Ui->GetHandler();
         if(p_Handler != NULL)
            pc_Uploader->SetTarget(p_Handler);
      }
   }

}


/**
* @date     24/02/2010
* 
*  Message handler.
*
******************************************************************************/
void VtApp::MessageReceived(BMessage *p_Msg)
{
   status_t i_Ret;

   if(p_Msg == NULL)
      return;

   switch(p_Msg->what)
   {
      case APP_MSG_ANALYZE_HASH:
         i_Ret = pc_Uploader->SetToHash(c_Hash);
         if(i_Ret == B_OK)
         {
            pc_Ui->Start();
            pc_Uploader->Analyze();
         }
         break;

      case APP_MSG_ANALYZE_FILE:
         i_Ret = pc_Uploader->SetToFile(c_FileName);
         if(i_Ret == B_OK)
         {
            pc_Ui->Start();
            pc_Uploader->Analyze();
         }
         break;

      default:
         BApplication::MessageReceived(p_Msg);
         break;
   }
   return;
}

/**
* @date     24/02/2010
* 
*  Exit hook.
*
******************************************************************************/
bool VtApp::QuitRequested(void)
{
   if(pc_Ui != NULL)
      pc_Ui->Stop();

   pc_Uploader->Lock();
   pc_Uploader->Quit();
   return(true);
}
