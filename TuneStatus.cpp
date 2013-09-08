//---------------------------------------------------------------------------
#include <vcl.h>
#include <windows.h>
#pragma hdrstop
#pragma argsused
#include <inifiles.hpp>
#include "Aqq.h"
#include "MainFrm.h"
//---------------------------------------------------------------------------

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
  return 1;
}
//---------------------------------------------------------------------------

//Uchwytu do formy ustawien
TMainForm *hMainForm;
//Struktury glowne
TPluginLink PluginLink;
TPluginInfo PluginInfo;
TPluginAction PopPluginsAccess;
TPluginAction FastAccessButton;
TPluginStateChange PluginStateChange;
TPluginShowInfo PluginShowInfo;
TPluginContactSimpleInfo PluginContactSimpleInfo;
PPluginStateChange StateChange;
PPluginXMLChunk XMLChunk;
PPluginSong Song;
//Indeksy ikon
int POPPLUGINSACCESS;
int FASTACCESS_ON;
int FASTACCESS_OFF;
//Utwor przekazany przez wtyczki np. AQQ Radio
UnicodeString PluginSong;
//Zezwalanie na notyfikacje UserTune
bool AllowUserTune = false;
//Czas wyswietlania chmurki
int UserTuneCloud;
//Czy wyswietlac notyfikacje User Tune
bool UserTuneNotif;
//Do wyjatkow z notyfikacji User Tune
TStringList *UserTuneExceptions = new TStringList;

//Pobieranie sciezki do skorki kompozycji
UnicodeString GetThemeSkinDir()
{
  return StringReplace((wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETTHEMEDIR,0,0)), "\\", "\\\\", TReplaceFlags() << rfReplaceAll) + "\\\\Skin";
}
//---------------------------------------------------------------------------
//Pobieranie sciezki katalogu prywatnego uzytkownika
UnicodeString GetPluginUserDir()
{
  return StringReplace((wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,0,0)), "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
}
//---------------------------------------------------------------------------
//Pobieranie sciezki do pliku wtyczki
UnicodeString GetPluginDir()
{
  return StringReplace((wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINDIR,(WPARAM)(HInstance),0)), "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
}
//---------------------------------------------------------------------------

//Sprawdzanie czy wlaczona jest obsluga stylow obramowania okien
bool ChkSkinEnabled()
{
  TStrings* IniList = new TStringList();
  IniList->SetText((wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_FETCHSETUP,0,0)));
  TMemIniFile *Settings = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
  Settings->SetStrings(IniList);
  delete IniList;
  UnicodeString AlphaSkinsEnabled = Settings->ReadString("Settings","UseSkin","1");
  delete Settings;
  return StrToBool(AlphaSkinsEnabled);
}
//---------------------------------------------------------------------------

//Sprawdzanie czy wlaczony jest natywny styl Windows
bool ChkNativeEnabled()
{
  TStrings* IniList = new TStringList();
  IniList->SetText((wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_FETCHSETUP,0,0)));
  TMemIniFile *Settings = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
  Settings->SetStrings(IniList);
  delete IniList;
  UnicodeString NativeEnabled = Settings->ReadString("Settings","Native","0");
  delete Settings;
  return StrToBool(NativeEnabled);
}
//---------------------------------------------------------------------------

//Zmiana skorki wtyczki
void ChangePluginSkin()
{
  if(hMainForm)
  {
	UnicodeString ThemeSkinDir = GetThemeSkinDir();
	if((FileExists(ThemeSkinDir + "\\\\Skin.asz"))&&(!ChkNativeEnabled()))
	{
	  ThemeSkinDir = StringReplace(ThemeSkinDir, "\\\\", "\\", TReplaceFlags() << rfReplaceAll);
	  hMainForm->sSkinManager->SkinDirectory = ThemeSkinDir;
	  hMainForm->sSkinManager->SkinName = "Skin.asz";
	  hMainForm->sSkinProvider->DrawNonClientArea = ChkSkinEnabled();
	  hMainForm->sSkinManager->Active = true;
	}
	else
	 hMainForm->sSkinManager->Active = false;
  }
}
//---------------------------------------------------------------------------

//Hook na zmianê kompozycji
int __stdcall OnThemeChanged (WPARAM wParam, LPARAM lParam)
{
  ChangePluginSkin();
  return 0;
}
//---------------------------------------------------------------------------

