//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#include <inifiles.hpp>
#include <tlhelp32.h>
#pragma hdrstop
#include "MainFrm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "XPMan"
#pragma link "LMDControl"
#pragma link "LMDCustomBevelPanel"
#pragma link "LMDCustomControl"
#pragma link "LMDCustomPanel"
#pragma link "LMDCustomPanelFill"
#pragma link "LMDCustomParentPanel"
#pragma link "LMDCustomSheetControl"
#pragma link "LMDPageControl"
#pragma link "LMDButtonControl"
#pragma link "LMDCheckBox"
#pragma link "LMDCustomCheckBox"
#pragma link "LMDRadioButton"
#pragma link "LMDBaseEdit"
#pragma link "LMDCustomEdit"
#pragma link "LMDCustomExtSpinEdit"
#pragma link "LMDCustomMaskEdit"
#pragma link "LMDExtSpinEdit"
#pragma link "LMDSpinEdit"
#pragma resource "*.dfm"
TMainForm *MainForm;
//---------------------------------------------------------------------------
__declspec(dllimport)void UstawOpis(AnsiString opis, bool force);
__declspec(dllimport)AnsiString GetPluginUserDir(AnsiString Dir);
__declspec(dllimport)AnsiString GetPluginDir(AnsiString Dir);
__declspec(dllimport)AnsiString PobierzOpis(AnsiString opis);
__declspec(dllimport)AnsiString GetAQQRadioSong(AnsiString Song);
__declspec(dllimport)void PrzypiszButton();
__declspec(dllimport)void UsunButton();
__declspec(dllimport)void UpdateButton(bool OnOff);
//---------------------------------------------------------------------------
AnsiString ePluginDirectory; //Zmiena sciezki
AnsiString opis; //Zmienna opisu
AnsiString Samplerate; //Samplerate piosenki
AnsiString Bitrate; //Bitrate piosenki
AnsiString Channels; //Liczba kana³ów (mono/stereo)
AnsiString SongLength; //Dlugosc piosenki
AnsiString PlayerName; //Zmienna nazwy playera (tag CC_PLAYERNAME)
AnsiString PlayerName_TMP; //Do porównania nazwy playera z nowa pobranym
bool IsSamplerate=0; //Czy w opisie jest tag CC_SAMPLERATE
bool IsBitrate=0; //Czy w opisie jest tag CC_BITRATE
bool IsChannels=0; //Czy w opisie jest tag CC_CHANNELS
bool IsSongLength=0; //Czy w opisie jest tag CC_SONGLENGTH
bool IsPlayerName=0; //Czy w opisie jest tag CC_PLAYERNAME
bool JustEnabled=0; //Do pierwszego uruchomienia SongTimer
AnsiString iTunesStatusPath; //Do sciezki iTunes (TuneStatus.txt)
AnsiString iTunesPlayerPath; //Do sciezka iTunes
bool EnableFastOnOffCheck; //Do pokazywania/ukrywania szybkiego przycisku
bool DisableSongTimerCheck; //Do wy³¹czania SongTimer'a dla AQQ Radio
bool CutRadiostationNameCheck; //Do ucinania nazwy radiostacji z AQQ Radio
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
AnsiString StrToIniStr(const AnsiString Str)
{
 char Buffer[512];
 PChar B, S;

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
String IniStrToStr(const String Str)
{
 char Buffer[512];
 PChar B, S;

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
AnsiString GetPathOfProces(DWORD PID)
{
  HANDLE hSnapshot=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,PID);
  MODULEENTRY32 me32;
  Module32First(hSnapshot,&me32);

  CloseHandle(hSnapshot);

  return me32.szExePath;
}
//---------------------------------------------------------------------------

//Szukanie okna Last.fm
bool CALLBACK EnumWindowsProc(HWND hWnd)
{
  char WindowName[2048], ClassName[2048];
  DWORD procesID;

  GetWindowText(hWnd, WindowName, 2048);
  GetClassName(hWnd, ClassName, 2048);
  GetWindowThreadProcessId(hWnd, &procesID);

  AnsiString ClassNameAnsi = ClassName;
  AnsiString WindowNameAnsi = WindowName;
  AnsiString PlayerPath = GetPathOfProces(procesID);
  
  if(ExtractFileName(PlayerPath)=="LastFM.exe")
  {
    PlayerName="Last.fm";

    if(ClassNameAnsi=="QWidget")
    {
      if(
         (WindowNameAnsi!="LastFM")
         &&(WindowNameAnsi!="Last.fm")
         &&(WindowNameAnsi!="Diagnostyka")
         &&(WindowNameAnsi!="Poleæ")
         &&(WindowNameAnsi!="Zaloguj")
         &&(WindowNameAnsi!="Kreator automatycznej aktualizacji")
         &&(WindowNameAnsi!="Kreator ustawieñ")
         &&(WindowNameAnsi!="Opcje programu Last.fm")
         &&(WindowNameAnsi!="Dodaj u¿ytkownika")
         &&(WindowNameAnsi!="Aktualne")
         &&(WindowNameAnsi!="Last.fm — Informacje")
         &&(WindowNameAnsi!="")
        )
      {
        opis = WindowNameAnsi;
      }
    }
  }
  return true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aWinampDownExecute(TObject *Sender)
{
  HWND hwndWinamp = FindWindow("Winamp v1.x",NULL);
  if(!hwndWinamp)
   hwndWinamp = FindWindow("Studio",NULL);

  if(hwndWinamp!=NULL)
  {
    //Informacje o Playerze
    if(IsPlayerName==1)
    {
      DWORD procesID;
      GetWindowThreadProcessId(hwndWinamp, &procesID);
      AnsiString PlayerPath = GetPathOfProces(procesID);
      PlayerPath = StringReplace(PlayerPath, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);

      PlayerName = GetFileVersionInfo(PlayerPath.c_str(),"ProductName");
      if(PlayerName=="") PlayerName = PlayerName_TMP;
      else PlayerName_TMP = PlayerName;
    }

    char this_title[2048];
    GetWindowText(hwndWinamp,this_title,sizeof(this_title));

    opis = this_title;

    //Usuwanie "- Winamp"
    int x = AnsiPos("- Winamp", opis);
    int y = opis.Length();
    if (x>0)
    {
      opis.Delete(x, y + 1);
      opis=opis.Trim();
    }

    //Usuwanie "*** "
    x = AnsiPos("*** ", opis);
    if(x>0)
    {
      opis.Delete(1, x + 3);
      opis=opis.Trim();
    }

    //Usuwanie "Winamp"
    x = AnsiPos("Winamp", opis);
    if(x>0)
     opis = "";

    //Winamp STOP?
    int res = SendMessage(hwndWinamp,WM_USER,0,104);
    if(res==0)
     opis = "";

    //Samplerate
    if(IsSamplerate==1)
    {
      res = SendMessage(hwndWinamp, WM_USER, 0, 126);
      Samplerate = IntToStr(res);
      if(Samplerate.Length()<=2)
       Samplerate = Samplerate + "KHz";
      else if(Samplerate.Length()>2)
       Samplerate = Samplerate + "Hz";
    }

    //Bitrate
    if(IsBitrate==1)
    {
      res = SendMessage(hwndWinamp, WM_USER, 1, 126);
      Bitrate = IntToStr(res) + "kbps";
    }

    //Channels
    if(IsChannels==1)
    {
      res = SendMessage(hwndWinamp, WM_USER, 2, 126);
      if(res==1)
       Channels = "Mono";
      else if(res==2)
       Channels = "Stereo";
      else
       Channels = res;
    }

    //SongLength
    if(IsSongLength==1)
    {
     res = SendMessage(hwndWinamp,WM_USER,1,105);
      if(res!=-1)
      {
        int sekundy = res % 60;
        res = res - sekundy;
        int minuty = res / 60;
        if(sekundy<10)
         SongLength = IntToStr(minuty) + ":0" + IntToStr(sekundy);
        else
         SongLength = IntToStr(minuty) + ":" + IntToStr(sekundy);
      }
    }
  }
  else
   opis = "";
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aFoobarDownExecute(TObject *Sender)
{
  HWND hwndFoobar = FindWindow("Foobar_TuneStatus",NULL);
  if(!hwndFoobar) hwndFoobar = FindWindow("PanelsUI",NULL);
  if(!hwndFoobar) hwndFoobar = FindWindow("{DA7CD0DE-1602-45e6-89A1-C2CA151E008E}/1",NULL);
  if(!hwndFoobar) hwndFoobar = FindWindow("{DA7CD0DE-1602-45e6-89A1-C2CA151E008E}",NULL);
  if(!hwndFoobar) hwndFoobar = FindWindow("{DA7CD0DE-1602-45e6-89A1-C2CA151E008E}",NULL);
  if(!hwndFoobar) hwndFoobar = FindWindow("{97E27FAA-C0B3-4b8e-A693-ED7881E99FC1}",NULL);
  if(!hwndFoobar) hwndFoobar = FindWindow("{E7076D1C-A7BF-4f39-B771-BCBE88F2A2A8}",NULL);
                                                           
  if(hwndFoobar!=NULL)
  {
    //Informacje o Playerze
    if(IsPlayerName==1)
    {
      DWORD procesID;
      GetWindowThreadProcessId(hwndFoobar, &procesID);
      AnsiString PlayerPath = GetPathOfProces(procesID);
      PlayerPath = StringReplace(PlayerPath, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);

      PlayerName = GetFileVersionInfo(PlayerPath.c_str(),"ProductName");
      if(PlayerName=="") PlayerName = PlayerName_TMP;
      else PlayerName_TMP = PlayerName;
    } 

    char this_title[2048];
    GetWindowText(hwndFoobar,this_title,sizeof(this_title));

    opis = this_title;

    //Usuwanie "[foobar2000"
    int x = AnsiPos("[foobar2000", opis);
    int y = opis.Length();
    if(x>0)
    {
      opis.Delete(x, y + 1);
      opis=opis.Trim();
    }

    //Usuwanie "foobar2000"
    y = opis.Length();
    x = AnsiPos("foobar2000", opis);
    if(x>0)
    {
      opis.Delete(x, y + 1);
      opis=opis.Trim();
    }

    //Usuwanie "fooAvA"
    x = AnsiPos("fooAvA", opis);
    if(x>0)
     opis = "";

    //"Foo AvA" - Stopped
    x = AnsiPos("Foo AvA", opis);
    if(x>0)
     opis = "";
  }
  else
   opis = "";
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aMPCDownExecute(TObject *Sender)
{
  HWND hwndMPC = FindWindow("MediaPlayerClassicW",NULL);

  if(hwndMPC!=NULL)
  {
    //Informacje o Playerze
    if(IsPlayerName==1)
    {
      DWORD procesID;
      GetWindowThreadProcessId(hwndMPC, &procesID);
      AnsiString PlayerPath = GetPathOfProces(procesID);
      PlayerPath = StringReplace(PlayerPath, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);

      PlayerName = GetFileVersionInfo(PlayerPath.c_str(),"ProductName");
      if(PlayerName=="") PlayerName = PlayerName_TMP;
      else PlayerName_TMP = PlayerName;
    }

    char this_title[2048];
    GetWindowText(hwndMPC,this_title,sizeof(this_title));

    opis = this_title;

    //Usuwanie "- Media Player Classic"
    int x = AnsiPos("- Media Player Classic", opis);
    int y = opis.Length();
    if(x>0)
    {
      opis.Delete(x, y + 1);
    opis=opis.Trim();
    }
  }
  else
   opis = "";
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aLastFMDownExecute(TObject *Sender)
{
  opis = "";
  EnumWindows((WNDENUMPROC)EnumWindowsProc,0);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aWMPDownExecute(TObject *Sender)
{
  HWND hwndWMP = FindWindow("WMPlayerApp",NULL);
  if(!hwndWMP) hwndWMP = FindWindow("Media Player 2",NULL);

  if(hwndWMP!=NULL)
  {
    //Informacje o Playerze
    if(IsPlayerName==1)
    {
      DWORD procesID;
      GetWindowThreadProcessId(hwndWMP, &procesID);
      AnsiString PlayerPath = GetPathOfProces(procesID);
      PlayerPath = StringReplace(PlayerPath, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);

      PlayerName = GetFileVersionInfo(PlayerPath.c_str(),"ProductName");
      if(PlayerName=="") PlayerName = PlayerName_TMP;
      else PlayerName_TMP = PlayerName;
    }

    char this_title[2048];
    GetWindowText(hwndWMP,this_title,sizeof(this_title));

    opis = this_title;

    //Usuwanie "- Windows Media Player""
    int x = AnsiPos("- Windows Media Player", opis);
    int y = opis.Length();
    if(x>0)
    {
      opis.Delete(x, y + 1);
      opis=opis.Trim();
    }

    //Usuwanie "Windows Media Player"
    if(opis=="Windows Media Player")
     opis = "";
  }
  else
   opis = "";
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::TimerTimer(TObject *Sender)
{
  //Reset zmiennych - POTRZEBNE!
  Samplerate="";
  Bitrate="";
  Channels="";
  SongLength="";
  PlayerName="";

  //Pobieranie tekstu okien
  if(WinampDownRadio->Checked==true)
   aWinampDown->Execute();
  else if(FoobarDownRadio->Checked==true)
   aFoobarDown->Execute();
  else if(LastFMDownRadio->Checked==true)
   aLastFMDown->Execute();
  else if(WMPDownRadio->Checked==true)
   aWMPDown->Execute();
  else if(VUPlayerDownRadio->Checked==true)
   aVUPlayerDown->Execute();
  else if(XMPlayDownRadio->Checked==true)
   aXMPlayDown->Execute();
  else if(MPCDownRadio->Checked==true)
   aMPCDown->Execute();
  else if(iTunesDownRadio->Checked==true)
   aiTunesDown->Execute();
  else if(ALSongDownRadio->Checked==true)
   aALSongDown->Execute();
  else if(PluginAQQRadioDownRadio->Checked==true)
   aPluginAQQRadioDown->Execute();
  else if(AutoDownRadio->Checked==true)
   aAutoDown->Execute();

  //Ucinanie numeru utworu
  aCutSongNumber->Execute();
  //Odtagowywanie opisu na opis w³asciwy
  aSetStatusLooks->Execute();

  //Uciannie spacji z lewej i prawej strony
  opis=opis.Trim();

  //Jezeli opis cos zawiera
  if(opis!="")
  {
    if(opis!=opisTMP)
    {
      SongTimer->Enabled=false;
      SongTimer->Enabled=true;
      opisTMP=opis;
    }
  }
  //Jak opis jest pusty
  else
  {
    if(opis_pocz!=opisTMP)
    {
      opisTMP=opis_pocz;
      UstawOpis(opis_pocz,!SetOnlyInJabberCheck);
      //Symulacja pierwszego uruchomienia SongTimer
      SongTimer->Enabled=false;
      SongTimer->Interval=1000;
      JustEnabled=1;
      SongTimer->Enabled=true;
    }
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::SaveButtonClick(TObject *Sender)
{
  //Zapis ustawien
  aSaveSettings->Execute();
  //Ustawianie Timer'a
  SongTimer->Interval=(1000*(SongTimerSpin->Value));
  IntervalValue=(1000*(SongTimerSpin->Value));
  //Wymuszenie natychmiastowego ustawienienia w opisie dokonanych zmian
  if(Timer->Enabled==true)
  {
    SongTimer->Enabled=false;
    JustEnabled=1;
    SongTimer->Interval=1000;
    SongTimer->Enabled=true;
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
  HWND hwndVUPlayer = FindWindow("VUPlayerClass",NULL);

  if(hwndVUPlayer!=NULL)
  {
    //Informacje o Playerze
    if(IsPlayerName==1)
    {
      DWORD procesID;
      GetWindowThreadProcessId(hwndVUPlayer, &procesID);
      AnsiString PlayerPath = GetPathOfProces(procesID);
      PlayerPath = StringReplace(PlayerPath, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);

      PlayerName = GetFileVersionInfo(PlayerPath.c_str(),"ProductName");
      if(PlayerName=="") PlayerName = PlayerName_TMP;
      else PlayerName_TMP = PlayerName;
    }

    char this_title[2048];
    GetWindowText(hwndVUPlayer,this_title,sizeof(this_title));

    opis = this_title;

    //Usuwanie " ["
    int x = AnsiPos(" [", opis);
    int y = opis.Length();
    if(x>0)
    {
      opis.Delete(x, y + 1);
      opis=opis.Trim();
    }
  }
  else
   opis = "";
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aXMPlayDownExecute(TObject *Sender)
{
  HWND hwndXMPlay = FindWindow("XMPLAY-MAIN",NULL);

  if(hwndXMPlay!=NULL)
  {
    //Informacje o Playerze
    if(IsPlayerName==1)
    {
      DWORD procesID;
      GetWindowThreadProcessId(hwndXMPlay, &procesID);
      AnsiString PlayerPath = GetPathOfProces(procesID);
      PlayerPath = StringReplace(PlayerPath, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);

      PlayerName = GetFileVersionInfo(PlayerPath.c_str(),"ProductName");
      if(PlayerName=="") PlayerName = PlayerName_TMP;
      else PlayerName_TMP = PlayerName;
    }

    char this_title[2048];
    GetWindowText(hwndXMPlay,this_title,sizeof(this_title));

    opis = this_title;

    //Usuwanie "XMPlay"
    int x = AnsiPos("XMPlay", opis);
    if(x>0)
     opis = "";

    if(hwndXMPlay!=NULL)
    {
      int res = SendMessage(hwndXMPlay,WM_USER,0,104);
      if(res==0)
       opis = "";
    }
  }
  else
   opis = "";
}
//---------------------------------------------------------------------------

//Sprawdzanie czy w danym tekscie s¹ tylko liczby
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
  int x = AnsiPos(". ", opis);
  if((x>0)&&(x<6))
  {
    AnsiString opis2=opis;
    opis2.Delete(x, opis2.Length());
    if(TestDigit(opis2)==true)
    {
      opis.Delete(1, x);
      opis=opis.Trim();
    }
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aReadSettingsExecute(TObject *Sender)
{
  ePluginDirectory = GetPluginUserDir(ePluginDirectory);

  TIniFile *Ini = new TIniFile(ePluginDirectory + "\\\\TuneStatus\\\\TuneStatus.ini");

  AnsiString Boxy = Ini->ReadString("Settings", "Box", "Auto");
  if(Boxy=="Winamp")
   WinampDownRadio->Checked=true;
  else if(Boxy=="Foobar")
   FoobarDownRadio->Checked=true;
  else if(Boxy=="Lastfm")
   LastFMDownRadio->Checked=true;
  else if(Boxy=="WMP")
   WMPDownRadio->Checked=true;
  else if(Boxy=="VUPlayer")
   VUPlayerDownRadio->Checked=true;
  else if(Boxy=="XMPlay")
   XMPlayDownRadio->Checked=true;
  else if(Boxy=="MPC")
   MPCDownRadio->Checked=true;
  else if(Boxy=="iTunes")
   iTunesDownRadio->Checked=true;
  else if(Boxy=="ALSong")
   ALSongDownRadio->Checked=true;
  else if(Boxy=="AQQRadio")
   PluginAQQRadioDownRadio->Checked=true;
  else if(Boxy=="Auto")
   AutoDownRadio->Checked=true;

  String Status = IniStrToStr(Ini->ReadString("Settings", "Status", ""));
  if(Status!="")
   StatusMemo->Text = Status;

  SetOnlyInJabberCheckBox->Checked = Ini->ReadBool("Settings", "SetOnlyInJabber", 0);
  SetOnlyInJabberCheck = Ini->ReadBool("Settings", "SetOnlyInJabber", 0);

  EnablePluginOnStartCheckBox->Checked = Ini->ReadBool("Settings", "EnablePluginOnStart", 0);
  
  EnableFastOnOffCheckBox->Checked = Ini->ReadBool("Settings", "EnableFastOnOff", 1);
  EnableFastOnOffCheck = Ini->ReadBool("Settings", "EnableFastOnOff", 1);

  SongTimer->Interval = 1000*(Ini->ReadInteger("Settings", "SongTimerInterval", 5));
  IntervalValue = 1000*(Ini->ReadInteger("Settings", "SongTimerInterval", 5));
  SongTimerSpin->Value = Ini->ReadInteger("Settings", "SongTimerInterval", 5);

  DisableSongTimerCheckBox->Checked = Ini->ReadBool("Settings", "DisableSongTimer", 1);
  DisableSongTimerCheck = Ini->ReadBool("Settings", "DisableSongTimer", 1);

  CutRadiostationNameCheckBox->Checked = Ini->ReadBool("Settings", "CutRadiostationName", 1);
  CutRadiostationNameCheck = Ini->ReadBool("Settings", "CutRadiostationName", 1);

  delete Ini;

  PreviewStatusMemo->Text=StatusMemo->Text;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aSaveSettingsExecute(TObject *Sender)
{
  StatusMemo->Text=PreviewStatusMemo->Text;

  ePluginDirectory = GetPluginUserDir(ePluginDirectory);

  TIniFile *Ini = new TIniFile(ePluginDirectory + "\\\\TuneStatus\\\\TuneStatus.ini");

  if(WinampDownRadio->Checked==true)
   Ini->WriteString("Settings", "Box", "Winamp");
  if(FoobarDownRadio->Checked==true)
   Ini->WriteString("Settings", "Box", "Foobar");
  if(LastFMDownRadio->Checked==true)
   Ini->WriteString("Settings", "Box", "Lastfm");
  if(WMPDownRadio->Checked==true)
   Ini->WriteString("Settings", "Box", "WMP");
  if(VUPlayerDownRadio->Checked==true)
   Ini->WriteString("Settings", "Box", "VUPlayer");
  if(XMPlayDownRadio->Checked==true)
   Ini->WriteString("Settings", "Box", "XMPlay");
  if(MPCDownRadio->Checked==true)
   Ini->WriteString("Settings", "Box", "MPC");
  if(iTunesDownRadio->Checked==true)
   Ini->WriteString("Settings", "Box", "iTunes");
  if(ALSongDownRadio->Checked==true)
   Ini->WriteString("Settings", "Box", "ALSong");
  if(PluginAQQRadioDownRadio->Checked==true)
   Ini->WriteString("Settings", "Box", "AQQRadio");
  if(AutoDownRadio->Checked==true)
   Ini->WriteString("Settings", "Box", "Auto");

  Ini->WriteString("Settings", "Status", StrToIniStr(StatusMemo->Text.SubString(0, 2047)).TrimRight());

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
          UstawOpis(opis_pocz,SetOnlyInJabberCheck);
        }
        if(opis!="")
        {
          if(opis!=opisTMP)
          {
            opisTMP=opis;
            UstawOpis(opis,!SetOnlyInJabberCheck);
          }
        }
      }
      else //wszystkie
      {
        opisTMP=opis;
        UstawOpis(opis,!SetOnlyInJabberCheck);
      }
    }
  }

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

  delete Ini;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aAutoDownExecute(TObject *Sender)
{
  aWinampDown->Execute();
  if(opis=="")
   aFoobarDown->Execute();
  if(opis=="")
   aWMPDown->Execute();
  if(opis=="")
   aVUPlayerDown->Execute();
  if(opis=="")
   aXMPlayDown->Execute();
  if(opis=="")
   aMPCDown->Execute();
  if(opis=="")
   aiTunesDown->Execute();
  if(opis=="")
   aALSongDown->Execute();
  if(opis=="")
   aPluginAQQRadioDown->Execute();
  if(opis=="")
   aLastFMDown->Execute();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::RunPluginCheckBoxChange(TObject *Sender)
{
  if(RunPluginCheckBox->Checked==true)
  {
    opis_pocz = PobierzOpis(opis_pocz);
    SongTimer->Interval=1000;
    JustEnabled=1;
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
    opisTMP=PobierzOpis(opisTMP);
    if(opis_pocz!=opisTMP)
    {
      UstawOpis(opis_pocz,!SetOnlyInJabberCheck);
      opisTMP="";
    }
  }        
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aSetStatusLooksExecute(TObject *Sender)
{
  if(opis!="")
  {
    AnsiString opis_TMP= StatusMemo->Text.SubString(0, 512);
    //Pobrany utwór
    int x = AnsiPos("CC_TUNESTATUS", opis_TMP);
    if(x!=0)
     opis = StringReplace(opis_TMP, "CC_TUNESTATUS", opis, TReplaceFlags() << rfReplaceAll);
    //Opis pocz¹tkowy
    x = AnsiPos("CC_STARTSTATUS", opis);
    if(x!=0)
     opis = StringReplace(opis, "CC_STARTSTATUS", opis_pocz, TReplaceFlags() << rfReplaceAll);
    //Wersja TuneStatus
    x = AnsiPos("CC_PLUGINVERSION", opis);
    if(x!=0)
     opis = StringReplace(opis, "CC_PLUGINVERSION", GetFileVersionInfo(GetPluginDir(ePluginDirectory).c_str(), "FileVersion"), TReplaceFlags() << rfReplaceAll);
    //Wersja AQQ
    x = AnsiPos("CC_AQQVERSION", opis);
    if(x!=0)
     opis = StringReplace(opis, "CC_AQQVERSION", GetFileVersionInfo(NULL, "FileVersion"), TReplaceFlags() << rfReplaceAll);
    //PlayerName
    x = AnsiPos("CC_PLAYERNAME", opis);
    if(x!=0)
    {
      opis = StringReplace(opis, "CC_PLAYERNAME", PlayerName, TReplaceFlags() << rfReplaceAll);
      IsPlayerName=1;
    }
    else IsPlayerName=0;
    //Samplerate
    x = AnsiPos("CC_SAMPLERATE", opis);
    if(x!=0)
    {
      opis = StringReplace(opis, "CC_SAMPLERATE", Samplerate, TReplaceFlags() << rfReplaceAll);
      IsSamplerate=1;
    }
    else IsSamplerate=0;
    //Bitrate
    x = AnsiPos("CC_BITRATE", opis);
    if(x!=0)
    {
      opis = StringReplace(opis, "CC_BITRATE", Bitrate, TReplaceFlags() << rfReplaceAll);
      IsBitrate=1;
    }
    else IsBitrate=0;
    //Channels
    x = AnsiPos("CC_CHANNELS", opis);
    if(x!=0)
    {
      opis = StringReplace(opis, "CC_CHANNELS", Channels, TReplaceFlags() << rfReplaceAll);
      IsChannels=1;
    }
    else IsChannels=0;
    //SongLength
    x = AnsiPos("CC_SONGLENGTH", opis);
    if(x!=0)
    {
      opis = StringReplace(opis, "CC_SONGLENGTH", SongLength, TReplaceFlags() << rfReplaceAll);
      IsSongLength=1;
    }
    else IsSongLength=0;
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::TagsBoxSelect(TObject *Sender)
{
  AnsiString Tag = TagsBox->Text;
  Tag.Delete(AnsiPos("(", Tag), Tag.Length() + 1);
  Tag=Tag.Trim();

  int x = AnsiPos(Tag, PreviewStatusMemo->Text);
  if(x==0)
  {
    AnsiString Before,Fore;

    Before=PreviewStatusMemo->Text.SubString(0,PreviewStatusMemo->SelStart);
    Fore=PreviewStatusMemo->Text.SubString(PreviewStatusMemo->SelStart+1,(PreviewStatusMemo->Text).Length());
    PreviewStatusMemo->Clear();

    PreviewStatusMemo->Text=Before+Tag+Fore;
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
   SaveButton->Enabled=true;
  else
   SaveButton->Enabled=false;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::SongTimerTimer(TObject *Sender)
{
  SongTimer->Enabled=false;
  if(opis==opisTMP)
   UstawOpis(opis,!SetOnlyInJabberCheck);
  //Sprawdzenie czy to by³o pierwsze uruchomienie Timer'a
  if(JustEnabled==1)
  {
    //Przywrócenie zapisanych ustawieñ
    SongTimer->Interval = IntervalValue;
    JustEnabled=0;
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aiTunesDownExecute(TObject *Sender)
{
  HWND hwndiTunes = FindWindow("iTunes",NULL);

  if(hwndiTunes!=NULL)
  {
    //Ustalanie sciezki iTunes
    if(iTunesStatusPath=="")
    {
      DWORD procesID;
      GetWindowThreadProcessId(hwndiTunes, &procesID);
      iTunesStatusPath = GetPathOfProces(procesID);
      iTunesStatusPath = StringReplace(iTunesStatusPath, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
      iTunesStatusPath = ExtractFilePath(iTunesStatusPath) + "TuneStatus.txt";
    }
    if(IsPlayerName==1)
    {
      //Ustalanie sciezki iTunes
      if(iTunesPlayerPath=="")
      {
        DWORD procesID;
        GetWindowThreadProcessId(hwndiTunes, &procesID);
        iTunesPlayerPath = GetPathOfProces(procesID);
        iTunesPlayerPath = StringReplace(iTunesPlayerPath, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
      }
      //Pobieranie iformacji o playerze
      PlayerName = GetFileVersionInfo(iTunesPlayerPath.c_str(),"ProductName");
      if(PlayerName=="") PlayerName = PlayerName_TMP;
      else PlayerName_TMP = PlayerName;
    }
    //Pobieranie opisu z pliku
    try
    {
      SongFromFile->Lines->LoadFromFile(iTunesStatusPath);
      opis = SongFromFile->Text;
    }
    catch(...) { opis = SongFromFile->Text; }
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aALSongDownExecute(TObject *Sender)
{
  HWND hwndALSong = FindWindow("ALSongKernelWindow",NULL);

  if(hwndALSong!=NULL)
  {
    //Informacje o Playerze
    if(IsPlayerName==1)
    {
      DWORD procesID;
      GetWindowThreadProcessId(hwndALSong, &procesID);
      AnsiString PlayerPath = GetPathOfProces(procesID);
      PlayerPath = StringReplace(PlayerPath, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);

      PlayerName = GetFileVersionInfo(PlayerPath.c_str(),"ProductName");
      if(PlayerName=="") PlayerName = PlayerName_TMP;
      else PlayerName_TMP = PlayerName;
    } 

    char this_title[2048];
    GetWindowText(hwndALSong,this_title,sizeof(this_title));

    opis = this_title;

    //Usuwanie "ALSong"
    int x = AnsiPos("ALSong", opis);
    if(x>0)
     opis = "";

  }
  else
   opis = "";        
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aPluginAQQRadioDownExecute(TObject *Sender)
{
  opis=GetAQQRadioSong(opis);

  //Usuwanie "Wtyczka AQQ Radio:"
  int x = AnsiPos("Wtyczka AQQ Radio:", opis);
  if (x>0)
  {
    x = AnsiPos(":", opis);

    //Symulacja pierwszego uruchomienia SongTimer
    if(DisableSongTimerCheck==true)
    {
      SongTimer->Enabled=false;
      SongTimer->Interval=1000;
      JustEnabled=1;
      SongTimer->Enabled=true;
    }

    PlayerName = "Wtyczka AQQ Radio";
    opis.Delete(1, x);
    opis=opis.Trim();
    if(CutRadiostationNameCheck==1)
    {
      x = AnsiPos("- ", opis);
      opis.Delete(1, x);
      opis=opis.Trim();
    }
  }
  else
   opis = "";
}
//---------------------------------------------------------------------------



