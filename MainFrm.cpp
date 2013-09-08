//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#include <inifiles.hpp>
#include <tlhelp32.h>
#include <Registry.hpp>
#pragma hdrstop
#include "MainFrm.h"
#include "UserTuneExceptionFrm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CSPIN"
#pragma link "cspin"
#pragma link "cspin"
#pragma link "cspin"
#pragma link "cspin"
#pragma link "sButton"
#pragma link "sPageControl"
#pragma link "sSkinManager"
#pragma link "sSkinProvider"
#pragma link "sCheckListBox"
#pragma link "sLabel"
#pragma link "sListBox"
#pragma link "sBevel"
#pragma link "sComboBox"
#pragma link "sMemo"
#pragma link "sEdit"
#pragma link "sSpinEdit"
#pragma link "sCheckBox"
#pragma resource "*.dfm"
TMainForm *MainForm;
//---------------------------------------------------------------------------
__declspec(dllimport)UnicodeString GetThemeSkinDir();
__declspec(dllimport)bool ChkSkinEnabled();
__declspec(dllimport)bool ChkNativeEnabled();
__declspec(dllimport)void SetStatus(UnicodeString SetStatusStatus, bool SetStatusForce);
__declspec(dllimport)UnicodeString GetPluginUserDir();
__declspec(dllimport)UnicodeString GetPluginDir();
__declspec(dllimport)UnicodeString GetStatus();
__declspec(dllimport)UnicodeString GetPluginSong();
__declspec(dllimport)void BuildFastAccess();
__declspec(dllimport)void DestroyFastAccess();
__declspec(dllimport)void UpdateFastAccess(bool Enabled);
__declspec(dllimport)bool AllowChangeStatus(bool WithInvisible);
__declspec(dllimport)void ChangeUserTune(bool Enabled, int Cloud);
__declspec(dllimport)void SetUserTune(UnicodeString Tune, UnicodeString Time);
__declspec(dllimport)void SetAllowUserTune();
__declspec(dllimport)void TurnOffCoreUserTune();
//---------------------------------------------------------------------------
int GetStatusTimerInterval; //W³aczenie wtyczki wraz z AQQ - pobieranie opisu
UnicodeString PlayerCaption; //Pobrany tekst okna
UnicodeString UserTunePlayerCaption; //Pobrany tekst okna (User Tune)
UnicodeString UserTuneStatus; //Zmienna opisu do UserTune
UnicodeString UserTuneStatusTMP; //Zmienna tymczasowa opisu do UserTune (do porownyawania zmian z w.w. zmienna)
bool IgnoreCoreUserTuneChk; //czy WYSY£ANIE UserTune jest w³aczone
bool JustEnabled; //do pierwszego uruchomienia SetStatusTimer
UnicodeString iTunesStatusPath; //do sciezki iTunes (TuneStatus.txt)
UnicodeString iTunesPlayerPath; //do sciezka iTunes
UnicodeString PlayerPath; //do sciezka Lastfm/Songbird
bool FastAccessChk; //do pokazywania/ukrywania szybkiego przycisku
bool IgnorePluginsChk; //do wy³¹czania SetStatusTimer'a dla AQQ Radio
bool DoIgnorePluginsChk; //j.w.
bool CutRadiostationNameChk; //do ucinania nazwy radiostacji z AQQ Radio
bool CutWWWChk; //do wycinania stron WWW z pobranego utworu
bool AutoTurnOffChk; //do wylaczania dziala wtyczki gdy utwor nie zmienia sie od X minut
bool MovieExceptionCk; //do wykluczenia filmow z pobierania
UnicodeString CC_SONGLENGTH; //dlugosc piosenki
//---------------------------------------------------------------------------
HWND LastfmWindowHwnd; //uchwyt do okna Last.fm Player
HWND SongbirdWindowHwnd; //uchwyt do okna Songbird
HWND ScreamerRadioWindowHwnd; //uchwyt do okna Screamer Radio
HWND aTunesWindowHwnd; //uchwyt do okna iTunes
//---------------------------------------------------------------------------

__fastcall TMainForm::TMainForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

//Pobieranie informacji o pliku (wersja itp)
String GetFileVersionInfo(char *ModulePath, String KeyName)
{
  LPVOID lpStr1 = NULL, lpStr2 = NULL;
  WORD* wTmp;
  DWORD dwHandlev = NULL;
  UINT dwLength;
  char sFileName[1024] = {0};
  char sTmp[1024] = {0};
  String sInfo;
  LPVOID* pVersionInfo;

  if(ModulePath == NULL) GetModuleFileName(NULL, sFileName, 1024);
  else strcpy(sFileName, ModulePath);

  DWORD dwInfoSize = GetFileVersionInfoSize((char*)(LPCTSTR)sFileName, &dwHandlev);
  if(dwInfoSize)
  {
	pVersionInfo = new LPVOID[dwInfoSize];
	if(GetFileVersionInfo((char*)(LPCTSTR)sFileName, dwHandlev, dwInfoSize, pVersionInfo))
	{
	  if(VerQueryValue(pVersionInfo, "\\VarFileInfo\\Translation", &lpStr1, &dwLength))
	  {
		wTmp = (WORD*)lpStr1;
		sprintf(sTmp, ("\\StringFileInfo\\%04x%04x\\" + KeyName).t_str(), *wTmp, *(wTmp + 1));
		if(VerQueryValue(pVersionInfo, sTmp, &lpStr2, &dwLength)) sInfo = (LPSTR)lpStr2;
	  }
	}
	delete[] pVersionInfo;
  }

  return sInfo;
}
//---------------------------------------------------------------------------

//Zapis zawartosci TMemo do pliku INI
UnicodeString StrToIniStr(UnicodeString Str)
{
  char Buffer[512];
  char* B;
  char* S;

  S = Str.t_str();
  B = Buffer;
  while(*S != '\0')
  {
	switch (*S)
	{
	case 13:
	case 10:
	  if((*S == 13) && (S[1] == 10)) S++;
	  else if((*S == 10) && (S[1] == 13)) S++;
	  *B = '\\';
	  B++;
	  *B = 'n';
	  B++;
	  S++;
	  break;
	default:
	  *B = *S;
	  B++;
	  S++;
	}
  }
  *B = '\0';

  return String(Buffer);
}
//---------------------------------------------------------------------------

//Odczyt zawartosci pliku INI do TMemo
String IniStrToStr(UnicodeString Str)
{
  char Buffer[512];
  char* B;
  char* S;

  S = Str.t_str();
  B = Buffer;
  while(*S != '\0')
  {
	if((S[0] == '\\') && (S[1] == 'n'))
	{
	  *B = 13;
	  B++;
	  *B = 10;
	  B++;
	  S++;
	  S++;
	}
	else
	{
	  *B = *S;
	  B++;
	  S++;
	}
  }
  *B = '\0';

  return String(Buffer);
}
//---------------------------------------------------------------------------

//Pobieranie sciezki procesu po PID
UnicodeString GetPathOfProces(DWORD PID)
{
  HANDLE hSnapshot=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,PID);
  MODULEENTRY32 me32;
  Module32First(hSnapshot,&me32);
  CloseHandle(hSnapshot);
  return me32.szExePath;
}
//---------------------------------------------------------------------------

//Szukanie okna Last.fm
bool CALLBACK FindLastfm(HWND hWnd, LPARAM lParam)
{
  wchar_t WindowCaptionName[255];
  GetClassNameW(hWnd, WindowCaptionName, sizeof(WindowCaptionName));

  if((UnicodeString)WindowCaptionName=="QWidget")
  {
	DWORD procesID;
	GetWindowThreadProcessId(hWnd, &procesID);
	PlayerPath = GetPathOfProces(procesID);

	if(ExtractFileName(PlayerPath)=="LastFM.exe")
	{
	  GetWindowTextW(hWnd, WindowCaptionName, sizeof(WindowCaptionName));

	  if(
		 ((UnicodeString)WindowCaptionName!="LastFM")
		 &&((UnicodeString)WindowCaptionName!="Diagnostyka")
		 &&((UnicodeString)WindowCaptionName!="Diagnostics")
		 &&((UnicodeString)WindowCaptionName!="Poleæ")
		 &&((UnicodeString)WindowCaptionName!="Share")
		 &&((UnicodeString)WindowCaptionName!="Zaloguj")
		 &&((UnicodeString)WindowCaptionName!="Kreator automatycznej aktualizacji")
		 &&((UnicodeString)WindowCaptionName!="Kreator ustawieñ")
		 &&((UnicodeString)WindowCaptionName!="Opcje programu Last.fm")
		 &&((UnicodeString)WindowCaptionName!="Dodaj u¿ytkownika")
		 &&((UnicodeString)WindowCaptionName!="Aktualne")
		 &&((UnicodeString)WindowCaptionName!="Last.fm — Informacje")
		 &&((UnicodeString)WindowCaptionName!="Form")
		 &&((UnicodeString)WindowCaptionName!="volume")
		 &&((UnicodeString)WindowCaptionName!="recommendTypeBox")
		 &&((UnicodeString)WindowCaptionName!="label")
		 &&((UnicodeString)WindowCaptionName!="label_2")
		 &&((UnicodeString)WindowCaptionName!="label_3")
		 &&((UnicodeString)WindowCaptionName!="messageEdit")
		 &&((UnicodeString)WindowCaptionName!="userEdit")
		 &&((UnicodeString)WindowCaptionName!="buttonBox")
		 &&((UnicodeString)WindowCaptionName!="qt_scrollarea_hcontainer")
		 &&((UnicodeString)WindowCaptionName!="qt_scrollarea_vcontainer")
		 &&((UnicodeString)WindowCaptionName!="qt_scrollarea_viewport")
		 &&((UnicodeString)WindowCaptionName!="")
		)
	  {
		LastfmWindowHwnd = hWnd;
		return false;
	  }
	}
  }
  return true;
}
//---------------------------------------------------------------------------

