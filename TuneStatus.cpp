//---------------------------------------------------------------------------
#include <vcl.h>
#include <windows.h>
#pragma hdrstop
#pragma argsused
#include <inifiles.hpp>
#include <memory>
#include "Aqq.h"
#include "MainFrm.h"
//---------------------------------------------------------------------------

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
  return 1;
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
TSaveSetup SaveSetup;

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
UnicodeString AQQRadioSong;
PPluginSong Song;

//Do User Tune
UnicodeString UserTuneSong;
UnicodeString UserTuneSongTMP;
UnicodeString UserTuneFrom;
UnicodeString UserTuneFromTMP;
UnicodeString UserTuneTo;
int IsThereUserTune;
UnicodeString UserTuneIconPath;
bool AllowUserTuneN=false;
int UserTuneTimeOut;
bool UserTuneEnableN;
bool UserTuneEnableS;
UnicodeString ExceptionJID[50];
int ExceptionCount;
bool AllowException;

//do blokowania zmiany opisu
UnicodeString nowy_opis;

//Pobieranie aktualnego opisu
UnicodeString GetStatus()
{
  UnicodeString opis;
  PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&PluginStateChange),0);
  opis = (wchar_t*)PluginStateChange.Status;
  return opis;
}
//---------------------------------------------------------------------------

//Ustawianie opisu
void SetStatus(UnicodeString opis, bool force)
{
  //opis = opis.w_str();

  PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&PluginStateChange),0);

  PluginStateChange.cbSize = sizeof(TPluginStateChange);
  PluginStateChange.Status = opis.c_str();
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
	if((PluginStateChange.NewState!=0)&&(PluginStateChange.NewState!=6))
     return true;
    else
     return false;
  }
  else
  {
    PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&PluginStateChange),0);

    PluginStateChange.cbSize = sizeof(TPluginStateChange);
	if(PluginStateChange.NewState!=0)
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
	Application->Handle = (HWND__*)MainForm;
    handle = new TMainForm(Application);
	handle->aReadSettings->Execute();
  }

  if(handle->Timer->Enabled==false)
  {
	handle->opis_pocz = GetStatus();
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
	handle->opisTMP=GetStatus();
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

  return 0;
}
//---------------------------------------------------------------------------

//Serwis otwarcia formy
int __stdcall TuneStatusService (WPARAM, LPARAM)
{
  if(handle==NULL)
  {
	Application->Handle = (HWND__*)MainForm;
	handle = new TMainForm(Application);
  }

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
			  AllowException=true;

			  if(ExceptionCount!=0)
			  {
				for(int Count=0;Count<ExceptionCount;Count++)
                {
				  if(UserTuneFrom==ExceptionJID[Count])
				   AllowException=false;
                }
              }

			  if(AllowException==true)
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
				UserTuneSong = UTF8ToUnicodeString(UserTuneSong.t_str());
				//UserTuneSong = UTF8ToUnicodeString((const char *)UserTuneSong.t_str());
				UserTuneSong = StringReplace(UserTuneSong, "&apos;", "'", TReplaceFlags() << rfReplaceAll);
				UserTuneSong = StringReplace(UserTuneSong, "&quot;", "\"", TReplaceFlags() << rfReplaceAll);
				UserTuneSong = StringReplace(UserTuneSong, "&amp;", "&", TReplaceFlags() << rfReplaceAll);
				UserTuneSong = StringReplace(UserTuneSong, "&gt;", ">", TReplaceFlags() << rfReplaceAll);
				UserTuneSong = StringReplace(UserTuneSong, "&lt;", "<", TReplaceFlags() << rfReplaceAll);

				//Usuwanie "*** "
				if(AnsiPos("*** ", UserTuneSong)>0)
				{
				  UserTuneSong.Delete(1, AnsiPos("*** ", UserTuneSong) + 3);
				  UserTuneSong=UserTuneSong.Trim();
				}

				//Usuwanie numeru piosenki
				if((AnsiPos(". ", UserTuneSong)>0)&&(AnsiPos(". ", UserTuneSong)<7))
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
				PluginContactSimpleInfo.JID = UserTuneFrom.w_str();
				PluginLink.CallService(AQQ_CONTACTS_FILLSIMPLEINFO,0,(LPARAM)(&PluginContactSimpleInfo));
				UserTuneFrom = (wchar_t*)(PluginContactSimpleInfo.Nick);
				if(UserTuneFrom=="")
				 UserTuneFrom = UserTuneFromTMP;

				UserTuneIconPath = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPNG_FILEPATH,76,0));

				//Chmurka "kto slucha"
				PluginShowInfo.cbSize = sizeof(TPluginShowInfo);
				PluginShowInfo.Event = tmeInfo;
				PluginShowInfo.Text = (UserTuneFrom + " s³ucha:").w_str();
				PluginShowInfo.ImagePath = UserTuneIconPath.w_str();
				PluginShowInfo.TimeOut = 1000 * UserTuneTimeOut;
				PluginLink.CallService(AQQ_FUNCTION_SHOWINFO,0,(LPARAM)(&PluginShowInfo));
				//Chmurka "co slucha"
				PluginShowInfo.cbSize = sizeof(TPluginShowInfo);
				PluginShowInfo.Event = tmeInfo;
				PluginShowInfo.Text = UserTuneSong.w_str();
				PluginShowInfo.ImagePath = L"";
				PluginShowInfo.TimeOut = 1000 * UserTuneTimeOut;
				PluginLink.CallService(AQQ_FUNCTION_SHOWINFO,0,(LPARAM)(&PluginShowInfo));
			  }
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
	Application->Handle = (HWND__*)MainForm;
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

