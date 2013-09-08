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
AnsiString opis_TMP; //Zmienna opisu (tymczasowa)
AnsiString opis2; //j.w.

int res; //Do pobierania nizej wymienionych danych
AnsiString Samplerate; //Samplerate piosenki
AnsiString Bitrate; //Bitrate piosenki
AnsiString Channels; //Liczba kana³ów (mono/stereo)
AnsiString SongLength; //Dlugosc piosenki
int sekundy; //j.w.
int minuty; //j.w.
bool IsSamplerate=0; //Czy w opisie jest tag CC_SAMPLERATE
bool IsBitrate=0; //Czy w opisie jest tag CC_BITRATE
bool IsChannels=0; //Czy w opisie jest tag CC_CHANNELS
bool IsSongLength=0; //Czy w opisie jest tag CC_SONGLENGTH

bool JustEnabled=0; //Do pierwszego uruchomienia SongTimer
AnsiString iTunesStatusPath; //Do sciezki iTunes (TuneStatus.txt)
AnsiString iTunesPlayerPath; //Do sciezka iTunes
AnsiString PlayerPath; //Do sciezka Lastfm/Songbird
bool EnableFastOnOffCheck; //Do pokazywania/ukrywania szybkiego przycisku
bool DisableSongTimerCheck; //Do wy³¹czania SongTimer'a dla AQQ Radio
bool CutRadiostationNameCheck; //Do ucinania nazwy radiostacji z AQQ Radio

//Do trybu automatycznego
AnsiString Player;
bool PlayerCheck;

HWND WindowHwnd; //Zmienna uchwytu do okien
HWND LastfmWindowHwnd; //j.w.
HWND SongbirdWindowHwnd; //j.w.
char this_title[2048]; //Do pobierania tekstu okna
char WindowName[2048], ClassName[2048]; //j.w. ale dla Lastfm/Songbird

AnsiString ClassNameAnsi;
AnsiString WindowNameAnsi;

int IsThere; //Do sprawdzania czy dany ciag char/AnsiString jest w innym ciagu
int IsThereLength; //j.w. ale dlugosc

DWORD procesID; //do PID
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
bool CALLBACK FindLastfm(HWND hWnd)
{
  GetWindowText(hWnd, WindowName, 2048);
  GetClassName(hWnd, ClassName, 2048);
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
        opis=WindowNameAnsi;
        LastfmWindowHwnd = hWnd;
      }
    }
  }
  return true;
}
//---------------------------------------------------------------------------

