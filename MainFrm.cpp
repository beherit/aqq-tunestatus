//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#include <inifiles.hpp>
#include <tlhelp32.h>
#pragma hdrstop
#include "MainFrm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CSPIN"
#pragma resource "*.dfm"
TMainForm *MainForm;
//---------------------------------------------------------------------------
__declspec(dllimport)void SetStatus(UnicodeString opis, bool force);
__declspec(dllimport)UnicodeString GetPluginUserDir();
__declspec(dllimport)UnicodeString GetPluginDir();
__declspec(dllimport)UnicodeString GetStatus();
__declspec(dllimport)UnicodeString GetAQQRadioSong();
__declspec(dllimport)void PrzypiszButton();
__declspec(dllimport)void UsunButton();
__declspec(dllimport)void UpdateButton(bool OnOff);
__declspec(dllimport)bool AllowChangeStatus(bool WithInvisible);
__declspec(dllimport)void ChangeUserTune(bool Enabled, int Value);
__declspec(dllimport)void SetUserTune(UnicodeString Tune);
__declspec(dllimport)void SetAllowUserTuneNTrue();
__declspec(dllimport)void SetAQQUserTuneOff();
//---------------------------------------------------------------------------
int GetStatusTimerInterval=0; //w�aczenie wtyczki wraz z AQQ - pobieranie opisu

UnicodeString ePluginDirectory=""; //Zmiena sciezki
UnicodeString opis_UserTune; //Zmienna "opisu" do UserTune
UnicodeString opis_UserTuneTMP=""; //Zmienna opisu tymczasowa dla pobierania (az do wycinania stron WWW)
UnicodeString opis_TMP=""; //Zmienna tymczasowa opisu
UnicodeString opis2=""; //j.w.
bool EnableAQQUserTuneS;

int res=0; //Do pobierania nizej wymienionych danych
UnicodeString Samplerate=""; //Samplerate piosenki
UnicodeString Bitrate=""; //Bitrate piosenki
UnicodeString Channels=""; //Liczba kana��w (mono/stereo)
UnicodeString SongLength=""; //Dlugosc piosenki
int Seconds=0; //j.w.
int Minutes=0; //j.w.
bool IsSamplerate=false; //Czy w opisie jest tag CC_SAMPLERATE
bool IsBitrate=false; //Czy w opisie jest tag CC_BITRATE
bool IsChannels=false; //Czy w opisie jest tag CC_CHANNELS
bool IsSongLength=false; //Czy w opisie jest tag CC_SONGLENGTH

bool JustEnabled=false; //Do pierwszego uruchomienia SongTimer
UnicodeString iTunesStatusPath=""; //Do sciezki iTunes (TuneStatus.txt)
UnicodeString iTunesPlayerPath=""; //Do sciezka iTunes
UnicodeString PlayerPath=""; //Do sciezka Lastfm/Songbird
bool EnableFastOnOffCheck=true; //Do pokazywania/ukrywania szybkiego przycisku
bool DisableSongTimerCheck=true; //Do wy��czania SongTimer'a dla AQQ Radio
bool DoDisableSongTimerCheck=false;
bool CutRadiostationNameCheck=true; //Do ucinania nazwy radiostacji z AQQ Radio
bool CutWWWCheck=false; //Do wycinania stron WWW z pobranego utworu
bool TimeTurnOffCheck=false; //Do wylaczania dziala wtyczki gdy utwor nie zmienia sie od X minut

HWND WindowHwnd=NULL; //Zmienna uchwytu do okien
HWND LastfmWindowHwnd=NULL; //j.w.
HWND SongbirdWindowHwnd=NULL; //j.w.
HWND ScreamerRadioWindowHwnd=NULL; //j.w.
HWND aTunesWindowHwnd=NULL; //j.w.
wchar_t this_title[2048]; //Do pobierania tekstu okna
wchar_t WindowName[2048], ClassName[2048]; //j.w. ale dla Lastfm/Songbird

UnicodeString ClassNameAnsi="";
UnicodeString WindowNameAnsi="";

int IsThere=0; //Do sprawdzania czy dany ciag char/AnsiString jest w innym ciagu

DWORD procesID=0; //do PID

//Do Lastfm/Songbird/ScreamerRadio
UnicodeString ExeName="";
bool IsThereExe=false;
bool IsThereExe2=false;
bool IsThereExe3=false;

bool BlockStatus=false,BlockAuto=false;
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
  HWND hWnd = 0;;
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
		sprintf(sTmp, ("\\StringFileInfo\\%04x%04x\\" + KeyName).c_str(), *wTmp, *(wTmp + 1));
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

  S = Str.c_str();
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

  S = Str.c_str();
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
  GetWindowTextW(hWnd, WindowName, sizeof(WindowName));
  GetClassNameW(hWnd, ClassName, sizeof(ClassName));
  GetWindowThreadProcessId(hWnd, &procesID);

  ClassNameAnsi = ClassName;
  WindowNameAnsi = WindowName;
  PlayerPath = GetPathOfProces(procesID);

  if(ExtractFileName(PlayerPath)=="LastFM.exe")
  {
	if(ClassNameAnsi=="QWidget")
	{
	  if(
		 (WindowNameAnsi!="LastFM")
		 &&(WindowNameAnsi!="Diagnostyka")
		 &&(WindowNameAnsi!="Pole�")
		 &&(WindowNameAnsi!="Zaloguj")
		 &&(WindowNameAnsi!="Kreator automatycznej aktualizacji")
		 &&(WindowNameAnsi!="Kreator ustawie�")
		 &&(WindowNameAnsi!="Opcje programu Last.fm")
		 &&(WindowNameAnsi!="Dodaj u�ytkownika")
		 &&(WindowNameAnsi!="Aktualne")
		 &&(WindowNameAnsi!="Last.fm � Informacje")
		 //&&(WindowNameAnsi!="")
		)
	  {
		LastfmWindowHwnd=hWnd;
	  }
	}
  }
  return true;
}
//---------------------------------------------------------------------------

//Szukanie okna Songbird
bool CALLBACK FindSongbird(HWND hWnd, LPARAM lParam)
{
  GetWindowTextW(hWnd, WindowName, sizeof(WindowName));
  GetClassNameW(hWnd, ClassName, sizeof(ClassName));
  GetWindowThreadProcessId(hWnd, &procesID);

  ClassNameAnsi = ClassName;
  WindowNameAnsi = WindowName;
  PlayerPath = GetPathOfProces(procesID);

  if(ExtractFileName(PlayerPath)=="songbird.exe")
  {
	if(ClassNameAnsi=="MozillaUIWindowClass")
	{
	  if(
		 (WindowNameAnsi!="Birdtitle notifer")
		 &&(WindowNameAnsi!="")
		)
	  {
		SongbirdWindowHwnd=hWnd;
	  }
	}
  }
  return true;
}
//---------------------------------------------------------------------------