UnicodeString GetAQQRadioSong()
{
  UnicodeString Song;
  Song=AQQRadioSong;
  return Song;
}
//---------------------------------------------------------------------------

//Notyfikacja zmiany statusu
int __stdcall OnPreSetNote(WPARAM wParam, LPARAM lParam)
{
  if((handle!=NULL)&&(handle->Timer->Enabled==true))
  {
	nowy_opis = (wchar_t*)(lParam);

	if((nowy_opis!=handle->opis_pocz)
	&&(nowy_opis!=handle->opisTMP)
	&&(nowy_opis!=handle->opis)
	&&(handle->opis!=""))
	{
	  handle->opis_pocz = nowy_opis;
	  return 1;
	}
	else
	 return 0;
  }
  else
   return 0;
}
//---------------------------------------------------------------------------

//Notyfikacja zmiany stanu
int __stdcall OnStateChange(WPARAM wParam, LPARAM lParam)
{
  if((handle!=NULL)&&(handle->Timer->Enabled==true)&&(handle->opis!=""))
  {
	if(AllowChangeStatus(handle->BlockInvisibleCheck)==0)
	{
	  handle->SongTimer->Enabled=false;
	  handle->Timer->Enabled=false;

	  handle->opisTMP=GetStatus();
	  if(handle->opis_pocz!=handle->opisTMP)
	  {
		SetStatus(handle->opis_pocz,!handle->SetOnlyInJabberCheck);
		handle->opisTMP="";
	  }

	  handle->Timer->Enabled=true;
	}
  }

  return 0;
}
//---------------------------------------------------------------------------

