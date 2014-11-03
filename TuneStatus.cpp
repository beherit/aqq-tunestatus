//---------------------------------------------------------------------------
// Copyright (C) 2009-2014 Krzysztof Grochocki
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

#include <vcl.h>
#include <windows.h>
#pragma hdrstop
#pragma argsused
#include <stdio.h>
#include <inifiles.hpp>
#include <Registry.hpp>
#include <tlhelp32.h>
#include <XMLDoc.hpp>
#include <PluginAPI.h>
#include "MainFrm.h"
#include <IdHashMessageDigest.hpp>

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
	return 1;
}
//---------------------------------------------------------------------------

//Uchwyt-do-formy-ustawien---------------------------------------------------
TMainForm *hMainForm;
//Struktury-glowne-----------------------------------------------------------
TPluginLink PluginLink;
TPluginInfo PluginInfo;
//IKONY-W-INTERFEJSIE--------------------------------------------------------
int FASTACCESS;
int FASTACCESS_ON;
int FASTACCESS_OFF;
//UCHWYTY-DO-OKIEN-----------------------------------------------------------
HWND hTimerFrm; //Uchwyt do okna timera
HWND VLCWindowHwnd; //Uchwyt do okna VLC media player
HWND LastfmWindowHwnd; //Uchwyt do okna Last.fm Player
HWND ScreamerRadioWindowHwnd; //Uchwyt do okna Screamer Radio
HWND aTunesWindowHwnd; //Uchwyt do okna iTunes
//LOAD/UNLOAD-PLUGIN---------------------------------------------------------
//Gdy zostalo uruchomione zaladowanie wtyczki
bool LoadExecuted = false;
//SETTINGS-------------------------------------------------------------------
TStringList *SupportedPlayers = new TStringList;
UnicodeString StatusLook;
int SetStatusDelayChk;
bool EnableOnStartChk;
bool FastAccessChk;
bool AutoTurnOffChk;
int AutoTurnOffDelayChk;
bool UserTuneNotifChk;
TStringList *UserTuneExceptions = new TStringList;
int UserTuneCloudChk;
bool UserTuneSendChk;
int UserTuneSendDelayChk;
//Inne-----------------------------------------------------------------------
UnicodeString Status; //Pobrany z odtwarzaczy utwor i sformatowany na nowy opis
UnicodeString StartStatus; //Opis startowy
UnicodeString TempStatus; //Zapamietany opis
UnicodeString UserTuneStatus; //Zmienna opisu do UserTune
UnicodeString UserTuneStatusTMP; //Zmienna tymczasowa opisu do UserTune (do porownyawania zmian)
UnicodeString SongLength;  //Dlugosc odtwarzanego utworu
UnicodeString PluginSong; //Utwor przekazany przez wtyczki np. AQQ Radio
bool JustEnabled = false; //Zasygnalizowanie dopiero co wlaczenia dzialania wtyczki
bool AllowUserTune = false; //Zezwalanie na notyfikacje UserTune
bool AutoModeEnabled = false; //Dzialnie wtyczki wylaczone/wlaczone
bool UserTuneEnabled = false; //User Tune wylaczone/wlaczone
int GetStatusTimerInterval = 0; //Interwal timera pobieranie aktualnego opisu
UnicodeString iTunesPath; //Sciezka do procesu iTunes
TMemIniFile* ContactsNickList = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI")); //Lista JID wraz z nickami
DWORD ReplyListID = 0; //ID wywolania enumeracji listy kontaktow
bool BlockUserTuneSend = false; //Blokada User Tune przez wtyczke Auto-Tune ShortCut
//TIMERY---------------------------------------------------------------------
#define TIMER_ALLOWUSERTUNE 10
#define TIMER_GETSTATUS 20
#define TIMER_STATECHANGED 30
#define TIMER_AUTOMODE 40
#define TIMER_SETSTATUS 50
#define TIMER_AUTOTURNOFF 60
#define TIMER_USERTUNE 70
#define TIMER_SETUSERTUNE 80
//FORWARD-RETRIEVE-DATA-FROM-PLAYERS-----------------------------------------
UnicodeString GetDataFromWinamp();
UnicodeString GetDataFromFoobar();
UnicodeString GetDataFromWMP();
UnicodeString GetDataFromVUPlayer();
UnicodeString GetDataFromXMPlay();
UnicodeString GetDataFromMPC();
UnicodeString GetDataFromiTunes();
UnicodeString GetDataFromALSong();
UnicodeString GetDataFromPlugins();
UnicodeString GetDataFromScreamerRadio();
UnicodeString GetDataFromaTunes();
UnicodeString GetDataFromLastFM();
UnicodeString GetDataFromVLC();
UnicodeString GetDataFromSpotify();
//FORWARD-AQQ-HOOKS----------------------------------------------------------
INT_PTR __stdcall OnContactsUpdate(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnCurrentSong(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnListReady(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnModulesLoaded(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnPreSetNote(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnReplyList(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnStateChange(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnThemeChanged(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnXMLDebug(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceTuneStatusFastOperationItem(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceTuneStatusFastSettingsItem(WPARAM wParam, LPARAM lParam);
//FORWARD-TIMER--------------------------------------------------------------
LRESULT CALLBACK TimerFrmProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//FORWARD-OTHER-FUNCTION-----------------------------------------------------
void UpdateTuneStatusFastOperation(bool Enabled);
//---------------------------------------------------------------------------

//Pobieranie sciezki katalogu prywatnego wtyczek
UnicodeString GetPluginUserDir()
{
  return StringReplace((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,0,0), "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
}
//---------------------------------------------------------------------------

//Pobieranie sciezki do skorki kompozycji
UnicodeString GetThemeSkinDir()
{
  return StringReplace((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETTHEMEDIR,0,0), "\\", "\\\\", TReplaceFlags() << rfReplaceAll) + "\\\\Skin";
}
//---------------------------------------------------------------------------

//Pobieranie sciezki do pliku DLL wtyczki
UnicodeString GetPluginDir()
{
  return StringReplace((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETPLUGINDIR,(WPARAM)(HInstance),0), "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
}
//---------------------------------------------------------------------------

//Sprawdzanie czy  wlaczona jest zaawansowana stylizacja okien
bool ChkSkinEnabled()
{
  TStrings* IniList = new TStringList();
  IniList->SetText((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_FETCHSETUP,0,0));
  TMemIniFile *Settings = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
  Settings->SetStrings(IniList);
  delete IniList;
  UnicodeString SkinsEnabled = Settings->ReadString("Settings","UseSkin","1");
  delete Settings;
  return StrToBool(SkinsEnabled);
}
//---------------------------------------------------------------------------

//Sprawdzanie ustawien animacji AlphaControls
bool ChkThemeAnimateWindows()
{
  TStrings* IniList = new TStringList();
  IniList->SetText((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_FETCHSETUP,0,0));
  TMemIniFile *Settings = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
  Settings->SetStrings(IniList);
  delete IniList;
  UnicodeString AnimateWindowsEnabled = Settings->ReadString("Theme","ThemeAnimateWindows","1");
  delete Settings;
  return StrToBool(AnimateWindowsEnabled);
}
//---------------------------------------------------------------------------
bool ChkThemeGlowing()
{
  TStrings* IniList = new TStringList();
  IniList->SetText((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_FETCHSETUP,0,0));
  TMemIniFile *Settings = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
  Settings->SetStrings(IniList);
  delete IniList;
  UnicodeString GlowingEnabled = Settings->ReadString("Theme","ThemeGlowing","1");
  delete Settings;
  return StrToBool(GlowingEnabled);
}
//---------------------------------------------------------------------------

//Pobieranie ustawien koloru AlphaControls
int GetHUE()
{
  return (int)PluginLink.CallService(AQQ_SYSTEM_COLORGETHUE,0,0);
}
//---------------------------------------------------------------------------
int GetSaturation()
{
  return (int)PluginLink.CallService(AQQ_SYSTEM_COLORGETSATURATION,0,0);
}
//---------------------------------------------------------------------------
int GetBrightness()
{
  return (int)PluginLink.CallService(AQQ_SYSTEM_COLORGETBRIGHTNESS,0,0);
}
//---------------------------------------------------------------------------

//Pobieranie informacji o pliku (wersja itp)
UnicodeString GetFileInfo(wchar_t *ModulePath, String KeyName)
{
  LPVOID lpStr1 = NULL, lpStr2 = NULL;
  WORD* wTmp;
  DWORD dwHandlev = NULL;
  UINT dwLength;
  wchar_t sFileName[1024] = {0};
  wchar_t sTmp[1024] = {0};
  UnicodeString sInfo;
  LPBYTE *pVersionInfo;

  if(ModulePath == NULL) GetModuleFileName( NULL, sFileName, 1024);
  else wcscpy(sFileName, ModulePath);

  DWORD dwInfoSize = GetFileVersionInfoSize(sFileName, &dwHandlev);

  if(dwInfoSize)
  {
	pVersionInfo = new LPBYTE[dwInfoSize];
	if(GetFileVersionInfo(sFileName, dwHandlev, dwInfoSize, pVersionInfo))
	{
	  if(VerQueryValue(pVersionInfo, L"\\VarFileInfo\\Translation", &lpStr1, &dwLength))
	  {
		wTmp = (WORD*)lpStr1;
		swprintf(sTmp, ("\\StringFileInfo\\%04x%04x\\" + KeyName).w_str(), *wTmp, *(wTmp + 1));
		if(VerQueryValue(pVersionInfo, sTmp, &lpStr2, &dwLength)) sInfo = (LPCTSTR)lpStr2;
	  }
	}
	delete[] pVersionInfo;
  }
  return sInfo;
}
//---------------------------------------------------------------------------

//Kodowanie ciagu znakow do Base64
UnicodeString EncodeBase64(UnicodeString Str)
{
  return (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_BASE64,(WPARAM)Str.w_str(),3);
}
//---------------------------------------------------------------------------

//Dekodowanie ciagu znakow z Base64
UnicodeString DecodeBase64(UnicodeString Str)
{
  return (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_BASE64,(WPARAM)Str.w_str(),2);
}
//---------------------------------------------------------------------------

//Pobieranie pseudonimu kontaktu podajac jego JID
UnicodeString GetContactNick(UnicodeString JID)
{
  //Odczyt pseudonimu z pliku INI
  UnicodeString Nick = ContactsNickList->ReadString("Nick",JID,"");
  //Pseudonim nie zostal pobrany
  if(Nick.IsEmpty())
  {
	//Skracanie JID do ladniejszej formy
	if(JID.Pos("@")) JID.Delete(JID.Pos("@"),JID.Length());
	if(JID.Pos(":")) JID.Delete(JID.Pos(":"),JID.Length());
	return JID;
  }
  return Nick;
}
//---------------------------------------------------------------------------

//Pobieranie aktualnego opisu
UnicodeString GetStatus()
{
  TPluginStateChange PluginStateChange;
  PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)&PluginStateChange,0);
  return (wchar_t*)PluginStateChange.Status;
}
//---------------------------------------------------------------------------

//Ustawianie nowego opisu
void SetStatus(UnicodeString SetStatusStatus)
{
  TPluginStateChange PluginStateChange;
  PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)&PluginStateChange,0);
  PluginStateChange.cbSize = sizeof(TPluginStateChange);
  PluginStateChange.Status = SetStatusStatus.w_str();
  PluginStateChange.Force = true;
  PluginLink.CallService(AQQ_SYSTEM_SETSHOWANDSTATUS,0,(LPARAM)&PluginStateChange);
}
//---------------------------------------------------------------------------

//Wymuszenie natychmiastowego ustawienienia w opisie dokonanych zmian
void ForceChangeStatus()
{
  if(AutoModeEnabled) JustEnabled = true;
}
//---------------------------------------------------------------------------

//Sprawdzanie stanu glownego konta - czy rozny od rozlaczony/niewidoczny
bool AllowChangeStatus()
{
  TPluginStateChange PluginStateChange;
  PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)&PluginStateChange,0);
  PluginStateChange.cbSize = sizeof(TPluginStateChange);
  if((PluginStateChange.NewState!=0)&&(PluginStateChange.NewState!=6))
   return true;
  else
   return false;
}
//---------------------------------------------------------------------------

//Sprawdzanie stanu glownego konta dla UserTune - czy rozny od rozlaczony/niewidoczny
bool AllowChangeUserTuneStatus(int Account)
{
  TPluginStateChange PluginStateChange;
  PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)&PluginStateChange,Account);
  PluginStateChange.cbSize = sizeof(TPluginStateChange);
  if((PluginStateChange.OldState!=0)&&(PluginStateChange.OldState!=6))
   return true;
  else
   return false;
}
//---------------------------------------------------------------------------

//Pobieranie nazwy konta przez podanie jego indeksu
UnicodeString ReciveAccountJID(int UserIdx)
{
  TPluginStateChange PluginStateChange;
  PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)&PluginStateChange,UserIdx);
  return (wchar_t*)PluginStateChange.JID;;
}
//---------------------------------------------------------------------------

//Pobieranie utwor przekazanego przez wtyczki np. AQQ Radio
UnicodeString GetPluginSong()
{
  return PluginSong;
}
//---------------------------------------------------------------------------

//Sprawdzanie czy w danym tekscie s¹ tylko liczby
bool TestDigit(UnicodeString Text)
{
  int digit[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  int x = 0;
  while(x < 10)
  {
	if(Text.Pos((String)digit[x]) > 0) return true;
	x++;
  }
  return false;
}
//---------------------------------------------------------------------------

//Wycinanie numeru utworu
UnicodeString CutSongNumber(UnicodeString Text)
{
  if((Text.Pos(". ")>0)&&(Text.Pos(". ")<7))
  {
	//Pobranie tekstu sprzed danej frazy
	UnicodeString TMP = Text;
	TMP = TMP.Delete(TMP.Pos(". "), TMP.Length());
	//Sprawdzanie wystepowania liczb we frazie
	if(TestDigit(TMP))
	{
	  Text = Text.Delete(1, Text.Pos(". "));
	  Text = Text.Trim();
	}
  }
  return Text;
}
//---------------------------------------------------------------------------

//Wycianie adresow URL
UnicodeString CutURL(UnicodeString Text)
{
  while(Text.Pos("http://")>0)
  {
	//Usuniecie wszystkiego sprzed adresu URL
	UnicodeString TMP = Text;
	TMP = TMP.Delete(1,TMP.Pos("http://")-1);
	if(TMP.Pos(" ")>0) TMP = TMP.Delete(TMP.Pos(" "),TMP.Length());
	//Usuniecie wszystkiego pod adresie URL
	UnicodeString TMP2 = Text;
	TMP2 = TMP2.Delete(TMP2.Pos("http://"),TMP2.Length());
	while(TMP2.Pos(" ")) TMP2 = TMP2.Delete(1,TMP2.Pos(" "));
	//Scalenie fragmentow
	TMP = TMP2 + TMP;
	//Usuniecie frazy z opisu
	Text = StringReplace(Text, TMP, "", TReplaceFlags() << rfReplaceAll);
	Text = StringReplace(Text, "  ", " ", TReplaceFlags() << rfReplaceAll);
	Text = Text.Trim();
  }
  while(Text.Pos("www.")>0)
  {
	//Usuniecie wszystkiego sprzed adresu URL
	UnicodeString TMP = Text;
	TMP = TMP.Delete(1,TMP.Pos("www.")-1);
	if(TMP.Pos(" ")>0) TMP = TMP.Delete(TMP.Pos(" "),TMP.Length());
	//Usuniecie wszystkiego pod adresie URL
	UnicodeString TMP2 = Text;
	TMP2 = TMP2.Delete(TMP2.Pos("www."),TMP2.Length());
	while(TMP2.Pos(" ")) TMP2 = TMP2.Delete(1,TMP2.Pos(" "));
	//Scalenie fragmentow
	TMP = TMP2 + TMP;
	//Usuniecie frazy z opisu
	Text = StringReplace(Text, TMP, "", TReplaceFlags() << rfReplaceAll);
	Text = StringReplace(Text, "  ", " ", TReplaceFlags() << rfReplaceAll);
	Text = Text.Trim();
  }
  return Text;
}
//---------------------------------------------------------------------------

//Formatowanie docelowego wygladu opisu
UnicodeString SetStatusLook(UnicodeString Text)
{
  //Pobrany utwor
  if(StatusLook.Pos("CC_TUNESTATUS"))
   Text = StringReplace(StatusLook, "CC_TUNESTATUS", Text, TReplaceFlags());
  //Opis poczatkowy
  if(Text.Pos("CC_STARTSTATUS"))
   Text = StringReplace(Text, "CC_STARTSTATUS", StartStatus, TReplaceFlags());
  //Wersja TuneStatus
  if(Text.Pos("CC_PLUGINVERSION"))
   Text = StringReplace(Text, "CC_PLUGINVERSION", GetFileInfo(GetPluginDir().w_str(), L"FileVersion"), TReplaceFlags());
  //Wersja AQQ
  if(Text.Pos("CC_AQQVERSION"))
   Text = StringReplace(Text, "CC_AQQVERSION", GetFileInfo(NULL, "FileVersion"), TReplaceFlags());
  //Dlugosc utworu
  if(Text.Pos("CC_SONGLENGTH"))
   Text = StringReplace(Text, "CC_SONGLENGTH", SongLength, TReplaceFlags());
  return Text;
}
//---------------------------------------------------------------------------

//Pobieranie sciezki procesu
UnicodeString GetPathOfProces(HWND hWnd)
{
  //Pobieranie PID procesu
  DWORD procesID;
  GetWindowThreadProcessId(hWnd, &procesID);
  //Pobieranie sciezki procesu
  if(procesID)
  {
	MODULEENTRY32 lpModuleEntry = {0};
	HANDLE hSnapShot = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, procesID );
	if(!hSnapShot) return "";
	lpModuleEntry.dwSize = sizeof(lpModuleEntry);
	if(Module32First(hSnapShot, &lpModuleEntry))
	{
	  CloseHandle(hSnapShot);
	  return lpModuleEntry.szExePath;
	}
	CloseHandle(hSnapShot);
	return "";
  }
  return "";
}
//---------------------------------------------------------------------------

bool CALLBACK FindVLC(HWND hWnd, LPARAM lParam)
{
  //Pobranie klasy okna
  wchar_t WindowCaptionNameW[512];
  GetClassNameW(hWnd, WindowCaptionNameW, sizeof(WindowCaptionNameW));
  UnicodeString WindowCaptionName = WindowCaptionNameW;
  //Sprawdenie klasy okna
  if(WindowCaptionName=="QWidget")
  {
	//Pobranie sciezki procesu
	UnicodeString PlayerPath = GetPathOfProces(hWnd);
	//Sprawdzenie nazwy procesu
	if(ExtractFileName(PlayerPath)=="vlc.exe")
	{
	  //Pobranie tekstu okna
	  GetWindowTextW(hWnd, WindowCaptionNameW, sizeof(WindowCaptionNameW));
	  WindowCaptionName = WindowCaptionNameW;
	  //Sprawdzenie tekstu okna
	  if((WindowCaptionName!="vlc")
	  &&(WindowCaptionName!="O programie")
	  &&(WindowCaptionName!=""))
	  {
		//Przypisanie uchwytu
		VLCWindowHwnd = hWnd;
		return false;
	  }
	}
  }
  return true;
}
//---------------------------------------------------------------------------

//Szukanie okna Last.fm
bool CALLBACK FindLastfm(HWND hWnd, LPARAM lParam)
{
  //Pobranie klasy okna
  wchar_t WindowCaptionNameW[512];
  GetClassNameW(hWnd, WindowCaptionNameW, sizeof(WindowCaptionNameW));
  UnicodeString WindowCaptionName = WindowCaptionNameW;
  //Sprawdenie klasy okna
  if(WindowCaptionName=="QWidget")
  {
	//Pobranie sciezki procesu
	UnicodeString PlayerPath = GetPathOfProces(hWnd);
	//Sprawdzenie nazwy procesu
	if(ExtractFileName(PlayerPath)=="LastFM.exe")
	{
	  //Pobranie tekstu okna
	  GetWindowTextW(hWnd, WindowCaptionNameW, sizeof(WindowCaptionNameW));
	  WindowCaptionName = WindowCaptionNameW;
	  //Sprawdzenie tekstu okna
	  if((WindowCaptionName!="LastFM")
	  &&(WindowCaptionName!="Diagnostyka")
	  &&(WindowCaptionName!="Diagnostics")
	  &&(WindowCaptionName!="Poleæ")
	  &&(WindowCaptionName!="Share")
	  &&(WindowCaptionName!="Zaloguj")
	  &&(WindowCaptionName!="Kreator automatycznej aktualizacji")
	  &&(WindowCaptionName!="Kreator ustawieñ")
	  &&(WindowCaptionName!="Opcje programu Last.fm")
	  &&(WindowCaptionName!="Dodaj u¿ytkownika")
	  &&(WindowCaptionName!="Aktualne")
	  &&(WindowCaptionName!="Last.fm — Informacje")
	  &&(WindowCaptionName!="Form")
	  &&(WindowCaptionName!="volume")
	  &&(WindowCaptionName!="recommendTypeBox")
	  &&(WindowCaptionName!="label")
	  &&(WindowCaptionName!="label_2")
	  &&(WindowCaptionName!="label_3")
	  &&(WindowCaptionName!="messageEdit")
	  &&(WindowCaptionName!="userEdit")
	  &&(WindowCaptionName!="buttonBox")
	  &&(WindowCaptionName!="qt_scrollarea_hcontainer")
	  &&(WindowCaptionName!="qt_scrollarea_vcontainer")
	  &&(WindowCaptionName!="qt_scrollarea_viewport")
	  &&(WindowCaptionName!=""))
	  {
		//Przypisanie uchwytu
		LastfmWindowHwnd = hWnd;
		return false;
	  }
	}
  }
  return true;
}
//---------------------------------------------------------------------------

//Szukanie okna ScreamerRadio
bool CALLBACK FindScreamerRadio(HWND hWnd, LPARAM lParam)
{
  //Pobranie klasy okna
  wchar_t WindowCaptionNameW[512];
  GetClassNameW(hWnd, WindowCaptionNameW, sizeof(WindowCaptionNameW));
  UnicodeString WindowCaptionName = WindowCaptionNameW;
  //Sprawdenie klasy okna
  if(WindowCaptionName.Pos("32770"))
  {
	//Pobranie sciezki procesu
	UnicodeString PlayerPath = GetPathOfProces(hWnd);
	//Sprawdzenie nazwy procesu
	if(ExtractFileName(PlayerPath)=="screamer.exe")
	{
	  //Pobranie tekstu okna
	  GetWindowTextW(hWnd, WindowCaptionNameW, sizeof(WindowCaptionNameW));
	  WindowCaptionName = WindowCaptionNameW;
	  //Sprawdzenie tekstu okna
	  if((WindowCaptionName!="Screamer Log")
	  &&(!(WindowCaptionName).IsEmpty()))
	  {
		//Przypisanie uchwytu
		ScreamerRadioWindowHwnd = hWnd;
		return false;
	  }
	}
  }
  return true;
}
//---------------------------------------------------------------------------

//Szukanie okna aTunes
bool CALLBACK FindaTunes(HWND hWnd, LPARAM lParam)
{
  //Pobranie klasy okna
  wchar_t WindowCaptionNameW[512];
  GetClassNameW(hWnd, WindowCaptionNameW, sizeof(WindowCaptionNameW));
  UnicodeString WindowCaptionName = WindowCaptionNameW;
  //Sprawdenie klasy okna
  if(WindowCaptionName=="SunAwtFrame")
  {
	//Pobranie sciezki procesu
	UnicodeString PlayerPath = GetPathOfProces(hWnd);
	//Sprawdzenie nazwy procesu
	if(ExtractFileName(PlayerPath)=="aTunes.exe")
	{
	  //Przypisanie uchwytu
	  aTunesWindowHwnd = hWnd;
	  return false;
	}
  }
  return true;
}
//---------------------------------------------------------------------------

//Pobieranie danych z Winamp/AIMP/KMPlayer
UnicodeString GetDataFromWinamp()
{
  //Pobieranie uchwytu do okna odtwarzacza
  HWND PlayerHwnd = FindWindow(L"Winamp v1.x",NULL);
  if(!PlayerHwnd) PlayerHwnd = FindWindow(L"Studio",NULL);
  //Okno odtwarzacza istnieje
  if(PlayerHwnd)
  {
	//Winamp status !STOP
	if(SendMessage(PlayerHwnd,WM_USER,0,104))
	{
	  //Pobranie tekstu okna
	  wchar_t PlayerCaptionW[512];
	  GetWindowTextW(PlayerHwnd, PlayerCaptionW, sizeof(PlayerCaptionW));
	  UnicodeString PlayerCaption = PlayerCaptionW;
	  //Pomijanie "Winamp"
	  if(PlayerCaption.Pos("Winamp")==1) return "";
	  //Usuwanie "- Winamp"
	  if(PlayerCaption.Pos("- Winamp"))
	  {
		PlayerCaption = PlayerCaption.Delete(PlayerCaption.Pos("- Winamp"), PlayerCaption.Length());
		PlayerCaption = PlayerCaption.Trim();
	  }
	  //Usuwanie "*** "
	  if(PlayerCaption.Pos("*** "))
	  {
		PlayerCaption = PlayerCaption.Delete(1, PlayerCaption.Pos("*** ") + 3);
		PlayerCaption = PlayerCaption.Trim();
	  }
	  //Pobieranie dlugosci utworu
	  int Result = SendMessage(PlayerHwnd,WM_USER,1,105);
	  if(Result!=-1)
	  {
		//Obliczanie sekund
		int Seconds = Result % 60;
		//Obliczanie minut
		Result = Result - Seconds;
		int Minutes = Result / 60;
		//Formatowanie sekund
		if(Seconds<10)
		 SongLength = IntToStr(Minutes) + ":0" + IntToStr(Seconds);
		else
		 SongLength = IntToStr(Minutes) + ":" + IntToStr(Seconds);
	  }
	  else
	   SongLength = "";
	  //Zwrocenie odtwarzanego utworu
	  return PlayerCaption;
	}
	else return "";
  }
  return "";
}
//---------------------------------------------------------------------------

//Pobieranie danych z Foobar2000
UnicodeString GetDataFromFoobar()
{
  //Pobieranie uchwytu do okna odtwarzacza
  HWND PlayerHwnd = FindWindow(L"Foobar_TuneStatus",NULL);
  if(!PlayerHwnd) PlayerHwnd = FindWindow(L"PanelsUI",NULL);
  if(!PlayerHwnd) PlayerHwnd = FindWindow(L"{DA7CD0DE-1602-45e6-89A1-C2CA151E008E}/1",NULL);
  if(!PlayerHwnd) PlayerHwnd = FindWindow(L"{DA7CD0DE-1602-45e6-89A1-C2CA151E008E}",NULL);
  if(!PlayerHwnd) PlayerHwnd = FindWindow(L"{DA7CD0DE-1602-45e6-89A1-C2CA151E008E}",NULL);
  if(!PlayerHwnd) PlayerHwnd = FindWindow(L"{97E27FAA-C0B3-4b8e-A693-ED7881E99FC1}",NULL);
  if(!PlayerHwnd) PlayerHwnd = FindWindow(L"{E7076D1C-A7BF-4f39-B771-BCBE88F2A2A8}",NULL);
  //Okno odtwarzacza istnieje
  if(PlayerHwnd)
  {
	//Pobranie tekstu okna
	wchar_t PlayerCaptionW[512];
	GetWindowTextW(PlayerHwnd, PlayerCaptionW, sizeof(PlayerCaptionW));
	UnicodeString PlayerCaption = PlayerCaptionW;
	//Usuwanie "[foobar2000"
	if(PlayerCaption.Pos("[foobar2000"))
	{
	  PlayerCaption = PlayerCaption.Delete(PlayerCaption.Pos("[foobar2000"), PlayerCaption.Length());
	  PlayerCaption = PlayerCaption.Trim();
	}
	//Usuwanie "foobar2000"
	if(PlayerCaption.Pos("foobar2000"))
	{
	  PlayerCaption = PlayerCaption.Delete(PlayerCaption.Pos("foobar2000"), PlayerCaption.Length());
	  PlayerCaption = PlayerCaption.Trim();
	}
	//Pomijanie "fooAvA"
	if(PlayerCaption.Pos("fooAvA")) return "";
	//Pomijanie "Foo AvA"
	if(PlayerCaption.Pos("Foo AvA")) return "";
	//Zwrocenie odtwarzanego utworu
	return PlayerCaption;
  }
  return "";
}
//---------------------------------------------------------------------------

//Pobieranie danych z Windows Media Player
UnicodeString GetDataFromWMP()
{
  //Sposob I - pobieranie odtwarzanego utworu z rejestru
  TRegistry *WMP = new TRegistry();
  WMP->RootKey = HKEY_CURRENT_USER;
  WMP->OpenKey("Software\\Microsoft\\MediaPlayer\\CurrentMetadata", false);
  //Pobraie autora utworu
  UnicodeString PlayerCaption = WMP->ReadString("Author");
  if(!PlayerCaption.IsEmpty())
  {
    //Pobranie nazwy utworu
	PlayerCaption = PlayerCaption + " - " + WMP->ReadString("Title");
	//Pobieranie dlugosci utworu
	SongLength = WMP->ReadString("durationString");
	if(SongLength.Pos("0")==1) SongLength = SongLength.Delete(1,1);
	//Usuniecie wskaznika
	delete WMP;
	//Zwrocenie odtwarzanego utworu
	return PlayerCaption;
  }
  else SongLength = "";
  //Usuniecie wskaznika
  delete WMP;
  //Sposob II - pobranie tekstu okna
  //Pobieranie uchwytu do okna odtwarzacza
  HWND PlayerHwnd = FindWindow(L"WMPlayerApp",NULL);
  if(!PlayerHwnd) PlayerHwnd = FindWindow(L"Media Player 2",NULL);
  //Okno odtwarzacza istnieje
  if(PlayerHwnd)
  {
	//Pobranie tekstu okna
	wchar_t PlayerCaptionW[512];
	GetWindowTextW(PlayerHwnd, PlayerCaptionW, sizeof(PlayerCaptionW));
	UnicodeString PlayerCaption = PlayerCaptionW;
	//Usuwanie "- Windows Media Player"
	if(PlayerCaption.Pos("- Windows Media Player"))
	{
	  PlayerCaption = PlayerCaption.Delete(PlayerCaption.Pos("- Windows Media Player"), PlayerCaption.Length());
	  PlayerCaption = PlayerCaption.Trim();
	}
	//Pomijanie "Windows Media Player"
	if(PlayerCaption=="Windows Media Player") return "";
	//Zwrocenie odtwarzanego utworu
	return PlayerCaption;
  }
  return "";
}
//---------------------------------------------------------------------------

//Pobieranie danych z VUPlayer
UnicodeString GetDataFromVUPlayer()
{
  //Pobieranie uchwytu do okna odtwarzacza
  HWND PlayerHwnd = FindWindow(L"VUPlayerClass",NULL);
  //Okno odtwarzacza istnieje
  if(PlayerHwnd)
  {
	//Pobranie tekstu okna
	wchar_t PlayerCaptionW[512];
	GetWindowTextW(PlayerHwnd, PlayerCaptionW, sizeof(PlayerCaptionW));
	UnicodeString PlayerCaption = PlayerCaptionW;
	//Usuwanie " ["
	if(PlayerCaption.Pos(" ["))
	{
	  PlayerCaption = PlayerCaption.Delete(PlayerCaption.Pos(" ["), PlayerCaption.Length());
	  PlayerCaption = PlayerCaption.Trim();
	}
	//Zwrocenie odtwarzanego utworu
	return PlayerCaption;
  }
  return "";
}
//---------------------------------------------------------------------------

//Pobieranie danych z XMPlayer
UnicodeString GetDataFromXMPlay()
{
  //Pobieranie uchwytu do okna odtwarzacza
  HWND PlayerHwnd = FindWindow(L"XMPLAY-MAIN",NULL);
  //Okno odtwarzacza istnieje
  if(PlayerHwnd)
  {
	//Pobranie tekstu okna
	wchar_t PlayerCaptionW[512];
	GetWindowTextW(PlayerHwnd, PlayerCaptionW, sizeof(PlayerCaptionW));
	UnicodeString PlayerCaption = PlayerCaptionW;
	//Pomijanie "XMPlay"
	if(PlayerCaption.Pos("XMPlay")) return "";
	//Odtwarzacz jest zatrzymany
	if(!SendMessage(PlayerHwnd,WM_USER,0,104)) return "";
    //Zwrocenie odtwarzanego utworu
	return PlayerCaption;
  }
  return "";
}
//---------------------------------------------------------------------------

//Pobieranie danych z Media Player Classic
UnicodeString GetDataFromMPC()
{
  //Pobieranie uchwytu do okna odtwarzacza
  HWND PlayerHwnd = FindWindow(L"MediaPlayerClassicW",NULL);
  //Okno odtwarzacza istnieje
  if(PlayerHwnd)
  {
	//Pobranie tekstu okna
	wchar_t PlayerCaptionW[512];
	GetWindowTextW(PlayerHwnd, PlayerCaptionW, sizeof(PlayerCaptionW));
	UnicodeString PlayerCaption = PlayerCaptionW;
	//Usuwanie "- Media Player Classic"
	if(PlayerCaption.Pos("- Media Player Classic"))
	{
	  PlayerCaption = PlayerCaption.Delete(PlayerCaption.Pos("- Media Player Classic"), PlayerCaption.Length());
	  PlayerCaption = PlayerCaption.Trim();
	}
	//Zwrocenie odtwarzanego utworu
	return PlayerCaption;
  }
  return "";
}
//---------------------------------------------------------------------------

//Pobieranie danych z iTunes
UnicodeString GetDataFromiTunes()
{
  //Pobieranie uchwytu do okna odtwarzacza
  HWND PlayerHwnd = FindWindow(L"iTunes",NULL);
  //Okno odtwarzacza istnieje
  if(PlayerHwnd)
  {
	//Ustalanie sciezki do procesu iTunes
	if(iTunesPath.IsEmpty())
	{
	  iTunesPath = GetPathOfProces(PlayerHwnd);
	  //Udalo sie pobrac sciezke do procesu
	  if(!iTunesPath.IsEmpty())
	  {
		iTunesPath = StringReplace(iTunesPath, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
		iTunesPath = ExtractFilePath(iTunesPath) + "TuneStatus.txt";
	  }
	  else return "";
	}
	//Pobieranie odtwarzanego utworu z pliku
	try
	{
	  //Jezeli uchwytu do formy nie jest przypisany
	  if(!hMainForm)
	  {
		//Przypisanie uchwytu do formy
		Application->Handle = (HWND)MainForm;
		hMainForm = new TMainForm(Application);
	  }
	  //Odczyt danych z pliku
	  hMainForm->SongFromFile->Lines->LoadFromFile(iTunesPath);
	  //Zwrocenie odtwarzanego utworu
	  return hMainForm->SongFromFile->Text.Trim();
    }
	catch(...) { return ""; }
  }
  else iTunesPath = "";
  return "";
}
//---------------------------------------------------------------------------

//Pobieranie danych z ALSong
UnicodeString GetDataFromALSong()
{
  //Pobieranie uchwytu do okna odtwarzacza
  HWND PlayerHwnd = FindWindow(L"ALSongKernelWindow",NULL);
  //Okno odtwarzacza istnieje
  if(PlayerHwnd)
  {
	//Pobranie tekstu okna
	wchar_t PlayerCaptionW[512];
	GetWindowTextW(PlayerHwnd, PlayerCaptionW, sizeof(PlayerCaptionW));
	UnicodeString PlayerCaption = PlayerCaptionW;
	//Pomijanie "ALSong"
	if(PlayerCaption.Pos("ALSong")) return "";
	//Zwrocenie odtwarzanego utworu
	return PlayerCaption;
  }
  return "";
}
//---------------------------------------------------------------------------

//Pobieranie danych z wtyczek
UnicodeString GetDataFromPlugins()
{
  //Pobieranie utwor przekazanego przez wtyczki np. AQQ Radio
  UnicodeString PlayerCaption = GetPluginSong();
  //Pomyslne pobranie odtwarzanego utworu
  if(!PlayerCaption.IsEmpty())
  {
	//Usuwanie "Wtyczka AQQ Radio:"
	if(PlayerCaption.Pos("Wtyczka AQQ Radio:"))
	{
	  PlayerCaption = PlayerCaption.Delete(1, PlayerCaption.Pos(":"));
	  PlayerCaption = PlayerCaption.Trim();
	  //Usuwanie nazwy radiostacji
	  PlayerCaption = PlayerCaption.Delete(1, PlayerCaption.Pos("- "));
	  PlayerCaption = PlayerCaption.Trim();
	}
	//Zwrocenie odtwarzanego utworu
	return PlayerCaption;
  }
  return "";
}
//---------------------------------------------------------------------------

//Pobieranie danych z ScreamerRadio
UnicodeString GetDataFromScreamerRadio()
{
  //Pobieranie uchwytu do okna odtwarzacza
  if(!ScreamerRadioWindowHwnd) EnumWindows((WNDENUMPROC)FindScreamerRadio,0);
  if(!ScreamerRadioWindowHwnd) EnumDesktopWindows(NULL,(WNDENUMPROC)FindScreamerRadio,0);
  //Okno odtwarzacza istnieje
  if(ScreamerRadioWindowHwnd)
  {
	//Okno jest oknem :D
	if(IsWindow(ScreamerRadioWindowHwnd))
	{
	  //Pobranie tekstu okna
      wchar_t PlayerCaptionW[512];
	  GetWindowTextW(ScreamerRadioWindowHwnd, PlayerCaptionW, sizeof(PlayerCaptionW));
	  UnicodeString PlayerCaption = PlayerCaptionW;\
	  //Pomijanie "Screamer Radio"
	  if(PlayerCaption.Pos("Screamer Radio")) return "";
	  //Zwrocenie odtwarzanego utworu
	  return PlayerCaption;
	}
	else ScreamerRadioWindowHwnd = NULL;
	return "";
  }
  return "";
}
//---------------------------------------------------------------------------

//Pobieranie danych z aTunes
UnicodeString GetDataFromaTunes()
{
  //Pobieranie uchwytu do okna odtwarzacza
  if(!aTunesWindowHwnd) EnumWindows((WNDENUMPROC)FindaTunes,0);
  //Okno odtwarzacza istnieje
  if(!aTunesWindowHwnd)
  {
	//Okno jest oknem :D
	if(IsWindow(aTunesWindowHwnd))
	{
	  //Pobranie tekstu okna
	  wchar_t PlayerCaptionW[512];
	  GetWindowTextW(aTunesWindowHwnd, PlayerCaptionW, sizeof(PlayerCaptionW));
	  UnicodeString PlayerCaption = PlayerCaptionW;
	  //Usuwanie "- aTunes [wersja]""
	  if(PlayerCaption.Pos("- aTunes"))
	  {
		PlayerCaption = PlayerCaption.Delete(PlayerCaption.Pos("- aTunes"), PlayerCaption.Length());
		PlayerCaption = PlayerCaption.Trim();
	  }
	  //Pomijanie "aTunes [wersja]"
	  if(PlayerCaption.Pos("aTunes")) PlayerCaption = NULL;
	  //Zwrocenie odtwarzanego utworu
	  return PlayerCaption;
	}
	else aTunesWindowHwnd = NULL;
	return "";
  }
  return "";
}
//---------------------------------------------------------------------------

//Pobieranie danych z Last.fm Player
UnicodeString GetDataFromLastFM()
{
  //Pobieranie uchwytu do okna odtwarzacza
  if(!LastfmWindowHwnd) EnumWindows((WNDENUMPROC)FindLastfm,0);
  //Okno odtwarzacza istnieje
  if(LastfmWindowHwnd)
  {
	//Okno jest oknem :D
	if(IsWindow(LastfmWindowHwnd))
	{
	  //Pobranie tekstu okna
	  wchar_t PlayerCaptionW[512];
	  GetWindowTextW(LastfmWindowHwnd, PlayerCaptionW, sizeof(PlayerCaptionW));
	  UnicodeString PlayerCaption = PlayerCaptionW;
	  //Pomijanie "Last.fm"
	  if(PlayerCaption.Pos("Last.fm")) return "";
	  //Zwrocenie odtwarzanego utworu
	  return PlayerCaption;
	}
	else LastfmWindowHwnd = NULL;
	return "";
  }
  return "";
}
//---------------------------------------------------------------------------

//Pobieranie danych z VLC media player
UnicodeString GetDataFromVLC()
{
  //Pobieranie uchwytu do okna odtwarzacza
  if(!VLCWindowHwnd) EnumWindows((WNDENUMPROC)FindVLC,0);
  //Okno odtwarzacza istnieje
  if(VLCWindowHwnd)
  {
	//Okno jest oknem :D
	if(IsWindow(VLCWindowHwnd))
	{
	  //Pobranie tekstu okna
	  wchar_t PlayerCaptionW[512];
	  GetWindowTextW(VLCWindowHwnd, PlayerCaptionW, sizeof(PlayerCaptionW));
	  UnicodeString PlayerCaption = PlayerCaptionW;
	  //Pomijanie "VLC media player"
	  if(PlayerCaption=="VLC media player") return "";
	  //Usuwanie "- VLC media player"
	  if(PlayerCaption.Pos("- VLC media player"))
	  {
		PlayerCaption = PlayerCaption.Delete(PlayerCaption.Pos("- VLC media player"), PlayerCaption.Length());
		PlayerCaption = PlayerCaption.Trim();
	  }
	  //Zwrocenie odtwarzanego utworu
	  return PlayerCaption;
	}
	else VLCWindowHwnd = NULL;
	return "";
  }
  return "";
}
//---------------------------------------------------------------------------

//Pobieranie danych z Spotify
UnicodeString GetDataFromSpotify()
{
  //Pobieranie uchwytu do okna odtwarzacza
  HWND PlayerHwnd = FindWindow(L"SpotifyMainWindow",NULL);
  //Okno odtwarzacza istnieje
  if(PlayerHwnd)
  {
	//Pobranie tekstu okna
	wchar_t PlayerCaptionW[512];
	GetWindowTextW(PlayerHwnd, PlayerCaptionW, sizeof(PlayerCaptionW));
	UnicodeString PlayerCaption = PlayerCaptionW;
	//Pomijanie "Spotify"
	if(PlayerCaption=="Spotify") return "";
	//Usuwanie "Spotify -"
	if(PlayerCaption.Pos("Spotify -"))
	{
	  PlayerCaption = PlayerCaption.Delete(PlayerCaption.Pos("Spotify -"), 9);
	  PlayerCaption = PlayerCaption.Trim();
	}
	//Pomijanie reklam Spotify
	if(PlayerCaption.Pos("Spotify")) return "";
	//Zwrocenie odtwarzanego utworu
	return PlayerCaption;
  }
  return "";
}
//---------------------------------------------------------------------------

UnicodeString GetDataFromPlayers(bool UserTune)
{
  //Zmienna przechowujaca pobrany utwor
  UnicodeString Text;
  //Pobranie odtwarzanego utworu z danego odtwarzacza
  for(int Count=0;Count<SupportedPlayers->Count;Count++)
  {
	//Uruchomienie pobierania utworu z danego odtwarzacza
	if(SupportedPlayers->Strings[Count]=="Winamp/AIMP/KMPlayer")
	 Text = GetDataFromWinamp();
	else if(SupportedPlayers->Strings[Count]=="Foobar2000")
	 Text = GetDataFromFoobar();
	else if(SupportedPlayers->Strings[Count]=="Windows Media Player")
	 Text = GetDataFromWMP();
	else if(SupportedPlayers->Strings[Count]=="VUPlayer")
	 Text = GetDataFromVUPlayer();
	else if(SupportedPlayers->Strings[Count]=="XMPlay")
	 Text = GetDataFromXMPlay();
	else if(SupportedPlayers->Strings[Count]=="Media Player Classic")
	 Text = GetDataFromMPC();
	else if(SupportedPlayers->Strings[Count]=="iTunes")
	 Text = GetDataFromiTunes();
	else if(SupportedPlayers->Strings[Count]=="ALSong")
	 Text = GetDataFromALSong();
	else if(SupportedPlayers->Strings[Count]=="Wtyczki (np. AQQ Radio)")
	 Text = GetDataFromPlugins();
	else if(SupportedPlayers->Strings[Count]=="Screamer Radio")
	 Text = GetDataFromScreamerRadio();
	else if(SupportedPlayers->Strings[Count]=="aTunes")
	 Text = GetDataFromaTunes();
	else if(SupportedPlayers->Strings[Count]=="Last.fm Player")
	 Text = GetDataFromLastFM();
	else if(SupportedPlayers->Strings[Count]=="VLC media player")
	 Text = GetDataFromVLC();
    else if(SupportedPlayers->Strings[Count]=="Spotify")
	 Text = GetDataFromSpotify();
	//Zablokowanie plikow filmowych
	if(!Text.IsEmpty())
	{
	  if(Text.LowerCase().Pos(".avi")>0) Text = "";
	  if(Text.LowerCase().Pos(".mpg")>0) Text = "";
	  if(Text.LowerCase().Pos(".mpeg")>0) Text = "";
	  if(Text.LowerCase().Pos(".rmvb")>0) Text = "";
	}
	//Pomyslnie pobrano utwor z odtwarza
	if(!Text.IsEmpty())
	{
	  //Usuniecie spacji z pobranego utworu
	  Text = Text.Trim();
	  //Usuniecie zbednych znakow
	  if(Text.LowerCase().Pos(".mp3")>0)
	   Text = Text.Delete(Text.LowerCase().Pos(".mp3"),Text.LowerCase().Pos(".mp3")+3);
	  if(Text.LowerCase().Pos(".wma")>0)
	   Text = Text.Delete(Text.LowerCase().Pos(".wma"),Text.LowerCase().Pos(".wma")+3);
	  if(Text.LowerCase().Pos(".avi")>0)
	   Text = Text.Delete(Text.LowerCase().Pos(".avi"),Text.LowerCase().Pos(".avi")+3);
	  if(Text.LowerCase().Pos(".mpg")>0)
	   Text = Text.Delete(Text.LowerCase().Pos(".mpg"),Text.LowerCase().Pos(".mpg")+3);
	  if(Text.LowerCase().Pos(".mpeg")>0)
	   Text = Text.Delete(Text.LowerCase().Pos(".mpeg"),Text.LowerCase().Pos(".mpeg")+4);
	  if(Text.LowerCase().Pos(".rmvb")>0)
	   Text = Text.Delete(Text.LowerCase().Pos(".rmvb"),Text.LowerCase().Pos(".rmvb")+4);
	  //Wycinanie numeru utworu
	  Text = CutSongNumber(Text);
	  //Wycianie adresow URL
	  Text = CutURL(Text);
	  //Zakonczenie petli
	  Count = SupportedPlayers->Count;
	}
  }
  //Pobrany i sformatowany wstepnie utwor nie jest pusty
  if((!Text.IsEmpty())&&(!UserTune))
  {
	//Formatowanie docelowego wygladu opisu
	Text = SetStatusLook(Text);
	//Usuniecie zbednych spacji
	Text = Text.Trim();
  }
  //Zwrocenie pobranego i sformatowanego pobranego odtwarzanego utworu
  return Text;
}
//---------------------------------------------------------------------------

//Wysylanie notyfikacji UserTune
void SetUserTune(UnicodeString Tune, UnicodeString Time)
{
  UnicodeString XML;
  UnicodeString AccountName;
  //Konwersja specjalnych znakow
  Tune = (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_CONVERTTOXML,0,(WPARAM)Tune.w_str());
  //Pobieranie ilosci kont
  int AccountsCount = PluginLink.CallService(AQQ_FUNCTION_GETUSEREXCOUNT,0,0);
  for(int Account=0;Account<AccountsCount;Account++)
  {
	//Gdy stan konta jest rozny od niewidoczny/rozlaczony oraz posiadamy dane dt. utworu
	if((AllowChangeUserTuneStatus(Account))&&(!Tune.IsEmpty()))
	{
	  //Gdy w zmienna dlugosci utworu jest pusta
	  if(Time.IsEmpty())
	   XML = "<iq type=\"set\" from=\"CC_JID\" id=\"CC_SESSION\"><pubsub xmlns=\"http://jabber.org/protocol/pubsub\"><publish node=\"http://jabber.org/protocol/tune\"><item><tune xmlns=\"http://jabber.org/protocol/tune\"><artist/><rating/><source/><title>CC_TUNESTATUS</title><track/><uri/></tune></item></publish></pubsub></iq>";
	  //W przeciwnym razie dodaje informacje o dlugosci utworu
	  else
	  {
		//Odkodowanie minut na sekund
		try
		{
		  UnicodeString TimeTMP = Time;
		  TimeTMP.Delete(TimeTMP.Pos(":"),TimeTMP.Length());
		  int TimeInt = StrToInt(TimeTMP) * 60;
		  TimeTMP = Time;
		  TimeTMP.Delete(1,TimeTMP.Pos(":"));
		  TimeInt = TimeInt + StrToInt(TimeTMP);
		  Time = IntToStr(TimeInt);
		  //Definicja pakieru XML
		  XML = "<iq type=\"set\" from=\"CC_JID\" id=\"CC_SESSION\"><pubsub xmlns=\"http://jabber.org/protocol/pubsub\"><publish node=\"http://jabber.org/protocol/tune\"><item><tune xmlns=\"http://jabber.org/protocol/tune\"><artist/><length>CC_SONGLENGTH</length><rating/><source/><title>CC_TUNESTATUS</title><track/><uri/></tune></item></publish></pubsub></iq>";
		}
		catch(...)
		{
		  //Blad = definicja standardowego pakietu XML
		  XML = "<iq type=\"set\" from=\"CC_JID\" id=\"CC_SESSION\"><pubsub xmlns=\"http://jabber.org/protocol/pubsub\"><publish node=\"http://jabber.org/protocol/tune\"><item><tune xmlns=\"http://jabber.org/protocol/tune\"><artist/><rating/><source/><title>CC_TUNESTATUS</title><track/><uri/></tune></item></publish></pubsub></iq>";
        }
	  }
	  //Zamiana CC_SESSION na wygenerowany ID
	  XML = StringReplace(XML, "CC_SESSION", ((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETSTRID,0,0)), TReplaceFlags());
	  //Zamiania CC_TUNESTATUS na odtwarzany utwor
	  XML = StringReplace(XML, "CC_TUNESTATUS", Tune, TReplaceFlags());
	  //Zmiana CC_SONGLENGTH na dlugosc utworu
	  XML = StringReplace(XML, "CC_SONGLENGTH", Time, TReplaceFlags());
	  //Zamiania CC_JID na JID konta
	  TPluginStateChange PluginStateChange;
	  PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)&PluginStateChange,Account);
	  UnicodeString JID = ((UnicodeString)(wchar_t*)PluginStateChange.JID) + "/" + ((UnicodeString)(wchar_t*)PluginStateChange.Resource);
	  XML = StringReplace(XML, "CC_JID", JID, TReplaceFlags());
      //Wyslanie pakietu XML
	  PluginLink.CallService(AQQ_SYSTEM_SENDXML,(WPARAM)XML.w_str(),Account);
	}
	//Gdy stan konta jest niewidoczny/rozlaczony lub nie posiadamy danych dt. utworu
	else
	{
	  XML = "<iq type=\"set\" from=\"CC_JID\" id=\"CC_SESSION\"><pubsub xmlns=\"http://jabber.org/protocol/pubsub\"><publish node=\"http://jabber.org/protocol/tune\"><item><tune xmlns=\"http://jabber.org/protocol/tune\"/></item></publish></pubsub></iq>";
	  //Zamiana CC_SESSION na wygenerowany ID
	  XML = StringReplace(XML, "CC_SESSION", ((wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETSTRID,0,0))), TReplaceFlags());
	  //Zamiania CC_JID na JID konta
	  TPluginStateChange PluginStateChange;
	  PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)&PluginStateChange,Account);
	  UnicodeString JID = ((UnicodeString)(wchar_t*)PluginStateChange.JID) + "/" + ((UnicodeString)(wchar_t*)PluginStateChange.Resource);
	  XML = StringReplace(XML, "CC_JID", JID, TReplaceFlags());
      //Wyslanie pakietu XML
	  PluginLink.CallService(AQQ_SYSTEM_SENDXML,(WPARAM)XML.w_str(),Account);
	}
  }
}
//---------------------------------------------------------------------------