//Szukanie okna ScreamerRadio
bool CALLBACK FindScreamerRadio(HWND hWnd, LPARAM lParam)
{
  GetWindowTextW(hWnd, WindowName, sizeof(WindowName));
  GetClassNameW(hWnd, ClassName, sizeof(ClassName));
  GetWindowThreadProcessId(hWnd, &procesID);

  ClassNameAnsi = ClassName;
  WindowNameAnsi = WindowName;
  PlayerPath = GetPathOfProces(procesID);

  if(ExtractFileName(PlayerPath)=="screamer.exe")
  {
	if(ClassNameAnsi=="#32770")
	{
	  if(
		 (WindowNameAnsi!="Screamer Log")
		 &&(WindowNameAnsi!="")
		)
	  {
		ScreamerRadioWindowHwnd=hWnd;
	  }
	}
  }
  return true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aWinampDownExecute(TObject *Sender)
{
  WindowHwnd = FindWindow("Winamp v1.x",NULL);
  if(!WindowHwnd)
   WindowHwnd = FindWindow("Studio",NULL);

  if(WindowHwnd!=NULL)
  {
	GetWindowTextW(WindowHwnd,this_title,sizeof(this_title));

	opis = this_title;

    //Usuwanie "- Winamp"
    IsThere = AnsiPos("- Winamp", opis);
	if(IsThere>0)
    {
      opis.Delete(IsThere, opis.Length() + 1);
      opis=opis.Trim();
    }

    //Usuwanie "*** "
    IsThere = AnsiPos("*** ", opis);
    if(IsThere>0)
    {
      opis.Delete(1, IsThere + 3);
      opis=opis.Trim();
    }

    //Usuwanie "Winamp"
    IsThere = AnsiPos("Winamp", opis);
    if(IsThere>0)
     opis = "";

    //Winamp STOP?
    res = SendMessage(WindowHwnd,WM_USER,0,104);
    if(res==0)
     opis = "";

    //Samplerate
	if(IsSamplerate==true)
    {
      res = SendMessage(WindowHwnd, WM_USER, 0, 126);
      Samplerate = IntToStr(res);
      if(Samplerate.Length()<=2)
       Samplerate = Samplerate + "KHz";
      else if(Samplerate.Length()>2)
       Samplerate = Samplerate + "Hz";
    }

    //Bitrate
	if(IsBitrate==true)
    {
      res = SendMessage(WindowHwnd, WM_USER, 1, 126);
	  Bitrate = IntToStr(res) + " kbps";
    }

    //Channels
    if(IsChannels==true)
    {
      res = SendMessage(WindowHwnd, WM_USER, 2, 126);
      if(res==1)
       Channels = "Mono";
      else if(res==2)
       Channels = "Stereo";
      else
       Channels = res;
    }

    //SongLength
    if(IsSongLength==true)
    {
     res = SendMessage(WindowHwnd,WM_USER,1,105);
      if(res!=-1)
      {
        Seconds = res % 60;
		res = res - Seconds;
		Minutes = res / 60;
		if(Seconds<10)
		 SongLength = IntToStr(Minutes) + ":0" + IntToStr(Seconds);
        else
		 SongLength = IntToStr(Minutes) + ":" + IntToStr(Seconds);
      }
    }
  }
  else
   opis = "";

  if(EnableAQQUserTuneS==false)
   opis_UserTune="";
  else
   opis_UserTune = opis;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aFoobarDownExecute(TObject *Sender)
{
  WindowHwnd = FindWindow("Foobar_TuneStatus",NULL);
  if(!WindowHwnd) WindowHwnd = FindWindow("PanelsUI",NULL);
  if(!WindowHwnd) WindowHwnd = FindWindow("{DA7CD0DE-1602-45e6-89A1-C2CA151E008E}/1",NULL);
  if(!WindowHwnd) WindowHwnd = FindWindow("{DA7CD0DE-1602-45e6-89A1-C2CA151E008E}",NULL);
  if(!WindowHwnd) WindowHwnd = FindWindow("{DA7CD0DE-1602-45e6-89A1-C2CA151E008E}",NULL);
  if(!WindowHwnd) WindowHwnd = FindWindow("{97E27FAA-C0B3-4b8e-A693-ED7881E99FC1}",NULL);
  if(!WindowHwnd) WindowHwnd = FindWindow("{E7076D1C-A7BF-4f39-B771-BCBE88F2A2A8}",NULL);
                                                           
  if(WindowHwnd!=NULL)
  {
    GetWindowTextW(WindowHwnd,this_title,sizeof(this_title));

    opis = this_title;

    //Usuwanie "[foobar2000"
    IsThere = AnsiPos("[foobar2000", opis);
    if(IsThere>0)
    {
      opis.Delete(IsThere, opis.Length() + 1);
      opis=opis.Trim();
    }

    //Usuwanie "foobar2000"
    IsThere = AnsiPos("foobar2000", opis);
    if(IsThere>0)
    {
      opis.Delete(IsThere, opis.Length() + 1);
      opis=opis.Trim();
    }

    //Usuwanie "fooAvA"
    IsThere = AnsiPos("fooAvA", opis);
    if(IsThere>0)
     opis = "";

    //"Foo AvA" - Stopped
    IsThere = AnsiPos("Foo AvA", opis);
    if(IsThere>0)
     opis = "";
  }
  else
   opis = "";

  if(EnableAQQUserTuneS==false)
   opis_UserTune="";
  else
   opis_UserTune = opis;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aMPCDownExecute(TObject *Sender)
{
  WindowHwnd = FindWindow("MediaPlayerClassicW",NULL);

  if(WindowHwnd!=NULL)
  {
    GetWindowTextW(WindowHwnd,this_title,sizeof(this_title));

    opis = this_title;

    //Usuwanie "- Media Player Classic"
    IsThere = AnsiPos("- Media Player Classic", opis);
    if(IsThere>0)
    {
      opis.Delete(IsThere, opis.Length() + 1);
      opis=opis.Trim();
    }
  }
  else
   opis = "";

  opis_UserTune=opis;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aLastFMDownExecute(TObject *Sender)
{
  if(LastfmWindowHwnd==NULL)
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
		   IsThereExe=true;
		}
	  }
	  while(Process32Next(Snap , &proces));
	}
	CloseHandle(Snap);

	if(IsThereExe==true)
	 EnumWindows((WNDENUMPROC)FindLastfm,0);
	else
	 opis="";
  }

  if(LastfmWindowHwnd!=NULL)
  {
	if(GetWindowTextW(LastfmWindowHwnd,this_title,sizeof(this_title))==0)
	 LastfmWindowHwnd=NULL;

    opis = this_title;

    IsThere = AnsiPos("Last.fm", opis);
    if(IsThere>0)
	 opis="";
  }
  else
   opis="";

  opis_UserTune=opis;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aWMPDownExecute(TObject *Sender)
{
  WindowHwnd = FindWindow("WMPlayerApp",NULL);
  if(!WindowHwnd) WindowHwnd = FindWindow("Media Player 2",NULL);

  if(WindowHwnd!=NULL)
  {
    GetWindowTextW(WindowHwnd,this_title,sizeof(this_title));

    opis = this_title;

    //Usuwanie "- Windows Media Player""
    IsThere = AnsiPos("- Windows Media Player", opis);
    if(IsThere>0)
    {
      opis.Delete(IsThere, opis.Length() + 1);
      opis=opis.Trim();
    }

    //Usuwanie "Windows Media Player"
    if(opis=="Windows Media Player")
     opis = "";
  }
  else
   opis = "";

  opis_UserTune=opis;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::TimerTimer(TObject *Sender)
{
  aAutoDown->Execute();

  //Jezeli opis cos zawiera
  if(opis!="")
  {
    if(opis!=opisTMP)
    {
	  opisTMP=opis;

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
  else if(opis=="")
  {
	if(opis_pocz!=opisTMP)
	{
	  opisTMP=opis_pocz;
	  SetStatus(opis_pocz,!SetOnlyInJabberCheck);
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
  TagsBox->Items->Delete(9);
  //Ukrywanie formy
  Visible=false;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormShow(TObject *Sender)
{
  //Odczyt ustawien
  aReadSettings->Execute();
  //Dodanie tekstu do TagsBox i ustawienie go jako element pokazywany
  TagsBox->Items->Add("Wybierz tag do wstawienia");
  TagsBox->ItemIndex = 9;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aVUPlayerDownExecute(TObject *Sender)
{
  WindowHwnd = FindWindow("VUPlayerClass",NULL);

  if(WindowHwnd!=NULL)
  {     
    GetWindowTextW(WindowHwnd,this_title,sizeof(this_title));

    opis = this_title;

    //Usuwanie " ["
    IsThere = AnsiPos(" [", opis);
    if(IsThere>0)
    {
      opis.Delete(IsThere, opis.Length() + 1);
      opis=opis.Trim();
    }
  }
  else
   opis = "";

  opis_UserTune=opis;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aXMPlayDownExecute(TObject *Sender)
{
  WindowHwnd = FindWindow("XMPLAY-MAIN",NULL);

  if(WindowHwnd!=NULL)
  {
    GetWindowTextW(WindowHwnd,this_title,sizeof(this_title));

    opis = this_title;

    //Usuwanie "XMPlay"
    IsThere = AnsiPos("XMPlay", opis);
    if(IsThere>0)
     opis = "";

    if(WindowHwnd!=NULL)
    {
      res = SendMessage(WindowHwnd,WM_USER,0,104);
      if(res==0)
       opis = "";
    }
  }
  else
   opis = "";

  opis_UserTune=opis;
}
//---------------------------------------------------------------------------

//Sprawdzanie czy w danym tekscie s� tylko liczby
BOOL TestDigit(AnsiString Text)
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
  IsThere = AnsiPos(". ", opis);
  if((IsThere>0)&&(IsThere<7))
  {
    opis2=opis;
    opis2.Delete(IsThere, opis2.Length());
    if(TestDigit(opis2)==true)
    {
      opis.Delete(1, IsThere);
      opis=opis.Trim();
    }
  }

  IsThere = AnsiPos(". ", opis_UserTune);
  if((IsThere>0)&&(IsThere<7))
  {
    opis2=opis_UserTune;
    opis2.Delete(IsThere, opis2.Length());
    if(TestDigit(opis2)==true)
    {
      opis_UserTune.Delete(1, IsThere);
      opis_UserTune=opis_UserTune.Trim();
    }
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aReadSettingsExecute(TObject *Sender)
{
  ePluginDirectory = GetPluginUserDir();

  TIniFile *Ini = new TIniFile(ePluginDirectory + "\\\\TuneStatus\\\\TuneStatus.ini");

  //Odczyt ustawie� trybu automatycznego/manualnego
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

  UnicodeString Status = UTF8ToUnicodeString(IniStrToStr(Ini->ReadString("Settings", "Status", "")).c_str());
  if(Status!="")
   StatusMemo->Text = Status;

  SetOnlyInJabberCheckBox->Checked = Ini->ReadBool("Settings", "SetOnlyInJabber", false);
  SetOnlyInJabberCheck = Ini->ReadBool("Settings", "SetOnlyInJabber", false);

  BlockInvisibleCheckBox->Checked = Ini->ReadBool("Settings", "BlockInvisible", true);
  BlockInvisibleCheck = Ini->ReadBool("Settings", "BlockInvisible", true);

  EnablePluginOnStartCheckBox->Checked = Ini->ReadBool("Settings", "EnablePluginOnStart", false);
  
  EnableFastOnOffCheckBox->Checked = Ini->ReadBool("Settings", "EnableFastOnOff", true);
  EnableFastOnOffCheck = Ini->ReadBool("Settings", "EnableFastOnOff", true);

  SongTimer->Interval = 1000*(Ini->ReadInteger("Settings", "SongTimerInterval", 5));
  IntervalValue = 1000*(Ini->ReadInteger("Settings", "SongTimerInterval", 5));
  if(IntervalValue<4000) IntervalValue = 4000;
  SongTimerSpin->Value = Ini->ReadInteger("Settings", "SongTimerInterval", 5);
  if(SongTimerSpin->Value<4) SongTimerSpin->Value = 4;

  DisableSongTimerCheckBox->Checked = Ini->ReadBool("Settings", "DisableSongTimer", true);
  DisableSongTimerCheck = Ini->ReadBool("Settings", "DisableSongTimer", true);

  CutRadiostationNameCheckBox->Checked = Ini->ReadBool("Settings", "CutRadiostationName", true);
  CutRadiostationNameCheck = Ini->ReadBool("Settings", "CutRadiostationName", true);

  CutWWWCheckBox->Checked = Ini->ReadBool("Settings", "CutWWW", false);
  CutWWWCheck = Ini->ReadBool("Settings", "CutWWW", false);

  TimeTurnOffCheckBox->Checked = Ini->ReadBool("Settings", "TimeTurnOff", false);
  TimeTurnOffCheck = Ini->ReadBool("Settings", "TimeTurnOff", false);
  TimeTurnOffSpin->Enabled=TimeTurnOffCheck;

  TurnOffTimer->Interval = 60000*(Ini->ReadInteger("Settings", "TimeTurnOffInterval", 15));
  TimeTurnOffSpin->Value = Ini->ReadInteger("Settings", "TimeTurnOffInterval", 15);

  EnableUserTuneNCheckBox->Checked = Ini->ReadBool("UserTune", "EnableN", false);
   UserTuneTimeNLabel->Enabled = Ini->ReadBool("UserTune", "EnableN", false);
   UserTuneNSpin->Enabled = Ini->ReadBool("UserTune", "EnableN", false);
  UserTuneNSpin->Value = Ini->ReadInteger("UserTune", "TimeOutN", 4);
  EnableUserTuneSCheckBox->Checked = Ini->ReadBool("UserTune", "EnableS", false);
   UserTuneTimeSLabel1->Enabled = Ini->ReadBool("UserTune", "EnableS", false);
   UserTuneTimeSLabel2->Enabled = Ini->ReadBool("UserTune", "EnableS", false);
   UserTuneSSpin->Enabled = Ini->ReadBool("UserTune", "EnableS", false);
   EnableAQQUserTuneSCheckBox->Enabled = Ini->ReadBool("UserTune", "EnableS", false);
  UserTuneSSpin->Value = Ini->ReadInteger("UserTune", "TimeOutS", 5);
  if( UserTuneSSpin->Value<4) UserTuneSSpin->Value = 4;
  UserTuneSongTimer->Interval = 1000*(Ini->ReadInteger("UserTune", "TimeOutS", 5));
  EnableAQQUserTuneSCheckBox->Checked = Ini->ReadBool("UserTune", "EnableAQQS", false);
   EnableAQQUserTuneS = Ini->ReadBool("UserTune", "EnableAQQS", false);
  if((EnableAQQUserTuneSCheckBox->Enabled==true)&&(EnableAQQUserTuneSCheckBox->Checked==true))
   SetAQQUserTuneOff();

  delete Ini;

  PreviewStatusMemo->Text=StatusMemo->Text;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aSaveSettingsExecute(TObject *Sender)
{
  StatusMemo->Text=PreviewStatusMemo->Text;

  ePluginDirectory = GetPluginUserDir();

  TIniFile *Ini = new TIniFile(ePluginDirectory + "\\\\TuneStatus\\\\TuneStatus.ini");

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
        if(opis_pocz!=opisTMP)
        {
          opisTMP=opis_pocz;
          SetStatus(opis_pocz,SetOnlyInJabberCheck);
        }
        if(opis!="")
        {
          if(opis!=opisTMP)
          {
            opisTMP=opis;
            SetStatus(opis,!SetOnlyInJabberCheck);
          }
        }
      }
      else //wszystkie
      {
        opisTMP=opis;
        SetStatus(opis,!SetOnlyInJabberCheck);
      }
    }
  }

  Ini->WriteBool("Settings", "BlockInvisible", BlockInvisibleCheckBox->Checked);
  BlockInvisibleCheck = BlockInvisibleCheckBox->Checked;

  Ini->WriteBool("Settings", "EnablePluginOnStart", EnablePluginOnStartCheckBox->Checked);

  Ini->WriteBool("Settings", "EnableFastOnOff", EnableFastOnOffCheckBox->Checked);
  if(EnableFastOnOffCheck!=EnableFastOnOffCheckBox->Checked)
  {
    if(EnableFastOnOffCheckBox->Checked==true)
    {
      PrzypiszButton();
      UpdateButton(Timer->Enabled);
    }
    else
    {
      UsunButton();
    }
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

  Ini->WriteBool("UserTune", "EnableN", EnableUserTuneNCheckBox->Checked);
  Ini->WriteInteger("UserTune", "TimeOutN", UserTuneNSpin->Value);
  ChangeUserTune(EnableUserTuneNCheckBox->Checked,UserTuneNSpin->Value);
  Ini->WriteBool("UserTune", "EnableS", EnableUserTuneSCheckBox->Checked);
  if((EnableUserTuneSCheckBox->Checked==false)&&(UserTuneTimer->Enabled==true))
  {
    UserTuneTimer->Enabled=false;
    SetUserTune("");
    opis_UserTuneTMP="";
  }
  if((EnableUserTuneSCheckBox->Checked==true)&&(UserTuneTimer->Enabled==false))
  {
    opis_UserTuneTMP="";
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

	  if(AnsiPos(".avi", opis)>0)
	   opis="";
	  if(AnsiPos(".mpg", opis)>0)
	   opis="";
	  if(AnsiPos(".mpeg", opis)>0)
	   opis="";
	  if(AnsiPos(".rmvb", opis)>0)
	   opis="";

	  if(opis!="")
       i=13;
    }
  }

  if(opis!="")
  {
    //Ucinanie numeru utworu
    aCutSongNumber->Execute();
    //Wycinanie adres�w stron WWW
    if(CutWWWCheck==true)
    {
      aCutWWW->Execute();
      aCutWWWUserTune->Execute();
    }
    //Odtagowywanie opisu na opis w�asciwy
    aSetStatusLooks->Execute();
    //Uciannie spacji z lewej i prawej strony
    opis=opis.Trim();
  }
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::aSetStatusLooksExecute(TObject *Sender)
{
  if(opis!="")
  {
	opis_TMP = StatusMemo->Text.SubString(0, 512);
    //Pobrany utw�r
    IsThere = AnsiPos("CC_TUNESTATUS", opis_TMP);
    if(IsThere!=0)
     opis = StringReplace(opis_TMP, "CC_TUNESTATUS", opis, TReplaceFlags() << rfReplaceAll);
    //Opis pocz�tkowy
    IsThere = AnsiPos("CC_STARTSTATUS", opis);
    if(IsThere!=0)
     opis = StringReplace(opis, "CC_STARTSTATUS", opis_pocz, TReplaceFlags() << rfReplaceAll);
    //Wersja TuneStatus
    IsThere = AnsiPos("CC_PLUGINVERSION", opis);
    if(IsThere!=0)
     opis = StringReplace(opis, "CC_PLUGINVERSION", GetFileVersionInfo(GetPluginDir().c_str(), "FileVersion"), TReplaceFlags() << rfReplaceAll);
    //Wersja AQQ
    IsThere = AnsiPos("CC_AQQVERSION", opis);
    if(IsThere!=0)
     opis = StringReplace(opis, "CC_AQQVERSION", GetFileVersionInfo(NULL, "FileVersion"), TReplaceFlags() << rfReplaceAll);
    //Samplerate
    IsThere = AnsiPos("CC_SAMPLERATE", opis);
    if(IsThere!=0)
    {
      opis = StringReplace(opis, "CC_SAMPLERATE", Samplerate, TReplaceFlags() << rfReplaceAll);
      IsSamplerate=true;
    }
    else IsSamplerate=false;
    //Bitrate
    IsThere = AnsiPos("CC_BITRATE", opis);
    if(IsThere!=0)
    {
      opis = StringReplace(opis, "CC_BITRATE", Bitrate, TReplaceFlags() << rfReplaceAll);
      IsBitrate=true;
    }
    else IsBitrate=false;
    //Channels
    IsThere = AnsiPos("CC_CHANNELS", opis);
    if(IsThere!=0)
    {
      opis = StringReplace(opis, "CC_CHANNELS", Channels, TReplaceFlags() << rfReplaceAll);
      IsChannels=true;
    }
    else IsChannels=false;
    //SongLength
    IsThere = AnsiPos("CC_SONGLENGTH", opis);
    if(IsThere!=0)
    {
      opis = StringReplace(opis, "CC_SONGLENGTH", SongLength, TReplaceFlags() << rfReplaceAll);
      IsSongLength=true;
    }
    else IsSongLength=false;
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::TagsBoxSelect(TObject *Sender)
{
  UnicodeString Tag = TagsBox->Text;
  if(Tag!="--------Tylko dla Winamp/AIMP2/KMPlayer--------")
  {
    Tag.Delete(AnsiPos("(", Tag), Tag.Length() + 1);
    Tag=Tag.Trim();

    int x = AnsiPos(Tag, PreviewStatusMemo->Text);
    if(x==0)
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
    TagsBox->Items->Delete(9);
    TagsBox->Items->Add("Wybierz tag do wstawienia");
    TagsBox->ItemIndex = 9;
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::TagsBoxDropDown(TObject *Sender)
{
  TagsBox->Items->Delete(9);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormClose(TObject *Sender, TCloseAction &Action)
{
  TagsBox->Items->Delete(9);
  aReadSettings->Execute();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::PreviewStatusMemoChange(TObject *Sender)
{
  int x = AnsiPos("CC_TUNESTATUS", PreviewStatusMemo->Text);
  if(x!=0)
  {
    BlockStatus=0;
    if(BlockAuto==0)
     SaveButton->Enabled=true;
  }
  else
  {
    BlockStatus=true;
    SaveButton->Enabled=false;
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::SongTimerTimer(TObject *Sender)
{
  SongTimer->Enabled=false;
  DoDisableSongTimerCheck=false;

  opis_TMP=GetStatus();

  if(opis!=opis_TMP)
  {
    TurnOffTimer->Enabled=false;
    if(AllowChangeStatus(BlockInvisibleCheck)==true)
    {
      SetStatus(opis,!SetOnlyInJabberCheck);
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
  //Sprawdzenie czy to by�o pierwsze uruchomienie Timer'a
  if(JustEnabled==true)
  {
    //Przywr�cenie zapisanych ustawie�
    SongTimer->Interval = IntervalValue;
    JustEnabled=false;
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aiTunesDownExecute(TObject *Sender)
{
  WindowHwnd = FindWindow("iTunes",NULL);

  if(WindowHwnd!=NULL)
  {
    //Ustalanie sciezki iTunes
    if(iTunesStatusPath=="")
	{
      GetWindowThreadProcessId(WindowHwnd, &procesID);
      iTunesStatusPath = GetPathOfProces(procesID);
      iTunesStatusPath = StringReplace(iTunesStatusPath, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
      iTunesStatusPath = ExtractFilePath(iTunesStatusPath) + "TuneStatus.txt";
    }
    //Pobieranie opisu z pliku
    try
    {
      SongFromFile->Lines->LoadFromFile(iTunesStatusPath);
      opis = SongFromFile->Text;
    }
    catch(...) { opis = SongFromFile->Text; }
  }
  else
   opis="";

  opis_UserTune=opis;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aALSongDownExecute(TObject *Sender)
{
  WindowHwnd = FindWindow("ALSongKernelWindow",NULL);

  if(WindowHwnd!=NULL)
  {
    GetWindowTextW(WindowHwnd,this_title,sizeof(this_title));

    opis = this_title;

    //Usuwanie "ALSong"
    IsThere = AnsiPos("ALSong", opis);
    if(IsThere>0)
     opis = "";  
  }
  else
   opis = "";

  opis_UserTune=opis;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aPluginAQQRadioDownExecute(TObject *Sender)
{
  opis="";
  opis=GetAQQRadioSong();

  //Usuwanie "Wtyczka AQQ Radio:"
  IsThere = AnsiPos("Wtyczka AQQ Radio:", opis);
  if (IsThere>0)
  {
    //Symulacja pierwszego uruchomienia SongTimer
    if(DisableSongTimerCheck==true)
	 DoDisableSongTimerCheck=true;

    IsThere = AnsiPos(":", opis);

    opis.Delete(1, IsThere);
    opis=opis.Trim();
    if(CutRadiostationNameCheck==true)
    {
      IsThere = AnsiPos("- ", opis);
      opis.Delete(1, IsThere);
      opis=opis.Trim();
    }
  }
  else
   opis = "";

  if(EnableAQQUserTuneS==false)
   opis_UserTune="";
  else
   opis_UserTune = opis;
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
		   IsThereExe2=true;
        }
      }
      while(Process32Next(Snap , &proces));
    }
    CloseHandle(Snap);

	if(IsThereExe2==true)
	 EnumWindows((WNDENUMPROC)FindSongbird,0);
    else
     opis="";
  }

  if(SongbirdWindowHwnd!=NULL)
  {
	if(GetWindowTextW(SongbirdWindowHwnd,this_title,sizeof(this_title))==0)
	 SongbirdWindowHwnd=NULL;

	opis = this_title;
    opis=opis.Trim();

    IsThere = AnsiPos("[Stopped]", opis);
    if(IsThere>0)
     opis = "";

    IsThere = AnsiPos("Songbird", opis);
    if(IsThere>0)
     opis = "";
  }
  else
   opis="";

  opis_UserTune=opis;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::SongbirdDownloadClick(TObject *Sender)
{
  ShellExecute(NULL, "open", "http://addons.songbirdnest.com/addon/42", NULL, NULL, SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::WMPDownloadClick(TObject *Sender)
{
  ShellExecute(NULL, "open", "http://beherit.pl/Download/WMP-Pluginy.zip", NULL, NULL, SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::iTunesDownloadClick(TObject *Sender)
{
  ShellExecute(NULL, "open", "http://beherit.pl/Download/iTunes-Plugin.zip", NULL, NULL, SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FoobarDownloadClick(TObject *Sender)
{
  ShellExecute(NULL, "open", "http://beherit.pl/Download/Foobar2000-Plugin.zip", NULL, NULL, SW_SHOWNORMAL);
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
           aTunesWindowHwnd = HwndPID(proces.th32ProcessID);
        }
      }
      while(Process32Next(Snap , &proces));
    }
    CloseHandle(Snap);
  }

  if(aTunesWindowHwnd!=NULL)
  {
	if(GetWindowTextW(aTunesWindowHwnd,this_title,sizeof(this_title))==0)
	 aTunesWindowHwnd=NULL;

	opis = this_title;
    opis=opis.Trim();

    //Usuwanie "- aTunes [wersja]""
    IsThere = AnsiPos("- aTunes", opis);
    if(IsThere>0)
    {
      opis.Delete(IsThere, opis.Length() + 1);
      opis=opis.Trim();
    }

    //Usuwanie "aTunes [wersja]"
    IsThere = AnsiPos("aTunes", opis);
    if(IsThere>0)
     opis = "";
  }
  else
   opis="";

  opis_UserTune=opis;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AutoDownCheckListBoxPreviewClick(
      TObject *Sender)
{
  SaveButton->Enabled=false;
  BlockAuto=true;

  for(int i=0;i<=12;i++)
  {
    if(AutoDownCheckListBoxPreview->Checked[i]==true)
    {
      BlockAuto=0;
      if(BlockStatus==0)
       SaveButton->Enabled=true;
    }
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AutoDownCheckListBoxPreviewDragDrop(
      TObject *Sender, TObject *Source, int X, int Y)
{
  if(dynamic_cast<TCheckListBox*>(Source)!=0)
  {
    TPoint Point;
    Point.x=X;
    Point.y=Y;
    if(AutoDownCheckListBoxPreview->ItemAtPos(Point,true)!=-1)
     AutoDownCheckListBoxPreview->Items->Exchange(AutoDownCheckListBoxPreview->ItemIndex,AutoDownCheckListBoxPreview->ItemAtPos(Point,true));
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AutoDownCheckListBoxPreviewDragOver(
      TObject *Sender, TObject *Source, int X, int Y, TDragState State,
      bool &Accept)
{
  if (dynamic_cast<TCheckListBox*>(Source))
   Accept = True;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aCutWWWExecute(TObject *Sender)
{
  while(AnsiPos("http://", opis)>0)
  {
    IsThere = AnsiPos("http://", opis);
    opis2=opis;

    if(IsThere>1)
    {
      opis2.Delete(IsThere,opis2.Length());
      opis2.Delete(1,opis2.Length()-1);
      if(opis2!=" ")
       IsThere--;
    }

    opis2=opis;

    opis2.Delete(1,IsThere-1);
    IsThere = AnsiPos(" ",opis2);
    opis2.Delete(IsThere,opis2.Length());

    IsThere = AnsiPos(opis2,opis);
    opis.Delete(IsThere,opis2.Length());
    opis = StringReplace(opis, "  ", " ", TReplaceFlags() << rfReplaceAll);
    opis=opis.Trim();
  }

  while(AnsiPos("www.", opis)>0)
  {
    IsThere = AnsiPos("www.", opis);
    opis2=opis;

    if(IsThere>1)
    {
      opis2.Delete(IsThere,opis2.Length());
      opis2.Delete(1,opis2.Length()-1);
      if(opis2!=" ")
       IsThere--;
    }

    opis2=opis;

    opis2.Delete(1,IsThere-1);
    IsThere = AnsiPos(" ",opis2);
    opis2.Delete(IsThere,opis2.Length());

    IsThere = AnsiPos(opis2,opis);
    opis.Delete(IsThere,opis2.Length());
    opis = StringReplace(opis, "  ", " ", TReplaceFlags() << rfReplaceAll);
    opis=opis.Trim();
  }

  while(AnsiPos(".com", opis)>0)
  {
    IsThere = AnsiPos(".com", opis);
    opis2=opis;

    opis2.Delete(1,IsThere+3);
    opis2.Delete(2,opis2.Length());
    if(opis2!=" ")
     IsThere++;

    opis2=opis;

    opis2.Delete(IsThere+4,opis2.Length());
    opis2=opis2.Trim();

    while(AnsiPos(" ",opis2)>0)
    {
      IsThere = AnsiPos(" ",opis2);
      opis2.Delete(1,IsThere);
    }

    IsThere = AnsiPos(opis2,opis);
    opis.Delete(IsThere,opis2.Length());
    opis = StringReplace(opis, "  ", " ", TReplaceFlags() << rfReplaceAll);
    opis=opis.Trim();
  }

  while(AnsiPos(".pl", opis)>0)
  {
    IsThere = AnsiPos(".pl", opis);
    opis2=opis;

    opis2.Delete(1,IsThere+2);
    opis2.Delete(2,opis2.Length());
    if(opis2!=" ")
     IsThere++;

    opis2=opis;

    opis2.Delete(IsThere+3,opis2.Length());
    opis2=opis2.Trim();

    while(AnsiPos(" ",opis2)>0)
    {
      IsThere = AnsiPos(" ",opis2);
      opis2.Delete(1,IsThere);
    }

    IsThere = AnsiPos(opis2,opis);
    opis.Delete(IsThere,opis2.Length());
    opis = StringReplace(opis, "  ", " ", TReplaceFlags() << rfReplaceAll);
    opis=opis.Trim();
  }

  while(AnsiPos(".eu", opis)>0)
  {
    IsThere = AnsiPos(".eu", opis);
    opis2=opis;

    opis2.Delete(1,IsThere+2);
    opis2.Delete(2,opis2.Length());
    if(opis2!=" ")
     IsThere++;

    opis2=opis;

    opis2.Delete(IsThere+3,opis2.Length());
    opis2=opis2.Trim();

    while(AnsiPos(" ",opis2)>0)
    {
      IsThere = AnsiPos(" ",opis2);
      opis2.Delete(1,IsThere);
    }

    IsThere = AnsiPos(opis2,opis);
    opis.Delete(IsThere,opis2.Length());
    opis = StringReplace(opis, "  ", " ", TReplaceFlags() << rfReplaceAll);
    opis=opis.Trim();
  }

  while(AnsiPos(".org", opis)>0)
  {
    IsThere = AnsiPos(".org", opis);
    opis2=opis;

    opis2.Delete(1,IsThere+3);
    opis2.Delete(2,opis2.Length());
    if(opis2!=" ")
     IsThere++;

    opis2=opis;

    opis2.Delete(IsThere+4,opis2.Length());
    opis2=opis2.Trim();

    while(AnsiPos(" ",opis2)>0)
    {
      IsThere = AnsiPos(" ",opis2);
      opis2.Delete(1,IsThere);
    }

    IsThere = AnsiPos(opis2,opis);
    opis.Delete(IsThere,opis2.Length());
    opis = StringReplace(opis, "  ", " ", TReplaceFlags() << rfReplaceAll);
    opis=opis.Trim();
  }

  while(AnsiPos(".edu", opis)>0)
  {
    IsThere = AnsiPos(".edu", opis);
    opis2=opis;

    opis2.Delete(1,IsThere+3);
    opis2.Delete(2,opis2.Length());
    if(opis2!=" ")
     IsThere++;

    opis2=opis;

    opis2.Delete(IsThere+4,opis2.Length());
    opis2=opis2.Trim();

    while(AnsiPos(" ",opis2)>0)
    {
      IsThere = AnsiPos(" ",opis2);
      opis2.Delete(1,IsThere);
    }

    IsThere = AnsiPos(opis2,opis);
    opis.Delete(IsThere,opis2.Length());
    opis = StringReplace(opis, "  ", " ", TReplaceFlags() << rfReplaceAll);
    opis=opis.Trim();
  }

  while(AnsiPos(".info", opis)>0)
  {
    IsThere = AnsiPos(".info", opis);
    opis2=opis;

    opis2.Delete(1,IsThere+4);
    opis2.Delete(2,opis2.Length());
    if(opis2!=" ")
     IsThere++;

    opis2=opis;

    opis2.Delete(IsThere+5,opis2.Length());
    opis2=opis2.Trim();

    while(AnsiPos(" ",opis2)>0)
    {
      IsThere = AnsiPos(" ",opis2);
      opis2.Delete(1,IsThere);
    }

    IsThere = AnsiPos(opis2,opis);
    opis.Delete(IsThere,opis2.Length());
    opis = StringReplace(opis, "  ", " ", TReplaceFlags() << rfReplaceAll);
    opis=opis.Trim();
  }

  while(AnsiPos(".net", opis)>0)
  {
    IsThere = AnsiPos(".net", opis);
    opis2=opis;

    opis2.Delete(1,IsThere+3);
    opis2.Delete(2,opis2.Length());
    if(opis2!=" ")
     IsThere++;

    opis2=opis;

    opis2.Delete(IsThere+4,opis2.Length());
    opis2=opis2.Trim();

    while(AnsiPos(" ",opis2)>0)
    {
      IsThere = AnsiPos(" ",opis2);
      opis2.Delete(1,IsThere);
    }

    IsThere = AnsiPos(opis2,opis);
    opis.Delete(IsThere,opis2.Length());
    opis = StringReplace(opis, "  ", " ", TReplaceFlags() << rfReplaceAll);
    opis=opis.Trim();
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
		   IsThereExe3=true;
		}
      }
	  while(Process32Next(Snap , &proces));
	}
	CloseHandle(Snap);

	if(IsThereExe3==true)
	 EnumWindows((WNDENUMPROC)FindScreamerRadio,0);
	else
	 opis="";
  }

  if(ScreamerRadioWindowHwnd!=NULL)
  {
	if(GetWindowTextW(ScreamerRadioWindowHwnd,this_title,sizeof(this_title))==0)
	 ScreamerRadioWindowHwnd=NULL;

    opis = this_title;

	//Usuwanie "Screamer Radio"
	IsThere = AnsiPos("Screamer Radio", opis);
    if(IsThere>0)
     opis = "";
  }
  else
   opis = "";

  opis_UserTune=opis;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::TurnOffTimerTimer(TObject *Sender)
{
  TurnOffTimer->Enabled=false;
  SongTimer->Enabled=false;
  Timer->Enabled=false;
  if(EnableFastOnOffCheckBox->Checked==true)
   UpdateButton(false);
  opisTMP=GetStatus();
  if(opis_pocz!=opisTMP)
  {
    SetStatus(opis_pocz,!SetOnlyInJabberCheck);
    opisTMP="";
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

  if(BlockStatus==false)
   SaveButton->Enabled=true;        
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
	  AutoDownCheckListBoxPreview->Hint="Dla lepszego dzia�ania zaleca si� zainstalowa� wtyczk� do Foobar2000";
      AutoDownCheckListBoxPreview->PopupMenu=FoobarPopupMenu;
      AutoDownCheckListBoxPreview->Cursor=crHelp;
    }
    else if(AutoDownCheckListBoxPreview->Items->Strings[AutoDownCheckListBoxPreview->ItemAtPos(Point,true)]=="Windows Media Player")
    {
      AutoDownCheckListBoxPreview->Hint="Dla wersji WMP powy�ej 7 do ob�sugi potrzebny jest plugin w WMP";
      AutoDownCheckListBoxPreview->PopupMenu=WMPPopupMenu;
      AutoDownCheckListBoxPreview->Cursor=crHelp;
    }
    else if(AutoDownCheckListBoxPreview->Items->Strings[AutoDownCheckListBoxPreview->ItemAtPos(Point,true)]=="iTunes")
    {
      AutoDownCheckListBoxPreview->Hint="Do obs�ugi potrzebny jest plugin w iTunes";
      AutoDownCheckListBoxPreview->PopupMenu=iTunesPopupMenu;
      AutoDownCheckListBoxPreview->Cursor=crHelp;
    }
    else if(AutoDownCheckListBoxPreview->Items->Strings[AutoDownCheckListBoxPreview->ItemAtPos(Point,true)]=="Songbird")
    {
      AutoDownCheckListBoxPreview->Hint="Do obs�ugi potrzebne jest rozszerzenie Birdtitle zainstalowane w Songbird";
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
  aAutoDown->Execute();

  //Uciannie spacji z lewej i prawej strony
  opis_UserTune=opis_UserTune.Trim();

  //Jezeli opis cos zawiera
  if(opis_UserTune!="")
  {
    if(opis_UserTune!=opis_UserTuneTMP)
    {
      UserTuneSongTimer->Enabled=false;
      opis_UserTuneTMP=opis_UserTune;
	  UserTuneSongTimer->Enabled=true;
    }
  }
  //Jak opis jest pusty
  else if(opis_UserTune=="")
  {
    if(opis_UserTuneTMP!="")
    {
      SetUserTune("");
      opis_UserTuneTMP="";
	  UserTuneSongTimer->Enabled=false;
    }
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::UserTuneSongTimerTimer(TObject *Sender)
{
  UserTuneSongTimer->Enabled=false;

  SetUserTune(opis_UserTuneTMP);
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::aCutWWWUserTuneExecute(TObject *Sender)
{
  while(AnsiPos("http://", opis_UserTune)>0)
  {
    IsThere = AnsiPos("http://", opis_UserTune);
    opis2=opis_UserTune;

    if(IsThere>1)
    {
      opis2.Delete(IsThere,opis2.Length());
      opis2.Delete(1,opis2.Length()-1);
      if(opis2!=" ")
       IsThere--;
    }

    opis2=opis_UserTune;

    opis2.Delete(1,IsThere-1);
    IsThere = AnsiPos(" ",opis2);
    opis2.Delete(IsThere,opis2.Length());

    IsThere = AnsiPos(opis2,opis_UserTune);
    opis_UserTune.Delete(IsThere,opis2.Length());
    opis_UserTune = StringReplace(opis_UserTune, "  ", " ", TReplaceFlags() << rfReplaceAll);
    opis_UserTune=opis_UserTune.Trim();
  }

  while(AnsiPos("www.", opis_UserTune)>0)
  {
    IsThere = AnsiPos("www.", opis_UserTune);
    opis2=opis_UserTune;

    if(IsThere>1)
    {
      opis2.Delete(IsThere,opis2.Length());
      opis2.Delete(1,opis2.Length()-1);
      if(opis2!=" ")
       IsThere--;
    }

    opis2=opis_UserTune;

    opis2.Delete(1,IsThere-1);
    IsThere = AnsiPos(" ",opis2);
    opis2.Delete(IsThere,opis2.Length());

    IsThere = AnsiPos(opis2,opis_UserTune);
    opis_UserTune.Delete(IsThere,opis2.Length());
    opis_UserTune = StringReplace(opis_UserTune, "  ", " ", TReplaceFlags() << rfReplaceAll);
    opis_UserTune=opis_UserTune.Trim();
  }

  while(AnsiPos(".com", opis_UserTune)>0)
  {
    IsThere = AnsiPos(".com", opis_UserTune);
    opis2=opis_UserTune;

    opis2.Delete(1,IsThere+3);
    opis2.Delete(2,opis2.Length());
    if(opis2!=" ")
     IsThere++;

    opis2=opis_UserTune;

    opis2.Delete(IsThere+4,opis2.Length());
    opis2=opis2.Trim();

    while(AnsiPos(" ",opis2)>0)
    {
      IsThere = AnsiPos(" ",opis2);
      opis2.Delete(1,IsThere);
    }

    IsThere = AnsiPos(opis2,opis_UserTune);
    opis_UserTune.Delete(IsThere,opis2.Length());
    opis_UserTune = StringReplace(opis_UserTune, "  ", " ", TReplaceFlags() << rfReplaceAll);
    opis_UserTune=opis_UserTune.Trim();
  }

  while(AnsiPos(".pl", opis_UserTune)>0)
  {
    IsThere = AnsiPos(".pl", opis_UserTune);
    opis2=opis_UserTune;

    opis2.Delete(1,IsThere+2);
    opis2.Delete(2,opis2.Length());
    if(opis2!=" ")
     IsThere++;

    opis2=opis_UserTune;

    opis2.Delete(IsThere+3,opis2.Length());
    opis2=opis2.Trim();

    while(AnsiPos(" ",opis2)>0)
    {
      IsThere = AnsiPos(" ",opis2);
      opis2.Delete(1,IsThere);
    }

    IsThere = AnsiPos(opis2,opis_UserTune);
    opis_UserTune.Delete(IsThere,opis2.Length());
    opis_UserTune = StringReplace(opis_UserTune, "  ", " ", TReplaceFlags() << rfReplaceAll);
    opis_UserTune=opis_UserTune.Trim();
  }

  while(AnsiPos(".eu", opis_UserTune)>0)
  {
    IsThere = AnsiPos(".eu", opis_UserTune);
    opis2=opis_UserTune;

    opis2.Delete(1,IsThere+2);
    opis2.Delete(2,opis2.Length());
    if(opis2!=" ")
     IsThere++;

    opis2=opis_UserTune;

    opis2.Delete(IsThere+3,opis2.Length());
    opis2=opis2.Trim();

    while(AnsiPos(" ",opis2)>0)
    {
      IsThere = AnsiPos(" ",opis2);
      opis2.Delete(1,IsThere);
    }

    IsThere = AnsiPos(opis2,opis_UserTune);
    opis_UserTune.Delete(IsThere,opis2.Length());
    opis_UserTune = StringReplace(opis_UserTune, "  ", " ", TReplaceFlags() << rfReplaceAll);
    opis_UserTune=opis_UserTune.Trim();
  }

  while(AnsiPos(".org", opis_UserTune)>0)
  {
    IsThere = AnsiPos(".org", opis_UserTune);
    opis2=opis_UserTune;

    opis2.Delete(1,IsThere+3);
    opis2.Delete(2,opis2.Length());
    if(opis2!=" ")
     IsThere++;

    opis2=opis_UserTune;

    opis2.Delete(IsThere+4,opis2.Length());
    opis2=opis2.Trim();

    while(AnsiPos(" ",opis2)>0)
    {
      IsThere = AnsiPos(" ",opis2);
      opis2.Delete(1,IsThere);
    }

    IsThere = AnsiPos(opis2,opis_UserTune);
    opis_UserTune.Delete(IsThere,opis2.Length());
    opis_UserTune = StringReplace(opis_UserTune, "  ", " ", TReplaceFlags() << rfReplaceAll);
    opis_UserTune=opis_UserTune.Trim();
  }

  while(AnsiPos(".edu", opis_UserTune)>0)
  {
    IsThere = AnsiPos(".edu", opis_UserTune);
    opis2=opis_UserTune;

    opis2.Delete(1,IsThere+3);
    opis2.Delete(2,opis2.Length());
    if(opis2!=" ")
     IsThere++;

    opis2=opis_UserTune;

    opis2.Delete(IsThere+4,opis2.Length());
    opis2=opis2.Trim();

    while(AnsiPos(" ",opis2)>0)
    {
      IsThere = AnsiPos(" ",opis2);
      opis2.Delete(1,IsThere);
    }

    IsThere = AnsiPos(opis2,opis_UserTune);
    opis_UserTune.Delete(IsThere,opis2.Length());
    opis_UserTune = StringReplace(opis_UserTune, "  ", " ", TReplaceFlags() << rfReplaceAll);
    opis_UserTune=opis_UserTune.Trim();
  }

  while(AnsiPos(".info", opis_UserTune)>0)
  {
    IsThere = AnsiPos(".info", opis_UserTune);
    opis2=opis_UserTune;

    opis2.Delete(1,IsThere+4);
    opis2.Delete(2,opis2.Length());
    if(opis2!=" ")
     IsThere++;

    opis2=opis_UserTune;

    opis2.Delete(IsThere+5,opis2.Length());
    opis2=opis2.Trim();

    while(AnsiPos(" ",opis2)>0)
    {
      IsThere = AnsiPos(" ",opis2);
      opis2.Delete(1,IsThere);
    }

    IsThere = AnsiPos(opis2,opis_UserTune);
    opis_UserTune.Delete(IsThere,opis2.Length());
    opis_UserTune = StringReplace(opis_UserTune, "  ", " ", TReplaceFlags() << rfReplaceAll);
    opis_UserTune=opis_UserTune.Trim();
  }

  while(AnsiPos(".net", opis_UserTune)>0)
  {
    IsThere = AnsiPos(".net", opis_UserTune);
    opis2=opis_UserTune;

    opis2.Delete(1,IsThere+3);
    opis2.Delete(2,opis2.Length());
    if(opis2!=" ")
     IsThere++;

    opis2=opis_UserTune;

    opis2.Delete(IsThere+4,opis2.Length());
    opis2=opis2.Trim();

    while(AnsiPos(" ",opis2)>0)
    {
      IsThere = AnsiPos(" ",opis2);
      opis2.Delete(1,IsThere);
    }

    IsThere = AnsiPos(opis2,opis_UserTune);
    opis_UserTune.Delete(IsThere,opis2.Length());
    opis_UserTune = StringReplace(opis_UserTune, "  ", " ", TReplaceFlags() << rfReplaceAll);
    opis_UserTune=opis_UserTune.Trim();
  }        
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AllowUserTuneNTimerTimer(TObject *Sender)
{
  AllowUserTuneNTimer->Enabled=false;
  SetAllowUserTuneNTrue();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::RunPluginCheckBoxClick(TObject *Sender)
{
  if(RunPluginCheckBox->Checked==true)
  {
	opis_pocz = GetStatus();
    SongTimer->Interval=1000;
    JustEnabled=true;
    Timer->Enabled=true;
    if(EnableFastOnOffCheckBox->Checked==true)
     UpdateButton(true);
  }
  else if(RunPluginCheckBox->Checked==false)
  {
    SongTimer->Enabled=false;
    Timer->Enabled=false;
    if(EnableFastOnOffCheckBox->Checked==true)
     UpdateButton(false);
    opisTMP=GetStatus();
    if(opis_pocz!=opisTMP)
    {
      SetStatus(opis_pocz,!SetOnlyInJabberCheck);
      opisTMP="";
    }
  }        
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::TimeTurnOffCheckBoxClick(TObject *Sender)
{
  TimeTurnOffSpin->Enabled=TimeTurnOffCheckBox->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::EnableUserTuneNCheckBoxClick(TObject *Sender)
{
  UserTuneTimeNLabel->Enabled = EnableUserTuneNCheckBox->Checked;
  UserTuneNSpin->Enabled = EnableUserTuneNCheckBox->Checked;        
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::EnableUserTuneSCheckBoxClick(TObject *Sender)
{
  UserTuneTimeSLabel1->Enabled = EnableUserTuneSCheckBox->Checked;
  UserTuneTimeSLabel2->Enabled = EnableUserTuneSCheckBox->Checked;
  UserTuneSSpin->Enabled = EnableUserTuneSCheckBox->Checked;
  EnableAQQUserTuneSCheckBox->Enabled = EnableUserTuneSCheckBox->Checked;        
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::GetStatusTimerTimer(TObject *Sender)
{
  opis_pocz = GetStatus();
  if((opis_pocz!="")||(GetStatusTimerInterval==100))
  {
	GetStatusTimer->Enabled=false;
	Timer->Enabled=true;
	RunPluginCheckBox->Checked=true;
	if(EnableFastOnOffCheckBox->Checked==true)
	 UpdateButton(true);
  }
  else
   GetStatusTimerInterval++;
}
//---------------------------------------------------------------------------

