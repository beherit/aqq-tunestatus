//---------------------------------------------------------------------------
#include <vcl.h>
#include <windows.h>
#pragma hdrstop
#pragma argsused
#include <inifiles.hpp>
#include <memory>
#include "ikonka.rh"
#include "Aqq.h"
#include "MainFrm.h"
//---------------------------------------------------------------------------

HINSTANCE hInstance; //uchwyt do wtyczki

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
  hInstance = hinst;
  return 1;
}

wchar_t *AnsiTowchar_t(AnsiString Str) //zamiana AnsiString->wchar_t*
{
  wchar_t *str = new wchar_t[Str.WideCharBufSize()];
  return Str.WideChar(str, Str.WideCharBufSize());
}

//utworzenie obiektow do struktur
PluginAction TPluginAction;
PluginAction TPluginActionButton;
PluginStateChange TPluginStateChange;
PluginLink TPluginLink;
PluginInfo TPluginInfo;

//tworzenie uchwytu do formy
TMainForm *handle;

//Zmienne do ikonu
int plugin_icon_idx;
int plugin_icon_idx_off;
int plugin_icon_idx_on;

//Do buttona
int EnableFastOnOff;
int EnablePluginOnStart;

AnsiString PobierzOpis(AnsiString opis)
{
  TPluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&TPluginStateChange),0);

  opis = TPluginStateChange.Status;

  return opis;
}

int __stdcall TuneStatus_FastOnOff (WPARAM, LPARAM)
{
  if (handle==NULL)
  {
    Application->Handle = MainForm;
    handle = new TMainForm(Application);
    handle->aReadSettings->Execute();
    if(handle->EnablePluginOnStartCheckBox->Checked==false)
    {
      handle->opis_pocz = PobierzOpis(handle->opis_pocz);
      handle->Timer->Enabled=!handle->Timer->Enabled;
      handle->RunPluginCheckBox->Checked=!handle->RunPluginCheckBox->Checked;
      if(handle->Timer->Enabled==true)
      {
        TPluginActionButton.IconIndex = plugin_icon_idx_on;
        TPluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_UPDATEBUTTON,0,(LPARAM)(&TPluginActionButton));
      }
      else
      {
        TPluginActionButton.IconIndex = plugin_icon_idx_off;
        TPluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_UPDATEBUTTON,0,(LPARAM)(&TPluginActionButton));
      }
    }
  }
  else
  {
    if(handle->RunPluginCheckBox->Checked==false)
     handle->opis_pocz = PobierzOpis(handle->opis_pocz);

    handle->aReadSettings->Execute();
    handle->Timer->Enabled=!handle->Timer->Enabled;
    handle->RunPluginCheckBox->Checked=!handle->RunPluginCheckBox->Checked;

    if(handle->Timer->Enabled==true)
    {
      TPluginActionButton.IconIndex = plugin_icon_idx_on;
      TPluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_UPDATEBUTTON,0,(LPARAM)(&TPluginActionButton));
    }
    else
    {
      TPluginActionButton.IconIndex = plugin_icon_idx_off;
      TPluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_UPDATEBUTTON,0,(LPARAM)(&TPluginActionButton));
    }
  }

  return 0;
}

//Serwis otwarcia formy
int __stdcall TuneStatusService (WPARAM, LPARAM)
{
  if (handle==NULL)
  {
  Application->Handle = MainForm;
  handle = new TMainForm(Application);
  handle->Show();
  }
  else
    handle->Show();

  return 0;
}

//Notyfikacja zmiany statusu
int __stdcall OnSetNoteClose (WPARAM wParam, LPARAM lParam)
{
  if (handle!=NULL)
  {
    TPluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&TPluginStateChange),0);
    AnsiString nowy_opis = TPluginStateChange.Status;
    if((nowy_opis!=handle->opis_pocz)&&(nowy_opis!=handle->opisTMP))
    {
      handle->opis_pocz=nowy_opis;
      handle->opisTMP=nowy_opis;
    }
  }

  return 0;
}