//Szukanie okna Songbird
bool CALLBACK FindSongbird(HWND hWnd, LPARAM lParam)
{
  wchar_t WindowCaptionName[255];
  GetClassNameW(hWnd, WindowCaptionName, sizeof(WindowCaptionName));

  if((UnicodeString)WindowCaptionName=="MozillaUIWindowClass")
  {
	DWORD procesID;
	GetWindowThreadProcessId(hWnd, &procesID);
	PlayerPath = GetPathOfProces(procesID);

	if(ExtractFileName(PlayerPath)=="songbird.exe")
	{
	  GetWindowTextW(hWnd, WindowCaptionName, sizeof(WindowCaptionName));

	  if(((UnicodeString)WindowCaptionName!="Birdtitle notifer")
		 &&(!((UnicodeString)WindowCaptionName).IsEmpty()))
	  {
		SongbirdWindowHwnd=hWnd;
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
  wchar_t WindowCaptionName[255];
  GetClassNameW(hWnd, WindowCaptionName, sizeof(WindowCaptionName));

  if(AnsiPos("32770", (UnicodeString)WindowCaptionName))
  {
	DWORD procesID;
	GetWindowThreadProcessId(hWnd, &procesID);
	PlayerPath = GetPathOfProces(procesID);

	if(ExtractFileName(PlayerPath)=="screamer.exe")
	{
	  GetWindowTextW(hWnd, WindowCaptionName, sizeof(WindowCaptionName));

	  if(((UnicodeString)WindowCaptionName!="Screamer Log")
		 &&(!((UnicodeString)WindowCaptionName).IsEmpty()))
	  {
		ScreamerRadioWindowHwnd=hWnd;
		return false;
	  }
	}
  }
  return true;
}
//---------------------------------------------------------------------------

//Szukanie okna ScreamerRadio
bool CALLBACK FindaTunes(HWND hWnd, LPARAM lParam)
{
  wchar_t WindowCaptionName[255];
  GetClassNameW(hWnd, WindowCaptionName, sizeof(WindowCaptionName));

  if((UnicodeString)WindowCaptionName=="SunAwtFrame")
  {
	DWORD procesID;
	GetWindowThreadProcessId(hWnd, &procesID);

	PlayerPath = GetPathOfProces(procesID);

	if(ExtractFileName(PlayerPath)=="aTunes.exe")
	{
	  aTunesWindowHwnd=hWnd;
	  return false;
	}
  }
  return true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aWinampDownExecute(TObject *Sender)
{
  HWND PlayerHwnd = FindWindow("Winamp v1.x",NULL);
  if(!PlayerHwnd)
   PlayerHwnd = FindWindow("Studio",NULL);

  if(PlayerHwnd)
  {
	//Winamp status !STOP
	if(SendMessage(PlayerHwnd,WM_USER,0,104))
	{
	  wchar_t PlayerTitle[255];
	  GetWindowTextW(PlayerHwnd,PlayerTitle,sizeof(PlayerTitle));
	  PlayerCaption = PlayerTitle;

	  //Usuwanie "Winamp"
	  if(PlayerCaption.Pos("Winamp")==1)
	   PlayerCaption = "";

	  //Usuwanie "- Winamp"
	  if(PlayerCaption.Pos("- Winamp"))
	  {
		PlayerCaption.Delete(PlayerCaption.Pos("- Winamp"), PlayerCaption.Length());
		PlayerCaption=PlayerCaption.Trim();
	  }

	  //Usuwanie "*** "
	  if(PlayerCaption.Pos("*** "))
	  {
		PlayerCaption.Delete(1, PlayerCaption.Pos("*** ") + 3);
		PlayerCaption=PlayerCaption.Trim();
	  }

	  //SongLength
	  int Result = SendMessage(PlayerHwnd,WM_USER,1,105);
	  if(Result!=-1)
	  {
		int Seconds = Result % 60;
		Result = Result - Seconds;
		int Minutes = Result / 60;
		//Formatowanie sekund
		if(Seconds<10)
		 CC_SONGLENGTH = IntToStr(Minutes) + ":0" + IntToStr(Seconds);
		else
		 CC_SONGLENGTH = IntToStr(Minutes) + ":" + IntToStr(Seconds);
	  }
	  else
	   CC_SONGLENGTH = "";
	}
	else
	 PlayerCaption = "";
  }
  else
   PlayerCaption = "";

  if(!IgnoreCoreUserTuneChk)
   UserTunePlayerCaption = "";
  else
   UserTunePlayerCaption = PlayerCaption;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aFoobarDownExecute(TObject *Sender)
{
  HWND PlayerHwnd = FindWindow("Foobar_TuneStatus",NULL);
  if(!PlayerHwnd) PlayerHwnd = FindWindow("PanelsUI",NULL);
  if(!PlayerHwnd) PlayerHwnd = FindWindow("{DA7CD0DE-1602-45e6-89A1-C2CA151E008E}/1",NULL);
  if(!PlayerHwnd) PlayerHwnd = FindWindow("{DA7CD0DE-1602-45e6-89A1-C2CA151E008E}",NULL);
  if(!PlayerHwnd) PlayerHwnd = FindWindow("{DA7CD0DE-1602-45e6-89A1-C2CA151E008E}",NULL);
  if(!PlayerHwnd) PlayerHwnd = FindWindow("{97E27FAA-C0B3-4b8e-A693-ED7881E99FC1}",NULL);
  if(!PlayerHwnd) PlayerHwnd = FindWindow("{E7076D1C-A7BF-4f39-B771-BCBE88F2A2A8}",NULL);
                                                           
  if(PlayerHwnd)
  {
	wchar_t PlayerTitle[255];
	GetWindowTextW(PlayerHwnd,PlayerTitle,sizeof(PlayerTitle));
	PlayerCaption = PlayerTitle;

	//Usuwanie "[foobar2000"
	if(PlayerCaption.Pos("[foobar2000"))
	{
	  PlayerCaption.Delete(PlayerCaption.Pos("[foobar2000"), PlayerCaption.Length());
	  PlayerCaption=PlayerCaption.Trim();
    }

	//Usuwanie "foobar2000"
	if(PlayerCaption.Pos("foobar2000"))
    {
	  PlayerCaption.Delete(PlayerCaption.Pos("foobar2000"), PlayerCaption.Length());
	  PlayerCaption=PlayerCaption.Trim();
    }

    //Usuwanie "fooAvA"
	if(PlayerCaption.Pos("fooAvA"))
	 PlayerCaption = "";

    //"Foo AvA" - Stopped
	if(PlayerCaption.Pos("Foo AvA"))
	 PlayerCaption = "";
  }
  else
   PlayerCaption = "";

  if(!IgnoreCoreUserTuneChk)
   UserTunePlayerCaption = "";
  else
   UserTunePlayerCaption = PlayerCaption;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aMPCDownExecute(TObject *Sender)
{
  HWND PlayerHwnd = FindWindow("MediaPlayerClassicW",NULL);

  if(PlayerHwnd)
  {
	wchar_t PlayerTitle[255];
	GetWindowTextW(PlayerHwnd,PlayerTitle,sizeof(PlayerTitle));
    PlayerCaption = PlayerTitle;

	//Usuwanie "- Media Player Classic"
	if(PlayerCaption.Pos("- Media Player Classic"))
	{
	  PlayerCaption.Delete(PlayerCaption.Pos("- Media Player Classic"), PlayerCaption.Length());
	  PlayerCaption = PlayerCaption.Trim();
    }
  }
  else
   PlayerCaption = "";

  UserTunePlayerCaption = PlayerCaption;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aLastFMDownExecute(TObject *Sender)
{
  if(!LastfmWindowHwnd)
  {
	bool ExeExists;
	void *Snap;
    PROCESSENTRY32 proces;

    Snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS , 0);
    proces.dwSize = sizeof(PROCESSENTRY32);

    if(Process32First(Snap , &proces))
	{
	  do
	  {
        if(proces.szExeFile[ 0 ] != '[')
        {
		  if(proces.szExeFile=="LastFM.exe")
		   ExeExists=true;
		}
	  }
	  while(Process32Next(Snap , &proces));
	}
	CloseHandle(Snap);

	if(ExeExists)
	 EnumWindows((WNDENUMPROC)FindLastfm,0);
	else
	 PlayerCaption="";
  }

  if(LastfmWindowHwnd)
  {
	if(IsWindow(LastfmWindowHwnd))
	{
	  wchar_t PlayerTitle[255];
	  GetWindowTextW(LastfmWindowHwnd,PlayerTitle,sizeof(PlayerTitle));
	  PlayerCaption = PlayerTitle;

	  if(PlayerCaption.Pos("Last.fm"))
	   PlayerCaption="";
	}
	else
	{
	  LastfmWindowHwnd=NULL;
	  PlayerCaption="";
	}
  }
  else
   PlayerCaption="";

  UserTunePlayerCaption = PlayerCaption;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aWMPDownExecute(TObject *Sender)
{
  //Sposob pierwszy
  TRegistry *WMP = new TRegistry();
  WMP->RootKey = HKEY_CURRENT_USER;
  WMP->OpenKey("Software\\Microsoft\\MediaPlayer\\CurrentMetadata", false);
  PlayerCaption = WMP->ReadString("Author");
  if(!PlayerCaption.IsEmpty())
  {
	PlayerCaption = PlayerCaption + " - " + WMP->ReadString("Title");
	//SongLength
	CC_SONGLENGTH = WMP->ReadString("durationString");
	if(CC_SONGLENGTH.Pos("0")==1) CC_SONGLENGTH.Delete(1,1);
  }
  else
   CC_SONGLENGTH = "";
  delete WMP;

  //Sposob drugi
  if(PlayerCaption.IsEmpty())
  {
	HWND PlayerHwnd = FindWindow("WMPlayerApp",NULL);
	if(!PlayerHwnd) PlayerHwnd = FindWindow("Media Player 2",NULL);

	if(PlayerHwnd)
	{
	  wchar_t PlayerTitle[255];
	  GetWindowTextW(PlayerHwnd,PlayerTitle,sizeof(PlayerTitle));
	  PlayerCaption = PlayerTitle;

	  //Usuwanie "- Windows Media Player"
	  if(PlayerCaption.Pos("- Windows Media Player"))
	  {
		PlayerCaption.Delete(PlayerCaption.Pos("- Windows Media Player"), PlayerCaption.Length());
		PlayerCaption=PlayerCaption.Trim();
	  }

	  //Usuwanie "Windows Media Player"
	  if(PlayerCaption=="Windows Media Player")
	   PlayerCaption = "";
	}
	else
	 PlayerCaption = "";
  }

  UserTunePlayerCaption = PlayerCaption;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AutoModeTimerTimer(TObject *Sender)
{
  aAutoMode->Execute();

  //Jezeli opis cos zawiera
  if(!Status.IsEmpty())
  {
	if(Status!=TempStatus)
	{
	  TempStatus = Status;

	  SetStatusTimer->Enabled = false;
	  if(DoIgnorePluginsChk)
      {
		SetStatusTimer->Interval = 1000;
		JustEnabled = true;
      }
	  SetStatusTimer->Enabled = true;
    }
  }
  //Jak opis jest pusty
  else if(Status.IsEmpty())
  {
	if(StartStatus!=TempStatus)
	{
	  TempStatus = StartStatus;
	  while(GetStatus()!=StartStatus)
	   SetStatus(StartStatus,!SetOnlyInJabberChk);
	  //Symulacja pierwszego uruchomienia SetStatusTimer
	  SetStatusTimer->Enabled = false;
	  SetStatusTimer->Interval = 1000;
	  JustEnabled = true;
	}
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::SaveButtonClick(TObject *Sender)
{
  //Zapis ustawien
  aSaveSettings->Execute();
  //Ustawianie Timer'a dla ustawiania opisu
  SetStatusTimer->Interval = (1000*(SetStatusSpin->Value));
  SetStatusDelay = (1000*(SetStatusSpin->Value));
  //Timer dla wylaczenia wtyczki
  AutoTurnOffTimer->Interval = (60000*(AutoTurnOffSpin->Value));
  if(!AutoTurnOffChk) AutoTurnOffTimer->Enabled = false;
  //Wymuszenie natychmiastowego ustawienienia w opisie dokonanych zmian
  if(AutoModeTimer->Enabled)
  {
	SetStatusTimer->Enabled = false;
	JustEnabled = true;
	SetStatusTimer->Interval = 1000;
  }
  //Usuwanie tekstu "Wybierz tag do wstawienia" z TagsBox
  TagsBox->Items->Delete(6);
  //Wylaczenie buttona
  SaveButton->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormShow(TObject *Sender)
{
  //AplhaSkins
  if(!ChkSkinEnabled())
  {
	UnicodeString ThemeSkinDir = GetThemeSkinDir();
	if((FileExists(ThemeSkinDir + "\\\\Skin.asz"))&&(!ChkNativeEnabled()))
	{
	  ThemeSkinDir = StringReplace(ThemeSkinDir, "\\\\", "\\", TReplaceFlags() << rfReplaceAll);
	  sSkinManager->SkinDirectory = ThemeSkinDir;
	  sSkinManager->SkinName = "Skin.asz";
	  sSkinProvider->DrawNonClientArea = false;
	  sSkinManager->Active = true;
	}
	else
	 sSkinManager->Active = false;
  }
  //Odczyt ustawien
  aLoadSettings->Execute();
  //Dodanie tekstu do TagsBox i ustawienie go jako element pokazywany
  TagsBox->Items->Add("Wybierz tag do wstawienia");
  TagsBox->ItemIndex = 6;
  //Wylaczanie przycisku zastosuj
  SaveButton->Enabled = false;
  //Domyslna karta
  PageControl->ActivePage = HandlingTabSheet;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aVUPlayerDownExecute(TObject *Sender)
{
  HWND PlayerHwnd = FindWindow("VUPlayerClass",NULL);

  if(PlayerHwnd)
  {     
	wchar_t PlayerTitle[255];
	GetWindowTextW(PlayerHwnd,PlayerTitle,sizeof(PlayerTitle));
	PlayerCaption = PlayerTitle;

	//Usuwanie " ["
	if(PlayerCaption.Pos(" ["))
    {
	  PlayerCaption.Delete(PlayerCaption.Pos(" ["), PlayerCaption.Length());
	  PlayerCaption=PlayerCaption.Trim();
    }
  }
  else
   PlayerCaption = "";

  UserTunePlayerCaption = PlayerCaption;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aXMPlayDownExecute(TObject *Sender)
{
  HWND PlayerHwnd = FindWindow("XMPLAY-MAIN",NULL);

  if(PlayerHwnd)
  {
	wchar_t PlayerTitle[255];
	GetWindowTextW(PlayerHwnd,PlayerTitle,sizeof(PlayerTitle));
    PlayerCaption = PlayerTitle;

    //Usuwanie "XMPlay"
	if(PlayerCaption.Pos("XMPlay"))
     PlayerCaption = "";

	if(!SendMessage(PlayerHwnd,WM_USER,0,104))
	 PlayerCaption = "";
  }
  else
   PlayerCaption = "";

  UserTunePlayerCaption = PlayerCaption;
}
//---------------------------------------------------------------------------

//Sprawdzanie czy w danym tekscie s¹ tylko liczby
BOOL TestDigit(UnicodeString Text)
{
  int digit[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  int count = 0;
  while(count<10)
  {
	if(Text.Pos((String)digit[count])) return true;
	count++;
  }
  return false;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aCutSongNumberExecute(TObject *Sender)
{
  //Status
  if((PlayerCaption.Pos(". ")>0)&&(PlayerCaption.Pos(". ")<7))
  {
	UnicodeString TMP = PlayerCaption;
	TMP.Delete(PlayerCaption.Pos(". "), TMP.Length());
	if(TestDigit(TMP))
	{
	  PlayerCaption.Delete(1, PlayerCaption.Pos(". "));
	  PlayerCaption = PlayerCaption.Trim();
	}
  }
  //User Tune
  if((UserTunePlayerCaption.Pos(". ")>0)&&(UserTunePlayerCaption.Pos(". ")<7))
  {
	UnicodeString TMP = UserTunePlayerCaption;
	TMP.Delete(UserTunePlayerCaption.Pos(". "), TMP.Length());
	if(TestDigit(TMP))
	{
	  UserTunePlayerCaption.Delete(1, UserTunePlayerCaption.Pos(". "));
	  UserTunePlayerCaption = UserTunePlayerCaption.Trim();
	}
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aLoadSettingsExecute(TObject *Sender)
{
  TIniFile *Ini = new TIniFile(GetPluginUserDir() + "\\\\TuneStatus\\\\TuneStatus.ini");
  //Odczyt ustawieñ trybu automatycznego
  AutoModeCheckListBox->Clear();
  AutoModeCheckListBoxPreview->Clear();
  for(int Count=0;Count<13;Count++)
  {
	UnicodeString Player = Ini->ReadString("AutoMode",("Player"+IntToStr(Count+1)),"");
	if(!Player.IsEmpty())
	{
	  UnicodeString Enabled = Player;
	  Enabled.Delete(1,Enabled.Pos(";"));
	  Player.Delete(Player.Pos(";"),Player.Length());
	  if(Player==1) Player = "Winamp/AIMP2/KMPlayer";
	  else if(Player==2) Player = "Foobar2000";
	  else if(Player==3) Player = "Windows Media Player";
	  else if(Player==4) Player = "VUPlayer";
	  else if(Player==5) Player = "XMPlay";
	  else if(Player==6) Player = "Media Player Classic";
	  else if(Player==7) Player = "iTunes";
	  else if(Player==8) Player = "ALSong";
	  else if(Player==9) Player = "Wtyczki (np. AQQ Radio)";
	  else if(Player==10) Player = "Screamer Radio";
	  else if(Player==11) Player = "aTunes";
	  else if(Player==12) Player = "Songbird";
	  else if(Player==13) Player = "Last.fm Player";
	  AutoModeCheckListBox->Items->Add(Player);
	  AutoModeCheckListBoxPreview->Items->Add(Player);
	  AutoModeCheckListBox->Checked[Count] = StrToBool(Enabled);
	  AutoModeCheckListBoxPreview->Checked[Count] = StrToBool(Enabled);
	}
  }
  //Sprawdzanie ilosci obslugiwanych odtwarzaczy
  if(AutoModeCheckListBox->Count!=13)
  {
	//Resetowanie ustawien
	aResetSettings->Execute();
	//Wczytanie ustawien trybu automatycznego/manualnego
	AutoModeCheckListBox->Clear();
	for(int Count=0;Count<13;Count++)
	{
	  AutoModeCheckListBox->Items->Add(AutoModeCheckListBoxPreview->Items->Strings[Count]);
	  AutoModeCheckListBox->Checked[Count] = AutoModeCheckListBoxPreview->Checked[Count];
	}
  }
  //Status
  UnicodeString Status = UTF8ToUnicodeString((IniStrToStr(Ini->ReadString("Settings", "Status", ""))).t_str());
  if(Status.IsEmpty()) Status = "Obecnie s³ucham: CC_TUNESTATUS";
  StatusMemo->Text = Status;
  PreviewStatusMemo->Text = StatusMemo->Text;
  //SetOnlyInJabber
  SetOnlyInJabberChk = Ini->ReadBool("Settings", "SetOnlyInJabber", false);
  SetOnlyInJabberCheckBox->Checked = SetOnlyInJabberChk;
  //BlockInvisible
  BlockInvisibleChk = Ini->ReadBool("Settings", "BlockInvisible", true);
  BlockInvisibleCheckBox->Checked = BlockInvisibleChk;
  //EnableOnStart
  EnableOnStartCheckBox->Checked = Ini->ReadBool("Settings", "EnableOnStart", false);
  //FastAccess
  FastAccessChk = Ini->ReadBool("Settings", "FastAccess", true);
  FastAccessCheckBox->Checked = FastAccessChk;
  //SetStatusDelay
  SetStatusDelay = 1000*(Ini->ReadInteger("Settings", "SetStatusDelay", 5));
  if(SetStatusDelay<4000) SetStatusDelay = 4000;
  SetStatusTimer->Interval = SetStatusDelay;
  SetStatusSpin->Value = SetStatusDelay/1000;
  //IgnorePlugins
  IgnorePluginsChk = Ini->ReadBool("Settings", "IgnorePlugins", true);
  IgnorePluginsCheckBox->Checked = IgnorePluginsChk;
  //CutRadiostationName
  CutRadiostationNameChk = Ini->ReadBool("Settings", "CutRadiostationName", true);
  CutRadiostationNameCheckBox->Checked = CutRadiostationNameChk;
  //CutWWW
  CutWWWChk = Ini->ReadBool("Settings", "CutWWW", false);
  CutWWWCheckBox->Checked = CutWWWChk;
  //AutoTurnOff
  AutoTurnOffChk = Ini->ReadBool("Settings", "AutoTurnOff", false);
  AutoTurnOffCheckBox->Checked = AutoTurnOffChk;
  AutoTurnOffSpin->Enabled = AutoTurnOffChk;
  //AutoTurnOffDelay
  AutoTurnOffTimer->Interval = 60000*(Ini->ReadInteger("Settings", "AutoTurnOffDelay", 15));
  AutoTurnOffSpin->Value = Ini->ReadInteger("Settings", "AutoTurnOffDelay", 15);
  //MovieException
  MovieExceptionCk = Ini->ReadBool("Settings", "MovieException", true);
  MovieExceptionCheckBox->Checked = MovieExceptionCk;
  //Notification
  UserTuneNotificationCheckBox->Checked = Ini->ReadBool("UserTune", "Notification", false);
  UserTuneNotificationSpin->Enabled = UserTuneNotificationCheckBox->Checked;
  UserTuneExceptionButton->Enabled = UserTuneNotificationCheckBox->Checked;
  //Cloud
  UserTuneNotificationSpin->Value = Ini->ReadInteger("UserTune", "Cloud", 4);
  //Send
  UserTuneSendCheckBox->Checked = Ini->ReadBool("UserTune", "Send", false);
  UserTuneSendLabel->Enabled = UserTuneSendCheckBox->Checked;
  UserTuneSendSpin->Enabled = UserTuneSendCheckBox->Checked;
  IgnoreCoreUserTuneCheckBox->Enabled = UserTuneSendCheckBox->Checked;
  //SendDelay
  UserTuneSendSpin->Value = Ini->ReadInteger("UserTune", "SendDelay", 5);
  if(UserTuneSendSpin->Value<4) UserTuneSendSpin->Value = 4;
  SetUserTuneTimer->Interval = 1000*(UserTuneSendSpin->Value);
  //IgnoreCore
  IgnoreCoreUserTuneChk = Ini->ReadBool("UserTune", "IgnoreCore", false);
  IgnoreCoreUserTuneCheckBox->Checked = IgnoreCoreUserTuneChk;
  if((IgnoreCoreUserTuneCheckBox->Enabled)&&(IgnoreCoreUserTuneCheckBox->Checked))
   TurnOffCoreUserTune();

  delete Ini;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aSaveSettingsExecute(TObject *Sender)
{
  //Ustawianie zdefiniowanego opisu
  if(PreviewStatusMemo->Text.Pos("CC_TUNESTATUS"))
   StatusMemo->Text = PreviewStatusMemo->Text;
  //Ustawianie domyslnego opisu
  else
  {
	StatusMemo->Text = "Obecnie s³ucham: CC_TUNESTATUS";
	PreviewStatusMemo->Text = "Obecnie s³ucham: CC_TUNESTATUS";
  }
  TIniFile *Ini = new TIniFile(GetPluginUserDir() + "\\\\TuneStatus\\\\TuneStatus.ini");
  //Zapis ustawien trybu automatycznego/manualnego
  AutoModeCheckListBox->Clear();
  for(int Count=0;Count<13;Count++)
  {
	AutoModeCheckListBox->Items->Add(AutoModeCheckListBoxPreview->Items->Strings[Count]);
	AutoModeCheckListBox->Checked[Count] = AutoModeCheckListBoxPreview->Checked[Count];

	UnicodeString Player = AutoModeCheckListBox->Items->Strings[Count];
	if(Player=="Winamp/AIMP2/KMPlayer") Player = 1;
	else if(Player=="Foobar2000") Player = 2;
	else if(Player=="Windows Media Player") Player = 3;
	else if(Player=="VUPlayer") Player = 4;
	else if(Player=="XMPlay") Player = 5;
	else if(Player=="Media Player Classic") Player = 6;
	else if(Player=="iTunes") Player = 7;
	else if(Player=="ALSong") Player = 8;
	else if(Player=="Wtyczki (np. AQQ Radio)") Player = 9;
	else if(Player=="Screamer Radio") Player = 10;
	else if(Player=="aTunes") Player = 11;
	else if(Player=="Songbird") Player = 12;
	else if(Player=="Last.fm Player") Player = 13;
	int Enabled = AutoModeCheckListBox->Checked[Count];
	Ini->WriteString("AutoMode", ("Player"+IntToStr(Count+1)), (Player+";"+IntToStr(Enabled)));
  }
  //Status
  UnicodeString Status = StatusMemo->Text;
  ShortString StatusShort = UTF8EncodeToShortString(Status);
  Ini->WriteString("Settings", "Status", StrToIniStr(StatusShort.operator AnsiString()));
  //SetOnlyInJabber
  Ini->WriteBool("Settings", "SetOnlyInJabber", SetOnlyInJabberCheckBox->Checked);
  if(SetOnlyInJabberChk!=SetOnlyInJabberCheckBox->Checked)
  {
	SetOnlyInJabberChk = SetOnlyInJabberCheckBox->Checked;
	if(RunPluginCheckBox->Checked)
	{
	  if(SetOnlyInJabberChk)
	  {
		if(StartStatus!=TempStatus)
		{
		  TempStatus = StartStatus;
		  while(GetStatus()!=StartStatus)
		   SetStatus(StartStatus,SetOnlyInJabberChk);
        }
		if(!Status.IsEmpty())
		{
		  if(Status!=TempStatus)
		  {
			TempStatus = Status;
			while(GetStatus()!=Status)
			 SetStatus(Status,!SetOnlyInJabberChk);
          }
        }
      }
	  else
	  {
		TempStatus = Status;
		while(GetStatus()!=Status)
		 SetStatus(Status,!SetOnlyInJabberChk);
      }
    }
  }
  //BlockInvisible
  Ini->WriteBool("Settings", "BlockInvisible", BlockInvisibleCheckBox->Checked);
  BlockInvisibleChk = BlockInvisibleCheckBox->Checked;
  //EnableOnStart
  Ini->WriteBool("Settings", "EnableOnStart", EnableOnStartCheckBox->Checked);
  //FastAccess
  Ini->WriteBool("Settings", "FastAccess", FastAccessCheckBox->Checked);
  FastAccessChk = FastAccessCheckBox->Checked;
  DestroyFastAccess();
  if(FastAccessChk)	BuildFastAccess();
  //SetStatusDelay
  Ini->WriteInteger("Settings", "SetStatusDelay", SetStatusSpin->Value);
  SetStatusDelay = 1000*SetStatusSpin->Value;
  SetStatusTimer->Interval = SetStatusDelay;
  //IgnorePlugins
  Ini->WriteBool("Settings", "IgnorePlugins", IgnorePluginsCheckBox->Checked);
  IgnorePluginsChk = IgnorePluginsCheckBox->Checked;
  //CutRadiostationName
  Ini->WriteBool("Settings", "CutRadiostationName", CutRadiostationNameCheckBox->Checked);
  CutRadiostationNameChk = CutRadiostationNameCheckBox->Checked;
  //CutWWW
  Ini->WriteBool("Settings", "CutWWW", CutWWWCheckBox->Checked);
  CutWWWChk = CutWWWCheckBox->Checked;
  //AutoTurnOff
  Ini->WriteBool("Settings", "AutoTurnOff", AutoTurnOffCheckBox->Checked);
  AutoTurnOffChk = AutoTurnOffCheckBox->Checked;
  //AutoTurnOffDelay
  Ini->WriteInteger("Settings", "AutoTurnOffDelay", AutoTurnOffSpin->Value);
  AutoTurnOffTimer->Interval = 60000*AutoTurnOffSpin->Value;
  if(!AutoTurnOffChk) AutoTurnOffTimer->Enabled = false;
  //MovieException
  Ini->WriteBool("Settings", "MovieException", MovieExceptionCheckBox->Checked);
  MovieExceptionCk = MovieExceptionCheckBox->Checked;
  //Notification
  Ini->WriteBool("UserTune", "Notification", UserTuneNotificationCheckBox->Checked);
  //Cloud
  Ini->WriteInteger("UserTune", "Cloud", UserTuneNotificationSpin->Value);
  ChangeUserTune(UserTuneNotificationCheckBox->Checked,UserTuneNotificationSpin->Value);
  //Send
  Ini->WriteBool("UserTune", "Send", UserTuneSendCheckBox->Checked);
  if((!UserTuneSendCheckBox->Checked)&&(UserTuneTimer->Enabled))
  {
	UserTuneTimer->Enabled = false;
	SetUserTune("","");
	UserTuneStatusTMP = "";
  }
  if((UserTuneSendCheckBox->Checked)&&(!UserTuneTimer->Enabled))
  {
	UserTuneStatusTMP = "";
	UserTuneTimer->Enabled = true;
  }
  //SendDelay
  Ini->WriteInteger("UserTune", "SendDelay", UserTuneSendSpin->Value);
  SetUserTuneTimer->Interval = 1000*(UserTuneSendSpin->Value);
  //IgnoreCore
  Ini->WriteBool("UserTune", "IgnoreCore", IgnoreCoreUserTuneCheckBox->Checked);
  IgnoreCoreUserTuneChk = IgnoreCoreUserTuneCheckBox->Checked;
  if((IgnoreCoreUserTuneCheckBox->Enabled)&&(IgnoreCoreUserTuneCheckBox->Checked))
   TurnOffCoreUserTune();

  delete Ini;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aAutoModeExecute(TObject *Sender)
{
  for(int Count=0;Count<13;Count++)
  {
	if(AutoModeCheckListBox->Checked[Count])
	{
	  if(AutoModeCheckListBox->Items->Strings[Count]=="Winamp/AIMP2/KMPlayer")
	   aWinampDown->Execute();
	  else if(AutoModeCheckListBox->Items->Strings[Count]=="Foobar2000")
	   aFoobarDown->Execute();
	  else if(AutoModeCheckListBox->Items->Strings[Count]=="Windows Media Player")
	   aWMPDown->Execute();
	  else if(AutoModeCheckListBox->Items->Strings[Count]=="VUPlayer")
	   aVUPlayerDown->Execute();
	  else if(AutoModeCheckListBox->Items->Strings[Count]=="XMPlay")
	   aXMPlayDown->Execute();
	  else if(AutoModeCheckListBox->Items->Strings[Count]=="Media Player Classic")
	   aMPCDown->Execute();
	  else if(AutoModeCheckListBox->Items->Strings[Count]=="iTunes")
	   aiTunesDown->Execute();
	  else if(AutoModeCheckListBox->Items->Strings[Count]=="ALSong")
	   aALSongDown->Execute();
	  else if(AutoModeCheckListBox->Items->Strings[Count]=="Wtyczki (np. AQQ Radio)")
	   aPluginAQQRadioDown->Execute();
	  else if(AutoModeCheckListBox->Items->Strings[Count]=="Screamer Radio")
       aScreamerRadioDown->Execute();
	  else if(AutoModeCheckListBox->Items->Strings[Count]=="aTunes")
	   aaTunesDown->Execute();
	  else if(AutoModeCheckListBox->Items->Strings[Count]=="Songbird")
	   aSongbirdDown->Execute();
	  else if(AutoModeCheckListBox->Items->Strings[Count]=="Last.fm Player")
	   aLastFMDown->Execute();

	  PlayerCaption = PlayerCaption.Trim();

	  //Zablokowanie plikow filmowych
	  if((!PlayerCaption.IsEmpty())&&(MovieExceptionCk))
	  {
		if(PlayerCaption.LowerCase().Pos(".avi")>0)
		 PlayerCaption="";
		if(PlayerCaption.LowerCase().Pos(".mpg")>0)
		 PlayerCaption="";
		if(PlayerCaption.LowerCase().Pos(".mpeg")>0)
		 PlayerCaption="";
		if(PlayerCaption.LowerCase().Pos(".rmvb")>0)
		 PlayerCaption="";
	  }

	  if(!PlayerCaption.IsEmpty())
	  {
		//Wywalanie zbednych znakow
		if(PlayerCaption.LowerCase().Pos(".mp3")>0)
		 PlayerCaption.Delete(PlayerCaption.LowerCase().Pos(".mp3"),PlayerCaption.LowerCase().Pos(".mp3")+3);
		if(PlayerCaption.LowerCase().Pos(".wma")>0)
		 PlayerCaption.Delete(PlayerCaption.LowerCase().Pos(".wma"),PlayerCaption.LowerCase().Pos(".wma")+3);
		if(PlayerCaption.LowerCase().Pos(".avi")>0)
		 PlayerCaption.Delete(PlayerCaption.LowerCase().Pos(".avi"),PlayerCaption.LowerCase().Pos(".avi")+3);
		if(PlayerCaption.LowerCase().Pos(".mpg")>0)
		 PlayerCaption.Delete(PlayerCaption.LowerCase().Pos(".mpg"),PlayerCaption.LowerCase().Pos(".mpg")+3);
		if(PlayerCaption.LowerCase().Pos(".mpeg")>0)
		 PlayerCaption.Delete(PlayerCaption.LowerCase().Pos(".mpeg"),PlayerCaption.LowerCase().Pos(".mpeg")+4);
		if(PlayerCaption.LowerCase().Pos(".rmvb")>0)
		 PlayerCaption.Delete(PlayerCaption.LowerCase().Pos(".rmvb"),PlayerCaption.LowerCase().Pos(".rmvb")+4);
		//Ucinanie numeru utworu
		aCutSongNumber->Execute();
		//Wycinanie adresów stron WWW
		if(CutWWWChk) aCutWWW->Execute();
		//Przypisanie wlasciwego opisu
		Status = PlayerCaption;
		//Zakonczenie petli
		Count = 13;
	  }
	  else
	   Status = "";
	}
  }

  if(!Status.IsEmpty())
  {
	//Odtagowywanie opisu na opis w³asciwy
	aSetStatusLooks->Execute();
	Status = Status.Trim();
  }
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::aSetStatusLooksExecute(TObject *Sender)
{
  if(!Status.IsEmpty())
  {
	//Pobrany utwór
	if(StatusMemo->Text.Pos("CC_TUNESTATUS"))
	 Status = StringReplace(StatusMemo->Text, "CC_TUNESTATUS", Status, TReplaceFlags());
	//Opis pocz¹tkowy
	if(Status.Pos("CC_STARTSTATUS"))
	 Status = StringReplace(Status, "CC_STARTSTATUS", StartStatus, TReplaceFlags());
	//Wersja TuneStatus
	if(Status.Pos("CC_PLUGINVERSION"))
	 Status = StringReplace(Status, "CC_PLUGINVERSION", GetFileVersionInfo(GetPluginDir().t_str(), "FileVersion"), TReplaceFlags());
	//Wersja AQQ
	if(Status.Pos("CC_AQQVERSION"))
	 Status = StringReplace(Status, "CC_AQQVERSION", GetFileVersionInfo(NULL, "FileVersion"), TReplaceFlags());
	//SongLength
	if(Status.Pos("CC_SONGLENGTH"))
	 Status = StringReplace(Status, "CC_SONGLENGTH", CC_SONGLENGTH, TReplaceFlags());
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::TagsBoxSelect(TObject *Sender)
{
  UnicodeString Tag = TagsBox->Text;
  if(Tag!="--------Tylko dla wybranych odtwarzaczy--------")
  {
	Tag.Delete(Tag.Pos("("),Tag.Length());
    Tag=Tag.Trim();

	if(PreviewStatusMemo->Text.Pos(Tag)==0)
	{
	  UnicodeString Before,After;

	  Before = PreviewStatusMemo->Text.SubString(0,PreviewStatusMemo->SelStart);
	  After = PreviewStatusMemo->Text.SubString(PreviewStatusMemo->SelStart+1,(PreviewStatusMemo->Text).Length());
      PreviewStatusMemo->Clear();
	  PreviewStatusMemo->Text = Before + " " + Tag + " " + After;
    }
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::TagsBoxCloseUp(TObject *Sender)
{
  if(TagsBox->Text.IsEmpty())
  {
	TagsBox->Items->Delete(6);
    TagsBox->Items->Add("Wybierz tag do wstawienia");
	TagsBox->ItemIndex = 6;
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::TagsBoxDropDown(TObject *Sender)
{
  TagsBox->Items->Delete(6);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormClose(TObject *Sender, TCloseAction &Action)
{
  TagsBox->Items->Delete(6);
  aLoadSettings->Execute();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::SetStatusTimerTimer(TObject *Sender)
{
  SetStatusTimer->Enabled = false;
  DoIgnorePluginsChk = false;

  if(Status!=GetStatus())
  {
	AutoTurnOffTimer->Enabled = false;
	if(AllowChangeStatus(BlockInvisibleChk))
	{
	  while(GetStatus()!=Status)
	   SetStatus(Status,!SetOnlyInJabberChk);
	  if(AutoTurnOffChk) AutoTurnOffTimer->Enabled = true;
    }
    else
    {
      //Symulacja pierwszego uruchomienia SetStatusTimer
	  SetStatusTimer->Enabled = false;
	  SetStatusTimer->Interval = 1000;
	  JustEnabled = true;
    }
  }
  //Sprawdzenie czy to by³o pierwsze uruchomienie Timer'a
  if(JustEnabled)
  {
    //Przywrócenie zapisanych ustawieñ
	SetStatusTimer->Interval = SetStatusDelay;
	JustEnabled = false;
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aiTunesDownExecute(TObject *Sender)
{
  HWND PlayerHwnd = FindWindow("iTunes",NULL);

  if(PlayerHwnd)
  {
    //Ustalanie sciezki iTunes
	if(iTunesStatusPath.IsEmpty())
	{
	  DWORD procesID;
	  GetWindowThreadProcessId(PlayerHwnd, &procesID);
      iTunesStatusPath = GetPathOfProces(procesID);
      iTunesStatusPath = StringReplace(iTunesStatusPath, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
      iTunesStatusPath = ExtractFilePath(iTunesStatusPath) + "TuneStatus.txt";
    }
    //Pobieranie opisu z pliku
    try
	{
      SongFromFile->Lines->LoadFromFile(iTunesStatusPath);
      PlayerCaption = SongFromFile->Text;
    }
	catch(...) { PlayerCaption = SongFromFile->Text; }
  }
  else
  {
	iTunesStatusPath = "";
	PlayerCaption = "";
  }

  UserTunePlayerCaption = PlayerCaption;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aALSongDownExecute(TObject *Sender)
{
  HWND PlayerHwnd = FindWindow("ALSongKernelWindow",NULL);

  if(PlayerHwnd)
  {
	wchar_t PlayerTitle[255];
	GetWindowTextW(PlayerHwnd,PlayerTitle,sizeof(PlayerTitle));
	PlayerCaption = PlayerTitle;

    //Usuwanie "ALSong"
	if(PlayerCaption.Pos("ALSong"))
	 PlayerCaption = "";
  }
  else
   PlayerCaption = "";

  UserTunePlayerCaption = PlayerCaption;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aPluginAQQRadioDownExecute(TObject *Sender)
{
  PlayerCaption = GetPluginSong();
  if(!PlayerCaption.IsEmpty())
  {
	//Usuwanie "Wtyczka AQQ Radio:"
	if(PlayerCaption.Pos("Wtyczka AQQ Radio:"))
	{
	  PlayerCaption.Delete(1, PlayerCaption.Pos(":"));
	  PlayerCaption = PlayerCaption.Trim();
	  if(CutRadiostationNameChk)
	  {
		PlayerCaption.Delete(1, PlayerCaption.Pos("- "));
		PlayerCaption = PlayerCaption.Trim();
	  }
	}
	//Symulacja pierwszego uruchomienia SetStatusTimer
	if(IgnorePluginsChk) DoIgnorePluginsChk = true;
  }

  if(!IgnoreCoreUserTuneChk)
   UserTunePlayerCaption = "";
  else
   UserTunePlayerCaption = PlayerCaption;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aSongbirdDownExecute(TObject *Sender)
{
  if(!SongbirdWindowHwnd)
  {
	bool ExeExists;
	void *Snap;
    PROCESSENTRY32 proces;

    Snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS , 0);
    proces.dwSize = sizeof(PROCESSENTRY32);

    if(Process32First(Snap , &proces))
    {
      do
      {
        if(proces.szExeFile[ 0 ] != '[')
        {
		  if(proces.szExeFile=="songbird.exe")
		   ExeExists = true;
        }
      }
      while(Process32Next(Snap , &proces));
    }
    CloseHandle(Snap);

	if(ExeExists)
	 EnumWindows((WNDENUMPROC)FindSongbird,0);
    else
     PlayerCaption = "";
  }

  if(SongbirdWindowHwnd)
  {
	if(IsWindow(SongbirdWindowHwnd))
	{
	  wchar_t PlayerTitle[255];
	  GetWindowTextW(SongbirdWindowHwnd,PlayerTitle,sizeof(PlayerTitle));
	  PlayerCaption = PlayerTitle;
	  PlayerCaption = PlayerCaption.Trim();

	  if(PlayerCaption.Pos("[Stopped]"))
	   PlayerCaption = "";

	  if(PlayerCaption.Pos("Songbird"))
	   PlayerCaption = "";
	}
	else
	{
	  SongbirdWindowHwnd = NULL;
	  PlayerCaption = "";
	}
  }
  else
   PlayerCaption = "";

  UserTunePlayerCaption = PlayerCaption;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::SongbirdDownloadClick(TObject *Sender)
{
  ShellExecute(NULL, "open", "http://addons.songbirdnest.com/addon/42", NULL, NULL, SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::WMPDownloadClick(TObject *Sender)
{
  ShellExecute(NULL, "open", "http://beherit.pl/download/WMP.zip", NULL, NULL, SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::iTunesDownloadClick(TObject *Sender)
{
  ShellExecute(NULL, "open", "http://beherit.pl/download/iTunes.zip", NULL, NULL, SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FoobarDownloadClick(TObject *Sender)
{
  ShellExecute(NULL, "open", "http://beherit.pl/download/Foobar2000.zip", NULL, NULL, SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aaTunesDownExecute(TObject *Sender)
{
  if(!aTunesWindowHwnd)
  {
	bool ExeExists;
	void *Snap;
    PROCESSENTRY32 proces;

    Snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS , 0);
    proces.dwSize = sizeof(PROCESSENTRY32);

    if(Process32First(Snap , &proces))
    {
      do
      {
        if(proces.szExeFile[ 0 ] != '[')
		{
		  if(proces.szExeFile=="aTunes.exe")
		   ExeExists = true;
        }
      }
      while(Process32Next(Snap , &proces));
	}
	CloseHandle(Snap);

	if(ExeExists)
	 EnumWindows((WNDENUMPROC)FindaTunes,0);
	else
	 PlayerCaption = "";
  }

  if(!aTunesWindowHwnd)
  {
	if(IsWindow(aTunesWindowHwnd))
	{
	  wchar_t PlayerTitle[255];
	  GetWindowTextW(aTunesWindowHwnd,PlayerTitle,sizeof(PlayerTitle));
	  PlayerCaption = PlayerTitle;
	  PlayerCaption = PlayerCaption.Trim();

	  //Usuwanie "- aTunes [wersja]""
	  if(PlayerCaption.Pos("- aTunes"))
	  {
		PlayerCaption.Delete(PlayerCaption.Pos("- aTunes"), PlayerCaption.Length());
		PlayerCaption = PlayerCaption.Trim();
	  }

	  //Usuwanie "aTunes [wersja]"
	  if(PlayerCaption.Pos("aTunes"))
	   PlayerCaption = "";
	}
	else
	{
	  aTunesWindowHwnd = NULL;
	  PlayerCaption = "";
    }
  }
  else
   PlayerCaption = "";

  UserTunePlayerCaption = PlayerCaption;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AutoModeCheckListBoxPreviewDragDrop(
	  TObject *Sender, TObject *Source, int X, int Y)
{
  if(Sender->ClassNameIs("TsCheckListBox") && Source->ClassNameIs("TsCheckListBox"))
  {
	TPoint Point;
	Point.x = X;
	Point.y = Y;
	if(AutoModeCheckListBoxPreview->ItemAtPos(Point,true)!=-1)
	{
	  AutoModeCheckListBoxPreview->Items->Exchange(AutoModeCheckListBoxPreview->ItemIndex,AutoModeCheckListBoxPreview->ItemAtPos(Point,true));
	  SaveButton->Enabled = true;
	}
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AutoModeCheckListBoxPreviewDragOver(
	  TObject *Sender, TObject *Source, int X, int Y, TDragState State,
	  bool &Accept)
{
  Accept = Source->ClassNameIs("TsCheckListBox");
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aCutWWWExecute(TObject *Sender)
{
  while(PlayerCaption.Pos("http://")>0)
  {
	UnicodeString TMP = PlayerCaption;

	TMP.Delete(1,TMP.Pos("http://")-1);
	if(TMP.Pos(" ")>0)
	 TMP.Delete(TMP.Pos(" "),TMP.Length());

	UnicodeString TMP2 = PlayerCaption;
	TMP2.Delete(TMP2.Pos("http://"),TMP2.Length());

	while(TMP2.Pos(" "))
	 TMP2.Delete(1,TMP2.Pos(" "));

	TMP = TMP2 + TMP;

	PlayerCaption = StringReplace(PlayerCaption, TMP, "", TReplaceFlags() << rfReplaceAll);
	PlayerCaption = StringReplace(PlayerCaption, "  ", " ", TReplaceFlags() << rfReplaceAll);
	PlayerCaption = PlayerCaption.Trim();
  }

  while(PlayerCaption.Pos("www.")>0)
  {
	UnicodeString TMP = PlayerCaption;

	TMP.Delete(1,TMP.Pos("www.")-1);
	if(TMP.Pos(" ")>0)
	 TMP.Delete(TMP.Pos(" "),TMP.Length());

	UnicodeString TMP2 = PlayerCaption;
	TMP2.Delete(TMP2.Pos("www."),TMP2.Length());

	while(TMP2.Pos(" "))
	 TMP2.Delete(1,TMP2.Pos(" "));

	TMP = TMP2 + TMP;

	PlayerCaption = StringReplace(PlayerCaption, TMP, "", TReplaceFlags() << rfReplaceAll);
	PlayerCaption = StringReplace(PlayerCaption, "  ", " ", TReplaceFlags() << rfReplaceAll);
	PlayerCaption = PlayerCaption.Trim();
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aScreamerRadioDownExecute(TObject *Sender)
{
  if(!ScreamerRadioWindowHwnd)
  {
	bool ExeExists;
	void *Snap;
	PROCESSENTRY32 proces;

	Snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS , 0);
	proces.dwSize = sizeof(PROCESSENTRY32);

	if(Process32First(Snap , &proces))
	{
	  do
	  {
		if(proces.szExeFile[ 0 ] != '[')
		{
		  if(proces.szExeFile=="screamer.exe")
		   ExeExists = true;
		}
	  }
	  while(Process32Next(Snap , &proces));
	}
	CloseHandle(Snap);

	if(ExeExists)
	{
	  EnumWindows((WNDENUMPROC)FindScreamerRadio,0);
	  if(ScreamerRadioWindowHwnd==NULL)
	   EnumDesktopWindows(NULL,(WNDENUMPROC)FindScreamerRadio,0);
	}
	else
	 PlayerCaption = "";
  }

  if(ScreamerRadioWindowHwnd)
  {
	if(IsWindow(ScreamerRadioWindowHwnd))
	{
	  wchar_t PlayerTitle[255];
	  GetWindowTextW(ScreamerRadioWindowHwnd,PlayerTitle,sizeof(PlayerTitle));
	  PlayerCaption = PlayerTitle;

	  //Usuwanie "Screamer Radio"
	  if(PlayerCaption.Pos("Screamer Radio"))
	   PlayerCaption = "";
	}
	else
	{
	  ScreamerRadioWindowHwnd = NULL;
	  PlayerCaption = "";
    }
  }
  else
   PlayerCaption = "";

  UserTunePlayerCaption = PlayerCaption;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AutoTurnOffTimerTimer(TObject *Sender)
{
  AutoTurnOffTimer->Enabled = false;
  SetStatusTimer->Enabled = false;
  AutoModeTimer->Enabled = false;
  if(FastAccessChk) UpdateFastAccess(false);
  TempStatus = GetStatus();
  if(StartStatus!=TempStatus)
  {
	while(GetStatus()!=StartStatus)
	 SetStatus(StartStatus,!SetOnlyInJabberChk);
    TempStatus = "";
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aResetSettingsExecute(TObject *Sender)
{
  AutoModeCheckListBoxPreview->Clear();

  AutoModeCheckListBoxPreview->Items->Add("Winamp/AIMP2/KMPlayer");
  AutoModeCheckListBoxPreview->Items->Add("Foobar2000");
  AutoModeCheckListBoxPreview->Items->Add("Windows Media Player");
  AutoModeCheckListBoxPreview->Items->Add("VUPlayer");
  AutoModeCheckListBoxPreview->Items->Add("XMPlay");
  AutoModeCheckListBoxPreview->Items->Add("Media Player Classic");
  AutoModeCheckListBoxPreview->Items->Add("iTunes");
  AutoModeCheckListBoxPreview->Items->Add("ALSong");
  AutoModeCheckListBoxPreview->Items->Add("Wtyczki (np. AQQ Radio)");
  AutoModeCheckListBoxPreview->Items->Add("Screamer Radio");
  AutoModeCheckListBoxPreview->Items->Add("aTunes");
  AutoModeCheckListBoxPreview->Items->Add("Songbird");
  AutoModeCheckListBoxPreview->Items->Add("Last.fm Player");

  for(int Count=0;Count<9;Count++)
   AutoModeCheckListBoxPreview->Checked[Count] = true;
  for(int Count=9;Count<13;Count++)
   AutoModeCheckListBoxPreview->Checked[Count] = false;

  SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AutoModeCheckListBoxPreviewMouseMove(
      TObject *Sender, TShiftState Shift, int X, int Y)
{
  TPoint Point;
  Point.x = X;
  Point.y = Y;
  if(AutoModeCheckListBoxPreview->ItemAtPos(Point,true)!=-1)
  {
	if(AutoModeCheckListBoxPreview->Items->Strings[AutoModeCheckListBoxPreview->ItemAtPos(Point,true)]=="Foobar2000")
	{
	  AutoModeCheckListBoxPreview->Hint = "Dla lepszego dzia³ania zaleca siê zainstalowaæ wtyczkê do Foobar2000";
	  AutoModeCheckListBoxPreview->PopupMenu = FoobarPopupMenu;
	  AutoModeCheckListBoxPreview->Cursor = crHelp;
    }
	else if(AutoModeCheckListBoxPreview->Items->Strings[AutoModeCheckListBoxPreview->ItemAtPos(Point,true)]=="Windows Media Player")
    {
	  AutoModeCheckListBoxPreview->Hint = "Dla wersji WMP powy¿ej 7 do ob³sugi potrzebny jest plugin w WMP";
	  AutoModeCheckListBoxPreview->PopupMenu = WMPPopupMenu;
	  AutoModeCheckListBoxPreview->Cursor = crHelp;
    }
	else if(AutoModeCheckListBoxPreview->Items->Strings[AutoModeCheckListBoxPreview->ItemAtPos(Point,true)]=="iTunes")
    {
	  AutoModeCheckListBoxPreview->Hint = "Do obs³ugi potrzebny jest plugin w iTunes";
	  AutoModeCheckListBoxPreview->PopupMenu = iTunesPopupMenu;
	  AutoModeCheckListBoxPreview->Cursor = crHelp;
    }
	else if(AutoModeCheckListBoxPreview->Items->Strings[AutoModeCheckListBoxPreview->ItemAtPos(Point,true)]=="Songbird")
	{
	  AutoModeCheckListBoxPreview->Hint = "Do obs³ugi potrzebne jest rozszerzenie Birdtitle zainstalowane w Songbird";
	  AutoModeCheckListBoxPreview->PopupMenu = SongbirdPopupMenu;
	  AutoModeCheckListBoxPreview->Cursor = crHelp;
    }
    else
    {
	  AutoModeCheckListBoxPreview->Hint = "";
	  AutoModeCheckListBoxPreview->PopupMenu = NULL;
	  AutoModeCheckListBoxPreview->Cursor = crDefault;
    }
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::UserTuneTimerTimer(TObject *Sender)
{
  aUserTuneAutoMode->Execute();

  //Jezeli opis cos zawiera
  if(!UserTuneStatus.IsEmpty())
  {
	if(UserTuneStatus!=UserTuneStatusTMP)
    {
	  SetUserTuneTimer->Enabled = false;
	  UserTuneStatusTMP = UserTuneStatus;
	  SetUserTuneTimer->Enabled = true;
    }
  }
  //Jak opis jest pusty
  else
  {
	if(!UserTuneStatusTMP.IsEmpty())
	{
      SetUserTune("","");
	  UserTuneStatusTMP = "";
	  SetUserTuneTimer->Enabled = false;
    }
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::SetUserTuneTimerTimer(TObject *Sender)
{
  SetUserTuneTimer->Enabled = false;

  SetUserTune(UserTuneStatusTMP,CC_SONGLENGTH);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aCutWWWUserTuneExecute(TObject *Sender)
{
  while(UserTunePlayerCaption.Pos("http://")>0)
  {
	UnicodeString TMP = UserTunePlayerCaption;

	TMP.Delete(1,TMP.Pos("http://")-1);
	if(TMP.Pos(" ")>0)
	 TMP.Delete(TMP.Pos(" "),TMP.Length());

	UnicodeString TMP2 = UserTunePlayerCaption;
	TMP2.Delete(TMP2.Pos("http://"),TMP2.Length());

	while(TMP2.Pos(" "))
	 TMP2.Delete(1,TMP2.Pos(" "));

	TMP = TMP2 + TMP;

	UserTunePlayerCaption = StringReplace(UserTunePlayerCaption, TMP, "", TReplaceFlags() << rfReplaceAll);
	UserTunePlayerCaption = StringReplace(UserTunePlayerCaption, "  ", " ", TReplaceFlags() << rfReplaceAll);
	UserTunePlayerCaption = UserTunePlayerCaption.Trim();
  }

  while(UserTunePlayerCaption.Pos("www.")>0)
  {
	UnicodeString TMP = UserTunePlayerCaption;

	TMP.Delete(1,TMP.Pos("www.")-1);
	if(TMP.Pos(" ")>0)
	 TMP.Delete(TMP.Pos(" "),TMP.Length());

	UnicodeString TMP2 = UserTunePlayerCaption;
	TMP2.Delete(TMP2.Pos("www."),TMP2.Length());

	while(TMP2.Pos(" "))
	 TMP2.Delete(1,TMP2.Pos(" "));

	TMP = TMP2 + TMP;

	UserTunePlayerCaption = StringReplace(UserTunePlayerCaption, TMP, "", TReplaceFlags() << rfReplaceAll);
	UserTunePlayerCaption = StringReplace(UserTunePlayerCaption, "  ", " ", TReplaceFlags() << rfReplaceAll);
	UserTunePlayerCaption = UserTunePlayerCaption.Trim();
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AllowUserTuneTimerTimer(TObject *Sender)
{
  AllowUserTuneTimer->Enabled = false;
  SetAllowUserTune();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::RunPluginCheckBoxClick(TObject *Sender)
{
  if(RunPluginCheckBox->Checked)
  {
	StartStatus = GetStatus();
	SetStatusTimer->Interval = 1000;
	JustEnabled = true;
	AutoModeTimer->Enabled = true;
	if(FastAccessChk) UpdateFastAccess(true);
  }
  else
  {
	SetStatusTimer->Enabled = false;
	AutoModeTimer->Enabled = false;
	if(FastAccessChk) UpdateFastAccess(false);
	TempStatus = GetStatus();
	if(StartStatus!=TempStatus)
	{
	  while(GetStatus()!=StartStatus)
	   SetStatus(StartStatus,!SetOnlyInJabberChk);
      TempStatus = "";
    }
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AutoTurnOffCheckBoxClick(TObject *Sender)
{
  AutoTurnOffSpin->Enabled = AutoTurnOffCheckBox->Checked;

  SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::UserTuneNotificationCheckBoxClick(TObject *Sender)
{
  UserTuneNotificationSpin->Enabled = UserTuneNotificationCheckBox->Checked;
  UserTuneExceptionButton->Enabled = UserTuneNotificationCheckBox->Checked;

  SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::UserTuneSendCheckBoxClick(TObject *Sender)
{
  UserTuneSendLabel->Enabled = UserTuneSendCheckBox->Checked;
  UserTuneSendSpin->Enabled = UserTuneSendCheckBox->Checked;
  IgnoreCoreUserTuneCheckBox->Enabled = UserTuneSendCheckBox->Checked;

  SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::GetStatusTimerTimer(TObject *Sender)
{
  StartStatus = GetStatus();
  if((!StartStatus.IsEmpty())||(GetStatusTimerInterval==100))
  {
	GetStatusTimer->Enabled = false;
	AutoModeTimer->Enabled = true;
	RunPluginCheckBox->Checked = true;
	if(FastAccessChk)
	 UpdateFastAccess(true);
  }
  else
   GetStatusTimerInterval++;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aExitExecute(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::UserTuneExceptionButtonClick(TObject *Sender)
{
  UserTuneExceptionForm = new TUserTuneExceptionForm(Application);
  UserTuneExceptionForm->ShowModal();
  delete UserTuneExceptionForm;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aUserTuneAutoModeExecute(TObject *Sender)
{
  for(int Count=0;Count<13;Count++)
  {
	if(AutoModeCheckListBox->Checked[Count])
	{
	  if(AutoModeCheckListBox->Items->Strings[Count]=="Winamp/AIMP2/KMPlayer")
	   aWinampDown->Execute();
	  else if(AutoModeCheckListBox->Items->Strings[Count]=="Foobar2000")
	   aFoobarDown->Execute();
	  else if(AutoModeCheckListBox->Items->Strings[Count]=="Windows Media Player")
	   aWMPDown->Execute();
	  else if(AutoModeCheckListBox->Items->Strings[Count]=="VUPlayer")
	   aVUPlayerDown->Execute();
	  else if(AutoModeCheckListBox->Items->Strings[Count]=="XMPlay")
	   aXMPlayDown->Execute();
	  else if(AutoModeCheckListBox->Items->Strings[Count]=="Media Player Classic")
	   aMPCDown->Execute();
	  else if(AutoModeCheckListBox->Items->Strings[Count]=="iTunes")
	   aiTunesDown->Execute();
	  else if(AutoModeCheckListBox->Items->Strings[Count]=="ALSong")
	   aALSongDown->Execute();
	  else if(AutoModeCheckListBox->Items->Strings[Count]=="Wtyczki (np. AQQ Radio)")
	   aPluginAQQRadioDown->Execute();
	  else if(AutoModeCheckListBox->Items->Strings[Count]=="Screamer Radio")
	   aScreamerRadioDown->Execute();
	  else if(AutoModeCheckListBox->Items->Strings[Count]=="aTunes")
	   aaTunesDown->Execute();
	  else if(AutoModeCheckListBox->Items->Strings[Count]=="Songbird")
	   aSongbirdDown->Execute();
	  else if(AutoModeCheckListBox->Items->Strings[Count]=="Last.fm Player")
	   aLastFMDown->Execute();

	  UserTunePlayerCaption = UserTunePlayerCaption.Trim();

	  //Zablokowanie plikow filmowych
	  if((!UserTunePlayerCaption.IsEmpty())&&(MovieExceptionCk))
	  {
		if(UserTunePlayerCaption.LowerCase().Pos(".avi"))
		 UserTunePlayerCaption = "";
		if(UserTunePlayerCaption.LowerCase().Pos(".mpg"))
		 UserTunePlayerCaption = "";
		if(UserTunePlayerCaption.LowerCase().Pos(".mpeg"))
		 UserTunePlayerCaption = "";
		if(UserTunePlayerCaption.LowerCase().Pos(".rmvb"))
		 UserTunePlayerCaption = "";
	  }

	  if(!UserTunePlayerCaption.IsEmpty())
	  {
		//Wywalanie zbednych znakow
		if(UserTunePlayerCaption.LowerCase().Pos(".mp3"))
		 UserTunePlayerCaption.Delete(UserTunePlayerCaption.LowerCase().Pos(".mp3"),UserTunePlayerCaption.LowerCase().Pos(".mp3")+3);
		if(UserTunePlayerCaption.LowerCase().Pos(".wma"))
		 UserTunePlayerCaption.Delete(UserTunePlayerCaption.LowerCase().Pos(".wma"),UserTunePlayerCaption.LowerCase().Pos(".wma")+3);
		if(UserTunePlayerCaption.LowerCase().Pos(".avi"))
		 UserTunePlayerCaption.Delete(UserTunePlayerCaption.LowerCase().Pos(".avi"),UserTunePlayerCaption.LowerCase().Pos(".avi")+3);
		if(UserTunePlayerCaption.LowerCase().Pos(".mpg"))
		 UserTunePlayerCaption.Delete(UserTunePlayerCaption.LowerCase().Pos(".mpg"),UserTunePlayerCaption.LowerCase().Pos(".mpg")+3);
		if(UserTunePlayerCaption.LowerCase().Pos(".mpeg"))
		 UserTunePlayerCaption.Delete(UserTunePlayerCaption.LowerCase().Pos(".mpeg"),UserTunePlayerCaption.LowerCase().Pos(".mpeg")+4);
		if(UserTunePlayerCaption.LowerCase().Pos(".rmvb"))
		 UserTunePlayerCaption.Delete(UserTunePlayerCaption.LowerCase().Pos(".rmvb"),UserTunePlayerCaption.LowerCase().Pos(".rmvb")+4);
		//Ucinanie numeru utworu
		aCutSongNumber->Execute();
		//Wycinanie adresów stron WWW
		if(CutWWWChk) aCutWWWUserTune->Execute();
		//Przypisanie UserTune
		UserTuneStatus = UserTunePlayerCaption.Trim();
		//Zakonczenie petli
		Count = 13;
	  }
	  else
       UserTuneStatus = "";
	}
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::StateChangeTimerTimer(TObject *Sender)
{
  //Wylaczanie timera
  SetStatusTimer->Enabled = false;
  AutoModeTimer->Enabled = false;

  TempStatus = GetStatus();
  if(StartStatus!=TempStatus)
  {
	while(GetStatus()!=StartStatus)
	 SetStatus(StartStatus,!SetOnlyInJabberChk);
	TempStatus = "";
  }

  //Wlaczanie timera
  SetStatusTimer->Interval = 1000;
  JustEnabled = true;
  AutoModeTimer->Enabled = true;

  StateChangeTimer->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aSelectAllExecute(TObject *Sender)
{
  PreviewStatusMemo->SelectAll();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::OKButtonClick(TObject *Sender)
{
  //Zapis ustawien
  aSaveSettings->Execute();
  //Wymuszenie natychmiastowego ustawienienia w opisie dokonanych zmian
  if(AutoModeTimer->Enabled)
  {
	SetStatusTimer->Enabled = false;
	JustEnabled = true;
	SetStatusTimer->Interval = 1000;
  }
  //Usuwanie tekstu "Wybierz tag do wstawienia" z TagsBox
  TagsBox->Items->Delete(6);
  //Ukrywanie formy
  Close();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aAllowApplyExecute(TObject *Sender)
{
  SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AutoModeCheckListBoxPreviewClick(TObject *Sender)
{
  if(AutoModeCheckListBoxPreview->Items->Text!=AutoModeCheckListBox->Items->Text)
   SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AutoModeCheckListBoxPreviewClickCheck(TObject *Sender)
{
  SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormCreate(TObject *Sender)
{
  //AlphaSkins
  if(ChkSkinEnabled())
  {
	UnicodeString ThemeSkinDir = GetThemeSkinDir();
	if((FileExists(ThemeSkinDir + "\\\\Skin.asz"))&&(!ChkNativeEnabled()))
	{
	  ThemeSkinDir = StringReplace(ThemeSkinDir, "\\\\", "\\", TReplaceFlags() << rfReplaceAll);
	  sSkinManager->SkinDirectory = ThemeSkinDir;
	  sSkinManager->SkinName = "Skin.asz";
	  sSkinProvider->DrawNonClientArea = true;
	  sSkinManager->Active = true;
	}
	else
	 sSkinManager->Active = false;
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aPageControlSheetChangeExecute(TObject *Sender)
{
  if(!SaveButton->Focused()&&!OKButton->Focused()&&!RunPluginCheckBox->Focused())
  {
	if(PageControl->TabIndex!=3)
	 PageControl->TabIndex = PageControl->TabIndex + 1;
	else
	 PageControl->TabIndex = 0;
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::OtherTabSheetShow(TObject *Sender)
{
  if(!AutoTurnOffSpin->Enabled)
  {
	AutoTurnOffSpin->Enabled = true;
	AutoTurnOffSpin->Enabled = false;
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::UserTuneTabSheetShow(TObject *Sender)
{
  if(!UserTuneNotificationSpin->Enabled)
  {
	UserTuneNotificationSpin->Enabled = true;
	UserTuneNotificationSpin->Enabled = false;
  }
  if(!UserTuneSendSpin->Enabled)
  {
	UserTuneSendSpin->Enabled = true;
	UserTuneSendSpin->Enabled = false;
  }
}
//---------------------------------------------------------------------------
