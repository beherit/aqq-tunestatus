//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "UserTuneExceptionFrm.h"
#include <inifiles.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TUserTuneExceptionForm *UserTuneExceptionForm;
//---------------------------------------------------------------------------
__declspec(dllimport)UnicodeString GetPluginUserDir();
__declspec(dllimport)void RefreshUserTuneException();
UnicodeString ePluginDirectory2; //sciezka prywatnego folderu wtyczek
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
  if(InputQuery("Nowy wyj¹tek","JID kontaktu:",JID))
  {
	if(JID!="")
	 JIDListBox->Items->Add(JID);
  }
}
//---------------------------------------------------------------------------

void __fastcall TUserTuneExceptionForm::JIDListBoxClick(TObject *Sender)
{
  if(JIDListBox->ItemIndex!=-1)
   DeleteButton->Enabled=true;
  else
   DeleteButton->Enabled=false;
}
//---------------------------------------------------------------------------

void __fastcall TUserTuneExceptionForm::DeleteButtonClick(TObject *Sender)
{
  JIDListBox->DeleteSelected();
  DeleteButton->Enabled=false;
}
//---------------------------------------------------------------------------

void __fastcall TUserTuneExceptionForm::aReadSettingsExecute(TObject *Sender)
{
  JIDListBox->Clear();

  ePluginDirectory2 = GetPluginUserDir();

  TIniFile *Ini = new TIniFile(ePluginDirectory2 + "\\\\TuneStatus\\\\TuneStatus.ini");
  int ExceptionCount = Ini->ReadInteger("UserTune", "ExceptionCount", 0);
  if(ExceptionCount!=0)
   for(int Count=1;Count<=ExceptionCount;Count++)
	JIDListBox->Items->Add(Ini->ReadString("UserTuneEx" + IntToStr(Count), "JID", ""));
  delete Ini;
}
//---------------------------------------------------------------------------

void __fastcall TUserTuneExceptionForm::aSaveSettingsExecute(TObject *Sender)
{
  ePluginDirectory2 = GetPluginUserDir();

  TIniFile *Ini = new TIniFile(ePluginDirectory2 + "\\\\TuneStatus\\\\TuneStatus.ini");
  Ini->WriteInteger("UserTune", "ExceptionCount", JIDListBox->Count);
  if(JIDListBox->Count!=0)
   for(int Count=0;Count<JIDListBox->Count;Count++)
   Ini->WriteString("UserTuneEx" + IntToStr(Count+1),"JID",JIDListBox->Items->Strings[Count]);
  delete Ini;
}
//---------------------------------------------------------------------------