//Pobieranie Nick kontaktu podajac jego JID
UnicodeString GetContactNick(UnicodeString JID)
{
  TPluginContactSimpleInfo PluginContactSimpleInfo;
  PluginContactSimpleInfo.cbSize = sizeof(TPluginContactSimpleInfo);
  PluginContactSimpleInfo.JID = JID.w_str();
  PluginLink.CallService(AQQ_CONTACTS_FILLSIMPLEINFO,0,(LPARAM)(&PluginContactSimpleInfo));
  UnicodeString Nick = (wchar_t*)PluginContactSimpleInfo.Nick;
  Nick = Nick.Trim();
  if(Nick.IsEmpty())
  {
	if(JID.Pos("@")) JID.Delete(JID.Pos("@"),JID.Length());
	return JID;
  }
  else
   return Nick;
}
//---------------------------------------------------------------------------

//Pobieranie aktualnego opisu
UnicodeString GetStatus()
{
  PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&PluginStateChange),0);
  return (wchar_t*)PluginStateChange.Status;
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
  if(WithInvisible)
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
bool AllowChangeUserTuneStatus(int Account)
{
  PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&PluginStateChange),Account);
  PluginStateChange.cbSize = sizeof(TPluginStateChange);
  if((PluginStateChange.OldState!=0)&&(PluginStateChange.OldState!=6))
   return true;
  else
   return false;
}
//---------------------------------------------------------------------------

//Sprawdzanie czy w danym tekscie s¹ tylko liczby
bool TestDigit(UnicodeString Text)
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
UnicodeString ReciveAccountJID(int UserIdx)
{
  PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&PluginStateChange),UserIdx);
  return (wchar_t*)PluginStateChange.JID;;
}
//---------------------------------------------------------------------------

//Zmiana ustawien User Tune
void ChangeUserTune(bool Enabled, int Cloud)
{
  UserTuneNotif = Enabled;
  UserTuneCloud = Cloud;
}
//---------------------------------------------------------------------------

//Zezwalanie na pokazywanie notyfikacji User Tune
void SetAllowUserTune()
{
  AllowUserTune = true;
}
//---------------------------------------------------------------------------

//Pobieranie utwor przekazanego przez wtyczki np. AQQ Radio
UnicodeString GetPluginSong()
{
  return PluginSong;
}
//---------------------------------------------------------------------------

//Wysylanie notyfikacji UserTune
void SetUserTune(UnicodeString Tune, UnicodeString Time)
{
  UnicodeString TuneXML;
  UnicodeString TuneAccountName;
  Tune = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_CONVERTTOXML,0,(WPARAM)Tune.w_str()));
  int TuneCount = PluginLink.CallService(AQQ_FUNCTION_GETUSEREXCOUNT,0,0);
  for(int TuneAccount=0;TuneAccount<TuneCount;TuneAccount++)
  {
	//Gdy stan konta jest rozny od niewidoczny/rozlaczony oraz posiadamy dane dt. utworu
	if((AllowChangeUserTuneStatus(TuneAccount))&&(!Tune.IsEmpty()))
	{
	  //Gdy w zmienna dlugosci utworu jest pusta
	  if(Time.IsEmpty())
	   TuneXML = "<iq type=\"set\" from=\"CC_JID\" id=\"CC_SESSION\"><pubsub xmlns=\"http://jabber.org/protocol/pubsub\"><publish node=\"http://jabber.org/protocol/tune\"><item><tune xmlns=\"http://jabber.org/protocol/tune\"><artist/><rating/><source/><title>CC_TUNESTATUS</title><track/><uri/></tune></item></publish></pubsub></iq>";
	  //W przeciwnym razie dodaje informacje o dlugosci utworu
	  else
	  {
		//Odkodowanie minut na sekund
		try
		{
		  UnicodeString TimeTMP = Time;
		  TimeTMP.Delete(TimeTMP.Pos(":"),TimeTMP.Length());
		  int TimeInt = StrToInt(TimeTMP) * 60;
		  TimeTMP = Time;
		  TimeTMP.Delete(1,TimeTMP.Pos(":"));
		  TimeInt = TimeInt + StrToInt(TimeTMP);
		  Time = IntToStr(TimeInt);
		  //Definicja pakieru XML
		  TuneXML = "<iq type=\"set\" from=\"CC_JID\" id=\"CC_SESSION\"><pubsub xmlns=\"http://jabber.org/protocol/pubsub\"><publish node=\"http://jabber.org/protocol/tune\"><item><tune xmlns=\"http://jabber.org/protocol/tune\"><artist/><length>CC_SONGLENGTH</length><rating/><source/><title>CC_TUNESTATUS</title><track/><uri/></tune></item></publish></pubsub></iq>";
		}
		catch(...)
		{
		  //Blad = definicja standardowego pakietu XML
		  TuneXML = "<iq type=\"set\" from=\"CC_JID\" id=\"CC_SESSION\"><pubsub xmlns=\"http://jabber.org/protocol/pubsub\"><publish node=\"http://jabber.org/protocol/tune\"><item><tune xmlns=\"http://jabber.org/protocol/tune\"><artist/><rating/><source/><title>CC_TUNESTATUS</title><track/><uri/></tune></item></publish></pubsub></iq>";
        }
	  }
	  //Zamiana CC_SESSION na wygenerowany ID
	  TuneXML = StringReplace(TuneXML, "CC_SESSION", ((wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETSTRID,0,0))), TReplaceFlags());
	  //Zamiania CC_TUNESTATUS na odtwarzany utwor
	  TuneXML = StringReplace(TuneXML, "CC_TUNESTATUS", Tune, TReplaceFlags());
	  //Zmiana CC_SONGLENGTH na dlugosc utworu
	  TuneXML = StringReplace(TuneXML, "CC_SONGLENGTH", Time, TReplaceFlags());
	  //Zamiania CC_JID na JID konta
	  PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&PluginStateChange),TuneAccount);
	  TuneAccountName = (UnicodeString)((wchar_t*)(PluginStateChange.JID)) + "/" + (UnicodeString)((wchar_t*)(PluginStateChange.Resource));
	  TuneXML = StringReplace(TuneXML, "CC_JID", TuneAccountName, TReplaceFlags());
      //Wyslanie pakietu XML
	  PluginLink.CallService(AQQ_SYSTEM_SENDXML,(WPARAM)TuneXML.w_str(),TuneAccount);
	}
	//Gdy stan konta jest niewidoczny/rozlaczony lub nie posiadamy danych dt. utworu
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