//Szukanie okna Last.fm
bool CALLBACK FindSongbird(HWND hWnd)
{
  GetWindowText(hWnd, WindowName, 2048);
  GetClassName(hWnd, ClassName, 2048);
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
         &&(WindowNameAnsi!="Songbird")
         &&(WindowNameAnsi!="")
        )
      {
        SongbirdWindowHwnd=hWnd;

        opis=WindowNameAnsi;
        opis=opis.Trim();

        IsThere = AnsiPos("[Stopped]", opis);
        if(IsThere>0)
         opis = "";
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
    GetWindowText(WindowHwnd,this_title,sizeof(this_title));

    opis = this_title;

    //Usuwanie "- Winamp"
    IsThere = AnsiPos("- Winamp", opis);
    IsThereLength = opis.Length();
    if (IsThere>0)
    {
      opis.Delete(IsThere, IsThereLength + 1);
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
    if(IsSamplerate==1)
    {
      res = SendMessage(WindowHwnd, WM_USER, 0, 126);
      Samplerate = IntToStr(res);
      if(Samplerate.Length()<=2)
       Samplerate = Samplerate + "KHz";
      else if(Samplerate.Length()>2)
       Samplerate = Samplerate + "Hz";
    }

    //Bitrate
    if(IsBitrate==1)
    {
      res = SendMessage(WindowHwnd, WM_USER, 1, 126);
      Bitrate = IntToStr(res) + "kbps";
    }

    //Channels
    if(IsChannels==1)
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
    if(IsSongLength==1)
    {
     res = SendMessage(WindowHwnd,WM_USER,1,105);
      if(res!=-1)
      {
        sekundy = res % 60;
        res = res - sekundy;
        minuty = res / 60;
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
  WindowHwnd = FindWindow("Foobar_TuneStatus",NULL);
  if(!WindowHwnd) WindowHwnd = FindWindow("PanelsUI",NULL);
  if(!WindowHwnd) WindowHwnd = FindWindow("{DA7CD0DE-1602-45e6-89A1-C2CA151E008E}/1",NULL);
  if(!WindowHwnd) WindowHwnd = FindWindow("{DA7CD0DE-1602-45e6-89A1-C2CA151E008E}",NULL);
  if(!WindowHwnd) WindowHwnd = FindWindow("{DA7CD0DE-1602-45e6-89A1-C2CA151E008E}",NULL);
  if(!WindowHwnd) WindowHwnd = FindWindow("{97E27FAA-C0B3-4b8e-A693-ED7881E99FC1}",NULL);
  if(!WindowHwnd) WindowHwnd = FindWindow("{E7076D1C-A7BF-4f39-B771-BCBE88F2A2A8}",NULL);
                                                           
  if(WindowHwnd!=NULL)
  {
    GetWindowText(WindowHwnd,this_title,sizeof(this_title));

    opis = this_title;

    //Usuwanie "[foobar2000"
    IsThere = AnsiPos("[foobar2000", opis);
    IsThereLength = opis.Length();
    if(IsThere>0)
    {
      opis.Delete(IsThere, IsThereLength + 1);
      opis=opis.Trim();
    }

    //Usuwanie "foobar2000"
    IsThereLength = opis.Length();
    IsThere = AnsiPos("foobar2000", opis);
    if(IsThere>0)
    {
      opis.Delete(IsThere, IsThereLength + 1);
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
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aMPCDownExecute(TObject *Sender)
{
  WindowHwnd = FindWindow("MediaPlayerClassicW",NULL);

  if(WindowHwnd!=NULL)
  {
    GetWindowText(WindowHwnd,this_title,sizeof(this_title));

    opis = this_title;

    //Usuwanie "- Media Player Classic"
    IsThere = AnsiPos("- Media Player Classic", opis);
    IsThereLength = opis.Length();
    if(IsThere>0)
    {
      opis.Delete(IsThere, IsThereLength + 1);
    opis=opis.Trim();
    }
  }
  else
   opis = "";
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aLastFMDownExecute(TObject *Sender)
{
  if(LastfmWindowHwnd==NULL)
   EnumWindows((WNDENUMPROC)FindLastfm,0);
  else
  {
    GetWindowText(LastfmWindowHwnd,this_title,sizeof(this_title));
    opis = this_title;
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aWMPDownExecute(TObject *Sender)
{
  WindowHwnd = FindWindow("WMPlayerApp",NULL);
  if(!WindowHwnd) WindowHwnd = FindWindow("Media Player 2",NULL);

  if(WindowHwnd!=NULL)
  {
    GetWindowText(WindowHwnd,this_title,sizeof(this_title));

    opis = this_title;

    //Usuwanie "- Windows Media Player""
    IsThere = AnsiPos("- Windows Media Player", opis);
    IsThereLength = opis.Length();
    if(IsThere>0)
    {
      opis.Delete(IsThere, IsThereLength + 1);
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
  //Pobieranie tekstu okien
  if(WinampDownRadio->Checked==true)
   aWinampDown->Execute();
  else if(FoobarDownRadio->Checked==true)
   aFoobarDown->Execute();
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
  else if(SongbirdDownRadio->Checked==true)
   aSongbirdDown->Execute();
  else if(LastFMDownRadio->Checked==true)
   aLastFMDown->Execute();
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

  AutoDownUpButton->Enabled=false;
  AutoDownDownButton->Enabled=false;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aVUPlayerDownExecute(TObject *Sender)
{
  WindowHwnd = FindWindow("VUPlayerClass",NULL);

  if(WindowHwnd!=NULL)
  {
    GetWindowText(WindowHwnd,this_title,sizeof(this_title));

    opis = this_title;

    //Usuwanie " ["
    IsThere = AnsiPos(" [", opis);
    IsThereLength = opis.Length();
    if(IsThere>0)
    {
      opis.Delete(IsThere, IsThereLength + 1);
      opis=opis.Trim();
    }
  }
  else
   opis = "";
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aXMPlayDownExecute(TObject *Sender)
{
  WindowHwnd = FindWindow("XMPLAY-MAIN",NULL);

  if(WindowHwnd!=NULL)
  {
    GetWindowText(WindowHwnd,this_title,sizeof(this_title));

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
  IsThere = AnsiPos(". ", opis);
  if((IsThere>0)&&(IsThere<6))
  {
    opis2=opis;
    opis2.Delete(IsThere, opis2.Length());
    if(TestDigit(opis2)==true)
    {
      opis.Delete(1, IsThere);
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
  else if(Boxy=="Songbird")
   SongbirdDownRadio->Checked=true;
  else if(Boxy=="Lastfm")
   LastFMDownRadio->Checked=true;
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

  //Tryb automatyczny
  AutoDownCheckListBox->Clear();
  AutoDownCheckListBoxPreview->Clear();

  AutoDownCheckListBox->Items->Add(Ini->ReadString("Auto", "Player0", "Winamp/AIMP2/KMPlayer"));
  AutoDownCheckListBox->Checked[0]=Ini->ReadBool("Auto", "PlayerCheck0", 1);
  AutoDownCheckListBox->Items->Add(Ini->ReadString("Auto", "Player1", "Foobar2000"));
  AutoDownCheckListBox->Checked[1]=Ini->ReadBool("Auto", "PlayerCheck1", 1);
  AutoDownCheckListBox->Items->Add(Ini->ReadString("Auto", "Player2", "Windows Media Player"));
  AutoDownCheckListBox->Checked[2]=Ini->ReadBool("Auto", "PlayerCheck2", 1);
  AutoDownCheckListBox->Items->Add(Ini->ReadString("Auto", "Player3", "VUPlayer"));
  AutoDownCheckListBox->Checked[3]=Ini->ReadBool("Auto", "PlayerCheck3", 1);
  AutoDownCheckListBox->Items->Add(Ini->ReadString("Auto", "Player4", "XMPlay"));
  AutoDownCheckListBox->Checked[4]=Ini->ReadBool("Auto", "PlayerCheck4", 1);
  AutoDownCheckListBox->Items->Add(Ini->ReadString("Auto", "Player5", "Media Player Classic"));
  AutoDownCheckListBox->Checked[5]=Ini->ReadBool("Auto", "PlayerCheck5", 1);
  AutoDownCheckListBox->Items->Add(Ini->ReadString("Auto", "Player6", "iTunes"));
  AutoDownCheckListBox->Checked[6]=Ini->ReadBool("Auto", "PlayerCheck6", 1);
  AutoDownCheckListBox->Items->Add(Ini->ReadString("Auto", "Player7", "ALSong"));
  AutoDownCheckListBox->Checked[7]=Ini->ReadBool("Auto", "PlayerCheck7", 1);
  AutoDownCheckListBox->Items->Add(Ini->ReadString("Auto", "Player8", "AQQ Radio"));
  AutoDownCheckListBox->Checked[8]=Ini->ReadBool("Auto", "PlayerCheck8", 1);
  AutoDownCheckListBox->Items->Add(Ini->ReadString("Auto", "Player9", "Songbird"));
  AutoDownCheckListBox->Checked[9]=Ini->ReadBool("Auto", "PlayerCheck9", 1);
  AutoDownCheckListBox->Items->Add(Ini->ReadString("Auto", "Player10", "Last.fm Player"));
  AutoDownCheckListBox->Checked[10]=Ini->ReadBool("Auto", "PlayerCheck10", 1);


  for(int i=0;i<11;i++)
  {
    AutoDownCheckListBoxPreview->Items->Add(AutoDownCheckListBox->Items->Strings[i]);
    AutoDownCheckListBoxPreview->Checked[i]=AutoDownCheckListBox->Checked[i];
  }

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
  if(SongbirdDownRadio->Checked==true)
   Ini->WriteString("Settings", "Box", "Songbird");
  if(LastFMDownRadio->Checked==true)
   Ini->WriteString("Settings", "Box", "Lastfm");
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

  //Tryb automatyczny
  AutoDownCheckListBox->Clear();

  for(int i=0;i<11;i++)
  {
    AutoDownCheckListBox->Items->Add(AutoDownCheckListBoxPreview->Items->Strings[i]);
    AutoDownCheckListBox->Checked[i]=AutoDownCheckListBoxPreview->Checked[i];

    Ini->WriteString("Auto", "Player" + IntToStr(i), AutoDownCheckListBox->Items->Strings[i]);
    Ini->WriteBool("Auto", "PlayerCheck" + IntToStr(i), AutoDownCheckListBox->Checked[i]);
  }

  delete Ini;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aAutoDownExecute(TObject *Sender)
{
  for(int i=0;i<11;i++)
  {
    Player = AutoDownCheckListBox->Items->Strings[i];
    PlayerCheck = AutoDownCheckListBox->Checked[i];

    if(PlayerCheck==1)
    {
      if(Player=="Winamp/AIMP2/KMPlayer")
       aWinampDown->Execute();
      if(Player=="Foobar2000")
       aFoobarDown->Execute();
      if(Player=="Windows Media Player")
       aWMPDown->Execute();
      if(Player=="VUPlayer")
       aVUPlayerDown->Execute();
      if(Player=="XMPlay")
       aXMPlayDown->Execute();
      if(Player=="Media Player Classic")
       aMPCDown->Execute();
      if(Player=="iTunes")
       aiTunesDown->Execute();
      if(Player=="ALSong")
       aALSongDown->Execute();
      if(Player=="AQQ Radio")
       aPluginAQQRadioDown->Execute();
      if(Player=="Songbird")
       aSongbirdDown->Execute();
      if(Player=="Last.fm Player")
       aLastFMDown->Execute();

      if(opis!="")
       i=11;
    }
  }
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
    opis_TMP = StatusMemo->Text.SubString(0, 512);
    //Pobrany utwór
    IsThere = AnsiPos("CC_TUNESTATUS", opis_TMP);
    if(IsThere!=0)
     opis = StringReplace(opis_TMP, "CC_TUNESTATUS", opis, TReplaceFlags() << rfReplaceAll);
    //Opis pocz¹tkowy
    IsThere = AnsiPos("CC_STARTSTATUS", opis);
    if(IsThere!=0)
     opis = StringReplace(opis, "CC_STARTSTATUS", opis_pocz, TReplaceFlags() << rfReplaceAll);
    //Wersja TuneStatus
    IsThere = AnsiPos("CC_PLUGINVERSION", opis);
    if(IsThere!=0)
     opis = StringReplace(opis, "CC_PLUGINVERSION", GetFileVersionInfo(GetPluginDir(ePluginDirectory).c_str(), "FileVersion"), TReplaceFlags() << rfReplaceAll);
    //Wersja AQQ
    IsThere = AnsiPos("CC_AQQVERSION", opis);
    if(IsThere!=0)
     opis = StringReplace(opis, "CC_AQQVERSION", GetFileVersionInfo(NULL, "FileVersion"), TReplaceFlags() << rfReplaceAll);
    //Samplerate
    IsThere = AnsiPos("CC_SAMPLERATE", opis);
    if(IsThere!=0)
    {
      opis = StringReplace(opis, "CC_SAMPLERATE", Samplerate, TReplaceFlags() << rfReplaceAll);
      IsSamplerate=1;
    }
    else IsSamplerate=0;
    //Bitrate
    IsThere = AnsiPos("CC_BITRATE", opis);
    if(IsThere!=0)
    {
      opis = StringReplace(opis, "CC_BITRATE", Bitrate, TReplaceFlags() << rfReplaceAll);
      IsBitrate=1;
    }
    else IsBitrate=0;
    //Channels
    IsThere = AnsiPos("CC_CHANNELS", opis);
    if(IsThere!=0)
    {
      opis = StringReplace(opis, "CC_CHANNELS", Channels, TReplaceFlags() << rfReplaceAll);
      IsChannels=1;
    }
    else IsChannels=0;
    //SongLength
    IsThere = AnsiPos("CC_SONGLENGTH", opis);
    if(IsThere!=0)
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
  if(Tag!="--------Tylko dla Winamp/AIMP2/KMPlayer--------")
  {
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
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aALSongDownExecute(TObject *Sender)
{
  WindowHwnd = FindWindow("ALSongKernelWindow",NULL);

  if(WindowHwnd!=NULL)
  {
    GetWindowText(WindowHwnd,this_title,sizeof(this_title));

    opis = this_title;

    //Usuwanie "ALSong"
    IsThere = AnsiPos("ALSong", opis);
    if(IsThere>0)
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
  IsThere = AnsiPos("Wtyczka AQQ Radio:", opis);
  if (IsThere>0)
  {
    IsThere = AnsiPos(":", opis);

    //Symulacja pierwszego uruchomienia SongTimer
    if(DisableSongTimerCheck==true)
    {
      SongTimer->Enabled=false;
      SongTimer->Interval=1000;
      JustEnabled=1;
      SongTimer->Enabled=true;
    }

    opis.Delete(1, IsThere);
    opis=opis.Trim();
    if(CutRadiostationNameCheck==1)
    {
      IsThere = AnsiPos("- ", opis);
      opis.Delete(1, IsThere);
      opis=opis.Trim();
    }
  }
  else
   opis = "";
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aSongbirdDownExecute(TObject *Sender)
{
  if(SongbirdWindowHwnd==NULL)
   EnumWindows((WNDENUMPROC)FindSongbird,0);
  else
  {
    GetWindowText(SongbirdWindowHwnd,this_title,sizeof(this_title));
    opis = this_title;

    opis=opis.Trim();

    IsThere = AnsiPos("[Stopped]", opis);
    if(IsThere>0)
     opis = "";
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::SongbirdDownloadClick(TObject *Sender)
{
  ShellExecute(NULL, "open", "http://addons.songbirdnest.com/addon/42", NULL, NULL, SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::WMPDownloadClick(TObject *Sender)
{
  ShellExecute(NULL, "open", "http://www.beherit.za.pl/Download/WMP-Pluginy.zip", NULL, NULL, SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::iTunesDownloadClick(TObject *Sender)
{
  ShellExecute(NULL, "open", "http://www.beherit.za.pl/Download/iTunes-Plugin.zip", NULL, NULL, SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FoobarDownloadClick(TObject *Sender)
{
  ShellExecute(NULL, "open", "http://www.beherit.za.pl/Download/Foobar2000-Plugin.zip", NULL, NULL, SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AutoDownUpButtonClick(TObject *Sender)
{
  if(AutoDownCheckListBoxPreview->ItemIndex>0)
  {
    AnsiString Obnizany = AutoDownCheckListBoxPreview->Items->Strings[AutoDownCheckListBoxPreview->ItemIndex-1];
    bool ObnizanyCheck = AutoDownCheckListBoxPreview->Checked[AutoDownCheckListBoxPreview->ItemIndex-1];
    AnsiString Podwyzszany = AutoDownCheckListBoxPreview->Items->Strings[AutoDownCheckListBoxPreview->ItemIndex];
    bool PodwyzszanyCheck = AutoDownCheckListBoxPreview->Checked[AutoDownCheckListBoxPreview->ItemIndex];

    AutoDownCheckListBoxPreview->Items->Strings[AutoDownCheckListBoxPreview->ItemIndex-1]=Podwyzszany;
    AutoDownCheckListBoxPreview->Items->Strings[AutoDownCheckListBoxPreview->ItemIndex]=Obnizany;

    AutoDownCheckListBoxPreview->Checked[AutoDownCheckListBoxPreview->ItemIndex-1]=PodwyzszanyCheck;
    AutoDownCheckListBoxPreview->Checked[AutoDownCheckListBoxPreview->ItemIndex]=ObnizanyCheck;

    AutoDownCheckListBoxPreview->ItemIndex=AutoDownCheckListBoxPreview->ItemIndex-1;
  }

  if(AutoDownCheckListBoxPreview->ItemIndex<=0)
   AutoDownUpButton->Enabled=false;
  else
  AutoDownUpButton->Enabled=true;

  if(AutoDownCheckListBoxPreview->ItemIndex==10)
   AutoDownDownButton->Enabled=false;
  else if(AutoDownCheckListBoxPreview->ItemIndex<0)
   AutoDownDownButton->Enabled=false;
  else
   AutoDownDownButton->Enabled=true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AutoDownDownButtonClick(TObject *Sender)
{
  if(AutoDownCheckListBoxPreview->ItemIndex<10)
  {
    AnsiString Obnizany = AutoDownCheckListBoxPreview->Items->Strings[AutoDownCheckListBoxPreview->ItemIndex];
    bool ObnizanyCheck = AutoDownCheckListBoxPreview->Checked[AutoDownCheckListBoxPreview->ItemIndex];
    AnsiString Podwyzszany = AutoDownCheckListBoxPreview->Items->Strings[AutoDownCheckListBoxPreview->ItemIndex+1];
    bool PodwyzszanyCheck = AutoDownCheckListBoxPreview->Checked[AutoDownCheckListBoxPreview->ItemIndex+1];

    AutoDownCheckListBoxPreview->Items->Strings[AutoDownCheckListBoxPreview->ItemIndex]=Podwyzszany;
    AutoDownCheckListBoxPreview->Items->Strings[AutoDownCheckListBoxPreview->ItemIndex+1]=Obnizany;

    AutoDownCheckListBoxPreview->Checked[AutoDownCheckListBoxPreview->ItemIndex]=PodwyzszanyCheck;
    AutoDownCheckListBoxPreview->Checked[AutoDownCheckListBoxPreview->ItemIndex+1]=ObnizanyCheck;

    AutoDownCheckListBoxPreview->ItemIndex=AutoDownCheckListBoxPreview->ItemIndex+1;
  }

  if(AutoDownCheckListBoxPreview->ItemIndex<=0)
   AutoDownUpButton->Enabled=false;
  else
  AutoDownUpButton->Enabled=true;

  if(AutoDownCheckListBoxPreview->ItemIndex==10)
   AutoDownDownButton->Enabled=false;
  else if(AutoDownCheckListBoxPreview->ItemIndex<0)
   AutoDownDownButton->Enabled=false;
  else
   AutoDownDownButton->Enabled=true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ResetButtonClick(TObject *Sender)
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
  AutoDownCheckListBoxPreview->Items->Add("Songbird");
  AutoDownCheckListBoxPreview->Items->Add("Last.fm Player");

  AutoDownCheckListBoxPreview->CheckAll();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AutoDownCheckListBoxPreviewSelect(
      TObject *Sender)
{
  if(AutoDownCheckListBoxPreview->ItemIndex<=0)
   AutoDownUpButton->Enabled=false;
  else
  AutoDownUpButton->Enabled=true;

  if(AutoDownCheckListBoxPreview->ItemIndex==10)
   AutoDownDownButton->Enabled=false;
  else if(AutoDownCheckListBoxPreview->ItemIndex<0)
   AutoDownDownButton->Enabled=false;
  else
   AutoDownDownButton->Enabled=true;
}
//---------------------------------------------------------------------------

