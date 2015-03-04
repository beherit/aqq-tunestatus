//---------------------------------------------------------------------------
// Copyright (C) 2009-2015 Krzysztof Grochocki
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
#include <inifiles.hpp>
#include <Clipbrd.hpp>
#include <LangAPI.hpp>
#pragma hdrstop
#include "SettingsFrm.h"
#include "UserTuneExceptionFrm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "sBevel"
#pragma link "sButton"
#pragma link "sCheckBox"
#pragma link "sCheckListBox"
#pragma link "sComboBox"
#pragma link "sEdit"
#pragma link "sLabel"
#pragma link "sListBox"
#pragma link "sMemo"
#pragma link "sPageControl"
#pragma link "sSkinManager"
#pragma link "sSkinProvider"
#pragma link "sSpinEdit"
#pragma resource "*.dfm"
TSettingsForm *SettingsForm;
//---------------------------------------------------------------------------
__declspec(dllimport)UnicodeString GetPluginUserDir();
__declspec(dllimport)UnicodeString GetPluginUserDirW();
__declspec(dllimport)UnicodeString GetThemeSkinDir();
__declspec(dllimport)void PluginShowMessage(UnicodeString Text);
__declspec(dllimport)bool ChkSkinEnabled();
__declspec(dllimport)bool ChkThemeAnimateWindows();
__declspec(dllimport)bool ChkThemeGlowing();
__declspec(dllimport)int GetHUE();
__declspec(dllimport)int GetSaturation();
__declspec(dllimport)int GetBrightness();
__declspec(dllimport)UnicodeString EncodeBase64(UnicodeString Str);
__declspec(dllimport)UnicodeString DecodeBase64(UnicodeString Str);
__declspec(dllimport)void BuildTuneStatusFastOperation();
__declspec(dllimport)void DestroyTuneStatusFastOperation();
__declspec(dllimport)void UpdateTuneStatusFastOperation(bool Enabled);
__declspec(dllimport)void FastOperation(bool FromForm);
__declspec(dllimport)void LoadSettings();
__declspec(dllimport)void ChangeUserTuneStatus(bool Enabled);
__declspec(dllimport)void ForceChangeStatus();
//---------------------------------------------------------------------------
UnicodeString AutoModeListText;
//---------------------------------------------------------------------------