//Program
extern "C"  __declspec(dllexport) PluginInfo* __stdcall AQQPluginInfo(DWORD AQQVersion)
{
  TPluginInfo.cbSize = sizeof(PluginInfo);
  TPluginInfo.ShortName = (wchar_t*)L"TuneStatus";
  TPluginInfo.Version = PLUGIN_MAKE_VERSION(1,0,3,2);
  TPluginInfo.Description = (wchar_t *)L"Wstawianie do opisu aktualnie s³uchanego utworu z wielu odtwarzaczy";
  TPluginInfo.Author = (wchar_t *)L"Krzysztof Grochocki (Beherit)";
  TPluginInfo.AuthorMail = (wchar_t *)L"beherit666@vp.pl";
  TPluginInfo.Copyright = (wchar_t *)L"Krzysztof Grochocki (Beherit)";
  TPluginInfo.Homepage = (wchar_t *)L"";
 
  return &TPluginInfo;
}

//Przypisywanie skrótu w popPlugins
void PrzypiszSkrot()
{
  TPluginAction.cbSize = sizeof(PluginAction);
  TPluginAction.pszName = (wchar_t*)L"TuneStatusServiceButton";
  TPluginAction.pszCaption = (wchar_t*) L"TuneStatus";
  TPluginAction.Position = 0;
  TPluginAction.IconIndex = plugin_icon_idx;
  TPluginAction.pszService = (wchar_t*) L"serwis_TuneStatusService";
  TPluginAction.pszPopupName = (wchar_t*) L"popPlugins";
  TPluginAction.PopupPosition = 0;

  TPluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&TPluginAction));
  TPluginLink.CreateServiceFunction(L"serwis_TuneStatusService",TuneStatusService);
}

void PrzypiszButton()
{
  TPluginActionButton.cbSize = sizeof(PluginAction);
  TPluginActionButton.pszName = (wchar_t*) L"TuneStatus_FastOnOff";
  TPluginActionButton.Position = 999;
  if(EnablePluginOnStart==1)
   TPluginActionButton.IconIndex = plugin_icon_idx_on;
  else
   TPluginActionButton.IconIndex = plugin_icon_idx_off;
  TPluginActionButton.pszService = (wchar_t*) L"serwis_TuneStatus_FastOnOff";

  TPluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_CREATEBUTTON,0,(LPARAM)(&TPluginActionButton));
  TPluginLink.CreateServiceFunction(L"serwis_TuneStatus_FastOnOff",TuneStatus_FastOnOff);

  EnableFastOnOff=1;
}

void UsunButton()
{
  TPluginLink.DestroyServiceFunction(TuneStatus_FastOnOff);
  TPluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_DESTROYBUTTON ,0,(LPARAM)(&TPluginActionButton));

  EnableFastOnOff=0;
}

void ExtractExe(unsigned short ID, AnsiString FileName)
{
  HRSRC rsrc = FindResource(HInstance, MAKEINTRESOURCE(ID), RT_RCDATA);

  DWORD Size = SizeofResource(HInstance, rsrc);
  HGLOBAL MemoryHandle = LoadResource(HInstance, rsrc);

  BYTE *MemPtr = (BYTE *)LockResource(MemoryHandle);

  std::auto_ptr<TMemoryStream>stream(new TMemoryStream);
  stream->Write(MemPtr, Size);
  stream->Position = 0;

  TMemoryStream *Ms = new TMemoryStream;
  Ms->Position = 0;
  Ms->LoadFromStream(stream.get());
  Ms->Position = 0;
  Ms->SaveToFile(FileName);
  Ms->Free();
}

