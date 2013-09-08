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
#pragma resource "*.dfm"
TMainForm *MainForm;
//---------------------------------------------------------------------------
__declspec(dllimport)void SetStatus(UnicodeString SetStatusStatus, bool SetStatusForce);
__declspec(dllimport)UnicodeString GetPluginUserDir();
__declspec(dllimport)UnicodeString GetPluginDir();
__declspec(dllimport)UnicodeString GetStatus();
__declspec(dllimport)UnicodeString GetAQQRadioSong();
__declspec(dllimport)void BuildFastEnableAccess();
__declspec(dllimport)void DestroyFastEnableAccess();
__declspec(dllimport)void UpdateButton(bool Enabled);
__declspec(dllimport)bool AllowChangeStatus(bool WithInvisible);
__declspec(dllimport)void ChangeUserTune(bool Enabled, int Value);
__declspec(dllimport)void SetUserTune(UnicodeString Tune, UnicodeString Time);
__declspec(dllimport)void SetAllowUserTuneNTrue();
__declspec(dllimport)void SetAQQUserTuneOff();
//---------------------------------------------------------------------------
int GetStatusTimerInterval; //W³aczenie wtyczki wraz z AQQ - pobieranie opisu

UnicodeString PlayerCaption; //Pobrany tekst okna
UnicodeString PlayerCaption_UserTune; //Pobrany tekst okna (User Tune)
UnicodeString Opis_TMP; //Zmienna tymczasowa opisu
UnicodeString Opis_UserTune; //Zmienna opisu do UserTune
UnicodeString Opis_UserTuneTMP; //Zmienna tymczasowa opisu do UserTune (do porownyawania zmian z w.w. zmienna)
UnicodeString TMP; //Zmienna tymczasowa dla wycinania stron WWW i numeru piosenki
UnicodeString TMP2; //j.w.

bool EnableAQQUserTuneS; //czy WYSY£ANIE UserTune jest w³aczone
bool JustEnabled; //do pierwszego uruchomienia SongTimer
UnicodeString iTunesStatusPath; //do sciezki iTunes (TuneStatus.txt)
UnicodeString iTunesPlayerPath; //do sciezka iTunes
UnicodeString PlayerPath; //do sciezka Lastfm/Songbird
bool EnableFastOnOffCheck; //do pokazywania/ukrywania szybkiego przycisku
bool DisableSongTimerCheck; //do wy³¹czania SongTimer'a dla AQQ Radio
bool DoDisableSongTimerCheck; //j.w.
bool CutRadiostationNameCheck; //do ucinania nazwy radiostacji z AQQ Radio
bool CutWWWCheck; //do wycinania stron WWW z pobranego utworu
bool TimeTurnOffCheck; //do wylaczania dziala wtyczki gdy utwor nie zmienia sie od X minut
bool MovieExceptionCheck; //do wykluczenia filmow z pobierania
//Zmienne-CC_SONGLENGTH------------------------------------------------------
int res; //do pobierania nizej wymienionych danych
UnicodeString SongLength; //dlugosc piosenki
int Seconds; //j.w.
int Minutes; //j.w.
//---------------------------------------------------------------------------
HWND PlayerHwnd; //uchwytu do okien
HWND LastfmWindowHwnd; //uchwyt do okna Last.fm Player
HWND SongbirdWindowHwnd; //uchwyt do okna Songbird
HWND ScreamerRadioWindowHwnd; //uchwyt do okna Screamer Radio
HWND aTunesWindowHwnd; //uchwyt do okna iTunes
wchar_t PlayerTitle[2048]; //Do pobierania tekstu okna
wchar_t WindowCaptionName[2048]; //j.w.
UnicodeString WindowCaptionNameW;
DWORD procesID; //PID procesow
//Do-Last.fm-Player/Songbird/Screamer-Radio/aTunes---------------------------
UnicodeString ExeName;
bool ExeExists;
//---------------------------------------------------------------------------

__fastcall TMainForm::TMainForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

