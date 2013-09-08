//---------------------------------------------------------------------------
#include <vcl.h>
#include <windows.h>
#pragma hdrstop
#pragma argsused
#include <inifiles.hpp>
#include <memory>
#include "ikonka.rh"
#include "AqqT.h"
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

//zamiana AnsiString->wchar_t*
wchar_t* AnsiTowchar_t(AnsiString Str)
{                                 
  const char* Text = Str.c_str();
  int size = MultiByteToWideChar(GetACP(), 0, Text, -1, 0,0);
  wchar_t* wbuffer = new wchar_t[size+1];

  MultiByteToWideChar(GetACP(), 0, Text, -1, wbuffer, size+1);

  return wbuffer;
}
//---------------------------------------------------------------------------

//utworzenie obiektow do struktur
TPluginAction PluginAction;
TPluginAction PluginActionButton;
TPluginStateChange PluginStateChange;
TPluginLink PluginLink;
TPluginInfo PluginInfo;

//tworzenie uchwytu do formy
TMainForm *handle;

//Zmienne do ikonu
int plugin_icon_idx;
int plugin_icon_idx_off;
int plugin_icon_idx_on;

//Do buttona
bool EnableFastOnOff;
bool EnablePluginOnStart;

//Do AQQ Radio
AnsiString AQQRadioSong;

//Pobieranie aktualnego opisu
AnsiString PobierzOpis(AnsiString opis)
{
  PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&PluginStateChange),0);

  opis = PluginStateChange.Status;

  return opis;
}
//---------------------------------------------------------------------------

//Ustawianie opisu
void UstawOpis(AnsiString opis, bool force)
{
  PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&PluginStateChange),0);

  PluginStateChange.cbSize = sizeof(TPluginStateChange);
  PluginStateChange.Status = AnsiTowchar_t(opis);
  PluginStateChange.Force = force;

  PluginLink.CallService(AQQ_SYSTEM_SETSHOWANDSTATUS,0,(LPARAM)(&PluginStateChange));
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
    handle->SongTimer->Interval=1000;
    handle->JustEnabled=1;
    handle->Timer->Enabled=true;
    handle->RunPluginCheckBox->Checked=true;
    //Update buttonu
    PluginActionButton.IconIndex = plugin_icon_idx_on;
    PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_UPDATEBUTTON,0,(LPARAM)(&PluginActionButton));
  }
  else
  {
    if(handle->Timer->Enabled==false)
    {
      handle->opis_pocz = PobierzOpis(handle->opis_pocz);
      handle->SongTimer->Interval=1000;
      handle->JustEnabled=1;
      handle->Timer->Enabled=true;
      //Update buttonu
      PluginActionButton.IconIndex = plugin_icon_idx_on;
      PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_UPDATEBUTTON,0,(LPARAM)(&PluginActionButton));
    }
    else
    {
      handle->SongTimer->Enabled=false;
      handle->Timer->Enabled=false;
      handle->opisTMP=PobierzOpis(handle->opisTMP);
      if(handle->opis_pocz!=handle->opisTMP)
      {
        UstawOpis(handle->opis_pocz,!handle->SetOnlyInJabberCheck);
        handle->opisTMP="";
      }
      //Update buttonu
      PluginActionButton.IconIndex = plugin_icon_idx_off;
      PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_UPDATEBUTTON,0,(LPARAM)(&PluginActionButton));
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

//Notyfikacja AQQ Radio
int __stdcall OnCurrentSong (WPARAM wParam, LPARAM lParam)
{
  TPluginSong *Song = (TPluginSong*)lParam;
  AQQRadioSong = (wchar_t*)(Song->Title);
  int Length = AQQRadioSong.Length();
  if(Length!=0)
    return 0;
  else
    return 1;
}
//---------------------------------------------------------------------------

AnsiString GetAQQRadioSong(AnsiString Song)
{
  Song=AQQRadioSong;
  return Song;
}

