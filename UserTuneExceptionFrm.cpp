//---------------------------------------------------------------------------
// Copyright (C) 2009-2013 Krzysztof Grochocki
//
// This file is part of TuneStatus
//
// TuneStatus is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3, or (at your option)
// any later version.
//
// TuneStatus is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GNU Radio; see the file COPYING. If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street,
// Boston, MA 02110-1301, USA.
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "UserTuneExceptionFrm.h"
#include "MainFrm.h"
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
__declspec(dllimport)UnicodeString GetPluginUserDir();
__declspec(dllimport)void RefreshUserTuneException();
//---------------------------------------------------------------------------
__fastcall TUserTuneExceptionForm::TUserTuneExceptionForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TUserTuneExceptionForm::WMTransparency(TMessage &Message)
{
  Application->ProcessMessages();
  if(MainForm->sSkinManager->Active) sSkinProvider->BorderForm->UpdateExBordersPos(true,(int)Message.LParam);
}
//---------------------------------------------------------------------------

void __fastcall TUserTuneExceptionForm::FormShow(TObject *Sender)
{
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