//Wylaczanie obslugi User Tune w AQQ
void TurnOffCoreUserTune()
{
  TSaveSetup SaveSetup;
  SaveSetup.Section = L"Settings";
  SaveSetup.Ident = L"Tune";
  SaveSetup.Value = L"0";
  PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
}
//---------------------------------------------------------------------------

//Odswiezanie wyjatkow z notyfikacji User Tune
void RefreshUserTuneException()
{
  UserTuneExceptions->Clear();
  TIniFile *Ini = new TIniFile(GetPluginUserDir() + "\\\\TuneStatus\\\\TuneStatus.ini");
  TStringList *JIDList = new TStringList;
  Ini->ReadSection("UserTuneEx",JIDList);
  int JIDListCount = JIDList->Count;
  delete JIDList;
  if(JIDListCount>0)
  {
	for(int Count=0;Count<JIDListCount;Count++)
	{
	  UnicodeString JID = Ini->ReadString("UserTuneEx",("JID"+IntToStr(Count+1)), "");
	  if(!JID.IsEmpty()) UserTuneExceptions->Add(JID);
	}
  }
  delete Ini;
}
//---------------------------------------------------------------------------

//Utwor przekazany przez wtyczki np. AQQ Radio
int __stdcall OnCurrentSong (WPARAM wParam, LPARAM lParam)
{
  Song = (PPluginSong)lParam;
  PluginSong = (wchar_t*)(Song->Title);
  return 0;
}
//---------------------------------------------------------------------------

