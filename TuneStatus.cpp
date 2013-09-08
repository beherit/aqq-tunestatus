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
#include "stdio.h"
//---------------------------------------------------------------------------

HINSTANCE hInstance; //uchwyt do wtyczki

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
  hInstance = hinst;
  return 1;
}
//---------------------------------------------------------------------------

wchar_t *AnsiTowchar_t(AnsiString Str) //zamiana AnsiString->wchar_t*
{
  wchar_t *str = new wchar_t[Str.WideCharBufSize()];
  return Str.WideChar(str, Str.WideCharBufSize());
}
//---------------------------------------------------------------------------

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
bool EnableFastOnOff;
bool EnablePluginOnStart;

//Pobieranie aktualnego opisu
AnsiString PobierzOpis(AnsiString opis)
{
  TPluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&TPluginStateChange),0);

  opis = TPluginStateChange.Status;

  return opis;
}
//---------------------------------------------------------------------------

//Ustawianie opisu
void UstawOpis(AnsiString opis, bool force)
{
  TPluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&TPluginStateChange),0);

  TPluginStateChange.cbSize = sizeof(PluginStateChange);
  TPluginStateChange.Status = AnsiTowchar_t(opis);
  TPluginStateChange.Force = force;

  TPluginLink.CallService(AQQ_SYSTEM_SETSHOWANDSTATUS,0,(LPARAM)(&TPluginStateChange));
}
//---------------------------------------------------------------------------

//Serwis szybkiego wlaczenia/wylaczenia wtyczki
int __stdcall TuneStatus_FastOnOff (WPARAM, LPARAM)
{
  if(handle==NULL)
  {
    Application->Handle = MainForm;
    handle = new TMainForm(Application);
    handle->aReadSettings->Execute();
    handle->opis_pocz = PobierzOpis(handle->opis_pocz);
    handle->JustEnabled=1;
    handle->Timer->Enabled=true;
    handle->RunPluginCheckBox->Checked=true;
    //Update buttonu
    TPluginActionButton.IconIndex = plugin_icon_idx_on;
    TPluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_UPDATEBUTTON,0,(LPARAM)(&TPluginActionButton));
  }
  else
  {
    if(handle->Timer->Enabled==false)
    {
      handle->opis_pocz = PobierzOpis(handle->opis_pocz);
      handle->JustEnabled=1;
      handle->Timer->Enabled=true;
      //Update buttonu
      TPluginActionButton.IconIndex = plugin_icon_idx_on;
      TPluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_UPDATEBUTTON,0,(LPARAM)(&TPluginActionButton));
    }
    else
    {
      handle->Timer->Enabled=false;
      handle->opisTMP=PobierzOpis(handle->opisTMP);
      if(handle->opis_pocz!=handle->opisTMP)
      {
        UstawOpis(handle->opis_pocz,!handle->SetOnlyInJabberCheck);
        handle->opisTMP="";
      }
      //Update buttonu
      TPluginActionButton.IconIndex = plugin_icon_idx_off;
      TPluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_UPDATEBUTTON,0,(LPARAM)(&TPluginActionButton));
    } 
    handle->RunPluginCheckBox->Checked=!handle->RunPluginCheckBox->Checked;
  }

  return 0;
}
//---------------------------------------------------------------------------

//Serwis otwarcia formy
int __stdcall TuneStatusService (WPARAM, LPARAM)
{
  if(handle==NULL)
  {
  Application->Handle = MainForm;
  handle = new TMainForm(Application);
  handle->Show();
  }
  else
    handle->Show();

  return 0;
}
//---------------------------------------------------------------------------

//Notyfikacja zmiany statusu
int __stdcall OnSetNoteClose (WPARAM wParam, LPARAM lParam)
{
  if(handle!=NULL)
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
//---------------------------------------------------------------------------

//Program
extern "C"  __declspec(dllexport) PluginInfo* __stdcall AQQPluginInfo(DWORD AQQVersion)
{
  //Sprawdzanie wersji AQQ
  if (CompareVersion(AQQVersion,PLUGIN_MAKE_VERSION(2,0,4,69))<0)
  {
    AQQVersion=false;
    Application->MessageBox(
      "Wymagana wesja AQQ przez wtyczkê to minimum 2.0.4.69!\n"
      "Wtyczka TuneStatus nie bêdzie dzia³aæ poprawnie!",
      "Nieprawid³owa wersja AQQ",
      MB_OK | MB_ICONEXCLAMATION);
  }
  TPluginInfo.cbSize = sizeof(PluginInfo);
  TPluginInfo.ShortName = (wchar_t*)L"TuneStatus";
  TPluginInfo.Version = PLUGIN_MAKE_VERSION(1,0,4,8);
  TPluginInfo.Description = (wchar_t *)L"Wstawianie do opisu aktualnie s³uchanego utworu z wielu odtwarzaczy";
  TPluginInfo.Author = (wchar_t *)L"Krzysztof Grochocki (Beherit)";
  TPluginInfo.AuthorMail = (wchar_t *)L"beherit666@vp.pl";
  TPluginInfo.Copyright = (wchar_t *)L"Krzysztof Grochocki (Beherit)";
  TPluginInfo.Homepage = (wchar_t *)L"http://www.beherit.za.pl/";
 
  return &TPluginInfo;
}
//---------------------------------------------------------------------------

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
//---------------------------------------------------------------------------

//Przypisywanie buttonu
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
//---------------------------------------------------------------------------

//Usuwanie buttonu
void UsunButton()
{
  TPluginLink.DestroyServiceFunction(TuneStatus_FastOnOff);
  TPluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_DESTROYBUTTON ,0,(LPARAM)(&TPluginActionButton));

  EnableFastOnOff=0;
}
//---------------------------------------------------------------------------

