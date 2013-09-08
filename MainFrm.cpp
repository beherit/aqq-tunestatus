//---------------------------------------------------------------------------
#include <vcl.h>
#include <inifiles.hpp>
#pragma hdrstop
#include "MainFrm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "XPMan"
#pragma resource "*.dfm"
TMainForm *MainForm;
//---------------------------------------------------------------------------
__declspec(dllimport)void UstawOpis(AnsiString opis, bool force);
__declspec(dllimport)AnsiString GetPluginPath(AnsiString Dir);
__declspec(dllimport)AnsiString PobierzOpis(AnsiString opis);
__declspec(dllimport)void PrzypiszButton();
__declspec(dllimport)void UsunButton();
__declspec(dllimport)void UpdateButton(bool OnOff);
//---------------------------------------------------------------------------
AnsiString ePluginDirectory;
AnsiString opis;
int FormShowed=0;
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

bool CALLBACK EnumWindowsProc(HWND hWnd)
{
 char WindowName[2048], ClassName[2048];

 GetWindowText(hWnd, WindowName, 2048);
 GetClassName(hWnd, ClassName, 2048);

 AnsiString ClassNameAnsi = ClassName;
 AnsiString WindowNameAnsi = WindowName;

 if(ClassNameAnsi=="QWidget")
 {
   if((WindowNameAnsi!="LastFM")&&(WindowNameAnsi!="Last.fm")&&(WindowNameAnsi!="Diagnostyka")&&(WindowNameAnsi!="Poleæ"))
   {
     opis = WindowNameAnsi;
   }
 }
 return true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aWinampDownExecute(TObject *Sender)
{
  HWND hwndWinamp = FindWindow("Winamp v1.x",NULL);
  if(!hwndWinamp) hwndWinamp = FindWindow("Studio",NULL);

  char this_title[2048];
  GetWindowText(hwndWinamp,this_title,sizeof(this_title));

  opis = this_title;

  int x = AnsiPos("- Winamp", opis);
  int y = opis.Length();
  if (x>0)
  {
    opis.Delete(x, y + 1);
    opis=opis.Trim();
  }

  x = AnsiPos("*** ", opis);
  if (x>0)
  {
    opis.Delete(1, x + 3);
    opis=opis.Trim();
  }

  x = AnsiPos("Winamp", opis);
  if (x>0)
   opis = "";

  if (hwndWinamp!=NULL)
  {
    int res = SendMessage(hwndWinamp,WM_USER,0,104);
    if(res==0)
     opis = "";
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aFoobarDownExecute(TObject *Sender)
{
  HWND hwndFoobar = FindWindow("{DA7CD0DE-1602-45e6-89A1-C2CA151E008E}/1",NULL);
  if(!hwndFoobar) hwndFoobar = FindWindow("{DA7CD0DE-1602-45e6-89A1-C2CA151E008E}",NULL);
  if(!hwndFoobar) hwndFoobar = FindWindow("{DA7CD0DE-1602-45e6-89A1-C2CA151E008E}",NULL);
  if(!hwndFoobar) hwndFoobar = FindWindow("{97E27FAA-C0B3-4b8e-A693-ED7881E99FC1}",NULL);
  if(!hwndFoobar) hwndFoobar = FindWindow("{E7076D1C-A7BF-4f39-B771-BCBE88F2A2A8}",NULL);

  char this_title[2048];
  GetWindowText(hwndFoobar,this_title,sizeof(this_title));

  opis = this_title;

  int x = AnsiPos("[foobar2000", opis);
  int y = opis.Length();
  if (x>0)
  {
    opis.Delete(x, y + 1);
    opis=opis.Trim();
  }

  y = opis.Length();
  x = AnsiPos("foobar2000", opis);
  if (x>0)
  {
    opis.Delete(x, y + 1);
    opis=opis.Trim();
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aWMP64DownExecute(TObject *Sender)
{
  HWND hwndWMP64 = FindWindow("Media Player 2",NULL);

  char this_title[2048];
  GetWindowText(hwndWMP64,this_title,sizeof(this_title));

  opis = this_title;

  int x = AnsiPos("- Windows Media Player", opis);
  int y = opis.Length();
  if (x>0)
  {
    opis.Delete(x, y + 1);
    opis=opis.Trim();
  }

  if(opis=="Windows Media Player")
   opis = "";
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aMPCDownExecute(TObject *Sender)
{
  HWND hwndMPC = FindWindow("MediaPlayerClassicW",NULL);

  char this_title[2048];
  GetWindowText(hwndMPC,this_title,sizeof(this_title));

  opis = this_title;

  int x = AnsiPos("- Media Player Classic", opis);
  int y = opis.Length();
  if (x>0)
  {
    opis.Delete(x, y + 1);
    opis=opis.Trim();
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aPreSufFixExecute(TObject *Sender)
{
  if(opis!="")
  {
    if(PrefixCheckBox->Checked==true)
    {
      opis = PrefixEdit->Text + opis;
      String opis_str = opis.c_str();
      opis = StringReplace(opis_str, "|", "\n", TReplaceFlags() << rfReplaceAll);
    }
    if(SuffixCheckBox->Checked==true)    
    {
      opis = opis + SuffixEdit->Text;
      String opis_str = opis.c_str();
      opis = StringReplace(opis_str, "|", "\n", TReplaceFlags() << rfReplaceAll);
    }
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aLastFMDownExecute(TObject *Sender)
{
  opis = "";
  EnumWindows((WNDENUMPROC)EnumWindowsProc,0);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aWMP7_11DownExecute(TObject *Sender)
{
  HWND hwndMPC = FindWindow("WMPlayerApp",NULL);

  char this_title[2048];
  GetWindowText(hwndMPC,this_title,sizeof(this_title));

  opis = this_title;

  int x = AnsiPos("- Windows Media Player", opis);
  int y = opis.Length();
  if (x>0)
  {
    opis.Delete(x, y + 1);
    opis=opis.Trim();
  }

  if(opis=="Windows Media Player")
   opis = "";
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::TimerTimer(TObject *Sender)
{
  if(WinampDownRadio->Checked==true)
   aWinampDown->Execute();
  else if(FoobarDownRadio->Checked==true)
   aFoobarDown->Execute();
  else if(LastFMDownRadio->Checked==true)
   aLastFMDown->Execute();
  else if(WMP7_11DownRadio->Checked==true)
   aWMP7_11Down->Execute();
  else if(WMP64DownRadio->Checked==true)
   aWMP64Down->Execute();
  else if(VUPlayerDownRadio->Checked==true)
   aVUPlayerDown->Execute();
  else if(XMPlayDownRadio->Checked==true)
   aXMPlayDown->Execute();
  else if(MPCDownRadio->Checked==true)
   aMPCDown->Execute();
  else if(AutoDownRadio->Checked==true)
   aAutoDown->Execute();

  aCutSongNumber->Execute();
  aPreSufFix->Execute();

  if(opis!="")
  {
    if(opis!=opisTMP)
    {
      opisTMP=opis;
      Preview->Text=opis;
      UstawOpis(opis,!SetOnlyInJabberCheckBox->Checked);
    }
  }
  else
  {
    if(opis_pocz!=opisTMP)
    {
      opisTMP=PobierzOpis(opisTMP);
      if(opis_pocz!=opisTMP)
      {
        opisTMP=opis_pocz;
        UstawOpis(opis_pocz,!SetOnlyInJabberCheckBox->Checked);
        Preview->Text="";
      }
    }
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::RunPluginCheckBoxClick(TObject *Sender)
{
  if(RunPluginCheckBox->Checked==true)
  {
    opis_pocz = PobierzOpis(opis_pocz);
    Timer->Enabled=true;
    if(EnableFastOnOffCheckBox->Checked==true)
     UpdateButton(true);
  }
  else if(RunPluginCheckBox->Checked==false)
  {
    Timer->Enabled=false;
    if(EnableFastOnOffCheckBox->Checked==true)
     UpdateButton(false);
    opisTMP=PobierzOpis(opisTMP);
    if(opis_pocz!=opisTMP)
    {
      UstawOpis(opis_pocz,!SetOnlyInJabberCheckBox->Checked);
      opisTMP="";
    }
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::OkButtonClick(TObject *Sender)
{
  aSaveSettings->Execute();

  Visible=false;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormShow(TObject *Sender)
{
  aReadSettings->Execute();
  FormShowed=1;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormClose(TObject *Sender, TCloseAction &Action)
{
  aSaveSettings->Execute();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aVUPlayerDownExecute(TObject *Sender)
{
  HWND hwndVUPlayer = FindWindow("VUPlayerClass",NULL);

  char this_title[2048];
  GetWindowText(hwndVUPlayer,this_title,sizeof(this_title));

  opis = this_title;

  int x = AnsiPos(" [", opis);
  int y = opis.Length();
  if (x>0)
  {
    opis.Delete(x, y + 1);
    opis=opis.Trim();
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aXMPlayDownExecute(TObject *Sender)
{
  HWND hwndXMPlay = FindWindow("XMPLAY-MAIN",NULL);

  char this_title[2048];
  GetWindowText(hwndXMPlay,this_title,sizeof(this_title));

  opis = this_title;

  int x = AnsiPos("XMPlay", opis);
  if (x>0)
   opis = "";

  if (hwndXMPlay!=NULL)
  {
    int res = SendMessage(hwndXMPlay,WM_USER,0,104);
    if(res==0)
     opis = "";
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aCutSongNumberExecute(TObject *Sender)
{
  if(CutSongNumberCheckBox->Checked==true)
  {
    int x = AnsiPos(". ", opis);
    if (x>0)
    {
      opis.Delete(1, x);
      opis=opis.Trim();
    }
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::SetOnlyInJabberCheckBoxClick(TObject *Sender)
{
  opisTMP="";
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aReadSettingsExecute(TObject *Sender)
{
  ePluginDirectory = GetPluginPath(ePluginDirectory);

  TIniFile *Ini = new TIniFile(ePluginDirectory + "\\\\TuneStatus.dat");

  AnsiString Boxy = Ini->ReadString("Settings", "Box", "Winamp");
  if(Boxy=="Winamp")
   WinampDownRadio->Checked=true;
  else if(Boxy=="Foobar")
   FoobarDownRadio->Checked=true;
  else if(Boxy=="Lastfm")
   LastFMDownRadio->Checked=true;
  else if(Boxy=="WMP7-11")
   WMP7_11DownRadio->Checked=true;
  else if(Boxy=="WMP6")
   WMP64DownRadio->Checked=true;
  else if(Boxy=="VUPlayer")
   VUPlayerDownRadio->Checked=true;
  else if(Boxy=="XMPlay")
   XMPlayDownRadio->Checked=true;
  else if(Boxy=="MPC")
   MPCDownRadio->Checked=true;
  else if(Boxy=="Auto")
   AutoDownRadio->Checked=true;

  AnsiString PrefixText = Ini->ReadString("Settings", "Prefix", ";");
  PrefixText.Delete(1, 1);
   PrefixEdit->Text=PrefixText.SetLength(PrefixText.Length()-1);
  int PrefixOn = Ini->ReadInteger("Settings", "PrefixOn", 0);
   PrefixCheckBox->Checked=PrefixOn;

  AnsiString SuffixText = Ini->ReadString("Settings", "Suffix", ";");
  SuffixText.Delete(1, 1);
   SuffixEdit->Text=SuffixText.SetLength(SuffixText.Length()-1);
  int SuffixOn = Ini->ReadInteger("Settings", "SuffixOn", 0);
   SuffixCheckBox->Checked=SuffixOn;

  CutSongNumberCheckBox->Checked = Ini->ReadInteger("Settings", "CutSongNumber", 0);

  SetOnlyInJabberCheckBox->Checked = Ini->ReadInteger("Settings", "SetOnlyInJabber", 0);

  EnablePluginOnStartCheckBox->Checked = Ini->ReadInteger("Settings", "EnablePluginOnStart", 0);

  EnableFastOnOffCheckBox->Checked = Ini->ReadInteger("Settings", "EnableFastOnOff", 0);

  delete Ini;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aSaveSettingsExecute(TObject *Sender)
{
  ePluginDirectory = GetPluginPath(ePluginDirectory);

  TIniFile *Ini = new TIniFile(ePluginDirectory + "\\\\TuneStatus.dat");

  if(WinampDownRadio->Checked==true)
   Ini->WriteString("Settings", "Box", "Winamp");
  if(FoobarDownRadio->Checked==true)
   Ini->WriteString("Settings", "Box", "Foobar");
  if(LastFMDownRadio->Checked==true)
   Ini->WriteString("Settings", "Box", "Lastfm");
  if(WMP7_11DownRadio->Checked==true)
   Ini->WriteString("Settings", "Box", "WMP7-11");
  if(WMP64DownRadio->Checked==true)
   Ini->WriteString("Settings", "Box", "WMP6");
  if(VUPlayerDownRadio->Checked==true)
   Ini->WriteString("Settings", "Box", "VUPlayer");
  if(XMPlayDownRadio->Checked==true)
   Ini->WriteString("Settings", "Box", "XMPlay");
  if(MPCDownRadio->Checked==true)
   Ini->WriteString("Settings", "Box", "MPC");
  if(AutoDownRadio->Checked==true)
   Ini->WriteString("Settings", "Box", "Auto");

  char Quote = '=';
  AnsiString PrefixText = AnsiQuotedStr(PrefixEdit->Text, Quote);
  Ini->WriteString("Settings", "Prefix", PrefixText);
  Ini->WriteInteger("Settings", "PrefixOn", PrefixCheckBox->Checked);

  AnsiString SuffixText = AnsiQuotedStr(SuffixEdit->Text, Quote);
  Ini->WriteString("Settings", "Suffix", SuffixText);
  Ini->WriteInteger("Settings", "SuffixOn", SuffixCheckBox->Checked);

  Ini->WriteInteger("Settings", "CutSongNumber", CutSongNumberCheckBox->Checked);

  Ini->WriteInteger("Settings", "SetOnlyInJabber", SetOnlyInJabberCheckBox->Checked);

  Ini->WriteInteger("Settings", "EnablePluginOnStart", EnablePluginOnStartCheckBox->Checked);

  Ini->WriteInteger("Settings", "EnableFastOnOff", EnableFastOnOffCheckBox->Checked);

  delete Ini;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::EnableFastOnOffCheckBoxClick(TObject *Sender)
{
  if(FormShowed==1)
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
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aAutoDownExecute(TObject *Sender)
{
  aWinampDown->Execute();
  if(opis=="")
   aFoobarDown->Execute();
  if(opis=="")
   aWMP7_11Down->Execute();
  if(opis=="")
   aWMP64Down->Execute();
  if(opis=="")
   aVUPlayerDown->Execute();
  if(opis=="")
   aXMPlayDown->Execute();
  if(opis=="")
   aMPCDown->Execute();
  if(opis=="")
   aLastFMDown->Execute();
}
//---------------------------------------------------------------------------