extern "C" __declspec(dllexport) PPluginInfo __stdcall AQQPluginInfo(DWORD AQQVersion)
{
  //Sprawdzanie wersji AQQ
  if (CompareVersion(AQQVersion,PLUGIN_MAKE_VERSION(2,1,0,20))<0)
  {
	AQQVersion=false;
	MessageBox(Application->Handle,
	  "Wymagana wesja AQQ przez wtyczkê to minimum 2.1.0.20!\n"
	  "Wtyczka TuneStatus nie bêdzie dzia³aæ poprawnie!",
	  "Nieprawid³owa wersja AQQ",
	  MB_OK | MB_ICONEXCLAMATION);
  }
  PluginInfo.cbSize = sizeof(TPluginInfo);
  PluginInfo.ShortName = (wchar_t*)L"TuneStatus";
  PluginInfo.Version = PLUGIN_MAKE_VERSION(2,0,1,0);
  PluginInfo.Description = (wchar_t *)L"Wstawianie do opisu aktualnie s³uchanego utworu z wielu odtwarzaczy";
  PluginInfo.Author = (wchar_t *)L"Krzysztof Grochocki (Beherit)";
  PluginInfo.AuthorMail = (wchar_t *)L"sirbeherit@gmail.com";
  PluginInfo.Copyright = (wchar_t *)L"Krzysztof Grochocki (Beherit)";
  PluginInfo.Homepage = (wchar_t *)L"http://beherit.pl";

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

//Zapisywanie zasobów
bool SaveResourceToFile(char *FileName, char *res)
{
  HRSRC hrsrc = FindResource(HInstance, res, RT_RCDATA);
  if(hrsrc == NULL) return false;
  DWORD size = SizeofResource(HInstance, hrsrc);
  HGLOBAL hglob = LoadResource(HInstance, hrsrc);
  LPVOID rdata = LockResource(hglob);
  HANDLE hFile = CreateFile(FileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  DWORD writ;
  WriteFile(hFile, rdata, size, &writ, NULL);
  CloseHandle(hFile);
  return true;
}
//---------------------------------------------------------------------------

//OnLoad plugin
extern "C" int __declspec(dllexport) __stdcall Load(PPluginLink Link)
{
  PluginLink = *Link;

  //sciezka katalogu prywatnego uzytkownika
  UnicodeString Dir = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,(WPARAM)(HInstance),0));
  Dir = StringReplace(Dir, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);

  if(!DirectoryExists(Dir + "\\\\TuneStatus"))
   CreateDir(Dir + "\\\\TuneStatus");

  //Wypakowanie ikon
  SaveResourceToFile((Dir + "\\\\TuneStatus\\\\TuneStatus.png").t_str(),"ID_PNG1");
  //Przypisanie ikony
  plugin_icon_idx=PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(Dir + "\\\\TuneStatus\\\\TuneStatus.png").w_str());
  //Usuniecie ikony
  DeleteFile(Dir + "\\\\TuneStatus\\\\TuneStatus.png");

  //Wypakowanie ikon
  if(!FileExists(Dir + "\\\\TuneStatus\\\\TuneStatusOff.png"))
   SaveResourceToFile((Dir + "\\\\TuneStatus\\\\TuneStatusOff.png").t_str(),"ID_PNG2");
  //Przypisanie ikony
  plugin_icon_idx_off=PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(Dir +"\\\\TuneStatus\\\\TuneStatusOff.png").w_str());

  //Wypakowanie ikon
  if(!FileExists(Dir + "\\\\TuneStatus\\\\TuneStatusOn.png"))

   SaveResourceToFile((Dir + "\\\\TuneStatus\\\\TuneStatusOn.png").t_str(),"ID_PNG3");
  //Przypisanie ikony
  plugin_icon_idx_on=PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(Dir +"\\\\TuneStatus\\\\TuneStatusOn.png").w_str());

  //Przyspisanie przycisku
  PrzypiszSkrot();

  //Hook dla zmiany opisu
  PluginLink.HookEvent(AQQ_WINDOW_PRESETNOTE_NOTE,OnPreSetNote);
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
  TIniFile *Ini = new TIniFile(Dir + "\\\\TuneStatus\\\\TuneStatus.ini");
  EnablePluginOnStart = Ini->ReadBool("Settings", "EnablePluginOnStart", 0);
  EnableFastOnOff = Ini->ReadBool("Settings", "EnableFastOnOff", 1);
  UserTuneEnableN = Ini->ReadBool("UserTune", "EnableN", 0);
  UserTuneEnableS = Ini->ReadBool("UserTune", "EnableS", 0);
  UserTuneTimeOut = Ini->ReadInteger("UserTune", "TimeOutN", 4);
  ExceptionCount = Ini->ReadInteger("UserTune", "ExceptionCount", 0);
  if(ExceptionCount!=0)
   for(int Count=0;Count<ExceptionCount;Count++)
	ExceptionJID[Count] = Ini->ReadString("UserTuneEx" + IntToStr(Count+1), "JID", "");
  delete Ini;

  if(UserTuneEnableS==1)
  {
	if(handle==NULL)
	{
	  Application->Handle = (HWND__*)MainForm;
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
	  Application->Handle = (HWND__*)MainForm;
	  handle = new TMainForm(Application);
	}
	handle->aReadSettings->Execute();
	handle->GetStatusTimer->Enabled=true;
	handle->SongTimer->Interval=1000;
	handle->JustEnabled=1;
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
	Application->Handle = (HWND__*)MainForm;
	handle = new TMainForm(Application);
  }
  handle->Show();

  return 0;
}
//---------------------------------------------------------------------------

//Pobieranie sciezki katalogu prywatnego uzytkownika
UnicodeString GetPluginUserDir()
{
  UnicodeString Dir;
  Dir = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,(WPARAM)(HInstance),0));
  Dir = StringReplace(Dir, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
  return Dir;
}
//---------------------------------------------------------------------------

//Pobieranie sciezki do pliku wtyczki
UnicodeString GetPluginDir()
{
  UnicodeString Dir;
  Dir = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINDIR,(WPARAM)(HInstance),0));
  Dir = StringReplace(Dir, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
  return Dir;
}
//---------------------------------------------------------------------------

