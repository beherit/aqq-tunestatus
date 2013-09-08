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
__declspec(dllimport)void UstawOpis(AnsiString opis);
__declspec(dllimport)AnsiString GetPluginPath(AnsiString Dir);
__declspec(dllimport)AnsiString PobierzOpis(AnsiString opis);
//---------------------------------------------------------------------------
AnsiString ePluginDirectory;
AnsiString opis;
AnsiString opisTMP;
AnsiString opis_pocz;
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

bool __stdcall EnumProc(HWND hWnd,/*LPARAM*/long/*lp*/)
{
  unsigned long* pPid;   //LPDWORD
  unsigned long result;      //DWORD
  void *hg;                  //HGLOBAL
  unsigned long id;

  if(hWnd==NULL)
   return false;

  hg = GlobalAlloc(GMEM_SHARE,sizeof(unsigned long));
  pPid = (unsigned long *)GlobalLock(hg);

  result = GetWindowThreadProcessId(hWnd,pPid);

  if(result)
  {
    char title[110];
    char className[95];
    char totalStr[256];
    GetClassName(hWnd,className,95);
    GetWindowText(hWnd,title,110);
    id=*pPid;
    ultoa(id,totalStr,10);
    strcat(totalStr,"\t");

    AnsiString classNameA = className;
    AnsiString titlea = title;

    if(classNameA=="QWidget")
    {
      if((titlea!="LastFM")&&(titlea!="Last.fm")&&(titlea!="Diagnostyka")&&(titlea!="Poleæ"))
      {
        opis = title;
      }
    }
  }
  else
  {
    GlobalUnlock(hg);
    GlobalFree(hg);
    return false;
  }
  GlobalUnlock(hg);
  GlobalFree(hg);
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
    opis.TrimRight();
  }

  x = AnsiPos("*** ", opis);
  if (x>0)
   opis.Delete(1, x + 3);

  if(opis=="Winamp")
   opis = "";

  aPreSufFix->Execute();        
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
    opis.TrimRight();
  }

  aPreSufFix->Execute();
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
    opis.TrimRight();
  }

  if(opis=="Windows Media Player")
   opis = "";

  aPreSufFix->Execute();
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
    opis.TrimRight();
  }

  aPreSufFix->Execute();
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
  EnumWindows((WNDENUMPROC)EnumProc,0);
  aPreSufFix->Execute();
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
    opis.TrimRight();
  }

  if(opis=="Windows Media Player")
   opis = "";

  aPreSufFix->Execute();
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

  if(opis!="")
  {
    if(opis!=opisTMP)
    {
      opisTMP=opis;
      Preview->Text=opis;
      UstawOpis(opis);
    }
  }
  else
  {
    if(opis_pocz!=opis)
    {
      opisTMP=opis_pocz;
      UstawOpis(opis_pocz);
      Preview->Text="";
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
  }
  if(RunPluginCheckBox->Checked==false)
  {
    Timer->Enabled=false;
    UstawOpis(opis_pocz);
    opisTMP="";
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::OkButtonClick(TObject *Sender)
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
  if(MPCDownRadio->Checked==true)
   Ini->WriteString("Settings", "Box", "MPC");

  Ini->WriteString("Settings", "Prefix", PrefixEdit->Text);
  Ini->WriteInteger("Settings", "PrefixOn", PrefixCheckBox->Checked);

  Ini->WriteString("Settings", "Suffix", SuffixEdit->Text);
  Ini->WriteInteger("Settings", "SuffixOn", SuffixCheckBox->Checked);

  delete Ini;

  Visible=false;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormShow(TObject *Sender)
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
  else if(Boxy=="MPC")
   MPCDownRadio->Checked=true;

  AnsiString PrefixText = Ini->ReadString("Settings", "Prefix", "");
   PrefixEdit->Text=PrefixText;
  int PrefixOn = Ini->ReadInteger("Settings", "PrefixOn", 0);
   PrefixCheckBox->Checked=PrefixOn;

  AnsiString SuffixText = Ini->ReadString("Settings", "Suffix", "");
   SuffixEdit->Text=SuffixText;
  int SuffixOn = Ini->ReadInteger("Settings", "SuffixOn", 0);
   SuffixCheckBox->Checked=SuffixOn;

  delete Ini;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormClose(TObject *Sender, TCloseAction &Action)
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
  if(MPCDownRadio->Checked==true)
   Ini->WriteString("Settings", "Box", "MPC");

  Ini->WriteString("Settings", "Prefix", PrefixEdit->Text);
  Ini->WriteInteger("Settings", "PrefixOn", PrefixCheckBox->Checked);

  Ini->WriteString("Settings", "Suffix", SuffixEdit->Text);
  Ini->WriteInteger("Settings", "SuffixOn", SuffixCheckBox->Checked); 

  delete Ini;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aVUPlayerDownExecute(TObject *Sender)
{
  HWND hwndMPC = FindWindow("VUPlayerClass",NULL);

  char this_title[2048];
  GetWindowText(hwndMPC,this_title,sizeof(this_title));

  opis = this_title;

  int x = AnsiPos(" [", opis);
  int y = opis.Length();
  if (x>0)
  {
    opis.Delete(x, y + 1);
    opis.TrimRight();
  }

  aPreSufFix->Execute();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aXMPlayDownExecute(TObject *Sender)
{
  HWND hwndMPC = FindWindow("XMPLAY-MAIN",NULL);

  char this_title[2048];
  GetWindowText(hwndMPC,this_title,sizeof(this_title));

  opis = this_title;

  aPreSufFix->Execute();
}
//---------------------------------------------------------------------------