__fastcall TSettingsForm::TSettingsForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::WMTransparency(TMessage &Message)
{
	Application->ProcessMessages();
	if(sSkinManager->Active) sSkinProvider->BorderForm->UpdateExBordersPos(true,(int)Message.LParam);
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::FormCreate(TObject *Sender)
{
	//Lokalizowanie formy
	LangForm(this);
	//Wlaczona zaawansowana stylizacja okien
	if(ChkSkinEnabled())
	{
		UnicodeString ThemeSkinDir = GetThemeSkinDir();
		//Plik zaawansowanej stylizacji okien istnieje
		if(FileExists(ThemeSkinDir + "\\\\Skin.asz"))
		{
			//Dane pliku zaawansowanej stylizacji okien
			ThemeSkinDir = StringReplace(ThemeSkinDir, "\\\\", "\\", TReplaceFlags() << rfReplaceAll);
			sSkinManager->SkinDirectory = ThemeSkinDir;
			sSkinManager->SkinName = "Skin.asz";
			//Ustawianie animacji AlphaControls
			if(ChkThemeAnimateWindows()) sSkinManager->AnimEffects->FormShow->Time = 200;
			else sSkinManager->AnimEffects->FormShow->Time = 0;
			sSkinManager->Effects->AllowGlowing = ChkThemeGlowing();
			//Zmiana kolorystyki AlphaControls
			sSkinManager->HueOffset = GetHUE();
			sSkinManager->Saturation = GetSaturation();
			sSkinManager->Brightness = GetBrightness();
			//Aktywacja skorkowania AlphaControls
			sSkinManager->Active = true;
		}
		//Brak pliku zaawansowanej stylizacji okien
		else sSkinManager->Active = false;
	}
	//Zaawansowana stylizacja okien wylaczona
	else sSkinManager->Active = false;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::FormShow(TObject *Sender)
{
	//Odczyt ustawien
	aLoadSettings->Execute();
	//Dodawanie brakujacych odtwarzaczy do listy
	if(AutoModeCheckListBoxPreview->Items->IndexOf("Winamp/AIMP/KMPlayer")==-1)
		AutoModeCheckListBoxPreview->Items->Add("Winamp/AIMP/KMPlayer");
	if(AutoModeCheckListBoxPreview->Items->IndexOf("Foobar2000")==-1)
		AutoModeCheckListBoxPreview->Items->Add("Foobar2000");
	if(AutoModeCheckListBoxPreview->Items->IndexOf("Windows Media Player")==-1)
		AutoModeCheckListBoxPreview->Items->Add("Windows Media Player");
	if(AutoModeCheckListBoxPreview->Items->IndexOf("VLC media player")==-1)
		AutoModeCheckListBoxPreview->Items->Add("VLC media player");
	if(AutoModeCheckListBoxPreview->Items->IndexOf("Spotify")==-1)
		AutoModeCheckListBoxPreview->Items->Add("Spotify");
	if(AutoModeCheckListBoxPreview->Items->IndexOf("VUPlayer")==-1)
		AutoModeCheckListBoxPreview->Items->Add("VUPlayer");
	if(AutoModeCheckListBoxPreview->Items->IndexOf("XMPlay")==-1)
		AutoModeCheckListBoxPreview->Items->Add("XMPlay");
	if(AutoModeCheckListBoxPreview->Items->IndexOf("Media Player Classic")==-1)
		AutoModeCheckListBoxPreview->Items->Add("Media Player Classic");
	if(AutoModeCheckListBoxPreview->Items->IndexOf("iTunes")==-1)
		AutoModeCheckListBoxPreview->Items->Add("iTunes");
	if(AutoModeCheckListBoxPreview->Items->IndexOf("ALSong")==-1)
		AutoModeCheckListBoxPreview->Items->Add("ALSong");
	if(AutoModeCheckListBoxPreview->Items->IndexOf(GetLangStr("Plugins"))==-1)
		AutoModeCheckListBoxPreview->Items->Add(GetLangStr("Plugins"));
	if(AutoModeCheckListBoxPreview->Items->IndexOf("Screamer Radio")==-1)
		AutoModeCheckListBoxPreview->Items->Add("Screamer Radio");
	if(AutoModeCheckListBoxPreview->Items->IndexOf("aTunes")==-1)
		AutoModeCheckListBoxPreview->Items->Add("aTunes");
	//Dodanie tekstu do TagsBox i ustawienie go jako element pokazywany
	TagsBox->Items->Add(GetLangStr("TagInfo"));
	TagsBox->ItemIndex = 5;
	//Wylaczenie przycisku zastosuj
	SaveButton->Enabled = false;
	//Ustawienie domyslnej karty okna ustawien
	PageControl->ActivePage = PlayersTabSheet;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::FormClose(TObject *Sender, TCloseAction &Action)
{
	//Usuwanie tekstu "Wybierz tag do wstawienia" z TagsBox
	TagsBox->Items->Delete(5);
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::SaveButtonClick(TObject *Sender)
{
	//Wylaczenie przyciskow
	SaveButton->Enabled = false;
	CancelButton->Enabled = false;
	OkButton->Enabled = false;
	//Zapisywanie ustawien
	aSaveSettingsW->Execute();
	//Wlaczenie przyciskow
	CancelButton->Enabled = true;
	OkButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::OkButtonClick(TObject *Sender)
{
	//Wylaczenie przyciskow
	SaveButton->Enabled = false;
	CancelButton->Enabled = false;
	OkButton->Enabled = false;
	//Zapisywanie ustawien
	aSaveSettingsW->Execute();
	//Wlaczenie przyciskow
	CancelButton->Enabled = true;
	OkButton->Enabled = true;
	//Zamkniecie formy
	Close();
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::RunPluginCheckBoxClick(TObject *Sender)
{
	if(RunPluginCheckBox->Focused()) FastOperation(true);
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::AutoModeCheckListBoxPreviewClick(TObject *Sender)
{
	//Stan komponentu ulegl zmianie
	if(AutoModeCheckListBoxPreview->Items->Text!=AutoModeListText)
	{
		//Zapisywanie nowego stanu obslugiwanych odtwarzaczy
		AutoModeListText = AutoModeCheckListBoxPreview->Items->Text;
		//Wlaczenie przycisku zastosuj
		SaveButton->Enabled = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::AutoModeCheckListBoxPreviewDragDrop(
		TObject *Sender, TObject *Source, int X, int Y)
{
	//Komponent docelowy i zrodlowy jest taki sam
	if((Sender->ClassNameIs("TsCheckListBox"))&&(Source->ClassNameIs("TsCheckListBox")))
	{
		TPoint Point;
		Point.x = X;
		Point.y = Y;
		//Upuszczenie na istniejacy element
		if(AutoModeCheckListBoxPreview->ItemAtPos(Point,true)!=-1)
		{
			//Zmiana miejsc elementow
			AutoModeCheckListBoxPreview->Items->Exchange(AutoModeCheckListBoxPreview->ItemIndex,AutoModeCheckListBoxPreview->ItemAtPos(Point,true));
			//Wlaczenie przycisku zastosuj
			SaveButton->Enabled = true;
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::AutoModeCheckListBoxPreviewDragOver(
		TObject *Sender, TObject *Source, int X, int Y, TDragState State,
		bool &Accept)
{
	//Akceptowanie tylko danego komponentu
	Accept = Source->ClassNameIs("TsCheckListBox");
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::AutoModeCheckListBoxPreviewMouseMove(
		TObject *Sender, TShiftState Shift, int X, int Y)
{
	TPoint Point;
	Point.x = X;
	Point.y = Y;
	//Kursor nad istniejacym elementem
	if(AutoModeCheckListBoxPreview->ItemAtPos(Point,true)!=-1)
	{
		//Foobar2000
		if(AutoModeCheckListBoxPreview->Items->Strings[AutoModeCheckListBoxPreview->ItemAtPos(Point,true)]=="Foobar2000")
		{
			AutoModeCheckListBoxPreview->Hint = GetLangStr("FoobarInfo");
			AutoModeCheckListBoxPreview->PopupMenu = FoobarPopupMenu;
			AutoModeCheckListBoxPreview->Cursor = crHelp;
		}
		//WMP
		else if(AutoModeCheckListBoxPreview->Items->Strings[AutoModeCheckListBoxPreview->ItemAtPos(Point,true)]=="Windows Media Player")
		{
			AutoModeCheckListBoxPreview->Hint = GetLangStr("WMPInfo");
			AutoModeCheckListBoxPreview->PopupMenu = WMPPopupMenu;
			AutoModeCheckListBoxPreview->Cursor = crHelp;
		}
		//iTunes
		else if(AutoModeCheckListBoxPreview->Items->Strings[AutoModeCheckListBoxPreview->ItemAtPos(Point,true)]=="iTunes")
		{
			AutoModeCheckListBoxPreview->Hint = GetLangStr("iTunesInfo");
			AutoModeCheckListBoxPreview->PopupMenu = iTunesPopupMenu;
			AutoModeCheckListBoxPreview->Cursor = crHelp;
		}
		//Inne
		else
		{
			AutoModeCheckListBoxPreview->Hint = "";
			AutoModeCheckListBoxPreview->PopupMenu = NULL;
			AutoModeCheckListBoxPreview->Cursor = crDefault;
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::FoobarDownloadClick(TObject *Sender)
{
	ShellExecute(NULL, L"open", L"http://beherit.pl/download/tunestatus-addon-foobar2000/", NULL, NULL, SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::WMPDownloadClick(TObject *Sender)
{
	ShellExecute(NULL, L"open", L"http://beherit.pl/download/tunestatus-addon-wmp/", NULL, NULL, SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::iTunesDownloadClick(TObject *Sender)
{
	ShellExecute(NULL, L"open", L"http://beherit.pl/download/tunestatus-addon-itunes/", NULL, NULL, SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::TagsBoxCloseUp(TObject *Sender)
{
	//Dodanie tekstu do TagsBox i ustawienie go jako element pokazywany
	if(TagsBox->Text.IsEmpty())
	{
		TagsBox->Items->Add(GetLangStr("TagInfo"));
		TagsBox->ItemIndex = 5;
	}
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::TagsBoxDropDown(TObject *Sender)
{
	//Usuwanie tekstu "Wybierz tag do wstawienia" z TagsBox
	TagsBox->Items->Delete(5);
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::TagsBoxSelect(TObject *Sender)
{
	//Pobranie tekstu wybranego elementu
	UnicodeString Tag = TagsBox->Text;
	//Parsowanie wybranego tagu
	Tag = Tag.Delete(Tag.Pos("("),Tag.Length());
	Tag = Tag.Trim();
	//Wybrany tag nie zostal juz wybrany
	if(!PreviewStatusMemo->Text.Pos(Tag))
	{
		UnicodeString Before,After;
		//Pobranie tekstu przed kusorem
		Before = PreviewStatusMemo->Text.SubString(0,PreviewStatusMemo->SelStart);
		//Pobranie tekstu po kursorze
		After = PreviewStatusMemo->Text.SubString(PreviewStatusMemo->SelStart+1,(PreviewStatusMemo->Text).Length());
		//Dodanie tagu miedzy tekst
		PreviewStatusMemo->Clear();
		PreviewStatusMemo->Text = Before + Tag + After;
	}
	//Dodanie tekstu do TagsBox i ustawienie go jako element pokazywany
	TagsBox->Items->Add(GetLangStr("TagInfo"));
	TagsBox->ItemIndex = 5;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::AutoTurnOffCheckBoxClick(TObject *Sender)
{
	AutoTurnOffSpin->Enabled = AutoTurnOffCheckBox->Checked;
	//Wlaczenie przycisku zastosuj
	SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::UserTuneNotificationCheckBoxClick(TObject *Sender)
{
	UserTuneNotificationSpin->Enabled = UserTuneNotificationCheckBox->Checked;
	UserTuneExceptionButton->Enabled = UserTuneNotificationCheckBox->Checked;
	//Wlaczenie przycisku zastosuj
	SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::UserTuneExceptionButtonClick(TObject *Sender)
{
	UserTuneExceptionForm = new TUserTuneExceptionForm(Application);
	UserTuneExceptionForm->SkinManagerEnabled = sSkinManager->Active;
	UserTuneExceptionForm->ShowModal();
	delete UserTuneExceptionForm;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::UserTuneSendCheckBoxClick(TObject *Sender)
{
	UserTuneSendLabel->Enabled = UserTuneSendCheckBox->Checked;
	UserTuneSendSpin->Enabled = UserTuneSendCheckBox->Checked;
	AutoTurnOffUserTuneSendCheckBox->Enabled = UserTuneSendCheckBox->Checked;
	AutoTurnOffUserTuneSendSpinEdit->Enabled = UserTuneSendCheckBox->Checked;
	if(AutoTurnOffUserTuneSendCheckBox->Enabled) AutoTurnOffUserTuneSendSpinEdit->Enabled = AutoTurnOffUserTuneSendCheckBox->Checked;
	//Wlaczenie przycisku zastosuj
	SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aLoadSettingsExecute(TObject *Sender)
{
	TIniFile *Ini = new TIniFile(GetPluginUserDir() + "\\\\TuneStatus\\\\TuneStatus.ini");
	//Obslugiwane odtwarzacze
	AutoModeCheckListBoxPreview->Clear();
	for(int Count=0;Count<15;Count++)
	{
		//Pobieranie elementow
		UnicodeString PlayerID = Ini->ReadString("AutoMode",("Player"+IntToStr(Count+1)),"");
		if(!PlayerID.IsEmpty())
		{
			//Pobieranie statusu wlaczenia elementu
			UnicodeString Enabled = PlayerID;
			Enabled = Enabled.Delete(1,Enabled.Pos(";"));
			//Odkodowywanie ID odtwarzacza
			PlayerID = PlayerID.Delete(PlayerID.Pos(";"),PlayerID.Length());
			if(PlayerID==1) PlayerID = "Winamp/AIMP/KMPlayer";
			else if(StrToInt(PlayerID)==2) PlayerID = "Foobar2000";
			else if(StrToInt(PlayerID)==3) PlayerID = "Windows Media Player";
			else if(StrToInt(PlayerID)==4) PlayerID = "VUPlayer";
			else if(StrToInt(PlayerID)==5) PlayerID = "XMPlay";
			else if(StrToInt(PlayerID)==6) PlayerID = "Media Player Classic";
			else if(StrToInt(PlayerID)==7) PlayerID = "iTunes";
			else if(StrToInt(PlayerID)==8) PlayerID = "ALSong";
			else if(StrToInt(PlayerID)==9) PlayerID = GetLangStr("Plugins");
			else if(StrToInt(PlayerID)==10) PlayerID = "Screamer Radio";
			else if(StrToInt(PlayerID)==11) PlayerID = "aTunes";
			else if(StrToInt(PlayerID)==12) PlayerID = "FREE_ID";
			else if(StrToInt(PlayerID)==13) PlayerID = "FREE_ID";
			else if(StrToInt(PlayerID)==14) PlayerID = "VLC media player";
			else if(StrToInt(PlayerID)==15) PlayerID = "Spotify";
			//Dodawnie odtwarzacza do listy
			AutoModeCheckListBoxPreview->Items->Add(PlayerID);
			AutoModeCheckListBoxPreview->Checked[Count] = StrToBool(Enabled);
		}
	}
	//Pozostalosci po usunietych odtwarzaczach
	while(AutoModeCheckListBoxPreview->Items->IndexOf("FREE_ID")!=-1)
		AutoModeCheckListBoxPreview->Items->Delete(AutoModeCheckListBoxPreview->Items->IndexOf("FREE_ID"));
	//Zapisywanie nowego stanu obslugiwanych odtwarzaczy
	AutoModeListText = AutoModeCheckListBoxPreview->Items->Text;
	//Wyglad opisu
	PreviewStatusMemo->Text = DecodeBase64(Ini->ReadString("Settings", "Status64", EncodeBase64(GetLangStr("DefaultStatus"))));
	//Opoznienie w funkcji zmiany opisu
	SetStatusSpin->Value = Ini->ReadInteger("Settings", "SetStatusDelay", 5);
	//Automatyczne wylaczenie funkcji zmiany opisu przy bezczynnosci
	AutoTurnOffCheckBox->Checked = Ini->ReadBool("Settings", "AutoTurnOff", false);
	AutoTurnOffSpin->Value = Ini->ReadInteger("Settings", "AutoTurnOffDelay", 15);
	//Wlaczanie funkcji zmiany opisu wraz z uruchomieniem
	EnableOnStartCheckBox->Checked = Ini->ReadBool("Settings", "EnableOnStart", false);
	//Pokazywanie przycisku szybkiego wlaczania/wylaczania funkcji zmiany opisu
	FastAccessCheckBox->Checked = Ini->ReadBool("Settings", "FastAccess", true);
	//Informowanie o aktualnym odtwarzanym utworze poprzez User Tune
	UserTuneSendCheckBox->Checked = Ini->ReadBool("UserTune", "Send", false);
	//Opoznienie informowania poprzez User Tune
	UserTuneSendSpin->Value = Ini->ReadInteger("UserTune", "SendDelay", 5);
	if(UserTuneSendSpin->Value<4) UserTuneSendSpin->Value = 4;
	//Automatyczne wylaczenie informowania poprzez User Tune przy bezczynnosci
	AutoTurnOffUserTuneSendCheckBox->Checked = Ini->ReadBool("UserTune", "AutoTurnOff", false);
	AutoTurnOffUserTuneSendSpinEdit->Value = Ini->ReadInteger("UserTune", "AutoTurnOffDelay", 15);
	//Powiadomienie o notyfikacji User Tune od kontaktow
	UserTuneNotificationCheckBox->Checked = Ini->ReadBool("UserTune", "Notification", false);
	//Czas wyswietlania chmurki informacyjnej
	UserTuneNotificationSpin->Value = Ini->ReadInteger("UserTune", "Cloud", 6);

	delete Ini;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aSaveSettingsExecute(TObject *Sender)
{
	//Ustawianie domyslnego opisu
	if(!PreviewStatusMemo->Text.Pos("CC_TUNESTATUS"))
		PreviewStatusMemo->Text = "Obecnie s�ucham: CC_TUNESTATUS";
	//Wczytanie pliku INI
	TIniFile *Ini = new TIniFile(GetPluginUserDir() + "\\\\TuneStatus\\\\TuneStatus.ini");
	//Zapis ustawien trybu automatycznego
	Ini->EraseSection("AutoMode");
	for(int Count=0;Count<AutoModeCheckListBoxPreview->Items->Count;Count++)
	{
		UnicodeString Player = AutoModeCheckListBoxPreview->Items->Strings[Count];
		if(Player=="Winamp/AIMP/KMPlayer") Player = 1;
		else if(Player=="Foobar2000") Player = 2;
		else if(Player=="Windows Media Player") Player = 3;
		else if(Player=="VUPlayer") Player = 4;
		else if(Player=="XMPlay") Player = 5;
		else if(Player=="Media Player Classic") Player = 6;
		else if(Player=="iTunes") Player = 7;
		else if(Player=="ALSong") Player = 8;
		else if(Player==GetLangStr("Plugins")) Player = 9;
		else if(Player=="Screamer Radio") Player = 10;
		else if(Player=="aTunes") Player = 11;
		//else if(Player=="FREE_ID") Player = 12;
		//else if(Player=="FREE_ID") Player = 13;
		else if(Player=="VLC media player") Player = 14;
		else if(Player=="Spotify") Player = 15;
		int Enabled = AutoModeCheckListBoxPreview->Checked[Count];
		Ini->WriteString("AutoMode", ("Player"+IntToStr(Count+1)), (Player+";"+IntToStr(Enabled)));
	}
	//Wyglad opisu
	Ini->WriteString("Settings", "Status64", EncodeBase64(PreviewStatusMemo->Text));
	//Opoznienie w funkcji zmiany opisu
	Ini->WriteInteger("Settings", "SetStatusDelay", SetStatusSpin->Value);
	//Automatyczne wylaczenie funkcji zmiany opisu przy bezczynnosci
	Ini->WriteBool("Settings", "AutoTurnOff", AutoTurnOffCheckBox->Checked);
	Ini->WriteInteger("Settings", "AutoTurnOffDelay", AutoTurnOffSpin->Value);
	//Wlaczanie funkcji zmiany opisu wraz z uruchomieniem
	Ini->WriteBool("Settings", "EnableOnStart", EnableOnStartCheckBox->Checked);
	//Pokazywanie przycisku szybkiego wlaczania/wylaczania funkcji zmiany opisu
	Ini->WriteBool("Settings", "FastAccess", FastAccessCheckBox->Checked);
	//Informowanie o aktualnym odtwarzanym utworze poprzez User Tune
	Ini->WriteBool("UserTune", "Send", UserTuneSendCheckBox->Checked);
	//Opoznienie informowania poprzez User Tune
	Ini->WriteInteger("UserTune", "SendDelay", UserTuneSendSpin->Value);
	//Automatyczne wylaczenie informowania poprzez User Tune przy bezczynnosci
	Ini->WriteBool("UserTune", "AutoTurnOff", AutoTurnOffUserTuneSendCheckBox->Checked);
	Ini->WriteInteger("UserTune", "AutoTurnOffDelay", AutoTurnOffUserTuneSendSpinEdit->Value);
	//Powiadomienie o notyfikacji User Tune od kontaktow
	Ini->WriteBool("UserTune", "Notification", UserTuneNotificationCheckBox->Checked);
	//Czas wyswietlania chmurki informacyjnej
	Ini->WriteInteger("UserTune", "Cloud", UserTuneNotificationSpin->Value);
	//Zwolnienie pliku INI
	delete Ini;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aResetSettingsExecute(TObject *Sender)
{
	//Usuwanie wszystkich elementow
	AutoModeCheckListBoxPreview->Clear();
	//Dodawanie elementow na nowo
	AutoModeCheckListBoxPreview->Items->Add("Winamp/AIMP/KMPlayer");
	AutoModeCheckListBoxPreview->Items->Add("Foobar2000");
	AutoModeCheckListBoxPreview->Items->Add("Windows Media Player");
	AutoModeCheckListBoxPreview->Items->Add("VLC media player");
	AutoModeCheckListBoxPreview->Items->Add("Spotify");
	AutoModeCheckListBoxPreview->Items->Add("VUPlayer");
	AutoModeCheckListBoxPreview->Items->Add("XMPlay");
	AutoModeCheckListBoxPreview->Items->Add("Media Player Classic");
	AutoModeCheckListBoxPreview->Items->Add("iTunes");
	AutoModeCheckListBoxPreview->Items->Add("ALSong");
	AutoModeCheckListBoxPreview->Items->Add(GetLangStr("Plugins"));
	AutoModeCheckListBoxPreview->Items->Add("Screamer Radio");
	AutoModeCheckListBoxPreview->Items->Add("aTunes");
	//Zaznaczanie/odznaczanie odpowiednich checkbox'ow
	for(int Count=0;Count<11;Count++)
		AutoModeCheckListBoxPreview->Checked[Count] = true;
	for(int Count=11;Count<13;Count++)
		AutoModeCheckListBoxPreview->Checked[Count] = false;
	//Wlaczenie przycisku zastosuj
	SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aAllowApplyExecute(TObject *Sender)
{
	//Wlaczenie przycisku zastosuj
	SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aExitExecute(TObject *Sender)
{
	//Zamkniecie formy
	Close();
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aSelectAllExecute(TObject *Sender)
{
	//Zaznaczenie calego tekstu w polu
	PreviewStatusMemo->SelectAll();
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::sSkinManagerSysDlgInit(TacSysDlgData DlgData, bool &AllowSkinning)
{
	AllowSkinning = false;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aSaveSettingsWExecute(TObject *Sender)
{
	//Usuwanie elementow z interfejsu AQQ
	DestroyTuneStatusFastOperation();
	//Zapis ustawien
	aSaveSettings->Execute();
	//Odczytywanie ustawien w rdzeniu wtyczki
	LoadSettings();
	//Tworzenie elementow w interfejsie AQQ
	BuildTuneStatusFastOperation();
	//Wlaczenie/wylaczenie informowania User Tune
	ChangeUserTuneStatus(UserTuneSendCheckBox->Checked);
	//Wymuszenie natychmiastowego ustawienienia w opisie dokonanych zmian
	ForceChangeStatus();
	//Usuwanie tekstu "Wybierz tag do wstawienia" z TagsBox
	TagsBox->Items->Delete(5);
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::iTunesPluginPathClick(TObject *Sender)
{
  //Kopiowanie do schowka sciezki dla iTunes
	Clipboard()->SetTextBuf((GetPluginUserDirW() + "\\TuneStatus\\iTunes.txt").w_str());
	PluginShowMessage(GetLangStr("iTunesPath"));
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::TagsBoxDrawItem(TWinControl *Control, int Index, TRect &Rect,
          TOwnerDrawState State)
{
  //Rozmieszczenie tagow w kontrolce
	TagsBox->Canvas->Brush->Style = bsClear;
	UnicodeString Tag = TagsBox->Items->Strings[Index];
	if(Tag.Pos("(")) Tag = Tag.Delete(Tag.Pos("(")-1,Tag.Length());
	UnicodeString Description = TagsBox->Items->Strings[Index];
	if(Description.Pos("(")) Description = Description.Delete(1, Description.Pos("(")-1);
	else Description = "";
	TagsBox->Canvas->TextOutW(Rect.Left+2,Rect.Top, Tag);
	TagsBox->Canvas->TextOutW(Rect.Right-Canvas->TextWidth(Description)-1,Rect.Top, Description);
}
//---------------------------------------------------------------------------
