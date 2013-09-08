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

//Do przywracania opisu po przejsciu na niewidoczny
PPluginStateChange StateChange;
int NewState;

//Uchwytu do formy ustawien
TMainForm *hMainForm;

//Indeksy ikon
int plugin_icon_idx;
int plugin_icon_idx_off;
int plugin_icon_idx_on;

//Button szybkiego dostepu
bool EnableFastOnOff;
//Wlaczanie dzialania wtyczki na starcie
bool EnablePluginOnStart;

//Do AQQ Radio
UnicodeString AQQRadioSong;
PPluginSong Song;

//Do notyfikacji User Tune
PPluginXMLChunk XMLChunk;
UnicodeString XML;
UnicodeString XMLFrom;
UnicodeString XMLTo;
int XMLUserIdx;
UnicodeString UserTuneSong;
UnicodeString UserTuneSongTMP;
UnicodeString UserTuneIconPath;
bool AllowUserTuneN=false; //Zezwalanie na notyfikacje
int UserTuneTimeOut; //Czas wyswietlania chmurki
bool UserTuneEnableN; //Czy wyswietlac notyfikacje
//Do wyjatkow z notyfikacji UserTune
UnicodeString ExceptionJID[50];
int ExceptionCount;
bool AllowException;
//Do wysylania informacji User Tune
UnicodeString TuneXML;
int TuneCount;
int TuneAccount;
UnicodeString TuneAccountName;

//Do blokowania zmiany opisu gdy wtyczka jest aktywna
UnicodeString nowy_opis;

//Pobieranie aktualnego opisu
UnicodeString GetStatus()
{
  UnicodeString GetStatusStatus;
  PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&PluginStateChange),0);
  GetStatusStatus = (wchar_t*)PluginStateChange.Status;
  return GetStatusStatus;
}
//---------------------------------------------------------------------------

//Ustawianie opisu
void SetStatus(UnicodeString SetStatusStatus, bool SetStatusForce)
{
  PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&PluginStateChange),0);

  PluginStateChange.cbSize = sizeof(TPluginStateChange);
  PluginStateChange.Status = SetStatusStatus.w_str();
  PluginStateChange.Force = SetStatusForce;

  PluginLink.CallService(AQQ_SYSTEM_SETSHOWANDSTATUS,0,(LPARAM)(&PluginStateChange));
}
//---------------------------------------------------------------------------

//Sprawdzanie stanu glownego konta - czy rozny od rozlaczony/niewidoczny
bool AllowChangeStatus(bool WithInvisible)
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

//Sprawdzanie stanu glownego konta dla UserTune - czy rozny od rozlaczony/niewidoczny
bool AllowChangeStatusUserTune(int Account)
{
  PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&PluginStateChange),Account);

  PluginStateChange.cbSize = sizeof(TPluginStateChange);
  if((PluginStateChange.OldState!=0)&&(PluginStateChange.OldState!=6))
   return true;
  else
   return false;
}
//---------------------------------------------------------------------------

//Serwis szybkiego wlaczenia/wylaczenia dzialania wtyczki
int __stdcall TuneStatus_FastOnOff (WPARAM, LPARAM)
{
  if(hMainForm==NULL)
  {
	Application->Handle = (HWND)MainForm;
	hMainForm = new TMainForm(Application);
	hMainForm->aReadSettings->Execute();
  }

  if(hMainForm->Timer->Enabled==false)
  {
	hMainForm->Opis_pocz = GetStatus();
	hMainForm->SongTimer->Interval=1000;
	hMainForm->JustEnabled=1;
	hMainForm->Timer->Enabled=true;
	//Update buttonu
	PluginActionButton.IconIndex = plugin_icon_idx_on;
	PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_UPDATEBUTTON,0,(LPARAM)(&PluginActionButton));
  }
  else
  {
	hMainForm->SongTimer->Enabled=false;
	hMainForm->Timer->Enabled=false;
	hMainForm->OpisTMP=GetStatus();
	if(hMainForm->Opis_pocz!=hMainForm->OpisTMP)
	{
	  while(GetStatus()!=hMainForm->Opis_pocz)
	   SetStatus(hMainForm->Opis_pocz,!hMainForm->SetOnlyInJabberCheck);
	  hMainForm->OpisTMP="";
	}
	//Update buttonu
	PluginActionButton.IconIndex = plugin_icon_idx_off;
	PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_UPDATEBUTTON,0,(LPARAM)(&PluginActionButton));
  }
  hMainForm->RunPluginCheckBox->Checked=!hMainForm->RunPluginCheckBox->Checked;

  return 0;
}
//---------------------------------------------------------------------------

