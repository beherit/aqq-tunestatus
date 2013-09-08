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
//#include "stdio.h"
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
TPluginShowInfo PluginShowInfo;
TPluginContactSimpleInfo PluginContactSimpleInfo;
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
PPluginSong Song;

//Do User Tune
AnsiString UserTuneSong;
UTF8String UserTuneSongUTF8;
AnsiString UserTuneSongTMP;
AnsiString UserTuneFrom;
AnsiString UserTuneFromTMP;
AnsiString UserTuneTo;
int IsThereUserTune;
AnsiString UserTuneIconPath;
bool AllowUserTuneN=false;
int UserTuneTimeOut;
bool UserTuneEnableN;
bool UserTuneEnableS;

//Pobieranie aktualnego opisu
AnsiString GetStatus(AnsiString opis)
{
  PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&PluginStateChange),0);

  opis = PluginStateChange.Status;

  return opis;
}
//---------------------------------------------------------------------------

//Ustawianie opisu
void SetStatus(AnsiString opis, bool force)
{
  PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&PluginStateChange),0);

  PluginStateChange.cbSize = sizeof(TPluginStateChange);
  PluginStateChange.Status = AnsiTowchar_t(opis);
  PluginStateChange.Force = force;

  PluginLink.CallService(AQQ_SYSTEM_SETSHOWANDSTATUS,0,(LPARAM)(&PluginStateChange));
}
//---------------------------------------------------------------------------

bool AllowChangeStatus(bool WithInvisible) //Sprawdzanie stanu czy rozny od rozlaczony/niewidoczny
{
  if(WithInvisible==1)
  {
    PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&PluginStateChange),0);

    PluginStateChange.cbSize = sizeof(TPluginStateChange);
    if((PluginStateChange.OldState!=0)&&(PluginStateChange.OldState!=6))
     return true;
    else
     return false;
  }
  else
  {
    PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&PluginStateChange),0);

    PluginStateChange.cbSize = sizeof(TPluginStateChange);
    if(PluginStateChange.OldState!=0)
     return true;
    else
     return false;
  }
}
//---------------------------------------------------------------------------

