//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "UserTuneExceptionFrm.h"
#include <inifiles.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "sBevel"
#pragma link "sButton"
#pragma link "sLabel"
#pragma link "sListBox"
#pragma link "sSkinManager"
#pragma link "sSkinProvider"
#pragma link "sTabControl"
#pragma resource "*.dfm"
TUserTuneExceptionForm *UserTuneExceptionForm;
//---------------------------------------------------------------------------
__declspec(dllimport)UnicodeString GetThemeSkinDir();
__declspec(dllimport)bool ChkSkinEnabled();
__declspec(dllimport)bool ChkNativeEnabled();
__declspec(dllimport)UnicodeString GetPluginUserDir();
__declspec(dllimport)void RefreshUserTuneException();
//---------------------------------------------------------------------------
__fastcall TUserTuneExceptionForm::TUserTuneExceptionForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TUserTuneExceptionForm::aExitExecute(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------

void __fastcall TUserTuneExceptionForm::SaveButtonClick(TObject *Sender)
{
  aSaveSettings->Execute();
  RefreshUserTuneException();
  Close();
}
//---------------------------------------------------------------------------

void __fastcall TUserTuneExceptionForm::AddButtonClick(TObject *Sender)
{
  UnicodeString JID;
  if(InputQuery("Nowy wyj¹tek","Identyfikator kontaktu:",JID))
  {
	if(!JID.IsEmpty())
	{
	  JIDListBox->Items->Add(JID);
	  SaveButton->Enabled = true;
	}
  }
}
//---------------------------------------------------------------------------

void __fastcall TUserTuneExceptionForm::JIDListBoxClick(TObject *Sender)
{
  if(JIDListBox->ItemIndex!=-1)
   DeleteButton->Enabled = true;
  else
   DeleteButton->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TUserTuneExceptionForm::DeleteButtonClick(TObject *Sender)
{
  JIDListBox->DeleteSelected();
  DeleteButton->Enabled = false;
  SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TUserTuneExceptionForm::aReadSettingsExecute(TObject *Sender)
{
  JIDListBox->Clear();
  TIniFile *Ini = new TIniFile(GetPluginUserDir() + "\\\\TuneStatus\\\\TuneStatus.ini");
  TStringList *JIDList = new TStringList;
  Ini->ReadSection("UserTuneEx",JIDList);
  int JIDListCount = JIDList->Count;
  delete JIDList;
  if(JIDListCount>0)
  {
	for(int Count=0;Count<JIDListCount;Count++)
	{
	  UnicodeString JID = Ini->ReadString("UserTuneEx",("JID"+IntToStr(Count+1)), "");
	  if(!JID.IsEmpty()) JIDListBox->Items->Add(JID);
    }
  }
  delete Ini;
}
//---------------------------------------------------------------------------

void __fastcall TUserTuneExceptionForm::aSaveSettingsExecute(TObject *Sender)
{
  UnicodeString ExceptionPluginDirectory = GetPluginUserDir();

  TIniFile *Ini = new TIniFile(ExceptionPluginDirectory + "\\\\TuneStatus\\\\TuneStatus.ini");
  Ini->EraseSection("UserTuneEx");
  if(JIDListBox->Count!=0)
   for(int Count=0;Count<JIDListBox->Count;Count++)
	Ini->WriteString("UserTuneEx",("JID"+IntToStr(Count+1)),JIDListBox->Items->Strings[Count]);
  delete Ini;
}
//---------------------------------------------------------------------------

void __fastcall TUserTuneExceptionForm::CancelButtonClick(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------

void __fastcall TUserTuneExceptionForm::FormCreate(TObject *Sender)
{
  if(ChkSkinEnabled())
  {
	UnicodeString ThemeSkinDir = GetThemeSkinDir();
	if((FileExists(ThemeSkinDir + "\\\\Skin.asz"))&&(!ChkNativeEnabled()))
	{
	  sSkinProvider->DrawNonClientArea = true;
	}
  }
}
//---------------------------------------------------------------------------

void __fastcall TUserTuneExceptionForm::FormShow(TObject *Sender)
{
  if(!ChkSkinEnabled())
  {
	UnicodeString ThemeSkinDir = GetThemeSkinDir();
	if((FileExists(ThemeSkinDir + "\\\\Skin.asz"))&&(!ChkNativeEnabled()))
	{
	  sSkinProvider->DrawNonClientArea = false;
	}
  }
  aReadSettings->Execute();
}
//---------------------------------------------------------------------------
