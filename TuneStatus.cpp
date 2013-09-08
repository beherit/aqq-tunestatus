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

//utworzenie obiektow do struktur
TPluginAction PopPluginsAccess;
TPluginAction FastEnableAccess;
TPluginStateChange PluginStateChange;
TPluginShowInfo PluginShowInfo;
TPluginContactSimpleInfo PluginContactSimpleInfo;
TPluginLink PluginLink;
TPluginInfo PluginInfo;

//Do przywracania opisu po przejsciu na niewidoczny
PPluginStateChange StateChange;

//Uchwytu do formy ustawien
TMainForm *hMainForm;

//Indeksy ikon
int POPPLUGINSACCESS;
int FASTENABLE_ON;
int FASTENABLE_OFF;

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
bool UserTuneNotif; //Czy wyswietlac notyfikacje
//Do wyjatkow z notyfikacji UserTune
UnicodeString ExceptionJID[50];
int ExceptionCount;
bool AllowException;

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
	FastEnableAccess.IconIndex = FASTENABLE_ON;
	PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_UPDATEBUTTON,0,(LPARAM)(&FastEnableAccess));
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
	FastEnableAccess.IconIndex = FASTENABLE_OFF;
	PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_UPDATEBUTTON,0,(LPARAM)(&FastEnableAccess));
  }
  hMainForm->RunPluginCheckBox->Checked=!hMainForm->RunPluginCheckBox->Checked;

  return 0;
}
//---------------------------------------------------------------------------