bool AllowChangeStatusUserTune(int Account) //Sprawdzanie stanu czy rozny od rozlaczony/niewidoczny
{
  PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&PluginStateChange),Account);

  PluginStateChange.cbSize = sizeof(TPluginStateChange);
  if((PluginStateChange.OldState!=0)&&(PluginStateChange.OldState!=6))
   return true;
  else
   return false;
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
    handle->opis_pocz = GetStatus(handle->opis_pocz);
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
      handle->opis_pocz = GetStatus(handle->opis_pocz);
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
      handle->opisTMP=GetStatus(handle->opisTMP);
      if(handle->opis_pocz!=handle->opisTMP)
      {
        SetStatus(handle->opis_pocz,!handle->SetOnlyInJabberCheck);
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
  Song = (PPluginSong)lParam;
  AQQRadioSong = (wchar_t*)(Song->Title);
  if(AQQRadioSong.Length()!=0)
    return 0;
  else
    return 1;
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

//Notyfikacja otrzymywanych/wychodzacych pakietow XML
int __stdcall OnXMLDebug (WPARAM wParam, LPARAM lParam)
{
  if((AllowUserTuneN==true)&&(UserTuneEnableN==true))
  {
    UserTuneSong = (wchar_t*)(wParam);

    int IsThereUserTune = AnsiPos("<tune xmlns='http://jabber.org/protocol/tune'>", UserTuneSong);
    if(IsThereUserTune>0)
    {
      IsThereUserTune = AnsiPos("<title>", UserTuneSong);
      if(IsThereUserTune>0)
      {
        IsThereUserTune = AnsiPos("type='error'", UserTuneSong);
        if(IsThereUserTune==0)
        {
          IsThereUserTune = AnsiPos("<message from", UserTuneSong);
          if(IsThereUserTune>0)
          {
            UserTuneFrom=UserTuneSong;
            UserTuneTo=UserTuneSong;

            IsThereUserTune = AnsiPos("from='", UserTuneFrom);
            UserTuneFrom.Delete(1,IsThereUserTune+5);
            IsThereUserTune = AnsiPos("'", UserTuneFrom);
            UserTuneFrom.Delete(IsThereUserTune,UserTuneFrom.Length());
            UserTuneFrom=UserTuneFrom.Trim();

            IsThereUserTune = AnsiPos("to='", UserTuneTo);
            UserTuneTo.Delete(1,IsThereUserTune+3);
            IsThereUserTune = AnsiPos("'", UserTuneTo);
            UserTuneTo.Delete(IsThereUserTune,UserTuneTo.Length());
            IsThereUserTune = AnsiPos("/", UserTuneTo);
            if(IsThereUserTune>0)
             UserTuneTo.Delete(IsThereUserTune,UserTuneTo.Length());
            UserTuneTo=UserTuneTo.Trim();

            if(UserTuneFrom!=UserTuneTo)
            {
              IsThereUserTune = AnsiPos("<artist>", UserTuneSong);
              if(IsThereUserTune>0) //poprawne kodowanie
              {
                UserTuneSongTMP=UserTuneSong;
                IsThereUserTune = AnsiPos("<artist>", UserTuneSongTMP);
                UserTuneSongTMP.Delete(1,IsThereUserTune+7);
                IsThereUserTune = AnsiPos("</artist>", UserTuneSongTMP);
                UserTuneSongTMP.Delete(IsThereUserTune, UserTuneSongTMP.Length());
                UserTuneSongTMP=UserTuneSongTMP.Trim();

                IsThereUserTune = AnsiPos("<title>", UserTuneSong);
                UserTuneSong.Delete(1,IsThereUserTune+6);
                IsThereUserTune = AnsiPos("</title>", UserTuneSong);
                UserTuneSong.Delete(IsThereUserTune, UserTuneSong.Length());
                UserTuneSong=UserTuneSong.Trim();

                UserTuneSong = UserTuneSongTMP + " - " + UserTuneSong;
              }
              else //kodowanie w AQQ
              {
                IsThereUserTune = AnsiPos("<title>", UserTuneSong);
                UserTuneSong.Delete(1,IsThereUserTune+6);
                IsThereUserTune = AnsiPos("</title>", UserTuneSong);
                UserTuneSong.Delete(IsThereUserTune, UserTuneSong.Length());
                UserTuneSong=UserTuneSong.Trim();
              }

              //poprawa kodowania
              UserTuneSongUTF8 = UserTuneSong;
              UserTuneSong=Utf8ToAnsi(UserTuneSongUTF8);
              UserTuneSong = StringReplace(UserTuneSong, "&apos;", "'", TReplaceFlags() << rfReplaceAll);
              UserTuneSong = StringReplace(UserTuneSong, "&amp;", "&", TReplaceFlags() << rfReplaceAll);

              //Usuwanie "*** "
              if(AnsiPos("*** ", UserTuneSong)>0)
              {
                UserTuneSong.Delete(1, AnsiPos("*** ", UserTuneSong) + 3);
                UserTuneSong=UserTuneSong.Trim();
              }

              //Usuwanie numeru piosenki
              if((AnsiPos(". ", UserTuneSong)>0)&&(AnsiPos(". ", UserTuneSong)<6))
              {
                UserTuneSongTMP=UserTuneSong;
                UserTuneSongTMP.Delete(AnsiPos(". ", UserTuneSong), UserTuneSongTMP.Length());
                if(TestDigit(UserTuneSongTMP)==true)
                {
                  UserTuneSong.Delete(1, AnsiPos(". ", UserTuneSong));
                  UserTuneSong=UserTuneSong.Trim();
                }
              }

              //Usuwanie "Wtyczka AQQ Radio: "
              if(AnsiPos("Wtyczka AQQ Radio: ", UserTuneSong)>0)
               UserTuneSong.Delete(1,AnsiPos("Wtyczka AQQ Radio: ", UserTuneSong)+18);

              //Pobieranie nick'a z JID
              UserTuneFromTMP = UserTuneFrom;
              PluginContactSimpleInfo.cbSize = sizeof(TPluginContactSimpleInfo);
              PluginContactSimpleInfo.JID = AnsiTowchar_t(UserTuneFrom);
              PluginLink.CallService(AQQ_CONTACTS_FILLSIMPLEINFO,0,(LPARAM)(&PluginContactSimpleInfo));
              UserTuneFrom = (wchar_t*)(PluginContactSimpleInfo.Nick);
              if(UserTuneFrom=="")
               UserTuneFrom = UserTuneFromTMP;

              UserTuneIconPath = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPNG_FILEPATH,76,0));

              //Chmurka "kto slucha"
              PluginShowInfo.cbSize = sizeof(TPluginShowInfo);
              PluginShowInfo.Event = tmeInfo;
              PluginShowInfo.Text = AnsiTowchar_t(UserTuneFrom + " s³ucha:");
              PluginShowInfo.ImagePath = AnsiTowchar_t(UserTuneIconPath);
              PluginShowInfo.TimeOut = 1000 * UserTuneTimeOut;
              PluginLink.CallService(AQQ_FUNCTION_SHOWINFO,0,(LPARAM)(&PluginShowInfo));
              //Chmurka "co slucha"
              PluginShowInfo.cbSize = sizeof(TPluginShowInfo);
              PluginShowInfo.Event = tmeInfo;
              PluginShowInfo.Text = AnsiTowchar_t(UserTuneSong);
              PluginShowInfo.ImagePath = L"";
              PluginShowInfo.TimeOut = 1000 * UserTuneTimeOut;
              PluginLink.CallService(AQQ_FUNCTION_SHOWINFO,0,(LPARAM)(&PluginShowInfo));
            }
          }
        }
      }
    }
  }

  return 0;
}
//---------------------------------------------------------------------------