//Notyfikacja otrzymywanych pakietow XML
int __stdcall OnXMLDebug (WPARAM wParam, LPARAM lParam)
{
  if((AllowUserTune)&&(UserTuneNotif))
  {
	XMLChunk = (PPluginXMLChunk)lParam;
	UnicodeString XML = (wchar_t*)XMLChunk->XML;
	if(((XML.Pos("<tune xmlns='http://jabber.org/protocol/tune'>"))||(XML.Pos("<tune xmlns=\"http://jabber.org/protocol/tune\">")))
	&&(!XML.Pos("type='error'"))
	&&(XML.Pos("<title>")))
	{
	  UnicodeString XMLFrom = (wchar_t*)XMLChunk->From;
	  int XMLUserIdx = XMLChunk->UserIdx;
	  UnicodeString XMLTo = ReciveAccountJID(XMLUserIdx);

	  if(XMLFrom!=XMLTo)
	  {
		if(UserTuneExceptions->IndexOf(XMLFrom)==-1)
		{
		  UnicodeString UserTuneSong;
		  UnicodeString UserTuneSongTMP;
		  //Artysta i tytul osobno
		  if(XML.Pos("<artist>")>0)
		  {
			UserTuneSongTMP = XML;
			UserTuneSongTMP.Delete(1,UserTuneSongTMP.Pos("<artist>")+7);
			UserTuneSongTMP.Delete(UserTuneSongTMP.Pos("</artist>"), UserTuneSongTMP.Length());
			UserTuneSongTMP=UserTuneSongTMP.Trim();
			UserTuneSong = UserTuneSongTMP;
			UserTuneSongTMP = XML;
			UserTuneSongTMP.Delete(1,UserTuneSongTMP.Pos("<title>")+6);
			UserTuneSongTMP.Delete(UserTuneSongTMP.Pos("</title>"), UserTuneSongTMP.Length());
			UserTuneSongTMP=UserTuneSongTMP.Trim();
			UserTuneSong = UserTuneSong + " - " + UserTuneSongTMP;
		  }
		  //Artysta i tytul razem w tagu <title>
		  else
		  {
			UserTuneSong = XML;
			UserTuneSong.Delete(1,UserTuneSong.Pos("<title>")+6);
			UserTuneSong.Delete(UserTuneSong.Pos("</title>"), UserTuneSong.Length());
			UserTuneSong = UserTuneSong.Trim();
		  }
		  //Poprawa kodowania UTF8
		  UserTuneSong = UTF8ToUnicodeString(UserTuneSong.t_str());
		  UserTuneSong = StringReplace(UserTuneSong, "&apos;", "'", TReplaceFlags() << rfReplaceAll);
		  UserTuneSong = StringReplace(UserTuneSong, "&quot;", "\"", TReplaceFlags() << rfReplaceAll);
		  UserTuneSong = StringReplace(UserTuneSong, "&amp;", "&", TReplaceFlags() << rfReplaceAll);
		  UserTuneSong = StringReplace(UserTuneSong, "&gt;", ">", TReplaceFlags() << rfReplaceAll);
		  UserTuneSong = StringReplace(UserTuneSong, "&lt;", "<", TReplaceFlags() << rfReplaceAll);
		  //Usuwanie "*** "
		  if(UserTuneSong.Pos("*** ")>0)
		  {
			UserTuneSong.Delete(1, UserTuneSong.Pos("*** ") + 3);
			UserTuneSong = UserTuneSong.Trim();
		  }
		  //Usuwanie numeru piosenki
		  if((UserTuneSong.Pos(". ")>0)&&(UserTuneSong.Pos(". ")<7))
		  {
			UserTuneSongTMP=UserTuneSong;
			UserTuneSongTMP.Delete(UserTuneSong.Pos(". "), UserTuneSongTMP.Length());
			if(TestDigit(UserTuneSongTMP))
			{
			  UserTuneSong.Delete(1, UserTuneSong.Pos(". "));
			  UserTuneSong = UserTuneSong.Trim();
			}
		  }
		  //Usuwanie "Wtyczka AQQ Radio: "
		  if(UserTuneSong.Pos("Wtyczka AQQ Radio: ")>0)
		   UserTuneSong.Delete(1,UserTuneSong.Pos("Wtyczka AQQ Radio: ")+18);
		  //Chmurka "kto slucha"
		  PluginShowInfo.cbSize = sizeof(TPluginShowInfo);
		  PluginShowInfo.Event = tmeInfo;
		  PluginShowInfo.Text = (GetContactNick(XMLFrom) + " s³ucha:").w_str();
		  PluginShowInfo.ImagePath = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPNG_FILEPATH,76,0));
		  PluginShowInfo.TimeOut = 1000 * UserTuneCloud;
		  PluginLink.CallService(AQQ_FUNCTION_SHOWINFO,0,(LPARAM)(&PluginShowInfo));
		  //Chmurka "co slucha"
		  PluginShowInfo.cbSize = sizeof(TPluginShowInfo);
		  PluginShowInfo.Event = tmeInfo;
		  PluginShowInfo.Text = UserTuneSong.w_str();
		  PluginShowInfo.ImagePath = L"";
		  PluginShowInfo.TimeOut = 1000 * UserTuneCloud;
		  PluginLink.CallService(AQQ_FUNCTION_SHOWINFO,0,(LPARAM)(&PluginShowInfo));
		}
	  }
    }
  }

  return 0;
}
//---------------------------------------------------------------------------