//Serwis otwarcia formy ustawien
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
  if(AQQRadioSong.IsEmpty())
    return 0;
  else
    return 1;
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
  if((AllowUserTuneN)&&(UserTuneNotif))
  {
	//UserTuneSong = (wchar_t*)(wParam);

	XMLChunk = (PPluginXMLChunk)lParam;

	XML = (wchar_t*)XMLChunk->XML;

	if(
	(XML.Pos("<tune xmlns='http://jabber.org/protocol/tune'>")>0)&&
	(XML.Pos("type='error'")==0)&&
	(XML.Pos("<title>")>0)
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
		  if(XML.Pos("<artist>")>0)
		  {
			UserTuneSongTMP=XML;
			UserTuneSongTMP.Delete(1,UserTuneSongTMP.Pos("<artist>")+7);
			UserTuneSongTMP.Delete(UserTuneSongTMP.Pos("</artist>"), UserTuneSongTMP.Length());
			UserTuneSongTMP=UserTuneSongTMP.Trim();
			UserTuneSong = UserTuneSongTMP;

			UserTuneSongTMP=XML;
			UserTuneSongTMP.Delete(1,UserTuneSongTMP.Pos("<title>")+6);
			UserTuneSongTMP.Delete(UserTuneSongTMP.Pos("</title>"), UserTuneSongTMP.Length());
			UserTuneSongTMP=UserTuneSongTMP.Trim();
			UserTuneSong = UserTuneSong + " - " + UserTuneSongTMP;
		  }
		  //Artysta i tytul razem w tagu <title>
		  else
		  {
			UserTuneSong=XML;
			UserTuneSong.Delete(1,UserTuneSong.Pos("<title>")+6);
			UserTuneSong.Delete(UserTuneSong.Pos("</title>"), UserTuneSong.Length());
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
		  if(UserTuneSong.Pos("*** ")>0)
		  {
			UserTuneSong.Delete(1, UserTuneSong.Pos("*** ") + 3);
			UserTuneSong=UserTuneSong.Trim();
		  }

		  //Usuwanie numeru piosenki
		  if((UserTuneSong.Pos(". ")>0)&&(UserTuneSong.Pos(". ")<7))
		  {
			UserTuneSongTMP=UserTuneSong;
			UserTuneSongTMP.Delete(UserTuneSong.Pos(". "), UserTuneSongTMP.Length());
			if(TestDigit(UserTuneSongTMP))
			{
			  UserTuneSong.Delete(1, UserTuneSong.Pos(". "));
			  UserTuneSong=UserTuneSong.Trim();
			}
		  }

		  //Usuwanie "Wtyczka AQQ Radio: "
		  if(UserTuneSong.Pos("Wtyczka AQQ Radio: ")>0)
		   UserTuneSong.Delete(1,UserTuneSong.Pos("Wtyczka AQQ Radio: ")+18);

		  //Pobieranie Nick'a z JID
		  PluginContactSimpleInfo.cbSize = sizeof(TPluginContactSimpleInfo);
		  PluginContactSimpleInfo.JID = XMLFrom.w_str();//UserTuneFrom.w_str();
		  PluginLink.CallService(AQQ_CONTACTS_FILLSIMPLEINFO,0,(LPARAM)(&PluginContactSimpleInfo));
		  UserTuneSongTMP = (wchar_t*)(PluginContactSimpleInfo.Nick);
		  UserTuneSongTMP = UserTuneSongTMP.Trim();
		  if(!UserTuneSongTMP.IsEmpty())
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
  UserTuneNotif = Enabled;
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
  hMainForm->AllowUserTuneNTimer->Enabled = true;

  return 0;
}
//---------------------------------------------------------------------------

//Zezwalanie na pokazywanie notyfikacji User Tune
void SetAllowUserTuneNTrue()
{
  AllowUserTuneN = true;
}
//---------------------------------------------------------------------------

//Pobieranie odtwarzanego utworu w AQQ Radio
UnicodeString GetAQQRadioSong()
{
  UnicodeString Song = AQQRadioSong;
  return Song;
}
//---------------------------------------------------------------------------

//Notyfikacja zmiany statusu
int __stdcall OnPreSetNote(WPARAM wParam, LPARAM lParam)
{
  if((hMainForm)&&(hMainForm->Timer->Enabled))
  {
	UnicodeString NewStatus = (wchar_t*)(lParam);

	if((NewStatus!=hMainForm->Opis_pocz)
	&&(NewStatus!=hMainForm->OpisTMP)
	&&(NewStatus!=hMainForm->Opis)
	&&(!hMainForm->Opis.IsEmpty()))
	{
	  hMainForm->Opis_pocz = NewStatus;
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
  if((hMainForm)&&(hMainForm->Timer->Enabled)&&(!hMainForm->Opis.IsEmpty()))
  {
    StateChange = (PPluginStateChange)lParam;
	int NewState = StateChange->NewState;

	//Jezeli blokowac przy niewidocznym
	if((hMainForm->BlockInvisibleCheck)&&(NewState==6))
	 hMainForm->StateChangeTimer->Enabled = true;
  }

  return 0;
}
//---------------------------------------------------------------------------

//Tworzenie skrótu w popPlugins
void BuildPopPluginsAccess()
{
  PopPluginsAccess.cbSize = sizeof(TPluginAction);
  PopPluginsAccess.pszName = (wchar_t*)L"TuneStatusPopPluginsAccess";
  PopPluginsAccess.pszCaption = (wchar_t*) L"TuneStatus";
  PopPluginsAccess.Position = 0;
  PopPluginsAccess.IconIndex = POPPLUGINSACCESS;
  PopPluginsAccess.pszService = (wchar_t*) L"sTuneStatusPopPluginsAccess";
  PopPluginsAccess.pszPopupName = (wchar_t*) L"popPlugins";
  PopPluginsAccess.PopupPosition = 0;

  PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&PopPluginsAccess));
  PluginLink.CreateServiceFunction(L"sTuneStatusPopPluginsAccess",TuneStatusService);
}
//---------------------------------------------------------------------------

//Tworzenie buttonu do szybkiego wlaczania/wylaczania
void BuildFastEnableAccess()
{
  FastEnableAccess.cbSize = sizeof(TPluginAction);
  FastEnableAccess.pszName = (wchar_t*) L"TuneStatusFastEnableAccess";
  FastEnableAccess.Position = 999;
  FastEnableAccess.IconIndex = FASTENABLE_OFF;
  FastEnableAccess.pszService = (wchar_t*) L"sTuneStatusFastEnableAccess";

  PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_CREATEBUTTON,0,(LPARAM)(&FastEnableAccess));
  PluginLink.CreateServiceFunction(L"sTuneStatusFastEnableAccess",TuneStatus_FastOnOff);
}
//---------------------------------------------------------------------------

//Usuwanie buttonu do szybkiego wlaczania/wylaczania
void DestroyFastEnableAccess()
{
  PluginLink.DestroyServiceFunction(TuneStatus_FastOnOff);
  PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_DESTROYBUTTON ,0,(LPARAM)(&FastEnableAccess));
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
  UnicodeString Dir = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,0,0));
  Dir = StringReplace(Dir, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
  return Dir;
}
//---------------------------------------------------------------------------