//Notyfikacja zmiany statusu
int __stdcall OnSetNote(WPARAM wParam, LPARAM lParam)
{
  if(handle!=NULL)
  {
    TPluginStateChange *StateChange = (TPluginStateChange*)lParam;
    AnsiString nowy_opis = (wchar_t*)(StateChange->Status);

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
extern "C" __declspec(dllexport) TPluginInfo* __stdcall AQQPluginInfo(DWORD AQQVersion)
{
  //Sprawdzanie wersji AQQ
  /*if (CompareVersion(AQQVersion,PLUGIN_MAKE_VERSION(2,0,4,69))<0)
  {
    AQQVersion=false;
    Application->MessageBox(
      "Wymagana wesja AQQ przez wtyczkê to minimum 2.0.4.69!\n"
      "Wtyczka TuneStatus nie bêdzie dzia³aæ poprawnie!",
      "Nieprawid³owa wersja AQQ",
      MB_OK | MB_ICONEXCLAMATION);
  }*/
  PluginInfo.cbSize = sizeof(TPluginInfo);
  PluginInfo.ShortName = (wchar_t*)L"TuneStatus";
  PluginInfo.Version = PLUGIN_MAKE_VERSION(1,0,4,14);
  PluginInfo.Description = (wchar_t *)L"Wstawianie do opisu aktualnie s³uchanego utworu z wielu odtwarzaczy";
  PluginInfo.Author = (wchar_t *)L"Krzysztof Grochocki (Beherit)";
  PluginInfo.AuthorMail = (wchar_t *)L"beherit666@vp.pl";
  PluginInfo.Copyright = (wchar_t *)L"Krzysztof Grochocki (Beherit)";
  PluginInfo.Homepage = (wchar_t *)L"http://www.beherit.za.pl";
 
  return &PluginInfo;
}
//---------------------------------------------------------------------------

//Przypisywanie skrótu w popPlugins
void PrzypiszSkrot()
{
  PluginAction.cbSize = sizeof(TPluginAction);
  PluginAction.pszName = (wchar_t*)L"TuneStatusServiceButton";
  PluginAction.pszCaption = (wchar_t*) L"TuneStatus";
  PluginAction.Position = 0;
  PluginAction.IconIndex = plugin_icon_idx;
  PluginAction.pszService = (wchar_t*) L"serwis_TuneStatusService";
  PluginAction.pszPopupName = (wchar_t*) L"popPlugins";
  PluginAction.PopupPosition = 0;

  PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&PluginAction));
  PluginLink.CreateServiceFunction(L"serwis_TuneStatusService",TuneStatusService);
}
//---------------------------------------------------------------------------

//Przypisywanie buttonu
void PrzypiszButton()
{
  PluginActionButton.cbSize = sizeof(TPluginAction);
  PluginActionButton.pszName = (wchar_t*) L"TuneStatus_FastOnOff";
  PluginActionButton.Position = 999;
  if(EnablePluginOnStart==1)
   PluginActionButton.IconIndex = plugin_icon_idx_on;
  else
   PluginActionButton.IconIndex = plugin_icon_idx_off;
  PluginActionButton.pszService = (wchar_t*) L"serwis_TuneStatus_FastOnOff";

  PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_CREATEBUTTON,0,(LPARAM)(&PluginActionButton));
  PluginLink.CreateServiceFunction(L"serwis_TuneStatus_FastOnOff",TuneStatus_FastOnOff);

  EnableFastOnOff=1;
}
//---------------------------------------------------------------------------