//Notyfikacja zmiany statusu
int __stdcall OnPreSetNote(WPARAM wParam, LPARAM lParam)
{
  if((hMainForm)&&(hMainForm->AutoModeTimer->Enabled))
  {
	UnicodeString NewStatus = (wchar_t*)(lParam);

	if((NewStatus!=hMainForm->StartStatus)
	&&(NewStatus!=hMainForm->TempStatus)
	&&(NewStatus!=hMainForm->Status)
	&&(!hMainForm->Status.IsEmpty()))
	{
	  hMainForm->StartStatus = NewStatus;
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
  if((hMainForm)&&(hMainForm->AutoModeTimer->Enabled)&&(!hMainForm->Status.IsEmpty()))
  {
	StateChange = (PPluginStateChange)lParam;
	int NewState = StateChange->NewState;
    //Jezeli blokowac przy niewidocznym
	if((hMainForm->BlockInvisibleChk)&&(NewState==6))
	 hMainForm->StateChangeTimer->Enabled = true;
  }

  return 0;
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

  hMainForm->aLoadSettings->Execute();
  hMainForm->AllowUserTuneTimer->Enabled = true;

  return 0;
}
//---------------------------------------------------------------------------

//Serwis otwarcia formy ustawien
int __stdcall TuneStatus_PopPluginsAccess (WPARAM, LPARAM)
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

//Tworzenie skrótu w popPlugins
void BuildPopPluginsAccess()
{
  PopPluginsAccess.cbSize = sizeof(TPluginAction);
  PopPluginsAccess.pszName = (wchar_t*)L"TuneStatus_PopPluginsAccess";
  PopPluginsAccess.pszCaption = (wchar_t*) L"TuneStatus";
  PopPluginsAccess.Position = 0;
  PopPluginsAccess.IconIndex = POPPLUGINSACCESS;
  PopPluginsAccess.pszService = (wchar_t*) L"sTuneStatus_PopPluginsAccess";
  PopPluginsAccess.pszPopupName = (wchar_t*) L"popPlugins";
  PopPluginsAccess.PopupPosition = 0;

  PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&PopPluginsAccess));
  PluginLink.CreateServiceFunction(L"sTuneStatus_PopPluginsAccess",TuneStatus_PopPluginsAccess);
}
//---------------------------------------------------------------------------

//Serwis szybkiego wlaczenia/wylaczenia dzialania wtyczki
int __stdcall TuneStatus_FastAccessButton (WPARAM, LPARAM)
{
  if(hMainForm==NULL)
  {
	Application->Handle = (HWND)MainForm;
	hMainForm = new TMainForm(Application);
	hMainForm->aLoadSettings->Execute();
  }

  if(!hMainForm->AutoModeTimer->Enabled)
  {
	hMainForm->StartStatus = GetStatus();
	hMainForm->SetStatusTimer->Interval = 1000;
	hMainForm->JustEnabled = true;
	hMainForm->AutoModeTimer->Enabled = true;
	//Update buttonu
	FastAccessButton.IconIndex = FASTACCESS_ON;
	PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_UPDATEBUTTON,0,(LPARAM)(&FastAccessButton));
  }
  else
  {
	hMainForm->SetStatusTimer->Enabled = false;
	hMainForm->AutoModeTimer->Enabled = false;
	hMainForm->TempStatus = GetStatus();
	if(hMainForm->StartStatus!=hMainForm->TempStatus)
	{
	  while(GetStatus()!=hMainForm->StartStatus)
	   SetStatus(hMainForm->StartStatus,!hMainForm->SetOnlyInJabberChk);
	  hMainForm->TempStatus = "";
	}
	//Update buttonu
	FastAccessButton.IconIndex = FASTACCESS_OFF;
	PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_UPDATEBUTTON,0,(LPARAM)(&FastAccessButton));
  }
  hMainForm->RunPluginCheckBox->Checked = !hMainForm->RunPluginCheckBox->Checked;

  return 0;
}
//---------------------------------------------------------------------------

//Tworzenie buttonu szybkiego wlaczania/wylaczania
void BuildFastAccess()
{
  FastAccessButton.cbSize = sizeof(TPluginAction);
  FastAccessButton.pszName = (wchar_t*) L"TuneStatus_FastAccessButton";
  FastAccessButton.Position = 999;
  if(hMainForm)
  {
	 if(hMainForm->AutoModeTimer->Enabled)
	  FastAccessButton.IconIndex = FASTACCESS_ON;
	 else
	  FastAccessButton.IconIndex = FASTACCESS_OFF;
  }
  else
   FastAccessButton.IconIndex = FASTACCESS_OFF;
  FastAccessButton.pszService = (wchar_t*) L"sTuneStatus_FastAccessButton";

  PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_CREATEBUTTON,0,(LPARAM)(&FastAccessButton));
  PluginLink.CreateServiceFunction(L"sTuneStatus_FastAccessButton",TuneStatus_FastAccessButton);
}
//---------------------------------------------------------------------------

//Usuwanie buttonu do szybkiego wlaczania/wylaczania
void DestroyFastAccess()
{
  PluginLink.DestroyServiceFunction(TuneStatus_FastAccessButton);
  PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_DESTROYBUTTON ,0,(LPARAM)(&FastAccessButton));
}
//---------------------------------------------------------------------------