//Zmiana stanu wysylanej notyfikacji User Tune
void ChangeUserTuneStatus(bool Enabled)
{
  //Usee Tune wlaczone
  if((Enabled)&&(!UserTuneEnabled))
  {
	//Przekazanie pustego odtwarzanego utworu
	UserTuneStatusTMP = "";
	//Wlaczenie wysylania informacji User Tune
	UserTuneEnabled = true;
	SetTimer(hTimerFrm,TIMER_USERTUNE,1000,(TIMERPROC)TimerFrmProc);
  }
  //User Tune wylaczone
  else if((!Enabled)&&(UserTuneEnabled))
  {
	//Zatrzymanie timerow
	UserTuneEnabled = false;
	KillTimer(hTimerFrm,TIMER_USERTUNE);
	KillTimer(hTimerFrm,TIMER_SETUSERTUNE);
	//Przekazanie pustego odtwarzanego utworu
	UserTuneStatusTMP = "";
	//Wyslanie informacji o pustym odtwarzanym utworze
	SetUserTune("","");
  }
}
//---------------------------------------------------------------------------

//Wylaczanie obslugi User Tune w rdzeniu AQQ
void TurnOffCoreUserTune()
{
  TSaveSetup SaveSetup;
  SaveSetup.Section = L"Settings";
  SaveSetup.Ident = L"Tune";
  SaveSetup.Value = L"0";
  PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)&SaveSetup);
}
//---------------------------------------------------------------------------