//Usuwanie buttonu
void UsunButton()
{
  PluginLink.DestroyServiceFunction(TuneStatus_FastOnOff);
  PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_DESTROYBUTTON ,0,(LPARAM)(&PluginActionButton));

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
extern "C" int __declspec(dllexport) __stdcall Load(TPluginLink *Link)
{
  PluginLink = *Link;

  //sciezka katalogu prywatnego uzytkownika
  AnsiString eDir = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,(WPARAM)(hInstance),0));
  eDir = StringReplace(eDir, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);

  if(!DirectoryExists(eDir + "\\\\TuneStatus"))
   CreateDir(eDir + "\\\\TuneStatus");

  //Wypakowanie ikon
  ExtractExe(ID_PNG1,"TuneStatus.png");
  //Przypisanie ikony
  wchar_t* plugin_icon_path = L"TuneStatus.png";
  plugin_icon_idx=PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(plugin_icon_path));
  //Usuniecie ikony
  DeleteFile("TuneStatus.png");

  //Wypakowanie ikon
  if(!FileExists(eDir + "\\\\TuneStatus\\\\TuneStatusOff.png"))
   ExtractExe(ID_PNG2,eDir + "\\\\TuneStatus\\\TuneStatusOff.png");
  //Przypisanie ikony
  plugin_icon_path = AnsiTowchar_t(eDir +"\\\\TuneStatus\\\TuneStatusOff.png");
  plugin_icon_idx_off=PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(plugin_icon_path));

  //Wypakowanie ikon
  if(!FileExists(eDir + "\\\\TuneStatus\\\TuneStatusOn.png"))
   ExtractExe(ID_PNG3,eDir + "\\\\TuneStatus\\\TuneStatusOn.png");
  //Przypisanie ikony
  plugin_icon_path = AnsiTowchar_t(eDir +"\\\\TuneStatus\\\TuneStatusOn.png");
  plugin_icon_idx_on=PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(plugin_icon_path));

  //Przyspisanie przycisku
  PrzypiszSkrot();

  //Hook dla zmiany opisu
  PluginLink.HookEvent(AQQ_SYSTEM_SETNOTE,OnSetNote);
  //Hook dla AQQ Radio
  PluginLink.HookEvent(AQQ_SYSTEM_CURRENTSONG,OnCurrentSong);

  //Odczyt ustawien
  TIniFile *Ini = new TIniFile(eDir + "\\\\TuneStatus\\\\\TuneStatus.ini");
  EnablePluginOnStart = Ini->ReadBool("Settings", "EnablePluginOnStart", 0);
  EnableFastOnOff = Ini->ReadBool("Settings", "EnableFastOnOff", 1);
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
  Dir = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,(WPARAM)(hInstance),0));
  Dir = StringReplace(Dir, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
  return Dir;
}

//Pobieranie sciezki do pliku wtyczki
AnsiString GetPluginDir(AnsiString Dir)
{
  Dir = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINDIR,(WPARAM)(hInstance),0));
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
  PluginLink.UnhookEvent(OnSetNote);
  PluginLink.UnhookEvent(OnCurrentSong);
  PluginLink.DestroyServiceFunction(TuneStatusService);
  PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM,0,(LPARAM)(&PluginAction));
  PluginLink.CallService(AQQ_ICONS_DESTROYPNGICON,0,(LPARAM)(plugin_icon_idx));
  PluginLink.CallService(AQQ_ICONS_DESTROYPNGICON,0,(LPARAM)(plugin_icon_idx_off));
  PluginLink.CallService(AQQ_ICONS_DESTROYPNGICON,0,(LPARAM)(plugin_icon_idx_on));
  if(EnableFastOnOff==1)
  {
    PluginLink.DestroyServiceFunction(TuneStatus_FastOnOff);
    PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_DESTROYBUTTON ,0,(LPARAM)(&PluginActionButton));
  }

  return 0;
}
//---------------------------------------------------------------------------

//Do aktualizowania grafiki buttonu
void UpdateButton(bool OnOff)
{
  if(OnOff==true)
  {
    PluginActionButton.IconIndex = plugin_icon_idx_on;
    PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_UPDATEBUTTON,0,(LPARAM)(&PluginActionButton));
  }
  else
  {
    PluginActionButton.IconIndex = plugin_icon_idx_off;
    PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_UPDATEBUTTON,0,(LPARAM)(&PluginActionButton));
  }
}
//---------------------------------------------------------------------------



