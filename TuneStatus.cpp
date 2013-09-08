//---------------------------------------------------------------------------
#include <vcl.h>
#include <windows.h>
#pragma hdrstop
#pragma argsused
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
PluginStateChange TPluginStateChange;
PluginLink TPluginLink;
PluginInfo TPluginInfo;

//tworzenie uchwytu do formy
TMainForm *handle;

//Zmienna opisu
AnsiString nowy_opis;

//Zmienne do ikonu
int plugin_icon_idx;

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
  TPluginInfo.Version = PLUGIN_MAKE_VERSION(1,0,2,1);
  TPluginInfo.Description = (wchar_t *)L"";
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

  //Wypakowanie ikon
  ExtractExe(ID_PNG,"TuneStatus.png");

  //Przypisanie ikony
  wchar_t* plugin_icon_path = L"TuneStatus.png";
  plugin_icon_idx=TPluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(plugin_icon_path));

  //Usuniecie ikony
  DeleteFile("TuneStatus.png");

  //Przyspisanie przycisku
  PrzypiszSkrot();

  TPluginLink.HookEvent(AQQ_WINDOW_SETNOTE_CLOSE, OnSetNoteClose);

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

AnsiString PobierzOpis(AnsiString opis)
{
  TPluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&TPluginStateChange),0);

  opis = TPluginStateChange.Status;

  return opis;
}

void UstawOpis(AnsiString opis)
{
  TPluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&TPluginStateChange),0);

  TPluginStateChange.cbSize = sizeof(PluginStateChange);
  TPluginStateChange.Status = AnsiTowchar_t(opis);
  TPluginStateChange.Force = true;

  TPluginLink.CallService(AQQ_SYSTEM_SETSHOWANDSTATUS,0,(LPARAM)(&TPluginStateChange));
}

extern "C" int __declspec(dllexport) __stdcall Unload()
{
  if (handle!=NULL)
  {
    if(handle->opis_pocz!="")
     UstawOpis(handle->opis_pocz);
  }
  TPluginLink.UnhookEvent(OnSetNoteClose);
  TPluginLink.DestroyServiceFunction(TuneStatusService);
  TPluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM,0,(LPARAM)(&TPluginAction));
  TPluginLink.CallService(AQQ_ICONS_DESTROYPNGICON,0,(LPARAM)(plugin_icon_idx));

  return 0;
}


 