//Aktulizacja stanu buttona szybkiego wlaczania/wylaczania
void UpdateFastAccess(bool Enabled)
{
  if(Enabled)
  {
	FastAccessButton.IconIndex = FASTACCESS_ON;
	PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_UPDATEBUTTON,0,(LPARAM)(&FastAccessButton));
  }
  else
  {
	FastAccessButton.IconIndex = FASTACCESS_OFF;
	PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_UPDATEBUTTON,0,(LPARAM)(&FastAccessButton));
  }
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

extern "C" int __declspec(dllexport) __stdcall Load(PPluginLink Link)
{
  PluginLink = *Link;
  //Sciezka katalogu prywatnego wtyczek
  UnicodeString Dir = GetPluginUserDir();
  //Tworzenie folderu wtyczki
  if(!DirectoryExists(Dir + "\\\\TuneStatus"))
   CreateDir(Dir + "\\\\TuneStatus");
  //Wypakowanie ikon
  if(!FileExists(Dir + "\\\\TuneStatus\\\\TuneStatus.png"))
   SaveResourceToFile((Dir + "\\\\TuneStatus\\\\TuneStatus.png").t_str(),"ID_PNG1");
  if(!FileExists(Dir + "\\\\TuneStatus\\\\TuneStatusOff.png"))
   SaveResourceToFile((Dir + "\\\\TuneStatus\\\\TuneStatusOff.png").t_str(),"ID_PNG2");
  if(!FileExists(Dir + "\\\\TuneStatus\\\\TuneStatusOn.png"))
   SaveResourceToFile((Dir + "\\\\TuneStatus\\\\TuneStatusOn.png").t_str(),"ID_PNG3");
  //Przypisanie ikon w interfejsie AQQ
  POPPLUGINSACCESS = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(Dir + "\\\\TuneStatus\\\\TuneStatus.png").w_str());
  FASTACCESS_OFF = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(Dir +"\\\\TuneStatus\\\\TuneStatusOff.png").w_str());
  FASTACCESS_ON = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(Dir +"\\\\TuneStatus\\\\TuneStatusOn.png").w_str());
  //Tworzenie w menu szybkiego dostpenu do wtyczek
  BuildPopPluginsAccess();
  //Hook na reczna zmiane opisu
  PluginLink.HookEvent(AQQ_WINDOW_PRESETNOTE_NOTE, OnPreSetNote);
  //Hook na zmiane stanu sieci
  PluginLink.HookEvent(AQQ_SYSTEM_STATECHANGE, OnStateChange);
  //Hook na przekazywanie utworu przez wtyczki np. AQQ Radio
  PluginLink.HookEvent(AQQ_SYSTEM_CURRENTSONG, OnCurrentSong);
  //Hook na przychodzace pakiety XML
  PluginLink.HookEvent(AQQ_SYSTEM_XMLDEBUG, OnXMLDebug);
  //Hook na zmianê kompozycji
  PluginLink.HookEvent(AQQ_SYSTEM_THEMECHANGED, OnThemeChanged);
  //Hook na zaladowanie wszystkich modolw
  AllowUserTune = PluginLink.CallService(AQQ_SYSTEM_MODULESLOADED,0,0);
  PluginLink.HookEvent(AQQ_SYSTEM_MODULESLOADED, OnModulesLoaded);
  //Odczyt ustawien
  TIniFile *Ini = new TIniFile(Dir + "\\\\TuneStatus\\\\TuneStatus.ini");
  //Konwersja-starej-struktury-pliku-ustawien----------------------------------
  if(Ini->ValueExists("Settings", "EnablePluginOnStart"))
  {
	bool OnStart = Ini->ReadBool("Settings", "EnablePluginOnStart", false);
	Ini->DeleteKey("Settings", "EnablePluginOnStart");
	Ini->WriteBool("Settings", "EnableOnStart", OnStart);
  }
  if(Ini->ValueExists("Settings", "SongTimerInterval"))
  {
	int SetDelay = Ini->ReadInteger("Settings", "SongTimerInterval", 5);
	Ini->DeleteKey("Settings", "SongTimerInterval");
	Ini->WriteInteger("Settings", "SetStatusDelay", SetDelay);
  }
  if(Ini->ValueExists("Settings", "DisableSongTimer"))
  {
	bool IgnorePlugins = Ini->ReadBool("Settings", "DisableSongTimer", true);
	Ini->DeleteKey("Settings", "DisableSongTimer");
	Ini->WriteBool("Settings", "IgnorePlugins", IgnorePlugins);
  }
  if(Ini->ValueExists("Settings", "TimeTurnOff"))
  {
	bool AutoTurnOff = Ini->ReadBool("Settings", "TimeTurnOff", false);
	Ini->DeleteKey("Settings", "TimeTurnOff");
	Ini->WriteBool("Settings", "AutoTurnOff", AutoTurnOff);
  }
  if(Ini->ValueExists("Settings", "TimeTurnOffInterval"))
  {
	int AutoTurnOffDelay = Ini->ReadInteger("Settings", "TimeTurnOffInterval", 15);
	Ini->DeleteKey("Settings", "TimeTurnOffInterval");
	Ini->WriteInteger("Settings", "AutoTurnOffDelay", AutoTurnOffDelay);
  }

  if(Ini->ValueExists("Settings", "EnableFastOnOff"))
  {
	bool FastAccess = Ini->ReadBool("Settings", "EnableFastOnOff", true);
	Ini->DeleteKey("Settings", "EnableFastOnOff");
	Ini->WriteBool("Settings", "FastAccess", FastAccess);
  }

  if(Ini->ValueExists("UserTune", "EnableN"))
  {
	bool Notification = Ini->ReadBool("UserTune", "EnableN", false);
	Ini->DeleteKey("UserTune", "EnableN");
	Ini->WriteBool("UserTune", "Notification", Notification);
  }
  if(Ini->ValueExists("UserTune", "EnableS"))
  {
	bool Send = Ini->ReadBool("UserTune", "EnableS", false);
	Ini->DeleteKey("UserTune", "EnableS");
	Ini->WriteBool("UserTune", "Send", Send);
  }
  if(Ini->ValueExists("UserTune", "TimeOutN"))
  {
	int Cloud = Ini->ReadInteger("UserTune", "TimeOutN", 4);
	Ini->DeleteKey("UserTune", "TimeOutN");
	Ini->WriteInteger("UserTune", "Cloud", Cloud);
  }
  if(Ini->ValueExists("UserTune", "TimeOutS"))
  {
	int SendDelay = Ini->ReadInteger("UserTune", "TimeOutS", 5);
	Ini->DeleteKey("UserTune", "TimeOutS");
	Ini->WriteInteger("UserTune", "SendDelay", SendDelay);
  }
  if(Ini->ValueExists("UserTune", "EnableAQQS"))
  {
	bool IgnoreCore = Ini->ReadBool("UserTune", "EnableAQQS", false);
	Ini->DeleteKey("UserTune", "EnableAQQS");
	Ini->WriteBool("UserTune", "IgnoreCore", IgnoreCore);
  }
  if(Ini->ValueExists("UserTune", "ExceptionCount"))
  {
	int ExceptionCount = Ini->ReadInteger("UserTune", "ExceptionCount", 0);
	if(ExceptionCount)
	{
	  Ini->DeleteKey("UserTune", "ExceptionCount");
	  for(int Count=0;Count<ExceptionCount;Count++)
	  {
		UnicodeString JID = Ini->ReadString("UserTuneEx" + IntToStr(Count+1), "JID", "");
		Ini->EraseSection("UserTuneEx" + IntToStr(Count+1));
		if(!JID.IsEmpty()) Ini->WriteString("UserTuneEx",("JID"+IntToStr(Count+1)),JID);
	  }
	}
  }
  if(Ini->SectionExists("Auto"))
  {
	for(int Count=0;Count<13;Count++)
	{
	  UnicodeString Player = Ini->ReadString("Auto",("Player"+IntToStr(Count)),"");
	  if(Player=="Winamp/AIMP2/KMPlayer") Player = 1;
	  else if(Player=="Foobar2000") Player = 2;
	  else if(Player=="Windows Media Player") Player = 3;
	  else if(Player=="VUPlayer") Player = 4;
	  else if(Player=="XMPlay") Player = 5;
	  else if(Player=="Media Player Classic") Player = 6;
	  else if(Player=="iTunes") Player = 7;
	  else if(Player=="ALSong") Player = 8;
	  else if(Player=="AQQ Radio") Player = 9;
	  else if(Player=="Screamer Radio") Player = 10;
	  else if(Player=="aTunes") Player = 11;
	  else if(Player=="Songbird") Player = 12;
	  else if(Player=="Last.fm Player") Player = 13;
	  int Enabled = Ini->ReadBool("Auto",("PlayerCheck"+IntToStr(Count)), false);
	  Ini->WriteString("AutoMode", ("Player"+IntToStr(Count+1)), (Player+";"+IntToStr(Enabled)));
	}
	Ini->EraseSection("Auto");
  }
  //---------------------------------------------------------------------------
  bool EnableOnStart = Ini->ReadBool("Settings", "EnableOnStart", false);
  bool FastAccessChk = Ini->ReadBool("Settings", "FastAccess", true);
  UserTuneNotif = Ini->ReadBool("UserTune", "Notification", false);
  bool UserTuneSend = Ini->ReadBool("UserTune", "Send", false);
  UserTuneCloud = Ini->ReadInteger("UserTune", "Cloud", 4);
  TStringList *JIDList = new TStringList;
  Ini->ReadSection("UserTuneEx",JIDList);
  int JIDListCount = JIDList->Count;
  delete JIDList;
  if(JIDListCount>0)
  {
	for(int Count=0;Count<JIDListCount;Count++)
	{
	  UnicodeString JID = Ini->ReadString("UserTuneEx",("JID"+IntToStr(Count+1)), "");
	  if(!JID.IsEmpty()) UserTuneExceptions->Add(JID);
	}
  }
  delete Ini;
  //Wlaczenie wysylania informacji User Tune
  if(UserTuneSend)
  {
	if(!hMainForm)
	{
	  Application->Handle = (HWND)MainForm;
	  hMainForm = new TMainForm(Application);
	}
	hMainForm->aLoadSettings->Execute();
	hMainForm->UserTuneTimer->Enabled = true;
  }
  //Wlaczenie dzialania wtyczki przy starcie
  if(EnableOnStart)
  {
	if(!hMainForm)
	{
	  Application->Handle = (HWND)MainForm;
	  hMainForm = new TMainForm(Application);
	}
	hMainForm->aLoadSettings->Execute();
	hMainForm->GetStatusTimer->Enabled = true;
	hMainForm->SetStatusTimer->Interval = 1000;
	hMainForm->JustEnabled = true;
  }
  //Tworzenie buttonu szybkiego wlaczania/wylaczania
  if(FastAccessChk)
   BuildFastAccess();

  return 0;
}
//---------------------------------------------------------------------------