//Serwis otwarcia formy
int __stdcall TuneStatusService (WPARAM, LPARAM)
{
  if(hMainForm==NULL)
  {
	Application->Handle = (HWND)MainForm;
	hMainForm = new TMainForm(Application);
  }

  hMainForm->Show();

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

//Pobieranie nazwy konta przez podanie jego indeksu
UnicodeString ReciveAccountName(int UserIdx)
{
  PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&PluginStateChange),UserIdx);
  UnicodeString Name = (wchar_t*)PluginStateChange.JID;

  return Name;
}
//---------------------------------------------------------------------------

//Notyfikacja otrzymywanych pakietow XML
int __stdcall OnXMLDebug (WPARAM wParam, LPARAM lParam)
{
  if((AllowUserTuneN==true)&&(UserTuneEnableN==true))
  {
	//UserTuneSong = (wchar_t*)(wParam);

	XMLChunk = (PPluginXMLChunk)lParam;

	XML = (wchar_t*)XMLChunk->XML;

	if(
	(AnsiPos("<tune xmlns='http://jabber.org/protocol/tune'>", XML)>0)&&
	(AnsiPos("type='error'", XML)==0)&&
	(AnsiPos("<title>", XML)>0)
	)
	{
	  XMLFrom = (wchar_t*)XMLChunk->From;
	  XMLUserIdx = XMLChunk->UserIdx;
	  XMLTo = ReciveAccountName(XMLUserIdx);

	  if(XMLFrom!=XMLTo)
	  {
		AllowException=true;

		if(ExceptionCount!=0)
		{
		  for(int Count=0;Count<ExceptionCount;Count++)
		  {
			if(XMLFrom==ExceptionJID[Count])
			 AllowException=false;
		  }
		}

		if(AllowException==true)
		{
		  //Artysta i tytul osobno
		  if(AnsiPos("<artist>", XML)>0)
		  {
			UserTuneSongTMP=XML;
			UserTuneSongTMP.Delete(1,AnsiPos("<artist>", UserTuneSongTMP)+7);
			UserTuneSongTMP.Delete(AnsiPos("</artist>", UserTuneSongTMP), UserTuneSongTMP.Length());
			UserTuneSongTMP=UserTuneSongTMP.Trim();
			UserTuneSong = UserTuneSongTMP;

			UserTuneSongTMP=XML;
			UserTuneSongTMP.Delete(1,AnsiPos("<title>", UserTuneSongTMP)+6);
			UserTuneSongTMP.Delete(AnsiPos("</title>", UserTuneSongTMP), UserTuneSongTMP.Length());
			UserTuneSongTMP=UserTuneSongTMP.Trim();
			UserTuneSong = UserTuneSong + " - " + UserTuneSongTMP;
		  }
		  //Artysta i tytul razem w tagu <title>
		  else
		  {
			UserTuneSong=XML;
			UserTuneSong.Delete(1,AnsiPos("<title>", UserTuneSong)+6);
			UserTuneSong.Delete(AnsiPos("</title>", UserTuneSong), UserTuneSong.Length());
			UserTuneSong=UserTuneSong.Trim();
		  }

		  //Poprawa kodowania UTF8
		  UserTuneSong = UTF8ToUnicodeString(UserTuneSong.t_str());
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

		  //Pobieranie Nick'a z JID
		  PluginContactSimpleInfo.cbSize = sizeof(TPluginContactSimpleInfo);
		  PluginContactSimpleInfo.JID = XMLFrom.w_str();//UserTuneFrom.w_str();
		  PluginLink.CallService(AQQ_CONTACTS_FILLSIMPLEINFO,0,(LPARAM)(&PluginContactSimpleInfo));
		  UserTuneSongTMP = (wchar_t*)(PluginContactSimpleInfo.Nick);
		  UserTuneSongTMP = UserTuneSongTMP.Trim();
		  if(UserTuneSongTMP!="")
		   XMLFrom = UserTuneSongTMP;

		  UserTuneIconPath = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPNG_FILEPATH,76,0));

		  //Chmurka "kto slucha"
		  PluginShowInfo.cbSize = sizeof(TPluginShowInfo);
		  PluginShowInfo.Event = tmeInfo;
		  PluginShowInfo.Text = (XMLFrom + " s³ucha:").w_str();
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

//Notyfikacja zaladowania wszystkich modulow w AQQ
int __stdcall OnModulesLoaded(WPARAM, LPARAM)
{
  if(hMainForm==NULL)
  {
	Application->Handle = (HWND)MainForm;
	hMainForm = new TMainForm(Application);
  }

  hMainForm->aReadSettings->Execute();
  hMainForm->AllowUserTuneNTimer->Enabled=true;

  return 0;
}
//---------------------------------------------------------------------------

//Zezwalanie na pokazywanie notyfikacji User Tune
void SetAllowUserTuneNTrue()
{
  AllowUserTuneN=true;
}
//---------------------------------------------------------------------------

//Pobieranie odtwarzanego utworu w AQQ Radio
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
  if((hMainForm!=NULL)&&(hMainForm->Timer->Enabled==true))
  {
	nowy_opis = (wchar_t*)(lParam);

	if((nowy_opis!=hMainForm->Opis_pocz)
	&&(nowy_opis!=hMainForm->OpisTMP)
	&&(nowy_opis!=hMainForm->Opis)
	&&(hMainForm->Opis!=""))
	{
	  hMainForm->Opis_pocz = nowy_opis;
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
  if((hMainForm!=NULL)&&(hMainForm->Timer->Enabled==true)&&(hMainForm->Opis!=""))
  {
    StateChange = (PPluginStateChange)lParam;
	NewState = StateChange->NewState;

	//Jezeli blokowac przy niewidocznym
	if((hMainForm->BlockInvisibleCheck==true)&&(NewState==6))
	 hMainForm->StateChangeTimer->Enabled=true;
  }

  return 0;
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

//Pobieranie sciezki katalogu prywatnego uzytkownika
UnicodeString GetPluginUserDir()
{
  UnicodeString PluginUserDir;
  PluginUserDir = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,(WPARAM)(HInstance),0));
  PluginUserDir = StringReplace(PluginUserDir, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
  return PluginUserDir;
}
//---------------------------------------------------------------------------

//Pobieranie sciezki do pliku wtyczki
UnicodeString GetPluginDir()
{
  UnicodeString PluginDir;
  PluginDir = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINDIR,(WPARAM)(HInstance),0));
  PluginDir = StringReplace(PluginDir, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
  return PluginDir;
}
//---------------------------------------------------------------------------

//Ustawianie UserTune
void SetUserTune(UnicodeString Tune)
{
  Tune = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_CONVERTTOXML,0,(WPARAM)Tune.w_str()));

  TuneCount = PluginLink.CallService(AQQ_FUNCTION_GETUSEREXCOUNT,0,0);
  for(TuneAccount=0;TuneAccount<TuneCount;TuneAccount++)
  {
	//Gdy stan konta jest rozny od niewidoczny/rozlaczony
	if(AllowChangeStatusUserTune(TuneAccount)==true)
	{
	  //Gdy w zmiennej Tune jest odtwarzany utwor
	  if(Tune!="")
	   TuneXML = "<iq type=\"set\" from=\"CC_JID\" id=\"CC_SESSION\"><pubsub xmlns=\"http://jabber.org/protocol/pubsub\"><publish node=\"http://jabber.org/protocol/tune\"><item><tune xmlns=\"http://jabber.org/protocol/tune\"><artist/><rating/><source/><title>CC_TUNESTATUS</title><track/><uri/></tune></item></publish></pubsub></iq>";
	  //W przeciwnym razie wylaczanie informowania User Tune
	  else
	   TuneXML = "<iq type=\"set\" from=\"CC_JID\" id=\"CC_SESSION\"><pubsub xmlns=\"http://jabber.org/protocol/pubsub\"><publish node=\"http://jabber.org/protocol/tune\"><item><tune xmlns=\"http://jabber.org/protocol/tune\"/></item></publish></pubsub></iq>";

	  //Zamiana CC_SESSION na wygenerowany ID
	  TuneXML = StringReplace(TuneXML, "CC_SESSION", ((wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETSTRID,0,0))), TReplaceFlags());
	  //Zamiania CC_TUNESTATUS na odtwarzany utwor
	  TuneXML = StringReplace(TuneXML, "CC_TUNESTATUS", Tune, TReplaceFlags());
	  //Zamiania CC_JID na JID konta
	  PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&PluginStateChange),TuneAccount);
	  TuneAccountName = (UnicodeString)((wchar_t*)(PluginStateChange.JID)) + "/" + (UnicodeString)((wchar_t*)(PluginStateChange.Resource));
	  TuneXML = StringReplace(TuneXML, "CC_JID", TuneAccountName, TReplaceFlags());
      //Wyslanie pakietu XML
	  PluginLink.CallService(AQQ_SYSTEM_SENDXML,(WPARAM)TuneXML.w_str(),TuneAccount);
	}
	//Gdy stan konta jest niewidoczny/rozlaczony - wylaczanie informowania User Tune
	else
	{
	  TuneXML = "<iq type=\"set\" from=\"CC_JID\" id=\"CC_SESSION\"><pubsub xmlns=\"http://jabber.org/protocol/pubsub\"><publish node=\"http://jabber.org/protocol/tune\"><item><tune xmlns=\"http://jabber.org/protocol/tune\"/></item></publish></pubsub></iq>";

	  //Zamiana CC_SESSION na wygenerowany ID
	  TuneXML = StringReplace(TuneXML, "CC_SESSION", ((wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETSTRID,0,0))), TReplaceFlags());
	  //Zamiania CC_JID na JID konta
	  PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&PluginStateChange),TuneAccount);
	  TuneAccountName = (UnicodeString)((wchar_t*)(PluginStateChange.JID)) + "/" + (UnicodeString)((wchar_t*)(PluginStateChange.Resource));
	  TuneXML = StringReplace(TuneXML, "CC_JID", TuneAccountName, TReplaceFlags() << rfReplaceAll);
      //Wyslanie pakietu XML
	  PluginLink.CallService(AQQ_SYSTEM_SENDXML,(WPARAM)TuneXML.w_str(),TuneAccount);
	}
  }
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
  UnicodeString TuneExceptionDir = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,(WPARAM)(HInstance),0));
  TuneExceptionDir = StringReplace(TuneExceptionDir, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);

  TIniFile *Ini = new TIniFile(TuneExceptionDir + "\\\\TuneStatus\\\\TuneStatus.ini");
  ExceptionCount = Ini->ReadInteger("UserTune", "ExceptionCount", 0);
  if(ExceptionCount!=0)
   for(int Count=0;Count<ExceptionCount;Count++)
	ExceptionJID[Count] = Ini->ReadString("UserTuneEx" + IntToStr(Count+1), "JID", "");
  delete Ini;
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
  PluginInfo.Version = PLUGIN_MAKE_VERSION(2,0,2,0);
  PluginInfo.Description = (wchar_t *)L"Wstawianie do opisu aktualnie s³uchanego utworu z wielu odtwarzaczy";
  PluginInfo.Author = (wchar_t *)L"Krzysztof Grochocki (Beherit)";
  PluginInfo.AuthorMail = (wchar_t *)L"email@beherit.pl";
  PluginInfo.Copyright = (wchar_t *)L"Krzysztof Grochocki (Beherit)";
  PluginInfo.Homepage = (wchar_t *)L"http://beherit.pl";

  return &PluginInfo;
}
//---------------------------------------------------------------------------

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
  bool UserTuneEnableS = Ini->ReadBool("UserTune", "EnableS", 0);
  UserTuneTimeOut = Ini->ReadInteger("UserTune", "TimeOutN", 4);
  ExceptionCount = Ini->ReadInteger("UserTune", "ExceptionCount", 0);
  if(ExceptionCount!=0)
   for(int Count=0;Count<ExceptionCount;Count++)
	ExceptionJID[Count] = Ini->ReadString("UserTuneEx" + IntToStr(Count+1), "JID", "");
  delete Ini;

  //Czy wlaczyc wysylanie informacji User Tune
  if(UserTuneEnableS==1)
  {
	if(hMainForm==NULL)
	{
	  Application->Handle = (HWND)MainForm;
	  hMainForm = new TMainForm(Application);
	}
	hMainForm->aReadSettings->Execute();
	hMainForm->UserTuneTimer->Enabled=true;
  }

  //Czy w³¹czonac na starcie
  if(EnablePluginOnStart==1)
  {
	if(hMainForm==NULL)
	{
	  Application->Handle = (HWND)MainForm;
	  hMainForm = new TMainForm(Application);
	}
	hMainForm->aReadSettings->Execute();
	hMainForm->GetStatusTimer->Enabled=true;
	hMainForm->SongTimer->Interval=1000;
	hMainForm->JustEnabled=1;
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
  if(hMainForm==NULL)
  {
	Application->Handle = (HWND)MainForm;
	hMainForm = new TMainForm(Application);
  }
  hMainForm->Show();

  return 0;
}
//---------------------------------------------------------------------------

extern "C" int __declspec(dllexport) __stdcall Unload()
{
  if((hMainForm!=NULL)&&(hMainForm->Timer->Enabled==true))
  {
	hMainForm->OpisTMP=GetStatus();
	if(hMainForm->Opis_pocz!=hMainForm->OpisTMP)
	{
	  while(GetStatus()!=hMainForm->Opis_pocz)
	   SetStatus(hMainForm->Opis_pocz,!hMainForm->SetOnlyInJabberCheck);
	}
  }
  if((hMainForm!=NULL)&&(hMainForm->UserTuneTimer->Enabled==true))
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