//Zmiana ustawien User Tune
void ChangeUserTune(bool Enabled, int Value)
{
  UserTuneEnableN = Enabled;
  UserTuneTimeOut = Value;
}
//---------------------------------------------------------------------------

int __stdcall OnModulesLoaded(WPARAM, LPARAM)
{
  if(handle==NULL)
  {
    //Tworzenie uchwytu do formy
    Application->Handle = MainForm;
    handle = new TMainForm(Application);
  }
  handle->aReadSettings->Execute();
  handle->AllowUserTuneNTimer->Enabled=true;

  return 0;
}
//---------------------------------------------------------------------------

void SetAllowUserTuneNTrue()
{
  AllowUserTuneN=true;
}
//---------------------------------------------------------------------------

AnsiString GetAQQRadioSong(AnsiString Song)
{
  Song=AQQRadioSong;
  return Song;
}
//---------------------------------------------------------------------------

//Notyfikacja zmiany statusu
int __stdcall OnSetNote(WPARAM wParam, LPARAM lParam)
{
  if(handle!=NULL)
  {
    PPluginStateChange StateChange = (PPluginStateChange)lParam;
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

//Notyfikacja zmiany stanu
int __stdcall OnStateChange(WPARAM wParam, LPARAM lParam)
{
  if((handle!=NULL)&&(handle->Timer->Enabled==true))
  {
    if(AllowChangeStatus(handle->BlockInvisibleCheck)==0)
    {
      handle->SongTimer->Enabled=false;
      handle->Timer->Enabled=false;
      
      handle->opisTMP=GetStatus(handle->opisTMP);
      if(handle->opis_pocz!=handle->opisTMP)
      {
        SetStatus(handle->opis_pocz,!handle->SetOnlyInJabberCheck);
        handle->opisTMP="";
      }

      handle->SongTimer->Enabled=true;
      handle->Timer->Enabled=true;
    }
  }

  return 0;
}
//---------------------------------------------------------------------------

//Program
extern "C" __declspec(dllexport) PPluginInfo __stdcall AQQPluginInfo(DWORD AQQVersion)
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
  PluginInfo.Version = PLUGIN_MAKE_VERSION(1,0,6,0);
  PluginInfo.Description = (wchar_t *)L"Wstawianie do opisu aktualnie s³uchanego utworu z wielu odtwarzaczy";
  PluginInfo.Author = (wchar_t *)L"Krzysztof Grochocki (Beherit)";
  PluginInfo.AuthorMail = (wchar_t *)L"beherit666@vp.pl";
  PluginInfo.Copyright = (wchar_t *)L"Krzysztof Grochocki (Beherit)";
  PluginInfo.Homepage = (wchar_t *)L"www.beherit.za.pl";
 
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
extern "C" int __declspec(dllexport) __stdcall Load(PPluginLink Link)
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
  //Hook dla zmiany stanu
  PluginLink.HookEvent(AQQ_SYSTEM_STATECHANGE,OnStateChange);
  //User tune
  PluginLink.HookEvent(AQQ_SYSTEM_XMLDEBUG,OnXMLDebug);
  //Po zaladowaniu wtyczek
  PluginLink.HookEvent(AQQ_SYSTEM_MODULESLOADED, OnModulesLoaded);
  AllowUserTuneN=PluginLink.CallService(AQQ_SYSTEM_MODULESLOADED,0,0);

  //Odczyt ustawien
  TIniFile *Ini = new TIniFile(eDir + "\\\\TuneStatus\\\\\TuneStatus.ini");
  EnablePluginOnStart = Ini->ReadBool("Settings", "EnablePluginOnStart", 0);
  EnableFastOnOff = Ini->ReadBool("Settings", "EnableFastOnOff", 1);
  UserTuneEnableN = Ini->ReadBool("UserTune", "EnableN", 0);
  UserTuneEnableS = Ini->ReadBool("UserTune", "EnableS", 0);
  UserTuneTimeOut = Ini->ReadInteger("UserTune", "TimeOutN", 4);
  delete Ini;      

  if(UserTuneEnableS==1)
  {
    if(handle==NULL)
    {
      //Tworzenie uchwytu do formy
      Application->Handle = MainForm;
      handle = new TMainForm(Application);
    }
    handle->aReadSettings->Execute();
    handle->UserTuneTimer->Enabled=true;
  }

  //Czy w³¹czonac na starcie?
  if(EnablePluginOnStart==1)
  {
    if(handle==NULL)
    {
      //Tworzenie uchwytu do formy
      Application->Handle = MainForm;
      handle = new TMainForm(Application);
    }
    handle->opis_pocz = GetStatus(handle->opis_pocz);
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
//---------------------------------------------------------------------------

//Pobieranie sciezki do pliku wtyczki
AnsiString GetPluginDir(AnsiString Dir)
{
  Dir = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINDIR,(WPARAM)(hInstance),0));
  Dir = StringReplace(Dir, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
  return Dir;
}
//---------------------------------------------------------------------------

//Ustawianie UserTune
void SetUserTune(AnsiString Tune, AnsiString XML, int Count, int Account, AnsiString AccountName)
{
  Tune = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_CONVERTTOXML,0,(WPARAM)(AnsiTowchar_t(Tune))));

  Count = PluginLink.CallService(AQQ_FUNCTION_GETUSEREXCOUNT,0,0);
  for(Account=0;Account<Count;Account++)
  {
    if(AllowChangeStatusUserTune(Account)==true)
    {
      if(Tune!="")
       XML = "<iq type=\"set\" from=\"CC_JID\" id=\"CC_SESSION\"><pubsub xmlns=\"http://jabber.org/protocol/pubsub\"><publish node=\"http://jabber.org/protocol/tune\"><item><tune xmlns=\"http://jabber.org/protocol/tune\"><artist/><length/><rating/><source/><title>CC_TUNESTATUS</title><track/><uri/></tune></item></publish></pubsub></iq>";
      else
       XML = "<iq type=\"set\" from=\"CC_JID\" id=\"CC_SESSION\"><pubsub xmlns=\"http://jabber.org/protocol/pubsub\"><publish node=\"http://jabber.org/protocol/tune\"><item><tune xmlns=\"http://jabber.org/protocol/tune\"/></item></publish></pubsub></iq>";

      XML = StringReplace(XML, "CC_SESSION", ((wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETSTRID,0,0))), TReplaceFlags());
      XML = StringReplace(XML, "CC_TUNESTATUS", Tune, TReplaceFlags());

      PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&PluginStateChange),Account);
      AccountName = (AnsiString)((wchar_t*)(PluginStateChange.JID)) + "/" + (AnsiString)((wchar_t*)(PluginStateChange.Resource));

      XML = StringReplace(XML, "CC_JID", AccountName, TReplaceFlags() << rfReplaceAll);

      PluginLink.CallService(AQQ_SYSTEM_SENDXML,(WPARAM)(AnsiTowchar_t(XML)),Account);
    }
    else
    {
      XML = "<iq type=\"set\" from=\"CC_JID\" id=\"CC_SESSION\"><pubsub xmlns=\"http://jabber.org/protocol/pubsub\"><publish node=\"http://jabber.org/protocol/tune\"><item><tune xmlns=\"http://jabber.org/protocol/tune\"/></item></publish></pubsub></iq>";

      XML = StringReplace(XML, "CC_SESSION", ((wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETSTRID,0,0))), TReplaceFlags());

      PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&PluginStateChange),Account);
      AccountName = (AnsiString)((wchar_t*)(PluginStateChange.JID)) + "/" + (AnsiString)((wchar_t*)(PluginStateChange.Resource));
      XML = StringReplace(XML, "CC_JID", AccountName, TReplaceFlags() << rfReplaceAll);

      PluginLink.CallService(AQQ_SYSTEM_SENDXML,(WPARAM)(AnsiTowchar_t(XML)),Account);
    }
  }
}
//---------------------------------------------------------------------------

//OnUnload plugin
extern "C" int __declspec(dllexport) __stdcall Unload()
{
  if((handle!=NULL)&&(handle->Timer->Enabled==true))
  {
    handle->opisTMP=GetStatus(handle->opisTMP);
    if(handle->opis_pocz!=handle->opisTMP)
     SetStatus(handle->opis_pocz,!handle->SetOnlyInJabberCheck);
  }
  if((handle!=NULL)&&(handle->UserTuneTimer->Enabled==true))
   SetUserTune("", NULL, NULL, NULL, NULL);
  PluginLink.UnhookEvent(OnSetNote);
  PluginLink.UnhookEvent(OnCurrentSong);
  PluginLink.UnhookEvent(OnStateChange);
  PluginLink.UnhookEvent(OnXMLDebug);
  PluginLink.UnhookEvent(OnModulesLoaded);
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