//Otwieranie formy ustawien
extern "C" int __declspec(dllexport)__stdcall Settings()
{
  if(!hMainForm)
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
  //Przywracanie poczatkowego opisu
  if((hMainForm)&&(hMainForm->AutoModeTimer->Enabled))
  {
	hMainForm->TempStatus = GetStatus();
	if(hMainForm->StartStatus!=hMainForm->TempStatus)
	{
	  while(hMainForm->StartStatus!=GetStatus())
	   SetStatus(hMainForm->StartStatus,!hMainForm->SetOnlyInJabberChk);
	}
  }
  //Wylacznie UserTune
  if((hMainForm)&&(hMainForm->UserTuneTimer->Enabled))
   SetUserTune("","");
   //Wyladowanie hookow
  PluginLink.UnhookEvent(OnPreSetNote);
  PluginLink.UnhookEvent(OnStateChange);
  PluginLink.UnhookEvent(OnCurrentSong);
  PluginLink.UnhookEvent(OnXMLDebug);
  PluginLink.UnhookEvent(OnThemeChanged);
  PluginLink.UnhookEvent(OnModulesLoaded);
  //Usuwanie listy wyjatkow User Tune
  delete UserTuneExceptions;
  //Usuniecie elementow interfejsu
  PluginLink.DestroyServiceFunction(TuneStatus_PopPluginsAccess);
  PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM,0,(LPARAM)(&PopPluginsAccess));
  PluginLink.CallService(AQQ_ICONS_DESTROYPNGICON,0,(LPARAM)(POPPLUGINSACCESS));
  PluginLink.CallService(AQQ_ICONS_DESTROYPNGICON,0,(LPARAM)(FASTACCESS_OFF));
  PluginLink.CallService(AQQ_ICONS_DESTROYPNGICON,0,(LPARAM)(FASTACCESS_ON));
  PluginLink.DestroyServiceFunction(TuneStatus_FastAccessButton);
  PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_DESTROYBUTTON ,0,(LPARAM)(&FastAccessButton));
  return 0;
}
//---------------------------------------------------------------------------

//Informacje o wtyczce
extern "C" __declspec(dllexport) PPluginInfo __stdcall AQQPluginInfo(DWORD AQQVersion)
{
  PluginInfo.cbSize = sizeof(TPluginInfo);
  PluginInfo.ShortName = L"TuneStatus";
  PluginInfo.Version = PLUGIN_MAKE_VERSION(2,2,1,0);
  PluginInfo.Description = L"Wstawianie do opisu aktualnie s³uchanego utworu z wielu odtwarzaczy";
  PluginInfo.Author = L"Krzysztof Grochocki (Beherit)";
  PluginInfo.AuthorMail = L"kontakt@beherit.pl";
  PluginInfo.Copyright = L"Krzysztof Grochocki (Beherit)";
  PluginInfo.Homepage = L"http://beherit.pl";

  return &PluginInfo;
}
//---------------------------------------------------------------------------