//Pobieranie uchwytu okna przez PID
HWND HwndPID(DWORD dwPID)
{
  HWND Hwnd = GetTopWindow(0);
  HWND hWnd = 0;
  DWORD pid;

  while(Hwnd)
  {
    GetWindowThreadProcessId(Hwnd, &pid);
    if(pid == dwPID) hWnd = Hwnd;
    Hwnd = GetNextWindow(Hwnd, GW_HWNDNEXT);
  }

  return hWnd;
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
  GetClassNameW(hWnd, WindowCaptionName, sizeof(WindowCaptionName));

  if((UnicodeString)WindowCaptionName=="QWidget")
  {
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
		LastfmWindowHwnd=hWnd;
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
  GetClassNameW(hWnd, WindowCaptionName, sizeof(WindowCaptionName));

  if((UnicodeString)WindowCaptionName=="MozillaUIWindowClass")
  {
	GetWindowThreadProcessId(hWnd, &procesID);
	PlayerPath = GetPathOfProces(procesID);

	if(ExtractFileName(PlayerPath)=="songbird.exe")
	{
	  GetWindowTextW(hWnd, WindowCaptionName, sizeof(WindowCaptionName));

	  if(
		 ((UnicodeString)WindowCaptionName!="Birdtitle notifer")
		 &&(!((UnicodeString)WindowCaptionName).IsEmpty())
		)
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
  GetClassNameW(hWnd, WindowCaptionName, sizeof(WindowCaptionName));
  WindowCaptionNameW = WindowCaptionName;

  if(WindowCaptionNameW.Pos("32770")>0)
  {
	GetWindowThreadProcessId(hWnd, &procesID);
	PlayerPath = GetPathOfProces(procesID);

	if(ExtractFileName(PlayerPath)=="screamer.exe")
	{
	  GetWindowTextW(hWnd, WindowCaptionName, sizeof(WindowCaptionName));

	  if(
		 ((UnicodeString)WindowCaptionName!="Screamer Log")
		 &&(!((UnicodeString)WindowCaptionName).IsEmpty())
		)
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
  GetClassNameW(hWnd, WindowCaptionName, sizeof(WindowCaptionName));

  if((UnicodeString)WindowCaptionName=="SunAwtFrame")
  {
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
  PlayerHwnd = FindWindow("Winamp v1.x",NULL);
  if(!PlayerHwnd)
   PlayerHwnd = FindWindow("Studio",NULL);

  if(PlayerHwnd)
  {
	//Winamp STOP?
	res = SendMessage(PlayerHwnd,WM_USER,0,104);
	if(res!=0)
	{
      GetWindowTextW(PlayerHwnd,PlayerTitle,sizeof(PlayerTitle));
	  PlayerCaption = PlayerTitle;

	  //Usuwanie "Winamp"
	  if(PlayerCaption.Pos("Winamp")==1)
	   PlayerCaption = "";

	  //Usuwanie "- Winamp"
	  if(PlayerCaption.Pos("- Winamp")>0)
	  {
		PlayerCaption.Delete(PlayerCaption.Pos("- Winamp"), PlayerCaption.Length());
		PlayerCaption=PlayerCaption.Trim();
	  }

	  //Usuwanie "*** "
	  if(PlayerCaption.Pos("*** ")>0)
	  {
		PlayerCaption.Delete(1, PlayerCaption.Pos("*** ") + 3);
		PlayerCaption=PlayerCaption.Trim();
	  }

	  //SongLength
	  res = SendMessage(PlayerHwnd,WM_USER,1,105);
	  if(res!=-1)
	  {
		Seconds = res % 60;
		res = res - Seconds;
		Minutes = res / 60;
		//Formatowanie sekund
		if(Seconds<10)
		 SongLength = IntToStr(Minutes) + ":0" + IntToStr(Seconds);
		else
		 SongLength = IntToStr(Minutes) + ":" + IntToStr(Seconds);
	  }
	  else
	   SongLength = "";
	}
	else
	 PlayerCaption = "";
  }
  else
   PlayerCaption = "";

  if(!EnableAQQUserTuneS)
   PlayerCaption_UserTune = "";
  else
   PlayerCaption_UserTune = PlayerCaption;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aFoobarDownExecute(TObject *Sender)
{
  PlayerHwnd = FindWindow("Foobar_TuneStatus",NULL);
  if(!PlayerHwnd) PlayerHwnd = FindWindow("PanelsUI",NULL);
  if(!PlayerHwnd) PlayerHwnd = FindWindow("{DA7CD0DE-1602-45e6-89A1-C2CA151E008E}/1",NULL);
  if(!PlayerHwnd) PlayerHwnd = FindWindow("{DA7CD0DE-1602-45e6-89A1-C2CA151E008E}",NULL);
  if(!PlayerHwnd) PlayerHwnd = FindWindow("{DA7CD0DE-1602-45e6-89A1-C2CA151E008E}",NULL);
  if(!PlayerHwnd) PlayerHwnd = FindWindow("{97E27FAA-C0B3-4b8e-A693-ED7881E99FC1}",NULL);
  if(!PlayerHwnd) PlayerHwnd = FindWindow("{E7076D1C-A7BF-4f39-B771-BCBE88F2A2A8}",NULL);
                                                           
  if(PlayerHwnd)
  {
	GetWindowTextW(PlayerHwnd,PlayerTitle,sizeof(PlayerTitle));
	PlayerCaption = PlayerTitle;

	//Usuwanie "[foobar2000"
	if(PlayerCaption.Pos("[foobar2000")>0)
	{
	  PlayerCaption.Delete(PlayerCaption.Pos("[foobar2000"), PlayerCaption.Length());
	  PlayerCaption=PlayerCaption.Trim();
    }

	//Usuwanie "foobar2000"
	if(PlayerCaption.Pos("foobar2000")>0)
    {
	  PlayerCaption.Delete(PlayerCaption.Pos("foobar2000"), PlayerCaption.Length());
	  PlayerCaption=PlayerCaption.Trim();
    }

    //Usuwanie "fooAvA"
	if(PlayerCaption.Pos("fooAvA")>0)
	 PlayerCaption = "";

    //"Foo AvA" - Stopped
	if(PlayerCaption.Pos("Foo AvA")>0)
	 PlayerCaption = "";
  }
  else
   PlayerCaption = "";

  if(!EnableAQQUserTuneS)
   PlayerCaption_UserTune = "";
  else
   PlayerCaption_UserTune = PlayerCaption;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aMPCDownExecute(TObject *Sender)
{
  PlayerHwnd = FindWindow("MediaPlayerClassicW",NULL);

  if(PlayerHwnd)
  {
	GetWindowTextW(PlayerHwnd,PlayerTitle,sizeof(PlayerTitle));
    PlayerCaption = PlayerTitle;

	//Usuwanie "- Media Player Classic"
	if(PlayerCaption.Pos("- Media Player Classic")>0)
	{
	  PlayerCaption.Delete(PlayerCaption.Pos("- Media Player Classic"), PlayerCaption.Length());
	  PlayerCaption = PlayerCaption.Trim();
    }
  }
  else
   PlayerCaption = "";

  PlayerCaption_UserTune = PlayerCaption;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aLastFMDownExecute(TObject *Sender)
{
  if(!LastfmWindowHwnd)
  {
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
		  ExeName=proces.szExeFile;
          if(ExeName=="LastFM.exe")
		   ExeExists=true;
		}
	  }
	  while(Process32Next(Snap , &proces));
	}
	CloseHandle(Snap);

	if(ExeExists==true)
	 EnumWindows((WNDENUMPROC)FindLastfm,0);
	else
	 PlayerCaption="";
  }

  if(LastfmWindowHwnd)
  {
	if(IsWindow(LastfmWindowHwnd))
	{
	  GetWindowTextW(LastfmWindowHwnd,PlayerTitle,sizeof(PlayerTitle));
	  PlayerCaption = PlayerTitle;

	  if(PlayerCaption.Pos("Last.fm")>0)
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

  PlayerCaption_UserTune=PlayerCaption;
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
   PlayerCaption = PlayerCaption + " - " + WMP->ReadString("Title");
  //Album
  //Author
  //Title
  //SongLength
  SongLength = WMP->ReadString("durationString");
  if(SongLength.Pos("0")==1)
   SongLength.Delete(1,1);

  delete WMP;

  //Sposob drugi
  if(PlayerCaption.IsEmpty())
  {
	PlayerHwnd = FindWindow("WMPlayerApp",NULL);
	if(!PlayerHwnd) PlayerHwnd = FindWindow("Media Player 2",NULL);

	if(PlayerHwnd)
	{
	  GetWindowTextW(PlayerHwnd,PlayerTitle,sizeof(PlayerTitle));
	  PlayerCaption = PlayerTitle;

	  //Usuwanie "- Windows Media Player"
	  if(PlayerCaption.Pos("- Windows Media Player")>0)
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

  PlayerCaption_UserTune=PlayerCaption;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::TimerTimer(TObject *Sender)
{
  aAutoDown->Execute();

  //Jezeli opis cos zawiera
  if(Opis!="")
  {
	if(Opis!=OpisTMP)
    {
	  OpisTMP=Opis;

	  SongTimer->Enabled=false;
	  if(DoDisableSongTimerCheck==true)
      {
        SongTimer->Interval=1000;
        JustEnabled=true;
      }
	  SongTimer->Enabled=true;
    }
  }
  //Jak opis jest pusty
  else if(Opis=="")
  {
	if(Opis_pocz!=OpisTMP)
	{
	  OpisTMP=Opis_pocz;
	  while(GetStatus()!=Opis_pocz)
	   SetStatus(Opis_pocz,!SetOnlyInJabberCheck);
	  //Symulacja pierwszego uruchomienia SongTimer
	  SongTimer->Enabled=false;
	  SongTimer->Interval=1000;
	  JustEnabled=true;
	}
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::SaveButtonClick(TObject *Sender)
{
  //Zapis ustawien
  aSaveSettings->Execute();
  //Ustawianie Timer'a dla ustawiania opisu
  SongTimer->Interval=(1000*(SongTimerSpin->Value));
  IntervalValue=(1000*(SongTimerSpin->Value));
  //Timer dla wylaczenia wtyczki
  TurnOffTimer->Interval=(60000*(TimeTurnOffSpin->Value));
  if(TimeTurnOffCheck==false)
   TurnOffTimer->Enabled=false;
  //Wymuszenie natychmiastowego ustawienienia w opisie dokonanych zmian
  if(Timer->Enabled==true)
  {
	SongTimer->Enabled=false;
	JustEnabled=true;
	SongTimer->Interval=1000;
  }
  //Usuwanie tekstu "Wybierz tag do wstawienia" z TagsBox
  TagsBox->Items->Delete(6);
  //Wylaczenie buttona
  SaveButton->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormShow(TObject *Sender)
{
  //Odczyt ustawien
  aReadSettings->Execute();
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
  PlayerHwnd = FindWindow("VUPlayerClass",NULL);

  if(PlayerHwnd!=NULL)
  {     
	GetWindowTextW(PlayerHwnd,PlayerTitle,sizeof(PlayerTitle));
	PlayerCaption = PlayerTitle;

	//Usuwanie " ["
	if(PlayerCaption.Pos(" [")>0)
    {
	  PlayerCaption.Delete(PlayerCaption.Pos(" ["), PlayerCaption.Length());
	  PlayerCaption=PlayerCaption.Trim();
    }
  }
  else
   PlayerCaption = "";

  PlayerCaption_UserTune=PlayerCaption;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aXMPlayDownExecute(TObject *Sender)
{
  PlayerHwnd = FindWindow("XMPLAY-MAIN",NULL);

  if(PlayerHwnd!=NULL)
  {
	GetWindowTextW(PlayerHwnd,PlayerTitle,sizeof(PlayerTitle));
    PlayerCaption = PlayerTitle;

    //Usuwanie "XMPlay"
	if(PlayerCaption.Pos("XMPlay")>0)
     PlayerCaption = "";

	if(PlayerHwnd!=NULL)
    {
      res = SendMessage(PlayerHwnd,WM_USER,0,104);
      if(res==0)
       PlayerCaption = "";
    }
  }
  else
   PlayerCaption = "";

  PlayerCaption_UserTune=PlayerCaption;
}
//---------------------------------------------------------------------------

//Sprawdzanie czy w danym tekscie s¹ tylko liczby
BOOL TestDigit(UnicodeString Text)
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

void __fastcall TMainForm::aCutSongNumberExecute(TObject *Sender)
{
  if((PlayerCaption.Pos(". ")>0)&&(PlayerCaption.Pos(". ")<7))
  {
	TMP=PlayerCaption;
	TMP.Delete(PlayerCaption.Pos(". "), TMP.Length());
	if(TestDigit(TMP)==true)
	{
	  PlayerCaption.Delete(1, PlayerCaption.Pos(". "));
	  PlayerCaption=PlayerCaption.Trim();
	}
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aReadSettingsExecute(TObject *Sender)
{
  TIniFile *Ini = new TIniFile(GetPluginUserDir() + "\\\\TuneStatus\\\\TuneStatus.ini");

  //Odczyt ustawieñ trybu automatycznego/manualnego
  AutoDownCheckListBox->Clear();
  AutoDownCheckListBoxPreview->Clear();
  AutoDownCheckListBox->Items->Add(Ini->ReadString("Auto", "Player0", "Winamp/AIMP2/KMPlayer"));
  AutoDownCheckListBox->Checked[0]=Ini->ReadBool("Auto", "PlayerCheck0", true);
  AutoDownCheckListBox->Items->Add(Ini->ReadString("Auto", "Player1", "Foobar2000"));
  AutoDownCheckListBox->Checked[1]=Ini->ReadBool("Auto", "PlayerCheck1", true);
  AutoDownCheckListBox->Items->Add(Ini->ReadString("Auto", "Player2", "Windows Media Player"));
  AutoDownCheckListBox->Checked[2]=Ini->ReadBool("Auto", "PlayerCheck2", true);
  AutoDownCheckListBox->Items->Add(Ini->ReadString("Auto", "Player3", "VUPlayer"));
  AutoDownCheckListBox->Checked[3]=Ini->ReadBool("Auto", "PlayerCheck3", true);
  AutoDownCheckListBox->Items->Add(Ini->ReadString("Auto", "Player4", "XMPlay"));
  AutoDownCheckListBox->Checked[4]=Ini->ReadBool("Auto", "PlayerCheck4", true);
  AutoDownCheckListBox->Items->Add(Ini->ReadString("Auto", "Player5", "Media Player Classic"));
  AutoDownCheckListBox->Checked[5]=Ini->ReadBool("Auto", "PlayerCheck5", true);
  AutoDownCheckListBox->Items->Add(Ini->ReadString("Auto", "Player6", "iTunes"));
  AutoDownCheckListBox->Checked[6]=Ini->ReadBool("Auto", "PlayerCheck6", true);
  AutoDownCheckListBox->Items->Add(Ini->ReadString("Auto", "Player7", "ALSong"));
  AutoDownCheckListBox->Checked[7]=Ini->ReadBool("Auto", "PlayerCheck7", true);
  AutoDownCheckListBox->Items->Add(Ini->ReadString("Auto", "Player8", "AQQ Radio"));
  AutoDownCheckListBox->Checked[8]=Ini->ReadBool("Auto", "PlayerCheck8", true);
  AutoDownCheckListBox->Items->Add(Ini->ReadString("Auto", "Player9", "Screamer Radio"));
  AutoDownCheckListBox->Checked[9]=Ini->ReadBool("Auto", "PlayerCheck9", true);
  AutoDownCheckListBox->Items->Add(Ini->ReadString("Auto", "Player10", "aTunes"));
  AutoDownCheckListBox->Checked[10]=Ini->ReadBool("Auto", "PlayerCheck10", false);
  AutoDownCheckListBox->Items->Add(Ini->ReadString("Auto", "Player11", "Songbird"));
  AutoDownCheckListBox->Checked[11]=Ini->ReadBool("Auto", "PlayerCheck11", false);
  AutoDownCheckListBox->Items->Add(Ini->ReadString("Auto", "Player12", "Last.fm Player"));
  AutoDownCheckListBox->Checked[12]=Ini->ReadBool("Auto", "PlayerCheck12", false);
  for(int i=0;i<=12;i++)
  {
	AutoDownCheckListBoxPreview->Items->Add(AutoDownCheckListBox->Items->Strings[i]);
	AutoDownCheckListBoxPreview->Checked[i]=AutoDownCheckListBox->Checked[i];
  }
  //Sprawdzanie ilosci obslugiwanych odtwarzaczy - czy resetowac ustawienia
  int Players = Ini->ReadInteger("Auto", "Players", 0);
  if(Players!=13)
   aResetSettings->Execute();

  UnicodeString Status = UTF8ToUnicodeString((IniStrToStr(Ini->ReadString("Settings", "Status", ""))).t_str());
  if(Status!="")
   StatusMemo->Text = Status;

  SetOnlyInJabberCheck = Ini->ReadBool("Settings", "SetOnlyInJabber", false);
   SetOnlyInJabberCheckBox->Checked = SetOnlyInJabberCheck;

  BlockInvisibleCheck = Ini->ReadBool("Settings", "BlockInvisible", true);
   BlockInvisibleCheckBox->Checked= BlockInvisibleCheck;

  EnablePluginOnStartCheckBox->Checked = Ini->ReadBool("Settings", "EnablePluginOnStart", false);

  EnableFastOnOffCheck = Ini->ReadBool("Settings", "EnableFastOnOff", true);
   EnableFastOnOffCheckBox->Checked = EnableFastOnOffCheck;

  IntervalValue = 1000*(Ini->ReadInteger("Settings", "SongTimerInterval", 5));
   SongTimer->Interval = IntervalValue;
   if(IntervalValue<4000)
   {
	 IntervalValue = 4000;
	 SongTimer->Interval = 4000;
   }
  SongTimerSpin->Value = Ini->ReadInteger("Settings", "SongTimerInterval", 5);
   if(SongTimerSpin->Value<4) SongTimerSpin->Value = 4;

  DisableSongTimerCheck = Ini->ReadBool("Settings", "DisableSongTimer", true);
   DisableSongTimerCheckBox->Checked = DisableSongTimerCheck;

  CutRadiostationNameCheck = Ini->ReadBool("Settings", "CutRadiostationName", true);
   CutRadiostationNameCheckBox->Checked = CutRadiostationNameCheck;

  CutWWWCheck = Ini->ReadBool("Settings", "CutWWW", false);
   CutWWWCheckBox->Checked = CutWWWCheck;

  TimeTurnOffCheck = Ini->ReadBool("Settings", "TimeTurnOff", false);
   TimeTurnOffCheckBox->Checked = TimeTurnOffCheck;
   TimeTurnOffSpin->Enabled = TimeTurnOffCheck;

  TurnOffTimer->Interval = 60000*(Ini->ReadInteger("Settings", "TimeTurnOffInterval", 15));
  TimeTurnOffSpin->Value = Ini->ReadInteger("Settings", "TimeTurnOffInterval", 15);

  MovieExceptionCheck = Ini->ReadBool("Settings", "MovieException", true);
   MovieExceptionCheckBox->Checked = MovieExceptionCheck;

  EnableUserTuneNCheckBox->Checked = Ini->ReadBool("UserTune", "EnableN", false);
   UserTuneTimeNLabel->Enabled = EnableUserTuneNCheckBox->Checked;
   UserTuneNSpin->Enabled = EnableUserTuneNCheckBox->Checked;
   UserTuneExceptionButton->Enabled = EnableUserTuneNCheckBox->Checked;
  UserTuneNSpin->Value = Ini->ReadInteger("UserTune", "TimeOutN", 4);
  EnableUserTuneSCheckBox->Checked = Ini->ReadBool("UserTune", "EnableS", false);
   UserTuneTimeSLabel1->Enabled = EnableUserTuneSCheckBox->Checked;
   UserTuneTimeSLabel2->Enabled = EnableUserTuneSCheckBox->Checked;
   UserTuneSSpin->Enabled = EnableUserTuneSCheckBox->Checked;
   EnableAQQUserTuneSCheckBox->Enabled = EnableUserTuneSCheckBox->Checked;
  UserTuneSSpin->Value = Ini->ReadInteger("UserTune", "TimeOutS", 5);
   if( UserTuneSSpin->Value<4) UserTuneSSpin->Value = 4;
   UserTuneSongTimer->Interval = 1000*(UserTuneSSpin->Value);
  EnableAQQUserTuneS = Ini->ReadBool("UserTune", "EnableAQQS", false);
   EnableAQQUserTuneSCheckBox->Checked = EnableAQQUserTuneS;
  if((EnableAQQUserTuneSCheckBox->Enabled==true)&&(EnableAQQUserTuneSCheckBox->Checked==true))
   SetAQQUserTuneOff();

  delete Ini;

  PreviewStatusMemo->Text=StatusMemo->Text;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aSaveSettingsExecute(TObject *Sender)
{
  if(PreviewStatusMemo->Text.Pos("CC_TUNESTATUS")>0)
   StatusMemo->Text = PreviewStatusMemo->Text;
  else
  {
	StatusMemo->Text = "Obecnie s³ucham: CC_TUNESTATUS";
	PreviewStatusMemo->Text = "Obecnie s³ucham: CC_TUNESTATUS";
  }

  TIniFile *Ini = new TIniFile(GetPluginUserDir() + "\\\\TuneStatus\\\\TuneStatus.ini");

  //Zapis ustawien trybu automatycznego/manualnego
  AutoDownCheckListBox->Clear();  
  for(int i=0;i<=12;i++)
  {
	AutoDownCheckListBox->Items->Add(AutoDownCheckListBoxPreview->Items->Strings[i]);
    AutoDownCheckListBox->Checked[i]=AutoDownCheckListBoxPreview->Checked[i];

	Ini->WriteString("Auto", "Player" + IntToStr(i), AutoDownCheckListBox->Items->Strings[i]);
	Ini->WriteBool("Auto", "PlayerCheck" + IntToStr(i), AutoDownCheckListBox->Checked[i]);
  }

  //Zapis ilosci obslugiwanych odtwarzaczy
  Ini->WriteInteger("Auto", "Players", 13);

  UnicodeString Status = StatusMemo->Text;
  ShortString StatusShort = UTF8EncodeToShortString(Status);
  Ini->WriteString("Settings", "Status", StrToIniStr(StatusShort.operator AnsiString()));

  Ini->WriteBool("Settings", "SetOnlyInJabber", SetOnlyInJabberCheckBox->Checked);
  if(SetOnlyInJabberCheck!=SetOnlyInJabberCheckBox->Checked)
  {
    SetOnlyInJabberCheck = SetOnlyInJabberCheckBox->Checked;
    if(RunPluginCheckBox->Checked==true)
    {
      if(SetOnlyInJabberCheck==true) //tylko jabber
      {
        if(Opis_pocz!=OpisTMP)
        {
		  OpisTMP=Opis_pocz;
		  while(GetStatus()!=Opis_pocz)
		   SetStatus(Opis_pocz,SetOnlyInJabberCheck);
        }
        if(Opis!="")
        {
          if(Opis!=OpisTMP)
          {
			OpisTMP=Opis;
			while(GetStatus()!=Opis)
             SetStatus(Opis,!SetOnlyInJabberCheck);
          }
        }
      }
      else //wszystkie
      {
		OpisTMP=Opis;
		while(GetStatus()!=Opis)
		 SetStatus(Opis,!SetOnlyInJabberCheck);
      }
    }
  }

  Ini->WriteBool("Settings", "BlockInvisible", BlockInvisibleCheckBox->Checked);
   BlockInvisibleCheck = BlockInvisibleCheckBox->Checked;

  Ini->WriteBool("Settings", "EnablePluginOnStart", EnablePluginOnStartCheckBox->Checked);

  Ini->WriteBool("Settings", "EnableFastOnOff", EnableFastOnOffCheckBox->Checked);
  DestroyFastEnableAccess();
  if(EnableFastOnOffCheckBox->Checked)
  {
	BuildFastEnableAccess();
	UpdateButton(Timer->Enabled);
  }

  Ini->WriteInteger("Settings", "SongTimerInterval", SongTimerSpin->Value);

  Ini->WriteBool("Settings", "DisableSongTimer", DisableSongTimerCheckBox->Checked);
   DisableSongTimerCheck = DisableSongTimerCheckBox->Checked;

  Ini->WriteBool("Settings", "CutRadiostationName", CutRadiostationNameCheckBox->Checked);
   CutRadiostationNameCheck = CutRadiostationNameCheckBox->Checked;

  Ini->WriteBool("Settings", "CutWWW", CutWWWCheckBox->Checked);
   CutWWWCheck = CutWWWCheckBox->Checked;

  Ini->WriteBool("Settings", "TimeTurnOff", TimeTurnOffCheckBox->Checked);
   TimeTurnOffCheck = TimeTurnOffCheckBox->Checked;
  Ini->WriteInteger("Settings", "TimeTurnOffInterval", TimeTurnOffSpin->Value);

  Ini->WriteBool("Settings", "MovieException", MovieExceptionCheckBox->Checked);
   MovieExceptionCheck = MovieExceptionCheckBox->Checked;

  Ini->WriteBool("UserTune", "EnableN", EnableUserTuneNCheckBox->Checked);
  Ini->WriteInteger("UserTune", "TimeOutN", UserTuneNSpin->Value);
  ChangeUserTune(EnableUserTuneNCheckBox->Checked,UserTuneNSpin->Value);
  Ini->WriteBool("UserTune", "EnableS", EnableUserTuneSCheckBox->Checked);
  if((EnableUserTuneSCheckBox->Checked==false)&&(UserTuneTimer->Enabled==true))
  {
    UserTuneTimer->Enabled=false;
    SetUserTune("","");
    Opis_UserTuneTMP="";
  }
  if((EnableUserTuneSCheckBox->Checked==true)&&(UserTuneTimer->Enabled==false))
  {
    Opis_UserTuneTMP="";
	UserTuneTimer->Enabled=true;
  }
  Ini->WriteInteger("UserTune", "TimeOutS", UserTuneSSpin->Value);
   UserTuneSongTimer->Interval = 1000*(UserTuneSSpin->Value);
  Ini->WriteBool("UserTune", "EnableAQQS", EnableAQQUserTuneSCheckBox->Checked);
  if((EnableAQQUserTuneSCheckBox->Enabled==true)&&(EnableAQQUserTuneSCheckBox->Checked==true))
   SetAQQUserTuneOff();
  EnableAQQUserTuneS = EnableAQQUserTuneSCheckBox->Checked;

  delete Ini;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aAutoDownExecute(TObject *Sender)
{
  for(int i=0;i<=12;i++)
  {
	if(AutoDownCheckListBox->Checked[i]==true)
	{
	  if(AutoDownCheckListBox->Items->Strings[i]=="Winamp/AIMP2/KMPlayer")
	   aWinampDown->Execute();
	  if(AutoDownCheckListBox->Items->Strings[i]=="Foobar2000")
	   aFoobarDown->Execute();
	  if(AutoDownCheckListBox->Items->Strings[i]=="Windows Media Player")
	   aWMPDown->Execute();
	  if(AutoDownCheckListBox->Items->Strings[i]=="VUPlayer")
	   aVUPlayerDown->Execute();
	  if(AutoDownCheckListBox->Items->Strings[i]=="XMPlay")
	   aXMPlayDown->Execute();
	  if(AutoDownCheckListBox->Items->Strings[i]=="Media Player Classic")
	   aMPCDown->Execute();
	  if(AutoDownCheckListBox->Items->Strings[i]=="iTunes")
	   aiTunesDown->Execute();
	  if(AutoDownCheckListBox->Items->Strings[i]=="ALSong")
	   aALSongDown->Execute();
	  if(AutoDownCheckListBox->Items->Strings[i]=="AQQ Radio")
	   aPluginAQQRadioDown->Execute();
	  if(AutoDownCheckListBox->Items->Strings[i]=="Screamer Radio")
       aScreamerRadioDown->Execute();
	  if(AutoDownCheckListBox->Items->Strings[i]=="aTunes")
	   aaTunesDown->Execute();
      if(AutoDownCheckListBox->Items->Strings[i]=="Songbird")
	   aSongbirdDown->Execute();
	  if(AutoDownCheckListBox->Items->Strings[i]=="Last.fm Player")
	   aLastFMDown->Execute();

	  PlayerCaption=PlayerCaption.Trim();

	  if(MovieExceptionCheck==true)
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

	  if(PlayerCaption!="")
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
		//Przypisanie wlasciwego opisu
		Opis = PlayerCaption;
		//Zakonczenie petli
		i=13;
	  }
	  else
	   Opis = "";
	}
  }

  if(Opis!="")
  {
	//Wycinanie adresów stron WWW
	if(CutWWWCheck==true)
	 aCutWWW->Execute();
	//Odtagowywanie opisu na opis w³asciwy
	aSetStatusLooks->Execute();
	//Uciannie spacji z lewej i prawej strony
	Opis=Opis.Trim();
  }
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::aSetStatusLooksExecute(TObject *Sender)
{
  if(Opis!="")
  {
	Opis_TMP = StatusMemo->Text.SubString(0, 512);
	//Pobrany utwór
	if(Opis_TMP.Pos("CC_TUNESTATUS")!=0)
     Opis = StringReplace(Opis_TMP, "CC_TUNESTATUS", Opis, TReplaceFlags() << rfReplaceAll);
    //Opis pocz¹tkowy
	if(Opis.Pos("CC_STARTSTATUS")!=0)
	 Opis = StringReplace(Opis, "CC_STARTSTATUS", Opis_pocz, TReplaceFlags() << rfReplaceAll);
	//Wersja TuneStatus
	if(Opis.Pos("CC_PLUGINVERSION")!=0)
	 Opis = StringReplace(Opis, "CC_PLUGINVERSION", GetFileVersionInfo(GetPluginDir().t_str(), "FileVersion"), TReplaceFlags() << rfReplaceAll);
	//Wersja AQQ
	if(Opis.Pos("CC_AQQVERSION")!=0)
	 Opis = StringReplace(Opis, "CC_AQQVERSION", GetFileVersionInfo(NULL, "FileVersion"), TReplaceFlags() << rfReplaceAll);
	//SongLength
	if(Opis.Pos("CC_SONGLENGTH")!=0)
	 Opis = StringReplace(Opis, "CC_SONGLENGTH", SongLength, TReplaceFlags() << rfReplaceAll);
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
	  UnicodeString Before,Fore;

	  Before=PreviewStatusMemo->Text.SubString(0,PreviewStatusMemo->SelStart);
	  Fore=PreviewStatusMemo->Text.SubString(PreviewStatusMemo->SelStart+1,(PreviewStatusMemo->Text).Length());
      PreviewStatusMemo->Clear();

	  PreviewStatusMemo->Text=Before+ " " + Tag + " " +Fore;
    }
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::TagsBoxCloseUp(TObject *Sender)
{
  if(TagsBox->Text=="")
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
  aReadSettings->Execute();
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::SongTimerTimer(TObject *Sender)
{
  SongTimer->Enabled=false;
  DoDisableSongTimerCheck=false;

  if(Opis!=GetStatus())
  {
    TurnOffTimer->Enabled=false;
	if(AllowChangeStatus(BlockInvisibleCheck)==true)
	{
	  while(GetStatus()!=Opis)
	   SetStatus(Opis,!SetOnlyInJabberCheck);
	  if(TimeTurnOffCheck==true)
       TurnOffTimer->Enabled=true;
    }
    else
    {
      //Symulacja pierwszego uruchomienia SongTimer
	  SongTimer->Enabled=false;
	  SongTimer->Interval=1000;
	  JustEnabled=true;
    }
  }
  //Sprawdzenie czy to by³o pierwsze uruchomienie Timer'a
  if(JustEnabled==true)
  {
    //Przywrócenie zapisanych ustawieñ
    SongTimer->Interval = IntervalValue;
    JustEnabled=false;
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aiTunesDownExecute(TObject *Sender)
{
  PlayerHwnd = FindWindow("iTunes",NULL);

  if(PlayerHwnd!=NULL)
  {
    //Ustalanie sciezki iTunes
	if(iTunesStatusPath=="")
	{
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
	iTunesStatusPath="";
	PlayerCaption="";
  }

  PlayerCaption_UserTune=PlayerCaption;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aALSongDownExecute(TObject *Sender)
{
  PlayerHwnd = FindWindow("ALSongKernelWindow",NULL);

  if(PlayerHwnd!=NULL)
  {
	GetWindowTextW(PlayerHwnd,PlayerTitle,sizeof(PlayerTitle));
	PlayerCaption = PlayerTitle;

    //Usuwanie "ALSong"
	if(PlayerCaption.Pos("ALSong")>0)
	 PlayerCaption = "";
  }
  else
   PlayerCaption = "";

  PlayerCaption_UserTune=PlayerCaption;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aPluginAQQRadioDownExecute(TObject *Sender)
{
  PlayerCaption="";
  PlayerCaption=GetAQQRadioSong();

  //Usuwanie "Wtyczka AQQ Radio:"
  if(PlayerCaption.Pos("Wtyczka AQQ Radio:")>0)
  {
    //Symulacja pierwszego uruchomienia SongTimer
    if(DisableSongTimerCheck==true)
	 DoDisableSongTimerCheck=true;

	PlayerCaption.Delete(1, PlayerCaption.Pos(":"));
    PlayerCaption=PlayerCaption.Trim();
    if(CutRadiostationNameCheck==true)
    {
	  PlayerCaption.Delete(1, PlayerCaption.Pos("- "));
	  PlayerCaption=PlayerCaption.Trim();
    }
  }
  else
   PlayerCaption = "";

  if(EnableAQQUserTuneS==false)
   PlayerCaption_UserTune="";
  else
   PlayerCaption_UserTune = PlayerCaption;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aSongbirdDownExecute(TObject *Sender)
{
  if(SongbirdWindowHwnd==NULL)
  {
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
          ExeName=proces.szExeFile;
          if(ExeName=="songbird.exe")
		   ExeExists=true;
        }
      }
      while(Process32Next(Snap , &proces));
    }
    CloseHandle(Snap);

	if(ExeExists==true)
	 EnumWindows((WNDENUMPROC)FindSongbird,0);
    else
     PlayerCaption="";
  }

  if(SongbirdWindowHwnd!=NULL)
  {
	if(IsWindow(SongbirdWindowHwnd)==true)
	{
	  GetWindowTextW(SongbirdWindowHwnd,PlayerTitle,sizeof(PlayerTitle));
	  PlayerCaption = PlayerTitle;
	  PlayerCaption=PlayerCaption.Trim();

	  if(PlayerCaption.Pos("[Stopped]")>0)
	   PlayerCaption = "";

	  if(PlayerCaption.Pos("Songbird")>0)
	   PlayerCaption = "";
	}
	else
	{
	  SongbirdWindowHwnd=NULL;
	  PlayerCaption="";
	}
  }
  else
   PlayerCaption="";

  PlayerCaption_UserTune=PlayerCaption;
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
  if(aTunesWindowHwnd==NULL)
  {
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
          ExeName=proces.szExeFile;
		  if(ExeName=="aTunes.exe")
		   ExeExists=true;
        }
      }
      while(Process32Next(Snap , &proces));
	}
	CloseHandle(Snap);

	if(ExeExists==true)
	 EnumWindows((WNDENUMPROC)FindaTunes,0);
	else
	 PlayerCaption="";
  }

  if(aTunesWindowHwnd!=NULL)
  {
	if(IsWindow(aTunesWindowHwnd)==true)
	{
	  GetWindowTextW(aTunesWindowHwnd,PlayerTitle,sizeof(PlayerTitle));
	  PlayerCaption = PlayerTitle;
	  PlayerCaption=PlayerCaption.Trim();

	  //Usuwanie "- aTunes [wersja]""
	  if(PlayerCaption.Pos("- aTunes")>0)
	  {
		PlayerCaption.Delete(PlayerCaption.Pos("- aTunes"), PlayerCaption.Length());
		PlayerCaption=PlayerCaption.Trim();
	  }

	  //Usuwanie "aTunes [wersja]"
	  if(PlayerCaption.Pos("aTunes")>0)
	   PlayerCaption = "";
	}
	else
	{
	  aTunesWindowHwnd=NULL;
	  PlayerCaption="";
    }
  }
  else
   PlayerCaption="";

  PlayerCaption_UserTune=PlayerCaption;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AutoDownCheckListBoxPreviewDragDrop(
	  TObject *Sender, TObject *Source, int X, int Y)
{
  if(Sender->ClassNameIs("TCheckListBox") && Source->ClassNameIs("TCheckListBox"))
  {
	TPoint Point;
	Point.x=X;
	Point.y=Y;
	if(AutoDownCheckListBoxPreview->ItemAtPos(Point,true)!=-1)
	{
	  AutoDownCheckListBoxPreview->Items->Exchange(AutoDownCheckListBoxPreview->ItemIndex,AutoDownCheckListBoxPreview->ItemAtPos(Point,true));
	  SaveButton->Enabled = true;
	}
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AutoDownCheckListBoxPreviewDragOver(
	  TObject *Sender, TObject *Source, int X, int Y, TDragState State,
	  bool &Accept)
{
  Accept = Source->ClassNameIs("TCheckListBox");
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aCutWWWExecute(TObject *Sender)
{
  while(Opis.Pos("http://")>0)
  {
	TMP=Opis;

	TMP.Delete(1,TMP.Pos("http://")-1);
	if(TMP.Pos(" ")>0)
	 TMP.Delete(TMP.Pos(" "),TMP.Length());

	TMP2 = Opis;
	TMP2.Delete(TMP2.Pos("http://"),TMP2.Length());

	while(TMP2.Pos(" "))
	 TMP2.Delete(1,TMP2.Pos(" "));

	TMP = TMP2 + TMP;

	Opis = StringReplace(Opis, TMP, "", TReplaceFlags() << rfReplaceAll);
	Opis = StringReplace(Opis, "  ", " ", TReplaceFlags() << rfReplaceAll);
	Opis=Opis.Trim();
  }

  while(Opis.Pos("www.")>0)
  {
	TMP=Opis;

	TMP.Delete(1,TMP.Pos("www.")-1);
	if(TMP.Pos(" ")>0)
	 TMP.Delete(TMP.Pos(" "),TMP.Length());

	TMP2 = Opis;
	TMP2.Delete(TMP2.Pos("www."),TMP2.Length());

	while(TMP2.Pos(" "))
	 TMP2.Delete(1,TMP2.Pos(" "));

	TMP = TMP2 + TMP;

	Opis = StringReplace(Opis, TMP, "", TReplaceFlags() << rfReplaceAll);
	Opis = StringReplace(Opis, "  ", " ", TReplaceFlags() << rfReplaceAll);
	Opis=Opis.Trim();
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aScreamerRadioDownExecute(TObject *Sender)
{
  if(ScreamerRadioWindowHwnd==NULL)
  {
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
		  ExeName=proces.szExeFile;
		  if(ExeName=="screamer.exe")
		   ExeExists=true;
		}
	  }
	  while(Process32Next(Snap , &proces));
	}
	CloseHandle(Snap);

	if(ExeExists==true)
	{
	  EnumWindows((WNDENUMPROC)FindScreamerRadio,0);
	  if(ScreamerRadioWindowHwnd==NULL)
	   EnumDesktopWindows(NULL,(WNDENUMPROC)FindScreamerRadio,0);
	}
	else
	 PlayerCaption="";
  }

  if(ScreamerRadioWindowHwnd!=NULL)
  {
	if(IsWindow(ScreamerRadioWindowHwnd)==true)
	{
	  GetWindowTextW(ScreamerRadioWindowHwnd,PlayerTitle,sizeof(PlayerTitle));
	  PlayerCaption=PlayerTitle;

	  //Usuwanie "Screamer Radio"
	  if(PlayerCaption.Pos("Screamer Radio")>0)
	   PlayerCaption="";
	}
	else
	{
	  ScreamerRadioWindowHwnd=NULL;
	  PlayerCaption="";
    }
  }
  else
   PlayerCaption="";

  PlayerCaption_UserTune=PlayerCaption;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::TurnOffTimerTimer(TObject *Sender)
{
  TurnOffTimer->Enabled=false;
  SongTimer->Enabled=false;
  Timer->Enabled=false;
  if(EnableFastOnOffCheckBox->Checked==true)
   UpdateButton(false);
  OpisTMP=GetStatus();
  if(Opis_pocz!=OpisTMP)
  {
	while(GetStatus()!=Opis_pocz)
	 SetStatus(Opis_pocz,!SetOnlyInJabberCheck);
    OpisTMP="";
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aResetSettingsExecute(TObject *Sender)
{
  AutoDownCheckListBoxPreview->Clear();

  AutoDownCheckListBoxPreview->Items->Add("Winamp/AIMP2/KMPlayer");
  AutoDownCheckListBoxPreview->Items->Add("Foobar2000");
  AutoDownCheckListBoxPreview->Items->Add("Windows Media Player");
  AutoDownCheckListBoxPreview->Items->Add("VUPlayer");
  AutoDownCheckListBoxPreview->Items->Add("XMPlay");
  AutoDownCheckListBoxPreview->Items->Add("Media Player Classic");
  AutoDownCheckListBoxPreview->Items->Add("iTunes");
  AutoDownCheckListBoxPreview->Items->Add("ALSong");
  AutoDownCheckListBoxPreview->Items->Add("AQQ Radio");
  AutoDownCheckListBoxPreview->Items->Add("Screamer Radio");
  AutoDownCheckListBoxPreview->Items->Add("aTunes");
  AutoDownCheckListBoxPreview->Items->Add("Songbird");
  AutoDownCheckListBoxPreview->Items->Add("Last.fm Player");

  for(int x=0;x<10;x++)
   AutoDownCheckListBoxPreview->Checked[x]=true;
  AutoDownCheckListBoxPreview->Checked[9]=false;
  AutoDownCheckListBoxPreview->Checked[10]=false;
  AutoDownCheckListBoxPreview->Checked[11]=false;
  AutoDownCheckListBoxPreview->Checked[12]=false;

  SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AutoDownCheckListBoxPreviewMouseMove(
      TObject *Sender, TShiftState Shift, int X, int Y)
{
  TPoint Point;
  Point.x=X;
  Point.y=Y;
  if(AutoDownCheckListBoxPreview->ItemAtPos(Point,true)!=-1)
  {
	if(AutoDownCheckListBoxPreview->Items->Strings[AutoDownCheckListBoxPreview->ItemAtPos(Point,true)]=="Foobar2000")
	{
	  AutoDownCheckListBoxPreview->Hint="Dla lepszego dzia³ania zaleca siê zainstalowaæ wtyczkê do Foobar2000";
      AutoDownCheckListBoxPreview->PopupMenu=FoobarPopupMenu;
      AutoDownCheckListBoxPreview->Cursor=crHelp;
    }
    else if(AutoDownCheckListBoxPreview->Items->Strings[AutoDownCheckListBoxPreview->ItemAtPos(Point,true)]=="Windows Media Player")
    {
      AutoDownCheckListBoxPreview->Hint="Dla wersji WMP powy¿ej 7 do ob³sugi potrzebny jest plugin w WMP";
      AutoDownCheckListBoxPreview->PopupMenu=WMPPopupMenu;
      AutoDownCheckListBoxPreview->Cursor=crHelp;
    }
    else if(AutoDownCheckListBoxPreview->Items->Strings[AutoDownCheckListBoxPreview->ItemAtPos(Point,true)]=="iTunes")
    {
      AutoDownCheckListBoxPreview->Hint="Do obs³ugi potrzebny jest plugin w iTunes";
      AutoDownCheckListBoxPreview->PopupMenu=iTunesPopupMenu;
      AutoDownCheckListBoxPreview->Cursor=crHelp;
    }
	else if(AutoDownCheckListBoxPreview->Items->Strings[AutoDownCheckListBoxPreview->ItemAtPos(Point,true)]=="Songbird")
	{
	  AutoDownCheckListBoxPreview->Hint="Do obs³ugi potrzebne jest rozszerzenie Birdtitle zainstalowane w Songbird";
      AutoDownCheckListBoxPreview->PopupMenu=SongbirdPopupMenu;
      AutoDownCheckListBoxPreview->Cursor=crHelp;
    }
    else
    {
      AutoDownCheckListBoxPreview->Hint="";
      AutoDownCheckListBoxPreview->PopupMenu=NULL;
      AutoDownCheckListBoxPreview->Cursor=crDefault;
    }
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::UserTuneTimerTimer(TObject *Sender)
{
  aAutoDownUserTune->Execute();

  //Jezeli opis cos zawiera
  if(Opis_UserTune!="")
  {
    if(Opis_UserTune!=Opis_UserTuneTMP)
    {
	  UserTuneSongTimer->Enabled=false;
	  Opis_UserTuneTMP=Opis_UserTune;
	  UserTuneSongTimer->Enabled=true;
    }
  }
  //Jak opis jest pusty
  else if(Opis_UserTune=="")
  {
	if(Opis_UserTuneTMP!="")
	{
      SetUserTune("","");
      Opis_UserTuneTMP="";
	  UserTuneSongTimer->Enabled=false;
    }
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::UserTuneSongTimerTimer(TObject *Sender)
{
  UserTuneSongTimer->Enabled=false;

  SetUserTune(Opis_UserTuneTMP,SongLength);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aCutWWWUserTuneExecute(TObject *Sender)
{
  while(Opis_UserTune.Pos("http://")>0)
  {
	TMP=Opis_UserTune;

	TMP.Delete(1,TMP.Pos("http://")-1);
	if(TMP.Pos(" ")>0)
	 TMP.Delete(TMP.Pos(" "),TMP.Length());

	TMP2 = Opis_UserTune;
	TMP2.Delete(TMP2.Pos("http://"),TMP2.Length());

	while(TMP2.Pos(" "))
	 TMP2.Delete(1,TMP2.Pos(" "));

	TMP = TMP2 + TMP;

	Opis_UserTune = StringReplace(Opis_UserTune, TMP, "", TReplaceFlags() << rfReplaceAll);
	Opis_UserTune = StringReplace(Opis_UserTune, "  ", " ", TReplaceFlags() << rfReplaceAll);
	Opis_UserTune=Opis_UserTune.Trim();
  }

  while(Opis_UserTune.Pos("www.")>0)
  {
	TMP=Opis_UserTune;

	TMP.Delete(1,TMP.Pos("www.")-1);
	if(TMP.Pos(" ")>0)
	 TMP.Delete(TMP.Pos(" "),TMP.Length());

	TMP2 = Opis_UserTune;
	TMP2.Delete(TMP2.Pos("www."),TMP2.Length());

	while(TMP2.Pos(" "))
	 TMP2.Delete(1,TMP2.Pos(" "));

	TMP = TMP2 + TMP;

	Opis_UserTune = StringReplace(Opis_UserTune, TMP, "", TReplaceFlags() << rfReplaceAll);
	Opis_UserTune = StringReplace(Opis_UserTune, "  ", " ", TReplaceFlags() << rfReplaceAll);
	Opis_UserTune  =Opis_UserTune.Trim();
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AllowUserTuneNTimerTimer(TObject *Sender)
{
  AllowUserTuneNTimer->Enabled = false;
  SetAllowUserTuneNTrue();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::RunPluginCheckBoxClick(TObject *Sender)
{
  if(RunPluginCheckBox->Checked)
  {
	Opis_pocz = GetStatus();
	SongTimer->Interval = 1000;
	JustEnabled = true;
	Timer->Enabled = true;
	if(EnableFastOnOffCheckBox->Checked)
     UpdateButton(true);
  }
  else
  {
	SongTimer->Enabled = false;
	Timer->Enabled = false;
	if(EnableFastOnOffCheckBox->Checked)
     UpdateButton(false);
    OpisTMP=GetStatus();
    if(Opis_pocz!=OpisTMP)
    {
	  while(GetStatus()!=Opis_pocz)
	   SetStatus(Opis_pocz,!SetOnlyInJabberCheck);
      OpisTMP = "";
    }
  }        
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::TimeTurnOffCheckBoxClick(TObject *Sender)
{
  TimeTurnOffSpin->Enabled=TimeTurnOffCheckBox->Checked;

  SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::EnableUserTuneNCheckBoxClick(TObject *Sender)
{
  UserTuneTimeNLabel->Enabled = EnableUserTuneNCheckBox->Checked;
  UserTuneNSpin->Enabled = EnableUserTuneNCheckBox->Checked;
  UserTuneExceptionButton->Enabled = EnableUserTuneNCheckBox->Checked;

  SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::EnableUserTuneSCheckBoxClick(TObject *Sender)
{
  UserTuneTimeSLabel1->Enabled = EnableUserTuneSCheckBox->Checked;
  UserTuneTimeSLabel2->Enabled = EnableUserTuneSCheckBox->Checked;
  UserTuneSSpin->Enabled = EnableUserTuneSCheckBox->Checked;
  EnableAQQUserTuneSCheckBox->Enabled = EnableUserTuneSCheckBox->Checked;

  SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::GetStatusTimerTimer(TObject *Sender)
{
  Opis_pocz = GetStatus();
  if((!Opis_pocz.IsEmpty())||(GetStatusTimerInterval==100))
  {
	GetStatusTimer->Enabled = false;
	Timer->Enabled = true;
	RunPluginCheckBox->Checked = true;
	if(EnableFastOnOffCheckBox->Checked)
	 UpdateButton(true);
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

void __fastcall TMainForm::aAutoDownUserTuneExecute(TObject *Sender)
{
  bool ThisSame;

  for(int i=0;i<=12;i++)
  {
	if(AutoDownCheckListBox->Checked[i]==true)
	{
	  if(AutoDownCheckListBox->Items->Strings[i]=="Winamp/AIMP2/KMPlayer")
	   aWinampDown->Execute();
	  if(AutoDownCheckListBox->Items->Strings[i]=="Foobar2000")
	   aFoobarDown->Execute();
	  if(AutoDownCheckListBox->Items->Strings[i]=="Windows Media Player")
	   aWMPDown->Execute();
	  if(AutoDownCheckListBox->Items->Strings[i]=="VUPlayer")
	   aVUPlayerDown->Execute();
	  if(AutoDownCheckListBox->Items->Strings[i]=="XMPlay")
	   aXMPlayDown->Execute();
	  if(AutoDownCheckListBox->Items->Strings[i]=="Media Player Classic")
	   aMPCDown->Execute();
	  if(AutoDownCheckListBox->Items->Strings[i]=="iTunes")
	   aiTunesDown->Execute();
	  if(AutoDownCheckListBox->Items->Strings[i]=="ALSong")
	   aALSongDown->Execute();
	  if(AutoDownCheckListBox->Items->Strings[i]=="AQQ Radio")
	   aPluginAQQRadioDown->Execute();
	  if(AutoDownCheckListBox->Items->Strings[i]=="Screamer Radio")
       aScreamerRadioDown->Execute();
	  if(AutoDownCheckListBox->Items->Strings[i]=="aTunes")
	   aaTunesDown->Execute();
      if(AutoDownCheckListBox->Items->Strings[i]=="Songbird")
	   aSongbirdDown->Execute();
	  if(AutoDownCheckListBox->Items->Strings[i]=="Last.fm Player")
	   aLastFMDown->Execute();

	  PlayerCaption = PlayerCaption.Trim();
	  PlayerCaption_UserTune = PlayerCaption_UserTune.Trim();

	  if(MovieExceptionCheck)
	  {
		if(PlayerCaption.LowerCase().Pos(".avi")>0)
		 PlayerCaption = "";
		if(PlayerCaption.LowerCase().Pos(".mpg")>0)
		 PlayerCaption = "";
		if(PlayerCaption.LowerCase().Pos(".mpeg")>0)
		 PlayerCaption = "";
		if(PlayerCaption.LowerCase().Pos(".rmvb")>0)
		 PlayerCaption = "";
	  }

	  if(!PlayerCaption.IsEmpty())
	  {
		//Sprawdzenie czy UserTune jest taki sam jak "Opis"
		if(PlayerCaption_UserTune==PlayerCaption)
		 ThisSame = true;
		else
		 ThisSame = false;
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
		//Przypisanie UserTune
		if(ThisSame)
		 Opis_UserTune = PlayerCaption;
		else
		 Opis_UserTune = "";
		//Zakonczenie petli
		i=13;
	  }
	  else
       Opis_UserTune = "";
	}
  }

  if(!Opis_UserTune.IsEmpty())
  {
	//Wycinanie adresów stron WWW
	if(CutWWWCheck)
	 aCutWWWUserTune->Execute();
	//Uciannie spacji z lewej i prawej strony
	Opis_UserTune = Opis_UserTune.Trim();
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::StateChangeTimerTimer(TObject *Sender)
{
  //Wylaczanie timera
  SongTimer->Enabled = false;
  Timer->Enabled = false;

  OpisTMP=GetStatus();
  if(Opis_pocz!=OpisTMP)
  {
	while(GetStatus()!=Opis_pocz)
	 SetStatus(Opis_pocz,!SetOnlyInJabberCheck);
	OpisTMP = "";
  }

  //Wlaczanie timera
  SongTimer->Interval=1000;
  JustEnabled=1;
  Timer->Enabled = true;

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
  //Ustawianie Timer'a dla ustawiania opisu
  SongTimer->Interval = (1000*(SongTimerSpin->Value));
  IntervalValue = (1000*(SongTimerSpin->Value));
  //Timer dla wylaczenia wtyczki
  TurnOffTimer->Interval = (60000*(TimeTurnOffSpin->Value));
  if(!TimeTurnOffCheck)
   TurnOffTimer->Enabled = false;
  //Wymuszenie natychmiastowego ustawienienia w opisie dokonanych zmian
  if(Timer->Enabled)
  {
	SongTimer->Enabled = false;
	JustEnabled = true;
	SongTimer->Interval = 1000;
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

void __fastcall TMainForm::AutoDownCheckListBoxPreviewClick(TObject *Sender)
{
  if(AutoDownCheckListBoxPreview->Items->Text!=AutoDownCheckListBox->Items->Text)
   SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AutoDownCheckListBoxPreviewClickCheck(TObject *Sender)

{
  SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