//Do wypakowywania RES
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
//---------------------------------------------------------------------------

//OnLoad plugin
extern "C" int __declspec(dllexport) __stdcall Load(PluginLink *Link)
{
  TPluginLink = *Link;

  //sciezka katalogu prywatnego uzytkownika
  AnsiString eDir = (wchar_t*)(TPluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,(WPARAM)(hInstance),0));
  eDir = StringReplace(eDir, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);

  if(!DirectoryExists(eDir + "\\\\TuneStatus"))
   CreateDir(eDir + "\\\\TuneStatus");

  //Wypakowanie ikon
  ExtractExe(ID_PNG1,"TuneStatus.png");
  //Przypisanie ikony
  wchar_t* plugin_icon_path = L"TuneStatus.png";
  plugin_icon_idx=TPluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(plugin_icon_path));
  //Usuniecie ikony
  DeleteFile("TuneStatus.png");

  //Wypakowanie ikon
  if(!FileExists(eDir + "\\\\TuneStatus\\\\TuneStatusOff.png"))
   ExtractExe(ID_PNG2,eDir + "\\\\TuneStatus\\\TuneStatusOff.png");
  //Przypisanie ikony
  plugin_icon_path = AnsiTowchar_t(eDir +"\\\\TuneStatus\\\TuneStatusOff.png");
  plugin_icon_idx_off=TPluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(plugin_icon_path));

  //Wypakowanie ikon
  if(!FileExists(eDir + "\\\\TuneStatus\\\TuneStatusOn.png"))
   ExtractExe(ID_PNG3,eDir + "\\\\TuneStatus\\\TuneStatusOn.png");
  //Przypisanie ikony
  plugin_icon_path = AnsiTowchar_t(eDir +"\\\\TuneStatus\\\TuneStatusOn.png");
  plugin_icon_idx_on=TPluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(plugin_icon_path));

  //Przyspisanie przycisku
  PrzypiszSkrot();

  TPluginLink.HookEvent(AQQ_WINDOW_SETNOTE_CLOSE, OnSetNoteClose);

  //Odczyt ustawien
  TIniFile *Ini = new TIniFile(eDir + "\\\\TuneStatus\\\\\TuneStatus.ini");
  EnablePluginOnStart = Ini->ReadInteger("Settings", "EnablePluginOnStart", 0);
  EnableFastOnOff = Ini->ReadInteger("Settings", "EnableFastOnOff", 1);
  delete Ini;

  //Czy w³¹czone
  if(EnablePluginOnStart==1)
  {
    Application->Handle = MainForm;
    handle = new TMainForm(Application);
    handle->opis_pocz = PobierzOpis(handle->opis_pocz);
    handle->aReadSettings->Execute();
    handle->JustEnabled=1;
    handle->Timer->Enabled=true;
    handle->RunPluginCheckBox->Checked=true;
  }

  //Czy przyspisac button
  if(EnableFastOnOff==1)
   PrzypiszButton();

  return 0;
}
//---------------------------------------------------------------------------

//Otwieranie formy przez ustawienia
extern "C" int __declspec(dllexport)__stdcall Settings()
{
  if(handle==NULL)
  {
  Application->Handle = MainForm;
  handle = new TMainForm(Application);
  handle->Show();
  }
  else
    handle->Show();

  return 0;
}
//---------------------------------------------------------------------------

//Pobieranie sciezki katalogu prywatnego uzytkownika
AnsiString GetPluginUserDir(AnsiString Dir)
{
  Dir = (wchar_t*)(TPluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,(WPARAM)(hInstance),0));
  Dir = StringReplace(Dir, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
  return Dir;
}

//Pobieranie sciezki do pliku wtyczki
AnsiString GetPluginDir(AnsiString Dir)
{
  Dir = (wchar_t*)(TPluginLink.CallService(AQQ_FUNCTION_GETPLUGINDIR,(WPARAM)(hInstance),0));
  Dir = StringReplace(Dir, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
  return Dir;
}
//---------------------------------------------------------------------------

//OnUnload plugin
extern "C" int __declspec(dllexport) __stdcall Unload()
{
  if((handle!=NULL)&&(handle->Timer->Enabled==true))
  {
    handle->opisTMP=PobierzOpis(handle->opisTMP);
    if(handle->opis_pocz!=handle->opisTMP)
     UstawOpis(handle->opis_pocz,!handle->SetOnlyInJabberCheck);
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
//---------------------------------------------------------------------------

//Do aktualizowania grafiki buttonu
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
//---------------------------------------------------------------------------


 