extern "C" int __declspec(dllexport) __stdcall Load(PluginLink *Link)
{
  TPluginLink = *Link;

  AnsiString eDir = (wchar_t*)(TPluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,(WPARAM)(hInstance),0));
  eDir = StringReplace(eDir, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);

  //Wypakowanie ikon
  ExtractExe(ID_PNG1,"TuneStatus.png");
  //Przypisanie ikony
  wchar_t* plugin_icon_path = L"TuneStatus.png";
  plugin_icon_idx=TPluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(plugin_icon_path));
  //Usuniecie ikony
  DeleteFile("TuneStatus.png");

  //Wypakowanie ikon
  if(!FileExists(eDir + "\\TuneStatusOff.png"))
   ExtractExe(ID_PNG2,eDir + "\\TuneStatusOff.png");
  //Przypisanie ikony
  plugin_icon_path = AnsiTowchar_t(eDir +"\\TuneStatusOff.png");
  plugin_icon_idx_off=TPluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(plugin_icon_path));
  //Usuniecie ikony
  //DeleteFile("TuneStatusOff.png");

  //Wypakowanie ikon
  if(!FileExists(eDir + "\\TuneStatusOn.png"))
   ExtractExe(ID_PNG3,eDir + "\\TuneStatusOn.png");
  //Przypisanie ikony
  plugin_icon_path = AnsiTowchar_t(eDir +"\\TuneStatusOn.png");
  plugin_icon_idx_on=TPluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(plugin_icon_path));
  //Usuniecie ikony
  //DeleteFile("TuneStatusOn.png");

  //Przyspisanie przycisku
  PrzypiszSkrot();

  TPluginLink.HookEvent(AQQ_WINDOW_SETNOTE_CLOSE, OnSetNoteClose);

  //Odczyt ustawien
  TIniFile *Ini = new TIniFile(eDir + "\\\\TuneStatus.dat");
  EnablePluginOnStart = Ini->ReadInteger("Settings", "EnablePluginOnStart", 0);
  EnableFastOnOff = Ini->ReadInteger("Settings", "EnableFastOnOff", 0);
  delete Ini;

  //Czy w³¹czone
  if(EnablePluginOnStart==1)
  {
    Application->Handle = MainForm;
    handle = new TMainForm(Application);
    handle->opis_pocz = PobierzOpis(handle->opis_pocz);
    handle->aReadSettings->Execute();
    handle->Timer->Enabled=true;
    handle->RunPluginCheckBox->Checked=true;
  }

  //Czy przyspisac button
  if(EnableFastOnOff==1)
   PrzypiszButton();

  return 0;
}

extern "C" int __declspec(dllexport)__stdcall Settings()
{
  if (handle==NULL)
  {
  Application->Handle = MainForm;
  handle = new TMainForm(Application);
  handle->Show();
  }
  else
    handle->Show();

  return 0;
}

AnsiString GetPluginPath(AnsiString Dir)
{
  Dir = (wchar_t*)(TPluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,(WPARAM)(hInstance),0));
  Dir = StringReplace(Dir, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
  return Dir;
}
 
void UstawOpis(AnsiString opis, bool force)
{
  TPluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&TPluginStateChange),0);

  TPluginStateChange.cbSize = sizeof(PluginStateChange);
  TPluginStateChange.Status = AnsiTowchar_t(opis);
  TPluginStateChange.Force = force;

  TPluginLink.CallService(AQQ_SYSTEM_SETSHOWANDSTATUS,0,(LPARAM)(&TPluginStateChange));
}

extern "C" int __declspec(dllexport) __stdcall Unload()
{
  if (handle!=NULL)
  {
    if(handle->opis_pocz!="")
     UstawOpis(handle->opis_pocz,!handle->SetOnlyInJabberCheckBox->Checked);
  }
  TPluginLink.UnhookEvent(OnSetNoteClose);
  TPluginLink.DestroyServiceFunction(TuneStatusService);
  TPluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM,0,(LPARAM)(&TPluginAction));
  TPluginLink.CallService(AQQ_ICONS_DESTROYPNGICON,0,(LPARAM)(plugin_icon_idx));
  TPluginLink.CallService(AQQ_ICONS_DESTROYPNGICON,0,(LPARAM)(plugin_icon_idx_off));
  TPluginLink.CallService(AQQ_ICONS_DESTROYPNGICON,0,(LPARAM)(plugin_icon_idx_on));
  if(EnableFastOnOff==1)
  {
    TPluginLink.DestroyServiceFunction(TuneStatus_FastOnOff);
    TPluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_DESTROYBUTTON ,0,(LPARAM)(&TPluginActionButton));
  }

  return 0;
}

void UpdateButton(bool OnOff)
{
  if(OnOff==true)
  {
    TPluginActionButton.IconIndex = plugin_icon_idx_on;
    TPluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_UPDATEBUTTON,0,(LPARAM)(&TPluginActionButton));
  }
  else
  {
    TPluginActionButton.IconIndex = plugin_icon_idx_off;
    TPluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_UPDATEBUTTON,0,(LPARAM)(&TPluginActionButton));
  }
}


 