//Pobieranie sciezki do pliku wtyczki
UnicodeString GetPluginDir()
{
  UnicodeString Dir = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINDIR,(WPARAM)(HInstance),0));
  Dir = StringReplace(Dir, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
  return Dir;
}
//---------------------------------------------------------------------------

//Ustawianie UserTune
void SetUserTune(UnicodeString Tune, UnicodeString Time)
{
  UnicodeString TuneXML;
  UnicodeString TuneAccountName;
  Tune = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_CONVERTTOXML,0,(WPARAM)Tune.w_str()));

  int TuneCount = PluginLink.CallService(AQQ_FUNCTION_GETUSEREXCOUNT,0,0);
  for(int TuneAccount=0;TuneAccount<TuneCount;TuneAccount++)
  {
	//Gdy stan konta jest rozny od niewidoczny/rozlaczony
	if((AllowChangeStatusUserTune(TuneAccount))&&(!Tune.IsEmpty()))
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
void UpdateButton(bool Enabled)
{
  if(Enabled)
  {
	FastEnableAccess.IconIndex = FASTENABLE_ON;
	PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_UPDATEBUTTON,0,(LPARAM)(&FastEnableAccess));
  }
  else
  {
	FastEnableAccess.IconIndex = FASTENABLE_OFF;
	PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_UPDATEBUTTON,0,(LPARAM)(&FastEnableAccess));
  }
}
//---------------------------------------------------------------------------

//Wylaczanie obslugi UserTune w AQQ
void SetAQQUserTuneOff()
{
  TSaveSetup SaveSetup;
  SaveSetup.Section = L"Settings";
  SaveSetup.Ident = L"Tune";
  SaveSetup.Value = L"0";

  PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
}
//---------------------------------------------------------------------------

//Odswiezanie wyjatkow z notyfikacji UserTune
void RefreshUserTuneException()
{
  TIniFile *Ini = new TIniFile(GetPluginUserDir() + "\\\\TuneStatus\\\\TuneStatus.ini");
  ExceptionCount = Ini->ReadInteger("UserTune", "ExceptionCount", 0);
  if(ExceptionCount!=0)
   for(int Count=0;Count<ExceptionCount;Count++)
	ExceptionJID[Count] = Ini->ReadString("UserTuneEx" + IntToStr(Count+1), "JID", "");
  delete Ini;
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
  //Przypisanie ikony
  POPPLUGINSACCESS = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(Dir + "\\\\TuneStatus\\\\TuneStatus.png").w_str());
  FASTENABLE_OFF = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(Dir +"\\\\TuneStatus\\\\TuneStatusOff.png").w_str());
  FASTENABLE_ON = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(Dir +"\\\\TuneStatus\\\\TuneStatusOn.png").w_str());
  //Przyspisanie przycisku w PopPlugins
  BuildPopPluginsAccess();
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
  AllowUserTuneN = PluginLink.CallService(AQQ_SYSTEM_MODULESLOADED,0,0);
  //Odczyt ustawien
  TIniFile *Ini = new TIniFile(Dir + "\\\\TuneStatus\\\\TuneStatus.ini");
  bool EnablePluginOnStart = Ini->ReadBool("Settings", "EnablePluginOnStart", 0);
  bool FastEnableAccessChk = Ini->ReadBool("Settings", "EnableFastOnOff", true);
  UserTuneNotif = Ini->ReadBool("UserTune", "EnableN", false);
  bool UserTuneSend = Ini->ReadBool("UserTune", "EnableS", false);
  UserTuneTimeOut = Ini->ReadInteger("UserTune", "TimeOutN", 4);
  ExceptionCount = Ini->ReadInteger("UserTune", "ExceptionCount", 0);
  if(ExceptionCount!=0)
   for(int Count=0;Count<ExceptionCount;Count++)
	ExceptionJID[Count] = Ini->ReadString("UserTuneEx" + IntToStr(Count+1), "JID", "");
  delete Ini;
  //Czy wlaczyc wysylanie informacji User Tune
  if(UserTuneSend)
  {
	if(!hMainForm)
	{
	  Application->Handle = (HWND)MainForm;
	  hMainForm = new TMainForm(Application);
	}
	hMainForm->aReadSettings->Execute();
	hMainForm->UserTuneTimer->Enabled = true;
  }
  //Czy w³¹czonac na starcie
  if(EnablePluginOnStart)
  {
	if(!hMainForm)
	{
	  Application->Handle = (HWND)MainForm;
	  hMainForm = new TMainForm(Application);
	}
	hMainForm->aReadSettings->Execute();
	hMainForm->GetStatusTimer->Enabled = true;
	hMainForm->SongTimer->Interval = 1000;
	hMainForm->JustEnabled = true;
  }
  //Czy przyspisac button
  if(FastEnableAccessChk)
   BuildFastEnableAccess();

  return 0;
}
//---------------------------------------------------------------------------