//Ustawianie UserTune
void SetUserTune(UnicodeString Tune)
{
  UnicodeString XML;
  int Count;
  int Account;
  UnicodeString AccountName;

  Tune = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_CONVERTTOXML,0,(WPARAM)Tune.w_str()));

  Count = PluginLink.CallService(AQQ_FUNCTION_GETUSEREXCOUNT,0,0);
  for(Account=0;Account<Count;Account++)
  {
    if(AllowChangeStatusUserTune(Account)==true)
    {
	  if(Tune!="")
	   XML = "<iq type=\"set\" from=\"CC_JID\" id=\"CC_SESSION\"><pubsub xmlns=\"http://jabber.org/protocol/pubsub\"><publish node=\"http://jabber.org/protocol/tune\"><item><tune xmlns=\"http://jabber.org/protocol/tune\"><artist/><rating/><source/><title>CC_TUNESTATUS</title><track/><uri/></tune></item></publish></pubsub></iq>";
	  else
	   XML = "<iq type=\"set\" from=\"CC_JID\" id=\"CC_SESSION\"><pubsub xmlns=\"http://jabber.org/protocol/pubsub\"><publish node=\"http://jabber.org/protocol/tune\"><item><tune xmlns=\"http://jabber.org/protocol/tune\"/></item></publish></pubsub></iq>";

	  XML = StringReplace(XML, "CC_SESSION", ((wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETSTRID,0,0))), TReplaceFlags());
	  XML = StringReplace(XML, "CC_TUNESTATUS", Tune, TReplaceFlags());

	  PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&PluginStateChange),Account);
	  AccountName = (UnicodeString)((wchar_t*)(PluginStateChange.JID)) + "/" + (UnicodeString)((wchar_t*)(PluginStateChange.Resource));

	  XML = StringReplace(XML, "CC_JID", AccountName, TReplaceFlags() << rfReplaceAll);

	  PluginLink.CallService(AQQ_SYSTEM_SENDXML,(WPARAM)XML.w_str(),Account);
	}
	else
	{
	  XML = "<iq type=\"set\" from=\"CC_JID\" id=\"CC_SESSION\"><pubsub xmlns=\"http://jabber.org/protocol/pubsub\"><publish node=\"http://jabber.org/protocol/tune\"><item><tune xmlns=\"http://jabber.org/protocol/tune\"/></item></publish></pubsub></iq>";

	  XML = StringReplace(XML, "CC_SESSION", ((wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETSTRID,0,0))), TReplaceFlags());

	  PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&PluginStateChange),Account);
	  AccountName = (UnicodeString)((wchar_t*)(PluginStateChange.JID)) + "/" + (UnicodeString)((wchar_t*)(PluginStateChange.Resource));
	  XML = StringReplace(XML, "CC_JID", AccountName, TReplaceFlags() << rfReplaceAll);

	  PluginLink.CallService(AQQ_SYSTEM_SENDXML,(WPARAM)XML.w_str(),Account);
	}
  }
}
//---------------------------------------------------------------------------

//OnUnload plugin
extern "C" int __declspec(dllexport) __stdcall Unload()
{
  if((handle!=NULL)&&(handle->Timer->Enabled==true))
  {
	handle->opisTMP=GetStatus();
    if(handle->opis_pocz!=handle->opisTMP)
	 SetStatus(handle->opis_pocz,!handle->SetOnlyInJabberCheck);
  }
  if((handle!=NULL)&&(handle->UserTuneTimer->Enabled==true))
   SetUserTune("");
  PluginLink.UnhookEvent(OnPreSetNote);
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

void SetAQQUserTuneOff()
{
  SaveSetup.Section = L"Settings";
  SaveSetup.Ident = L"Tune";
  SaveSetup.Value = L"0";

  PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
  PluginLink.CallService(AQQ_FUNCTION_REFRESHSETUP,0,0);
}
//---------------------------------------------------------------------------

void RefreshUserTuneException()
{
  UnicodeString Dir = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,(WPARAM)(HInstance),0));
  Dir = StringReplace(Dir, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);

  TIniFile *Ini = new TIniFile(Dir + "\\\\TuneStatus\\\\TuneStatus.ini");
  ExceptionCount = Ini->ReadInteger("UserTune", "ExceptionCount", 0);
  if(ExceptionCount!=0)
   for(int Count=0;Count<ExceptionCount;Count++)
	ExceptionJID[Count] = Ini->ReadString("UserTuneEx" + IntToStr(Count+1), "JID", "");
  delete Ini;
}
//---------------------------------------------------------------------------