//Odswiezanie wyjatkow z notyfikacji User Tune
void RefreshUserTuneException()
{
  UserTuneExceptions->Clear();
  TIniFile *Ini = new TIniFile(GetPluginUserDir() + "\\\\TuneStatus\\\\TuneStatus.ini");
  TStringList *JIDList = new TStringList;
  Ini->ReadSection("UserTuneEx",JIDList);
  int JIDListCount = JIDList->Count;
  delete JIDList;
  if(JIDListCount)
  {
	for(int Count=0;Count<JIDListCount;Count++)
	{
	  UnicodeString JID = Ini->ReadString("UserTuneEx",("JID"+IntToStr(Count+1)), "");
	  if(!JID.IsEmpty()) UserTuneExceptions->Add(JID);
	}
  }
  delete Ini;
}
//---------------------------------------------------------------------------

//Szybkie wlaczenie/wylaczenie dzialania wtyczki
void FastOperation(bool FromForm)
{
  //Jezeli wtyczka jest nieaktywna
  if(!AutoModeEnabled)
  {
	//Pobranie opisu poczatkowego
	StartStatus = GetStatus();
	//Zasygnalizowanie dopiero co wlaczenia dzialania wtyczki
	JustEnabled = true;
	//Wlaczanie timera pobieranie odtwarzanego utworu w odtwarzaczach
	AutoModeEnabled = true;
	SetTimer(hTimerFrm,TIMER_AUTOMODE,1000,(TIMERPROC)TimerFrmProc);
	//Aktualizacja przycisku
	UpdateTuneStatusFastOperation(true);
	//Zmiana statusu checkbox'a na formie
	if((hMainForm)&&(hMainForm->Visible)&&(!FromForm)) hMainForm->RunPluginCheckBox->Checked = true;
  }
  //Jezeli wtyczka jest aktywna
  else
  {
	//Zatrzymanie timerow
	KillTimer(hTimerFrm,TIMER_SETSTATUS);
	AutoModeEnabled = false;
	KillTimer(hTimerFrm,TIMER_AUTOMODE);
	//Aktualizacja przycisku
	UpdateTuneStatusFastOperation(false);
	//Zmiana statusu checkbox'a na formie
	if((hMainForm)&&(hMainForm->Visible)&&(!FromForm)) hMainForm->RunPluginCheckBox->Checked = false;
	//Pobranie aktualnego opisu
	TempStatus = GetStatus();
	//Przywrocenie poczatkowego opisu
	if(StartStatus!=TempStatus)
	{
	  while(GetStatus()!=StartStatus)
	   SetStatus(StartStatus);
	  TempStatus = "";
	}
  }
}
//---------------------------------------------------------------------------

