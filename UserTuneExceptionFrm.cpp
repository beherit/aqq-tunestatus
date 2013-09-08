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

void __fastcall TUserTuneExceptionForm::FormCreate(TObject *Sender)
{
  if(ChkSkinEnabled())
  {
	UnicodeString ThemeSkinDir = GetThemeSkinDir();
	if((FileExists(ThemeSkinDir + "\\\\Skin.asz"))&&(!ChkNativeEnabled()))
	{
	  //ThemeSkinDir = StringReplace(ThemeSkinDir, "\\\\", "\\", TReplaceFlags() << rfReplaceAll);
	  //sSkinManager->SkinDirectory = ThemeSkinDir;
	  //sSkinManager->SkinName = "Skin.asz";
	  sSkinProvider->DrawNonClientArea = true;
	  //sSkinManager->Active = true;
	}
	//else
	// sSkinManager->Active = false;
  }
}
//---------------------------------------------------------------------------

void __fastcall TUserTuneExceptionForm::FormShow(TObject *Sender)
{
  //Skorkowanie okna
  if(!ChkSkinEnabled())
  {
	UnicodeString ThemeSkinDir = GetThemeSkinDir();
	if((FileExists(ThemeSkinDir + "\\\\Skin.asz"))&&(!ChkNativeEnabled()))
	{
	  //ThemeSkinDir = StringReplace(ThemeSkinDir, "\\\\", "\\", TReplaceFlags() << rfReplaceAll);
	  //sSkinManager->SkinDirectory = ThemeSkinDir;
	  //sSkinManager->SkinName = "Skin.asz";
	  sSkinProvider->DrawNonClientArea = false;
	  //sSkinManager->Active = true;
	}
	//else
	// sSkinManager->Active = false;
  }
  //Odczyt ustawien
  aLoadSettings->Execute();
}
//---------------------------------------------------------------------------

void __fastcall TUserTuneExceptionForm::aExitExecute(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------

void __fastcall TUserTuneExceptionForm::SaveButtonClick(TObject *Sender)
{
  //Zapis ustawien
  aSaveSettings->Execute();
  //Odswiezenie wyjatkow w rdzeniu wtyczki
  RefreshUserTuneException();
  //Zamkniecie formy
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
  //Usuniecie zaznaczonego elementu
  JIDListBox->DeleteSelected();
  //Wylaczenie przycisku
  DeleteButton->Enabled = false;
  //Wlaczenie mozlisci zapisu ustawien
  SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TUserTuneExceptionForm::aLoadSettingsExecute(TObject *Sender)
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
  TIniFile *Ini = new TIniFile(GetPluginUserDir() + "\\\\TuneStatus\\\\TuneStatus.ini");
  Ini->EraseSection("UserTuneEx");
  if(JIDListBox->Count!=0)
   for(int Count=0;Count<JIDListBox->Count;Count++)
	Ini->WriteString("UserTuneEx",("JID"+IntToStr(Count+1)),JIDListBox->Items->Strings[Count]);
  delete Ini;
}
//---------------------------------------------------------------------------