//Otwieranie formy przez ustawienia
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
  if((hMainForm)&&(hMainForm->Timer->Enabled))
  {
	hMainForm->OpisTMP=GetStatus();
	if(hMainForm->Opis_pocz!=hMainForm->OpisTMP)
	{
	  while(GetStatus()!=hMainForm->Opis_pocz)
	   SetStatus(hMainForm->Opis_pocz,!hMainForm->SetOnlyInJabberCheck);
	}
  }
  if((hMainForm)&&(hMainForm->UserTuneTimer->Enabled))
   SetUserTune("","");
  PluginLink.UnhookEvent(OnPreSetNote);
  PluginLink.UnhookEvent(OnCurrentSong);
  PluginLink.UnhookEvent(OnStateChange);
  PluginLink.UnhookEvent(OnXMLDebug);
  PluginLink.UnhookEvent(OnModulesLoaded);
  PluginLink.DestroyServiceFunction(TuneStatusService);
  PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM,0,(LPARAM)(&PopPluginsAccess));
  PluginLink.CallService(AQQ_ICONS_DESTROYPNGICON,0,(LPARAM)(POPPLUGINSACCESS));
  PluginLink.CallService(AQQ_ICONS_DESTROYPNGICON,0,(LPARAM)(FASTENABLE_OFF));
  PluginLink.CallService(AQQ_ICONS_DESTROYPNGICON,0,(LPARAM)(FASTENABLE_ON));
  PluginLink.DestroyServiceFunction(TuneStatus_FastOnOff);
  PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_DESTROYBUTTON ,0,(LPARAM)(&FastEnableAccess));

  return 0;
}
//---------------------------------------------------------------------------

extern "C" __declspec(dllexport) PPluginInfo __stdcall AQQPluginInfo(DWORD AQQVersion)
{
  PluginInfo.cbSize = sizeof(TPluginInfo);
  PluginInfo.ShortName = (wchar_t*)L"TuneStatus";
  PluginInfo.Version = PLUGIN_MAKE_VERSION(2,1,0,0);
  PluginInfo.Description = (wchar_t *)L"Wstawianie do opisu aktualnie s³uchanego utworu z wielu odtwarzaczy";
  PluginInfo.Author = (wchar_t *)L"Krzysztof Grochocki (Beherit)";
  PluginInfo.AuthorMail = (wchar_t *)L"email@beherit.pl";
  PluginInfo.Copyright = (wchar_t *)L"Krzysztof Grochocki (Beherit)";
  PluginInfo.Homepage = (wchar_t *)L"http://beherit.pl";

  return &PluginInfo;
}
//---------------------------------------------------------------------------