//Serwis szybkiego wlaczenia/wylaczenia dzialania wtyczki
INT_PTR __stdcall ServiceTuneStatusFastOperationItem(WPARAM wParam, LPARAM lParam)
{
  FastOperation(false);

  return 0;
}
//---------------------------------------------------------------------------

//Aktulizacja stanu buttona szybkiego wlaczania/wylaczania
void UpdateTuneStatusFastOperation(bool Enabled)
{
  if(Enabled)
  {
	TPluginAction BuildTuneStatusFastOperationItem;
	ZeroMemory(&BuildTuneStatusFastOperationItem, sizeof(TPluginAction));
	BuildTuneStatusFastOperationItem.cbSize = sizeof(TPluginAction);
	BuildTuneStatusFastOperationItem.pszName = L"TuneStatusFastOperationItemButton";
	BuildTuneStatusFastOperationItem.IconIndex = FASTACCESS_ON;
	PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_UPDATEBUTTON,0,(LPARAM)&BuildTuneStatusFastOperationItem);
  }
  else
  {
	TPluginAction BuildTuneStatusFastOperationItem;
	ZeroMemory(&BuildTuneStatusFastOperationItem, sizeof(TPluginAction));
	BuildTuneStatusFastOperationItem.cbSize = sizeof(TPluginAction);
	BuildTuneStatusFastOperationItem.pszName = L"TuneStatusFastOperationItemButton";
	BuildTuneStatusFastOperationItem.IconIndex = FASTACCESS_OFF;
	PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_UPDATEBUTTON,0,(LPARAM)&BuildTuneStatusFastOperationItem);
  }
}
//---------------------------------------------------------------------------

//Usuwanie elementu szybkiego wlaczania/wylaczania dzialania wtyczki
void DestroyTuneStatusFastOperation()
{
  TPluginAction BuildTuneStatusFastOperationItem;
  ZeroMemory(&BuildTuneStatusFastOperationItem, sizeof(TPluginAction));
  BuildTuneStatusFastOperationItem.cbSize = sizeof(TPluginAction);
  BuildTuneStatusFastOperationItem.pszName = L"TuneStatusFastOperationItemButton";
  PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_DESTROYBUTTON ,0,(LPARAM)&BuildTuneStatusFastOperationItem);
}
//---------------------------------------------------------------------------

//Tworzenie interfejsu szybkiego wlaczania/wylaczania dzialania wtyczki
void BuildTuneStatusFastOperation()
{
  TPluginAction BuildTuneStatusFastOperationItem;
  ZeroMemory(&BuildTuneStatusFastOperationItem, sizeof(TPluginAction));
  BuildTuneStatusFastOperationItem.cbSize = sizeof(TPluginAction);
  BuildTuneStatusFastOperationItem.pszName = L"TuneStatusFastOperationItemButton";
  BuildTuneStatusFastOperationItem.Position = 999;
  if(AutoModeEnabled)
   BuildTuneStatusFastOperationItem.IconIndex = FASTACCESS_ON;
  else
   BuildTuneStatusFastOperationItem.IconIndex = FASTACCESS_OFF;
  BuildTuneStatusFastOperationItem.pszService = L"sTuneStatusFastOperationItem";
  PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_CREATEBUTTON,0,(LPARAM)&BuildTuneStatusFastOperationItem);
}
//---------------------------------------------------------------------------

//Serwis otwarcia formy ustawien
INT_PTR __stdcall ServiceTuneStatusFastSettingsItem(WPARAM wParam, LPARAM lParam)
{
  //Przypisanie uchwytu do formy
  if(!hMainForm)
  {
	Application->Handle = (HWND)MainForm;
	hMainForm = new TMainForm(Application);
  }
  //Zmiana statusu checkbox'a na formie
  hMainForm->RunPluginCheckBox->Checked = AutoModeEnabled;
  //Pokaznie okna
  hMainForm->Show();

  return 0;
}
//---------------------------------------------------------------------------

//Usuwanie elementu szybkiego dostepu do ustawien wtyczki
void DestroyTuneStatusFastSettings()
{
  TPluginAction BuildTuneStatusFastSettingsItem;
  ZeroMemory(&BuildTuneStatusFastSettingsItem, sizeof(TPluginAction));
  BuildTuneStatusFastSettingsItem.cbSize = sizeof(TPluginAction);
  BuildTuneStatusFastSettingsItem.pszName = L"TuneStatusFastSettingsItemButton";
  PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM,0,(LPARAM)&BuildTuneStatusFastSettingsItem);
}
//---------------------------------------------------------------------------

//Tworzenie elementu szybkiego dostepu do ustawien wtyczki
void BuildTuneStatusFastSettings()
{
  TPluginAction BuildTuneStatusFastSettingsItem;
  ZeroMemory(&BuildTuneStatusFastSettingsItem, sizeof(TPluginAction));
  BuildTuneStatusFastSettingsItem.cbSize = sizeof(TPluginAction);
  BuildTuneStatusFastSettingsItem.pszName = L"TuneStatusFastSettingsItemButton";
  BuildTuneStatusFastSettingsItem.pszCaption = L"TuneStatus";
  BuildTuneStatusFastSettingsItem.Position = 0;
  BuildTuneStatusFastSettingsItem.IconIndex = FASTACCESS;
  BuildTuneStatusFastSettingsItem.pszService = L"sTuneStatusFastSettingsItem";
  BuildTuneStatusFastSettingsItem.pszPopupName = L"popPlugins";
  BuildTuneStatusFastSettingsItem.PopupPosition = 0;
  PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)&BuildTuneStatusFastSettingsItem);
}
//---------------------------------------------------------------------------

//Procka okna timera
LRESULT CALLBACK TimerFrmProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if(uMsg==WM_TIMER)
  {
	//Zezwalanie na pokazywanie notyfikacji User Tune
	if(wParam==TIMER_ALLOWUSERTUNE)
	{
	  //Zatrymanie timera
	  KillTimer(hTimerFrm,TIMER_ALLOWUSERTUNE);
	  //Zezwalanie na pokazywanie notyfikacji User Tune
	  AllowUserTune = true;
	}
	//Pobieranie aktualnego opisu
	else if(wParam==TIMER_GETSTATUS)
	{
      //Pobieranie aktualnego opisu
	  StartStatus = GetStatus();
	  //Jezeli opis nie jest pusty lub przekroczono iterwal
	  if((!StartStatus.IsEmpty())||(GetStatusTimerInterval==100))
	  {
		//Zatrzymanie timera
		KillTimer(hTimerFrm,TIMER_GETSTATUS);
		//Zasygnalizowanie dopiero co wlaczenia dzialania wtyczki
		JustEnabled = true;
		//Wlaczenie dzialania wtyczki
		AutoModeEnabled = true;
		SetTimer(hTimerFrm,TIMER_AUTOMODE,1000,(TIMERPROC)TimerFrmProc);
		//Zmiana statusu checkbox'a na formie
		if(hMainForm) hMainForm->RunPluginCheckBox->Checked = true;
		//Aktualizacja przycisku
		if(FastAccessChk) UpdateTuneStatusFastOperation(true);
	  }
	  else
	   GetStatusTimerInterval++;
	}
	//Zmiana opisu + ponowne wlaczenie dzialania wtyczki
	else if(wParam==TIMER_STATECHANGED)
	{
	  //Zatrzymanie timera
	  KillTimer(hTimerFrm,TIMER_STATECHANGED);
	  //Zatrzymanie innych timerow
	  KillTimer(hTimerFrm,TIMER_SETSTATUS);
	  KillTimer(hTimerFrm,TIMER_AUTOMODE);
	  //Pobranie aktualnego opisu
	  TempStatus = GetStatus();
	  //Przywracanie poczatkowego opisu
	  if(StartStatus!=TempStatus)
	  {
		while(GetStatus()!=StartStatus)
		 SetStatus(StartStatus);
		TempStatus = "";
	  }
	  //Zasygnalizowanie dopiero co wlaczenia dzialania wtyczki
	  JustEnabled = true;
	  //Wlaczanie timera pobieranie odtwarzanego utworu w odtwarzaczach
	  AutoModeEnabled = true;
	  SetTimer(hTimerFrm,TIMER_AUTOMODE,1000,(TIMERPROC)TimerFrmProc);
	}
	//Pobieranie odtwarzanego utworu w odtwarzaczach
	else if(wParam==TIMER_AUTOMODE)
	{
	  //Uruchomienie automatycznego trybu pobierania odtwarzanego utworu
	  Status = GetDataFromPlayers(false);
	  //Opis cos zawiera
	  if(!Status.IsEmpty())
	  {
		//Opis jest rozny od poprzedniego opisu
		if(Status!=TempStatus)
		{
		  //Zapisanie opisu do pozniejszego porownania
		  TempStatus = Status;
		  //Zatrzymanie timera ustawiajacego nowy opis
		  KillTimer(hTimerFrm,TIMER_SETSTATUS);
		  //Wlaczenie timera stawiajacego nowy opis z symulacja pierwszego uruchomienia
		  if(JustEnabled)
		   SetTimer(hTimerFrm,TIMER_SETSTATUS,1000,(TIMERPROC)TimerFrmProc);
		  //Normalne wlaczenie timera ustawiajacego nowy opis
		  else
		   SetTimer(hTimerFrm,TIMER_SETSTATUS,SetStatusDelayChk,(TIMERPROC)TimerFrmProc);
		}
	  }
	  //Opis jest pusty
	  else if(Status.IsEmpty())
	  {
		//Opis jest rozny od opisu tymczasowego
		if(StartStatus!=TempStatus)
		{
		  //Zatrzymanie timera ustawiajacego nowy opis
		  KillTimer(hTimerFrm,TIMER_SETSTATUS);
		  //Symulacja pierwszego uruchomienia timera ustawiajacego nowy opis
		  JustEnabled = true;
		  //Przywrocenie opisu poczatkowego
		  TempStatus = StartStatus;
		  while(GetStatus()!=StartStatus)
		   SetStatus(StartStatus);
		}
	  }
	}
	//Ustawianie nowego opisu
	else if(wParam==TIMER_SETSTATUS)
	{
	  //Zatrzymanie timera
	  KillTimer(hTimerFrm,TIMER_SETSTATUS);
	  //Usuwanie symulacju pierwszego uruchomienia
	  JustEnabled = false;
	  //Opis jest inny niz obecny
	  if(Status!=GetStatus())
	  {
		//Zatrzymanie timera automatycznego wylaczania dzialania wtyczki
		KillTimer(hTimerFrm,TIMER_AUTOTURNOFF);
		//Jezeli mozna ustawic nowy opis
		if(AllowChangeStatus())
		{
		  //Ustawianie nowego opisu
		  while(GetStatus()!=Status) SetStatus(Status);
		  //Wlaczenie timera automatycznego wylaczania dzialania wtyczki
		  if(AutoTurnOffChk) SetTimer(hTimerFrm,TIMER_AUTOTURNOFF,AutoTurnOffDelayChk,(TIMERPROC)TimerFrmProc);
		}
		//Nie mozna ustawic nowego opisu
		else
		 //Symulacja pierwszego uruchomienia SetStatusTimer
		 JustEnabled = true;
	  }
	}
	//Automatyczne wylaczenie dzialania wtyczki
	else if(wParam==TIMER_AUTOTURNOFF)
	{
	  //Zatrzymanie wszystkich timerow
	  KillTimer(hTimerFrm,TIMER_AUTOTURNOFF);
	  KillTimer(hTimerFrm,TIMER_SETSTATUS);
	  AutoModeEnabled = false;
	  KillTimer(hTimerFrm,TIMER_AUTOMODE);
	  //Aktualizacja przycisku
	  if(FastAccessChk) UpdateTuneStatusFastOperation(false);
	  //Pobranie aktualnego opisu
	  TempStatus = GetStatus();
	  //Przywracanie poczatkowego opisu
	  if(StartStatus!=TempStatus)
	  {
		while(GetStatus()!=StartStatus)
		 SetStatus(StartStatus);
		TempStatus = "";
	  }
	}
	//Pobieranie odtwarzanego utworu w odtwarzaczach (User Tune)
	else if(wParam==TIMER_USERTUNE)
	{
	  //Uruchomienie automatycznego trybu pobierania odtwarzanego utworu
	  UserTuneStatus = GetDataFromPlayers(true);
	  //Dane zostaly pobrane
	  if(!UserTuneStatus.IsEmpty())
	  {
		//Aktualnie odtwarzany utwor jest rozny od poprzedniego
		if(UserTuneStatus!=UserTuneStatusTMP)
		{
		  //Zatrzymanie timera wyslania ifnormacji o odtwarzanym utworze
		  KillTimer(hTimerFrm,TIMER_SETUSERTUNE);
		  //Przekazanie nowego odtwarzanego utworu
		  UserTuneStatusTMP = UserTuneStatus;
		  //Wlaczenie timera wyslania ifnormacji o odtwarzanym utworze
		  SetTimer(hTimerFrm,TIMER_SETUSERTUNE,UserTuneSendDelayChk,(TIMERPROC)TimerFrmProc);
		}
	  }
	  //Dane nie zostaly pobrane
	  else
	  {
		//Poprzedni odtwazany utwor nie byl pusty
		if(!UserTuneStatusTMP.IsEmpty())
		{
		  //Zatrzymanie timera wyslania ifnormacji o odtwarzanym utworze
		  KillTimer(hTimerFrm,TIMER_SETUSERTUNE);
		  //Przekazanie pustego odtwarzanego utworu
		  UserTuneStatusTMP = "";
		  //Wyslanie informacji o pustym odtwarzanym utworze
		  SetUserTune("","");
		}
	  }
	}
	//Wyslanie ifnormacji o odtwarzanym utworze (User Tune)
	else if(wParam==TIMER_SETUSERTUNE)
	{
	  //Zatrzymanie timera
	  KillTimer(hTimerFrm,TIMER_SETUSERTUNE);
	  //Wyslanie informacji o nowym odtwarzanym utworze
	  SetUserTune(UserTuneStatusTMP,SongLength);
	}

	return 0;
  }

  return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
//---------------------------------------------------------------------------

//Hook na zmianê stanu kontaktu
INT_PTR __stdcall OnContactsUpdate(WPARAM wParam, LPARAM lParam)
{
  //Wlaczono notyfikacje User Tune
  if(UserTuneNotifChk)
  {
	//Pobieranie danych kontatku
	TPluginContact ContactsUpdateContact = *(PPluginContact)wParam;
	//Kontakt nie jest czatem
	if(!ContactsUpdateContact.IsChat)
	{
	  //Pobieranie identyfikatora kontaktu
	  UnicodeString JID = (wchar_t*)ContactsUpdateContact.JID;
	  //Pobieranie indeksu konta
	  UnicodeString UserIdx = ":" + IntToStr(ContactsUpdateContact.UserIdx);
	  //Pobieranie nicku kontaktu
	  UnicodeString Nick = (wchar_t*)ContactsUpdateContact.Nick;
	  //Pobieranie i zapisywanie nicku kontatku
	  ContactsNickList->WriteString("Nick",JID+UserIdx,Nick);
	}
  }

  return 0;
}
//---------------------------------------------------------------------------

//Utwor przekazany przez wtyczki np. AQQ Radio
INT_PTR __stdcall OnCurrentSong(WPARAM wParam, LPARAM lParam)
{
  //Pobranie danych nt utworu
  TPluginSong Song = *(PPluginSong)lParam;
  //Pobranie tytulu utworu
  PluginSong = (wchar_t*)Song.Title;

  return 0;
}
//---------------------------------------------------------------------------

INT_PTR __stdcall OnListReady(WPARAM wParam, LPARAM lParam)
{
  //Wlaczono notyfikacje User Tune
  if(UserTuneNotifChk)
  {
	//Pobranie ID dla enumeracji kontaktów
	ReplyListID = GetTickCount();
	//Wywolanie enumeracji kontaktow
	PluginLink.CallService(AQQ_CONTACTS_REQUESTLIST,(WPARAM)ReplyListID,0);
  }

  return 0;
}
//---------------------------------------------------------------------------

//Notyfikacja zaladowania wszystkich modulow w AQQ
INT_PTR __stdcall OnModulesLoaded(WPARAM wParam, LPARAM lParam)
{
  //Wlaczenie timera
  SetTimer(hTimerFrm,TIMER_ALLOWUSERTUNE,20000,(TIMERPROC)TimerFrmProc);

  return 0;
}
//---------------------------------------------------------------------------

//Notyfikacja zmiany statusu
INT_PTR __stdcall OnPreSetNote(WPARAM wParam, LPARAM lParam)
{
  //Jezeli dzialanie wtyczki jest wlaczone
  if(AutoModeEnabled)
  {
    //Pobranie nowego opisu
	UnicodeString NewStatus = (wchar_t*)lParam;
	//Jezeli nowy opis spelnia ponizsze warunki
	if((NewStatus!=StartStatus)
	&&(NewStatus!=TempStatus)
	&&(NewStatus!=Status)
	&&(!Status.IsEmpty()))
	{
	  //Zmiana opisu startowego
	  StartStatus = NewStatus;
	  return 1;
	}
	else
	 return 0;
  }
  else
   return 0;
}
//---------------------------------------------------------------------------

//Hook na enumeracje listy kontatkow
INT_PTR __stdcall OnReplyList(WPARAM wParam, LPARAM lParam)
{
  //Sprawdzanie ID wywolania enumeracji
  if(wParam==ReplyListID)
  {
	//Pobieranie danych kontatku
	TPluginContact ReplyListContact = *(PPluginContact)wParam;
	//Kontakt nie jest czatem
	if(!ReplyListContact.IsChat)
	{
	  //Pobieranie identyfikatora kontaktu
	  UnicodeString JID = (wchar_t*)ReplyListContact.JID;
	  //Pobieranie indeksu konta
	  UnicodeString UserIdx = ":" + IntToStr(ReplyListContact.UserIdx);
	  //Pobranie nicku kontaktu
	  UnicodeString Nick = (wchar_t*)ReplyListContact.Nick;
	  //Pobieranie i zapisywanie nicku kontatku
	  ContactsNickList->WriteString("Nick",JID+UserIdx,Nick);
	}
  }

  return 0;
}
//---------------------------------------------------------------------------

//Notyfikacja zmiany stanu
INT_PTR __stdcall OnStateChange(WPARAM wParam, LPARAM lParam)
{
  //Jezeli dzialanie wtyczki jest wlaczone i zdefiniowany opis nie jest pusty
  if((AutoModeEnabled)&&(!Status.IsEmpty()))
  {
	//Pobieranie danych
	TPluginStateChange StateChange = *(PPluginStateChange)lParam;
	//Pobranie nowego stanu konta
	int NewState = StateChange.NewState;
	//Blokowanie przy niewidocznym
	if(NewState==6) SetTimer(hTimerFrm,TIMER_STATECHANGED,500,(TIMERPROC)TimerFrmProc);
  }

  return 0;
}
//---------------------------------------------------------------------------

//Hook na zmiane kompozycji
INT_PTR __stdcall OnThemeChanged(WPARAM wParam, LPARAM lParam)
{
  //Okno ustawien zostalo juz stworzone
  if(hMainForm)
  {
	//Wlaczona zaawansowana stylizacja okien
	if(ChkSkinEnabled())
	{
	  //Pobieranie sciezki nowej aktywnej kompozycji
	  UnicodeString ThemeSkinDir = StringReplace((wchar_t*)lParam, "\\", "\\\\", TReplaceFlags() << rfReplaceAll) + "\\\\Skin";
	  //Plik zaawansowanej stylizacji okien istnieje
	  if(FileExists(ThemeSkinDir + "\\\\Skin.asz"))
	  {
		//Dane pliku zaawansowanej stylizacji okien
		ThemeSkinDir = StringReplace(ThemeSkinDir, "\\\\", "\\", TReplaceFlags() << rfReplaceAll);
		hMainForm->sSkinManager->SkinDirectory = ThemeSkinDir;
		hMainForm->sSkinManager->SkinName = "Skin.asz";
		//Ustawianie animacji AlphaControls
		if(ChkThemeAnimateWindows()) hMainForm->sSkinManager->AnimEffects->FormShow->Time = 200;
		else hMainForm->sSkinManager->AnimEffects->FormShow->Time = 0;
		hMainForm->sSkinManager->Effects->AllowGlowing = ChkThemeGlowing();
		//Zmiana kolorystyki AlphaControls
		hMainForm->sSkinManager->HueOffset = GetHUE();
		hMainForm->sSkinManager->Saturation = GetSaturation();
		hMainForm->sSkinManager->Brightness = GetBrightness();
		//Aktywacja skorkowania AlphaControls
		hMainForm->sSkinManager->Active = true;
	  }
	  //Brak pliku zaawansowanej stylizacji okien
	  else hMainForm->sSkinManager->Active = false;
	}
	//Zaawansowana stylizacja okien wylaczona
	else hMainForm->sSkinManager->Active = false;
  }

  return 0;
}
//---------------------------------------------------------------------------

//Notyfikacja otrzymywanych pakietow XML
INT_PTR __stdcall OnXMLDebug(WPARAM wParam, LPARAM lParam)
{
  //Wlaczone powiadomienie o aktualnie sluchanym utworze + zezwolenie na notyfikacje
  if((AllowUserTune)&&(UserTuneNotifChk))
  {
    //Pobranie pakietu XML
	UnicodeString XML = (wchar_t*)wParam;
	//Pakiet jest informacja User Tune (XEP-0118)
	if(((XML.Pos("<tune xmlns='http://jabber.org/protocol/tune'>"))||(XML.Pos("<tune xmlns=\"http://jabber.org/protocol/tune\">")))&&(XML.Pos("<title>")))
	{
	  //Pobieranie danych nt. pakietu XML
	  TPluginXMLChunk XMLChunk = *(PPluginXMLChunk)lParam;
	  //Indeks konta
	  UnicodeString XMLUserIdx = ":" + IntToStr(XMLChunk.UserIdx);
	  //Nadawca pakietu
	  UnicodeString XMLFrom = (wchar_t*)XMLChunk.From;
	  //Odbiorca pakietu
	  UnicodeString XMLTo = ReciveAccountJID(XMLChunk.UserIdx);
	  //Odbiorca rozny od nadawcy
	  if(XMLFrom!=XMLTo)
	  {
        //Nadawca pakietu nie znajduje sie na liscie wyjatkow
		if(UserTuneExceptions->IndexOf(XMLFrom)==-1)
		{
          //Wczytanie pakietu XML do parsera
		  _di_IXMLDocument XMLDoc = LoadXMLData(XML);
		  _di_IXMLNode Nodes = XMLDoc->DocumentElement;
		  //Sprawdzanie nazwy elementu
		  if(Nodes->NodeName=="message")
		  {
            //Pobranie pierwszego dziecka
			Nodes = Nodes->ChildNodes->GetNode(0);
			//Sprawdzanie nazwy elementu
			if((Nodes->NodeName=="event")&&(Nodes->Attributes["xmlns"]=="http://jabber.org/protocol/pubsub#event"))
			{
			  //Pobranie pierwszego dziecka
			  Nodes = Nodes->ChildNodes->GetNode(0);
			  //Sprawdzanie nazwy elementu
			  if((Nodes->NodeName=="items")&&(Nodes->Attributes["node"]=="http://jabber.org/protocol/tune"))
			  {
			    //Pobranie pierwszego dziecka
				Nodes = Nodes->ChildNodes->GetNode(0);
				//Sprawdzanie nazwy elementu
				if(Nodes->NodeName=="item")
				{
                  //Pobranie pierwszego dziecka
				  Nodes = Nodes->ChildNodes->GetNode(0);
				  //Sprawdzanie nazwy elementu
				  if((Nodes->NodeName=="tune")&&(Nodes->Attributes["xmlns"]=="http://jabber.org/protocol/tune"))
				  {
                    //Pobranie ilosci kolejnych dzieci
					int ItemsCount = Nodes->ChildNodes->GetCount();
					//Zmienne przechowujaca informacje o utworze
					UnicodeString Artist, Title;
					//Parsowanie kolejnych elementow
					for(int Count=0;Count<ItemsCount;Count++)
					{
					  //Pobieranie zawartosci dziecka
					  _di_IXMLNode ChildNodes = Nodes->ChildNodes->GetNode(Count);
					  //Sprawdzanie nazwy elementu
					  if(ChildNodes->NodeName=="artist") Artist = ChildNodes->Text;
					  else if(ChildNodes->NodeName=="title") Title = ChildNodes->Text;
					}
					//Skladanie tytulu utworu
					UnicodeString UserTuneSong;
					if(Artist.IsEmpty()) UserTuneSong = Title;
					else UserTuneSong = Artist + " - " + Title;
					//Poprawa kodowania UTF8
					UserTuneSong = UTF8ToUnicodeString(UserTuneSong.w_str());
					UserTuneSong = StringReplace(UserTuneSong, "&apos;", "'", TReplaceFlags() << rfReplaceAll);
					UserTuneSong = StringReplace(UserTuneSong, "&quot;", "\"", TReplaceFlags() << rfReplaceAll);
					UserTuneSong = StringReplace(UserTuneSong, "&amp;", "&", TReplaceFlags() << rfReplaceAll);
					UserTuneSong = StringReplace(UserTuneSong, "&gt;", ">", TReplaceFlags() << rfReplaceAll);
					UserTuneSong = StringReplace(UserTuneSong, "&lt;", "<", TReplaceFlags() << rfReplaceAll);
					//Usuwanie "*** "
					if(UserTuneSong.Pos("*** ")>0)
					{
					  UserTuneSong.Delete(1, UserTuneSong.Pos("*** ") + 3);
					  UserTuneSong = UserTuneSong.Trim();
					}
					//Usuwanie numeru piosenki
					if((UserTuneSong.Pos(". ")>0)&&(UserTuneSong.Pos(". ")<7))
					{
					  UnicodeString TMP = UserTuneSong;
					  TMP.Delete(UserTuneSong.Pos(". "), TMP.Length());
					  if(TestDigit(TMP))
					  {
						UserTuneSong.Delete(1, UserTuneSong.Pos(". "));
						UserTuneSong = UserTuneSong.Trim();
					  }
					}
					//Usuwanie "Wtyczka AQQ Radio: "
					if(UserTuneSong.Pos("Wtyczka AQQ Radio: ")>0)
					 UserTuneSong.Delete(1,UserTuneSong.Pos("Wtyczka AQQ Radio: ")+18);
					//Chmurka "kto slucha"
					TPluginShowInfo PluginShowInfo;
					PluginShowInfo.cbSize = sizeof(TPluginShowInfo);
					PluginShowInfo.Event = tmePseudoMsgCap;
					PluginShowInfo.Text = GetContactNick(XMLFrom+XMLUserIdx).w_str();
					PluginShowInfo.ImagePath = (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETPNG_FILEPATH,76,0);
					PluginShowInfo.TimeOut = 1000 * UserTuneCloudChk;
					PluginShowInfo.ActionID = L"";
					PluginShowInfo.Tick = 0;
					PluginLink.CallService(AQQ_FUNCTION_SHOWINFO,0,(LPARAM)&PluginShowInfo);
					//Chmurka "co slucha"
					PluginShowInfo.cbSize = sizeof(TPluginShowInfo);
					PluginShowInfo.Event = tmeInfo;
					PluginShowInfo.Text = UserTuneSong.w_str();
					PluginShowInfo.ImagePath = L"";
					PluginShowInfo.TimeOut = 1000 * UserTuneCloudChk;
					PluginShowInfo.ActionID = L"";
					PluginShowInfo.Tick = 0;
					PluginLink.CallService(AQQ_FUNCTION_SHOWINFO,0,(LPARAM)&PluginShowInfo);
				  }
                }
              }
            }
		  }
		}
	  }
    }
  }

  return 0;
}
//---------------------------------------------------------------------------

//Zapisywanie zasobów
void ExtractRes(wchar_t* FileName, wchar_t* ResName, wchar_t* ResType)
{
  TPluginTwoFlagParams PluginTwoFlagParams;
  PluginTwoFlagParams.cbSize = sizeof(TPluginTwoFlagParams);
  PluginTwoFlagParams.Param1 = ResName;
  PluginTwoFlagParams.Param2 = ResType;
  PluginTwoFlagParams.Flag1 = (int)HInstance;
  PluginLink.CallService(AQQ_FUNCTION_SAVERESOURCE,(WPARAM)&PluginTwoFlagParams,(LPARAM)FileName);
}
//---------------------------------------------------------------------------

//Obliczanie sumy kontrolnej pliku
UnicodeString MD5File(UnicodeString FileName)
{
  if(FileExists(FileName))
  {
	UnicodeString Result;
    TFileStream *fs;

	fs = new TFileStream(FileName, fmOpenRead | fmShareDenyWrite);
	try
	{
	  TIdHashMessageDigest5 *idmd5= new TIdHashMessageDigest5();
	  try
	  {
	    Result = idmd5->HashStreamAsHex(fs);
	  }
	  __finally
	  {
	    delete idmd5;
	  }
    }
	__finally
    {
	  delete fs;
    }

    return Result;
  }
  else
   return 0;
}
//---------------------------------------------------------------------------

//Odczyt ustawien
void LoadSettings()
{
  //Wczytanie pliku INI
  TIniFile *Ini = new TIniFile(GetPluginUserDir() + "\\\\TuneStatus\\\\TuneStatus.ini");
  //Obslugiwane odtwarzacze
  SupportedPlayers->Clear();
  for(int Count=0;Count<15;Count++)
  {
	//Pobieranie elementow
	UnicodeString PlayerID = Ini->ReadString("AutoMode",("Player"+IntToStr(Count+1)),"");
	if(!PlayerID.IsEmpty())
	{
	  //Pobieranie statusu wlaczenia elementu
	  UnicodeString Enabled = PlayerID;
	  Enabled = Enabled.Delete(1,Enabled.Pos(";"));
	  //Sprawdzanie czy element jest wlaczony
	  if(StrToBool(Enabled))
	  {
		//Odkodowywanie ID odtwarzacza
		PlayerID = PlayerID.Delete(PlayerID.Pos(";"),PlayerID.Length());
		if(StrToInt(PlayerID)==1) PlayerID = "Winamp/AIMP/KMPlayer";
		else if(StrToInt(PlayerID)==2) PlayerID = "Foobar2000";
		else if(StrToInt(PlayerID)==3) PlayerID = "Windows Media Player";
		else if(StrToInt(PlayerID)==4) PlayerID = "VUPlayer";
		else if(StrToInt(PlayerID)==5) PlayerID = "XMPlay";
		else if(StrToInt(PlayerID)==6) PlayerID = "Media Player Classic";
		else if(StrToInt(PlayerID)==7) PlayerID = "iTunes";
		else if(StrToInt(PlayerID)==8) PlayerID = "ALSong";
		else if(StrToInt(PlayerID)==9) PlayerID = "Wtyczki (np. AQQ Radio)";
		else if(StrToInt(PlayerID)==10) PlayerID = "Screamer Radio";
		else if(StrToInt(PlayerID)==11) PlayerID = "aTunes";
		else if(StrToInt(PlayerID)==12) PlayerID = "";
		else if(StrToInt(PlayerID)==13) PlayerID = "Last.fm Player";
		else if(StrToInt(PlayerID)==14) PlayerID = "VLC media player";
		else if(StrToInt(PlayerID)==15) PlayerID = "Spotify";
		//Dodawnie odtwarzacza do listy
		if(!PlayerID.IsEmpty()) SupportedPlayers->Add(PlayerID);
	  }
	}
  }
  //Wyglad opisu
  StatusLook = DecodeBase64(Ini->ReadString("Settings", "Status64", "T2JlY25pZSBzxYJ1Y2hhbTogQ0NfVFVORVNUQVRVUw=="));
  //Opoznienie ustawiania nowego opisu
  SetStatusDelayChk = 1000*Ini->ReadInteger("Settings", "SetStatusDelay", 5);
  if(SetStatusDelayChk<4000) SetStatusDelayChk = 4000;
  //Wlaczanie dzialnia wtyczki wraz z uruchomieniem
  EnableOnStartChk = Ini->ReadBool("Settings", "EnableOnStart", false);
  //Pokazywanie przycisku szybkiego wlaczania/wylaczania dzialnia wtyczki
  FastAccessChk = Ini->ReadBool("Settings", "FastAccess", true);
  //Automatyczne wylaczanie dzialania wtyczki przy bezczynnosci
  AutoTurnOffChk = Ini->ReadBool("Settings", "AutoTurnOff", false);
  //Czas automatycznego wylaczania dzialania wtyczki przy bezczynnosci
  AutoTurnOffDelayChk = 60000*Ini->ReadInteger("Settings", "AutoTurnOffDelay", 15);
  //User Tune - informowanie o aktualnym odtwarzanym przez nas utworze
  UserTuneSendChk = Ini->ReadBool("UserTune", "Send", false);
  //Wylaczanie obslugi User Tune w rdzeniu AQQ
  if(UserTuneSendChk) TurnOffCoreUserTune();
  //User Tune - opoznienie wysylania nowego odtwarzanego utworu
  UserTuneSendDelayChk = 1000*Ini->ReadInteger("UserTune", "SendDelay", 5);
  if(UserTuneSendDelayChk<4000) UserTuneSendDelayChk = 4000;
  //User Tune - powiadomienie o aktualnych sluchanych utworach przez inne kontakty
  UserTuneNotifChk = Ini->ReadBool("UserTune", "Notification", false);
  //User Tune - wyjatki wylaczone z powiadomienia
  UserTuneExceptions->Clear();
  TStringList *JIDList = new TStringList;
  Ini->ReadSection("UserTuneEx",JIDList);
  int JIDListCount = JIDList->Count;
  delete JIDList;
  if(JIDListCount)
  {
	for(int Count=0;Count<JIDListCount;Count++)
	{
	  UnicodeString JID = Ini->ReadString("UserTuneEx",("JID"+IntToStr(Count+1)), "");
	  if(!JID.IsEmpty()) UserTuneExceptions->Add(JID);
	}
  }
  //User Tune - czas wyswietlania chmurki informacyjnej
  UserTuneCloudChk = Ini->ReadInteger("UserTune", "Cloud", 6);
  //Wszystkie moduly zostaly zaladowane
  if(PluginLink.CallService(AQQ_SYSTEM_MODULESLOADED,0,0))
  {
	//Wlaczono notyfikacje User Tune
	if(UserTuneNotifChk)
	{
	  //Pobranie ID dla enumeracji kontaktów
	  ReplyListID = GetTickCount();
	  //Wywolanie enumeracji kontaktow
	  PluginLink.CallService(AQQ_CONTACTS_REQUESTLIST,(WPARAM)ReplyListID,0);
	}
	else if(!LoadExecuted) ContactsNickList->Clear();
  }
  //Zwolnienie pliku INI
  delete Ini;
}
//---------------------------------------------------------------------------

extern "C" int __declspec(dllexport) __stdcall Load(PPluginLink Link)
{
  //Info o rozpoczeciu procedury ladowania
  LoadExecuted = true;
  //Linkowanie wtyczki z komunikatorem
  PluginLink = *Link;
  //Sciezka katalogu prywatnego wtyczek
  UnicodeString PluginUserDir = GetPluginUserDir();
  //Tworzenie folderu wtyczki
  if(!DirectoryExists(PluginUserDir + "\\\\TuneStatus"))
   CreateDir(PluginUserDir + "\\\\TuneStatus");
  //Wypakiwanie ikonki TuneStatus.png
  //13C4EE8D3C0DC40E5C91BA6006889E05
  if(!FileExists(PluginUserDir + "\\\\TuneStatus\\\\TuneStatus.png"))
   ExtractRes((PluginUserDir + "\\\\TuneStatus\\\\TuneStatus.png").w_str(),L"FASTACCESS",L"DATA");
  else if(MD5File(PluginUserDir + "\\\\TuneStatus\\\\TuneStatus.png")!="13C4EE8D3C0DC40E5C91BA6006889E05")
   ExtractRes((PluginUserDir + "\\\\TuneStatus\\\\TuneStatus.png").w_str(),L"FASTACCESS",L"DATA");
   //Wypakiwanie ikonki TuneStatusOn.png
  //13C4EE8D3C0DC40E5C91BA6006889E05
  if(!FileExists(PluginUserDir + "\\\\TuneStatus\\\\TuneStatusOn.png"))
   ExtractRes((PluginUserDir + "\\\\TuneStatus\\\\TuneStatusOn.png").w_str(),L"FASTACCESS_ON",L"DATA");
  else if(MD5File(PluginUserDir + "\\\\TuneStatus\\\\TuneStatusOn.png")!="13C4EE8D3C0DC40E5C91BA6006889E05")
   ExtractRes((PluginUserDir + "\\\\TuneStatus\\\\TuneStatusOn.png").w_str(),L"FASTACCESS_ON",L"DATA");
   //Wypakiwanie ikonki TuneStatusOff.png
  //53F17EBD9F552D104FEF9DA1DC2CFAFB
  if(!FileExists(PluginUserDir + "\\\\TuneStatus\\\\TuneStatusOff.png"))
   ExtractRes((PluginUserDir + "\\\\TuneStatus\\\\TuneStatusOff.png").w_str(),L"FASTACCESS_OFF",L"DATA");
  else if(MD5File(PluginUserDir + "\\\\TuneStatus\\\\TuneStatusOff.png")!="53F17EBD9F552D104FEF9DA1DC2CFAFB")
   ExtractRes((PluginUserDir + "\\\\TuneStatus\\\\TuneStatusOff.png").w_str(),L"FASTACCESS_OFF",L"DATA");
   //Wypakiwanie ikonki TuneStatus.dll.png
  //0757AC4903B8DE698B1A39E5F61AA796
  if(!DirectoryExists(PluginUserDir + "\\\\Shared"))
   CreateDir(PluginUserDir + "\\\\Shared");
  if(!FileExists(PluginUserDir + "\\\\Shared\\\\TuneStatus.dll.png"))
   ExtractRes((PluginUserDir + "\\\\Shared\\\\TuneStatus.dll.png").w_str(),L"SHARED",L"DATA");
  else if(MD5File(PluginUserDir + "\\\\Shared\\\\TuneStatus.dll.png")!="0757AC4903B8DE698B1A39E5F61AA796")
   ExtractRes((PluginUserDir + "\\\\Shared\\\\TuneStatus.dll.png").w_str(),L"SHARED",L"DATA");
  //Przypisanie ikonek do interfejsu AQQ
  FASTACCESS = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(PluginUserDir + "\\\\TuneStatus\\\\TuneStatus.png").w_str());
  FASTACCESS_ON = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(PluginUserDir +"\\\\TuneStatus\\\\TuneStatusOn.png").w_str());
  FASTACCESS_OFF = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(PluginUserDir +"\\\\TuneStatus\\\\TuneStatusOff.png").w_str());
  //Tworzenie serwisu dla szybkiego dostep do ustawien wtyczki
  PluginLink.CreateServiceFunction(L"sTuneStatusFastSettingsItem",ServiceTuneStatusFastSettingsItem);
  //Tworzenie serwisu dla szybkiego dostep do ustawien wtyczki
  PluginLink.CreateServiceFunction(L"sTuneStatusFastOperationItem",ServiceTuneStatusFastOperationItem);
  //Hook na zmianê stanu kontaktu
  PluginLink.HookEvent(AQQ_CONTACTS_UPDATE,OnContactsUpdate);
  //Hook na przekazywanie utworu przez wtyczki np. AQQ Radio
  PluginLink.HookEvent(AQQ_SYSTEM_CURRENTSONG,OnCurrentSong);
  //Hook na zakonczenie ladowania listy kontaktow przy starcie AQQ
  PluginLink.HookEvent(AQQ_CONTACTS_LISTREADY,OnListReady);
  //Hook na zaladowanie wszystkich modolow
  PluginLink.HookEvent(AQQ_SYSTEM_MODULESLOADED,OnModulesLoaded);
  AllowUserTune = PluginLink.CallService(AQQ_SYSTEM_MODULESLOADED,0,0);
  //Hook na reczna zmiane opisu
  PluginLink.HookEvent(AQQ_WINDOW_PRESETNOTE_NOTE,OnPreSetNote);
  //Hook na enumeracje listy kontatkow
  PluginLink.HookEvent(AQQ_CONTACTS_REPLYLIST,OnReplyList);
  //Hook na zmiane stanu sieci
  PluginLink.HookEvent(AQQ_SYSTEM_STATECHANGE,OnStateChange);
  //Hook na zmiane kompozycji
  PluginLink.HookEvent(AQQ_SYSTEM_THEMECHANGED,OnThemeChanged);
  //Hook na przychodzace pakiety XML
  PluginLink.HookEvent(AQQ_SYSTEM_XMLDEBUG,OnXMLDebug);
  //Odczyt ustawien
  LoadSettings();
  //Tworzenie interfejsu szybkiego dostepu do ustawien wtyczki
  BuildTuneStatusFastSettings();
  //Tworzenie interfejsu szybkiego wlaczania/wylaczania dzialania wtyczki
  if(FastAccessChk) BuildTuneStatusFastOperation();
  //Rejestowanie klasy okna timera
  WNDCLASSEX wincl;
  wincl.cbSize = sizeof (WNDCLASSEX);
  wincl.style = 0;
  wincl.lpfnWndProc = TimerFrmProc;
  wincl.cbClsExtra = 0;
  wincl.cbWndExtra = 0;
  wincl.hInstance = HInstance;
  wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
  wincl.hbrBackground = (HBRUSH)COLOR_BACKGROUND;
  wincl.lpszMenuName = NULL;
  wincl.lpszClassName = L"TTuneStatusTimer";
  wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
  RegisterClassEx(&wincl);
  //Tworzenie okna timera
  hTimerFrm = CreateWindowEx(0, L"TTuneStatusTimer", L"",	0, 0, 0, 0, 0, NULL, NULL, HInstance, NULL);
  //Wlaczenie wysylania informacji User Tune
  if(UserTuneSendChk)
  {
	UserTuneEnabled = true;
	SetTimer(hTimerFrm,TIMER_USERTUNE,1000,(TIMERPROC)TimerFrmProc);
  }
  //Wlaczenie dzialania wtyczki przy starcie
  if(EnableOnStartChk)
   SetTimer(hTimerFrm,TIMER_GETSTATUS,100,(TIMERPROC)TimerFrmProc);
  //Info o zakonczeniu procedury ladowania
  LoadExecuted = false;

  return 0;
}
//---------------------------------------------------------------------------

//Otwieranie formy ustawien
extern "C" int __declspec(dllexport)__stdcall Settings()
{
  //Przypisanie uchwytu do formy
  if(!hMainForm)
  {
	Application->Handle = (HWND)MainForm;
	hMainForm = new TMainForm(Application);
  }
  //Zmiana statusu checkbox'a na formie
  hMainForm->RunPluginCheckBox->Checked = AutoModeEnabled;
  //Pokaznie okna
  hMainForm->Show();

  return 0;
}
//---------------------------------------------------------------------------

extern "C" int __declspec(dllexport) __stdcall Unload()
{
  //Wyladowanie timerow
  for(int TimerID=10;TimerID<=80;TimerID=TimerID+10) KillTimer(hTimerFrm,TimerID);
  //Usuwanie okna timera
  DestroyWindow(hTimerFrm);
  //Wyrejestowanie klasy okna timera
  UnregisterClass(L"TTuneStatusTimer",HInstance);
  //Przywracanie poczatkowego opisu jezeli dzialanie wtyczki jest wlaczone
  if(AutoModeEnabled)
  {
	//Pobranie aktualnego opisu
	TempStatus = GetStatus();
	//Jezeli opis startowy jest rozny od aktualnego
	if(StartStatus!=TempStatus)
	{
      //Przywracanie poczatkowego opisu
	  while(StartStatus!=GetStatus())
	   SetStatus(StartStatus);
	}
  }
  //Wylacznie User Tune
  if(UserTuneSendChk) SetUserTune("","");
  //Usuwanie elementu szybkiego dostepu do ustawien wtyczki
  DestroyTuneStatusFastSettings();
  //Usuwanie serwisu dla szybkiego dostep do ustawien wtyczki
  PluginLink.DestroyServiceFunction(ServiceTuneStatusFastSettingsItem);
  //Usuwanie elementu szybkiego wlaczania/wylaczania dzialania wtyczki
  DestroyTuneStatusFastOperation();
  //Usuwanie serwisu dla szybkiego wlaczania/wylaczania dzialania wtyczki
  PluginLink.DestroyServiceFunction(ServiceTuneStatusFastOperationItem);
  //Wyladowanie ikonek z intefejsu
  PluginLink.CallService(AQQ_ICONS_DESTROYPNGICON,0,(LPARAM)(FASTACCESS));
  PluginLink.CallService(AQQ_ICONS_DESTROYPNGICON,0,(LPARAM)(FASTACCESS_OFF));
  PluginLink.CallService(AQQ_ICONS_DESTROYPNGICON,0,(LPARAM)(FASTACCESS_ON));
  //Wyladowanie hookow
  PluginLink.UnhookEvent(OnContactsUpdate);
  PluginLink.UnhookEvent(OnCurrentSong);
  PluginLink.UnhookEvent(OnListReady);
  PluginLink.UnhookEvent(OnModulesLoaded);
  PluginLink.UnhookEvent(OnPreSetNote);
  PluginLink.UnhookEvent(OnReplyList);
  PluginLink.UnhookEvent(OnStateChange);
  PluginLink.UnhookEvent(OnThemeChanged);
  PluginLink.UnhookEvent(OnXMLDebug);

  return 0;
}
//---------------------------------------------------------------------------

//Informacje o wtyczce
extern "C" __declspec(dllexport) PPluginInfo __stdcall AQQPluginInfo(DWORD AQQVersion)
{
  PluginInfo.cbSize = sizeof(TPluginInfo);
  PluginInfo.ShortName = L"TuneStatus";
  PluginInfo.Version = PLUGIN_MAKE_VERSION(2,5,2,0);
  PluginInfo.Description = L"Wtyczka s³u¿y do informowania naszych znajomych o tym co aktualnie s³uchamy w odtwarzaczu plików audio. Informowanie odbywa siê poprzez zmianê naszego opisu oraz opcjonalnie poprzez wysy³anie notyfikacji User Tune (XEP-0118) w sieci Jabber.";
  PluginInfo.Author = L"Krzysztof Grochocki";
  PluginInfo.AuthorMail = L"kontakt@beherit.pl";
  PluginInfo.Copyright = L"Krzysztof Grochocki";
  PluginInfo.Homepage = L"http://beherit.pl";
  PluginInfo.Flag = 0;
  PluginInfo.ReplaceDefaultModule = 0;

  return &PluginInfo;
}
//---------------------------------------------------------------------------
