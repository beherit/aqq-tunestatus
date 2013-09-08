//---------------------------------------------------------------------------
#include <vcl.h>
#include <inifiles.hpp>
#pragma hdrstop
#include "MainFrm.h"
#include "UserTuneExceptionFrm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "sBevel"
#pragma link "sButton"
#pragma link "sCheckBox"
#pragma link "sCheckListBox"
#pragma link "sComboBox"
#pragma link "sEdit"
#pragma link "sLabel"
#pragma link "sListBox"
#pragma link "sMemo"
#pragma link "sPageControl"
#pragma link "sSkinManager"
#pragma link "sSkinProvider"
#pragma link "sSpinEdit"
#pragma resource "*.dfm"
TMainForm *MainForm;
//---------------------------------------------------------------------------
__declspec(dllimport)UnicodeString GetPluginUserDir();
__declspec(dllimport)UnicodeString GetThemeSkinDir();
__declspec(dllimport)bool ChkSkinEnabled();
__declspec(dllimport)bool ChkThemeAnimateWindows();
__declspec(dllimport)bool ChkThemeGlowing();
__declspec(dllimport)UnicodeString IniStrToStr(UnicodeString Str);
__declspec(dllimport)UnicodeString StrToIniStr(UnicodeString Str);
__declspec(dllimport)void BuildTuneStatusFastOperation();
__declspec(dllimport)void DestroyTuneStatusFastOperation();
__declspec(dllimport)void UpdateTuneStatusFastOperation(bool Enabled);
__declspec(dllimport)void FastOperation(bool FromForm);
__declspec(dllimport)void LoadSettings();
__declspec(dllimport)void ChangeUserTuneStatus(bool Enabled);
__declspec(dllimport)void TurnOffCoreUserTune();
__declspec(dllimport)void ForceChangeStatus();
//---------------------------------------------------------------------------
UnicodeString AutoModeListText;
//---------------------------------------------------------------------------

__fastcall TMainForm::TMainForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::WMTransparency(TMessage &Message)
{
  Application->ProcessMessages();
  sSkinProvider->BorderForm->UpdateExBordersPos(true,(int)Message.LParam);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormCreate(TObject *Sender)
{
  //Wlaczona zaawansowana stylizacja okien
  if(ChkSkinEnabled())
  {
	UnicodeString ThemeSkinDir = GetThemeSkinDir();
	//Plik zaawansowanej stylizacji okien istnieje
	if(FileExists(ThemeSkinDir + "\\\\Skin.asz"))
	{
	  ThemeSkinDir = StringReplace(ThemeSkinDir, "\\\\", "\\", TReplaceFlags() << rfReplaceAll);
	  sSkinManager->SkinDirectory = ThemeSkinDir;
	  sSkinManager->SkinName = "Skin.asz";
	  if(ChkThemeAnimateWindows()) sSkinManager->AnimEffects->FormShow->Time = 200;
	  else sSkinManager->AnimEffects->FormShow->Time = 0;
	  sSkinManager->Effects->AllowGlowing = ChkThemeGlowing();
	  sSkinManager->Active = true;
	}
	//Brak pliku zaawansowanej stylizacji okien
	else sSkinManager->Active = false;
  }
  //Zaawansowana stylizacja okien wylaczona
  else sSkinManager->Active = false;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormShow(TObject *Sender)
{
  //Odczyt ustawien
  aLoadSettings->Execute();
  //Dodawanie brakujacych odtwarzaczy do listy
  if(AutoModeCheckListBoxPreview->Items->IndexOf("Winamp/AIMP/KMPlayer")==-1)
   AutoModeCheckListBoxPreview->Items->Add("Winamp/AIMP/KMPlayer");
  if(AutoModeCheckListBoxPreview->Items->IndexOf("Foobar2000")==-1)
   AutoModeCheckListBoxPreview->Items->Add("Foobar2000");
  if(AutoModeCheckListBoxPreview->Items->IndexOf("Windows Media Player")==-1)
   AutoModeCheckListBoxPreview->Items->Add("Windows Media Player");
  if(AutoModeCheckListBoxPreview->Items->IndexOf("VLC media player")==-1)
   AutoModeCheckListBoxPreview->Items->Add("VLC media player");
  if(AutoModeCheckListBoxPreview->Items->IndexOf("Spotify")==-1)
   AutoModeCheckListBoxPreview->Items->Add("Spotify");
  if(AutoModeCheckListBoxPreview->Items->IndexOf("VUPlayer")==-1)
   AutoModeCheckListBoxPreview->Items->Add("VUPlayer");
  if(AutoModeCheckListBoxPreview->Items->IndexOf("XMPlay")==-1)
   AutoModeCheckListBoxPreview->Items->Add("XMPlay");
  if(AutoModeCheckListBoxPreview->Items->IndexOf("Media Player Classic")==-1)
   AutoModeCheckListBoxPreview->Items->Add("Media Player Classic");
  if(AutoModeCheckListBoxPreview->Items->IndexOf("iTunes")==-1)
   AutoModeCheckListBoxPreview->Items->Add("iTunes");
  if(AutoModeCheckListBoxPreview->Items->IndexOf("ALSong")==-1)
   AutoModeCheckListBoxPreview->Items->Add("ALSong");
  if(AutoModeCheckListBoxPreview->Items->IndexOf("Wtyczki (np. AQQ Radio)")==-1)
   AutoModeCheckListBoxPreview->Items->Add("Wtyczki (np. AQQ Radio)");
  if(AutoModeCheckListBoxPreview->Items->IndexOf("Screamer Radio")==-1)
   AutoModeCheckListBoxPreview->Items->Add("Screamer Radio");
  if(AutoModeCheckListBoxPreview->Items->IndexOf("aTunes")==-1)
   AutoModeCheckListBoxPreview->Items->Add("aTunes");
  if(AutoModeCheckListBoxPreview->Items->IndexOf("Songbird")==-1)
   AutoModeCheckListBoxPreview->Items->Add("Songbird");
  if(AutoModeCheckListBoxPreview->Items->IndexOf("Last.fm Player")==-1)
   AutoModeCheckListBoxPreview->Items->Add("Last.fm Player");
  //Dodanie tekstu do TagsBox i ustawienie go jako element pokazywany
  TagsBox->Items->Add("Wybierz tag do wstawienia");
  TagsBox->ItemIndex = 6;
  //Wylaczenie przycisku zastosuj
  SaveButton->Enabled = false;
  //Ustawienie domyslnej karty okna ustawien
  PageControl->ActivePage = HandlingTabSheet;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormClose(TObject *Sender, TCloseAction &Action)
{
  //Usuwanie tekstu "Wybierz tag do wstawienia" z TagsBox
  TagsBox->Items->Delete(6);
  //Odczyt ustawien
  aLoadSettings->Execute();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::SaveButtonClick(TObject *Sender)
{
  //Zapis ustawien
  aSaveSettings->Execute();
  //Odczyt ustawien w rdzeniu wtyczki
  LoadSettings();
  //Wylaczanie obslugi User Tune w rdzeniu AQQ
  if((UserTuneSendCheckBox->Checked)&&(IgnoreCoreUserTuneCheckBox->Checked)) TurnOffCoreUserTune();
  //Wlaczenie/wylaczenie obslugi User Tune
  ChangeUserTuneStatus(UserTuneSendCheckBox->Checked);
  //Wymuszenie natychmiastowego ustawienienia w opisie dokonanych zmian
  ForceChangeStatus();
  //Usuwanie tekstu "Wybierz tag do wstawienia" z TagsBox
  TagsBox->Items->Delete(6);
  //Wylaczenie przycisku zastosuj
  SaveButton->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::OKButtonClick(TObject *Sender)
{
  //Zapis ustawien
  aSaveSettings->Execute();
  //Odczyt ustawien w rdzeniu wtyczki
  LoadSettings();
  //Wylaczanie obslugi User Tune w rdzeniu AQQ
  if((UserTuneSendCheckBox->Checked)&&(IgnoreCoreUserTuneCheckBox->Checked)) TurnOffCoreUserTune();
  //Wlaczenie/wylaczenie obslugi User Tune
  ChangeUserTuneStatus(UserTuneSendCheckBox->Checked);
  //Wymuszenie natychmiastowego ustawienienia w opisie dokonanych zmian
  ForceChangeStatus();
  //Usuwanie tekstu "Wybierz tag do wstawienia" z TagsBox
  TagsBox->Items->Delete(6);
  //Ukrywanie formy
  Close();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::RunPluginCheckBoxClick(TObject *Sender)
{
  if(RunPluginCheckBox->Focused()) FastOperation(true);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AutoModeCheckListBoxPreviewClick(TObject *Sender)
{
  //Gdy stan komponentu ulegl zmianie
  if(AutoModeCheckListBoxPreview->Items->Text!=AutoModeListText)
  {
	//Przekazanie nowego stanu kontrolki
	AutoModeListText = AutoModeCheckListBoxPreview->Items->Text;
	//Wlaczenie przycisku zastosuj
	SaveButton->Enabled = true;
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AutoModeCheckListBoxPreviewDragDrop(
	  TObject *Sender, TObject *Source, int X, int Y)
{
  //Komponent docelowy i zrodlowy jest taki sam
  if((Sender->ClassNameIs("TsCheckListBox"))&&(Source->ClassNameIs("TsCheckListBox")))
  {
	TPoint Point;
	Point.x = X;
	Point.y = Y;
	//Upuszczenie na istniejacy element
	if(AutoModeCheckListBoxPreview->ItemAtPos(Point,true)!=-1)
	{
	  //Zmiana miejsc elementow
	  AutoModeCheckListBoxPreview->Items->Exchange(AutoModeCheckListBoxPreview->ItemIndex,AutoModeCheckListBoxPreview->ItemAtPos(Point,true));
	  //Wlaczenie przycisku zastosuj
	  SaveButton->Enabled = true;
	}
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AutoModeCheckListBoxPreviewDragOver(
	  TObject *Sender, TObject *Source, int X, int Y, TDragState State,
	  bool &Accept)
{
  //Akceptowanie tylko danego komponentu
  Accept = Source->ClassNameIs("TsCheckListBox");
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AutoModeCheckListBoxPreviewMouseMove(
	  TObject *Sender, TShiftState Shift, int X, int Y)
{
  TPoint Point;
  Point.x = X;
  Point.y = Y;
  //Kursor nad istniejacym elementem
  if(AutoModeCheckListBoxPreview->ItemAtPos(Point,true)!=-1)
  {
	//Foobar2000
	if(AutoModeCheckListBoxPreview->Items->Strings[AutoModeCheckListBoxPreview->ItemAtPos(Point,true)]=="Foobar2000")
	{
	  AutoModeCheckListBoxPreview->Hint = "Dla lepszego dzia³ania zaleca siê zainstalowaæ wtyczkê do Foobar2000";
	  AutoModeCheckListBoxPreview->PopupMenu = FoobarPopupMenu;
	  AutoModeCheckListBoxPreview->Cursor = crHelp;
	}
	//WMP
	else if(AutoModeCheckListBoxPreview->Items->Strings[AutoModeCheckListBoxPreview->ItemAtPos(Point,true)]=="Windows Media Player")
	{
	  AutoModeCheckListBoxPreview->Hint = "Dla wersji WMP powy¿ej 7 do ob³sugi potrzebny jest plugin w WMP";
	  AutoModeCheckListBoxPreview->PopupMenu = WMPPopupMenu;
	  AutoModeCheckListBoxPreview->Cursor = crHelp;
	}
	//iTunes
	else if(AutoModeCheckListBoxPreview->Items->Strings[AutoModeCheckListBoxPreview->ItemAtPos(Point,true)]=="iTunes")
	{
	  AutoModeCheckListBoxPreview->Hint = "Do obs³ugi potrzebny jest plugin w iTunes";
	  AutoModeCheckListBoxPreview->PopupMenu = iTunesPopupMenu;
	  AutoModeCheckListBoxPreview->Cursor = crHelp;
	}
	//Songbird
	else if(AutoModeCheckListBoxPreview->Items->Strings[AutoModeCheckListBoxPreview->ItemAtPos(Point,true)]=="Songbird")
	{
	  AutoModeCheckListBoxPreview->Hint = "Do obs³ugi potrzebne jest rozszerzenie Birdtitle zainstalowane w Songbird";
	  AutoModeCheckListBoxPreview->PopupMenu = SongbirdPopupMenu;
	  AutoModeCheckListBoxPreview->Cursor = crHelp;
	}
	//Inne
	else
	{
	  AutoModeCheckListBoxPreview->Hint = "";
	  AutoModeCheckListBoxPreview->PopupMenu = NULL;
	  AutoModeCheckListBoxPreview->Cursor = crDefault;
	}
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FoobarDownloadClick(TObject *Sender)
{
  ShellExecute(NULL, L"open", L"http://beherit.pl/download/Foobar2000.zip", NULL, NULL, SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::WMPDownloadClick(TObject *Sender)
{
  ShellExecute(NULL, L"open", L"http://beherit.pl/download/WMP.zip", NULL, NULL, SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::iTunesDownloadClick(TObject *Sender)
{
  ShellExecute(NULL, L"open", L"http://beherit.pl/download/iTunes.zip", NULL, NULL, SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::SongbirdDownloadClick(TObject *Sender)
{
  ShellExecute(NULL, L"open", L"http://addons.songbirdnest.com/addon/42", NULL, NULL, SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::TagsBoxCloseUp(TObject *Sender)
{
  //Dodanie tekstu do TagsBox i ustawienie go jako element pokazywany
  if(TagsBox->Text.IsEmpty())
  {
	TagsBox->Items->Delete(6);
	TagsBox->Items->Add("Wybierz tag do wstawienia");
	TagsBox->ItemIndex = 6;
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::TagsBoxDropDown(TObject *Sender)
{
  //Usuwanie tekstu "Wybierz tag do wstawienia" z TagsBox
  TagsBox->Items->Delete(6);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::TagsBoxSelect(TObject *Sender)
{
  //Pobranie tekstu wybranego elementu
  UnicodeString Tag = TagsBox->Text;
  //Wybrany element rozny od elementu sztucznego
  if(TagsBox->Text!="--------Tylko dla wybranych odtwarzaczy--------")
  {
	//Parsowanie wybranego tagu
	Tag = Tag.Delete(Tag.Pos("("),Tag.Length());
	Tag = Tag.Trim();
	//Wybrany tag nie zostal juz wybrany
	if(!PreviewStatusMemo->Text.Pos(Tag))
	{
	  UnicodeString Before,After;
	  //Pobranie tekstu przed kusorem
	  Before = PreviewStatusMemo->Text.SubString(0,PreviewStatusMemo->SelStart);
	  //Pobranie tekstu po kursorze
	  After = PreviewStatusMemo->Text.SubString(PreviewStatusMemo->SelStart+1,(PreviewStatusMemo->Text).Length());
	  //Dodanie tagu miedzy tekst
	  PreviewStatusMemo->Clear();
	  PreviewStatusMemo->Text = Before + " " + Tag + " " + After;
	}
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AutoTurnOffCheckBoxClick(TObject *Sender)
{
  AutoTurnOffSpin->Enabled = AutoTurnOffCheckBox->Checked;
  //Wlaczenie przycisku zastosuj
  SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::UserTuneNotificationCheckBoxClick(TObject *Sender)
{
  UserTuneNotificationSpin->Enabled = UserTuneNotificationCheckBox->Checked;
  UserTuneExceptionButton->Enabled = UserTuneNotificationCheckBox->Checked;
  //Wlaczenie przycisku zastosuj
  SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::UserTuneExceptionButtonClick(TObject *Sender)
{
  UserTuneExceptionForm = new TUserTuneExceptionForm(Application);
  UserTuneExceptionForm->ShowModal();
  delete UserTuneExceptionForm;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::UserTuneSendCheckBoxClick(TObject *Sender)
{
  UserTuneSendLabel->Enabled = UserTuneSendCheckBox->Checked;
  UserTuneSendSpin->Enabled = UserTuneSendCheckBox->Checked;
  IgnoreCoreUserTuneCheckBox->Enabled = UserTuneSendCheckBox->Checked;
  //Wlaczenie przycisku zastosuj
  SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::OtherTabSheetShow(TObject *Sender)
{
  //Poprawka do AlphaControls
  if(!AutoTurnOffSpin->Enabled)
  {
	AutoTurnOffSpin->Enabled = true;
	AutoTurnOffSpin->Enabled = false;
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::UserTuneTabSheetShow(TObject *Sender)
{
  //Poprawka do AlphaControls
  if(!UserTuneNotificationSpin->Enabled)
  {
	UserTuneNotificationSpin->Enabled = true;
	UserTuneNotificationSpin->Enabled = false;
  }
  if(!UserTuneSendSpin->Enabled)
  {
	UserTuneSendSpin->Enabled = true;
	UserTuneSendSpin->Enabled = false;
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aLoadSettingsExecute(TObject *Sender)
{
  TIniFile *Ini = new TIniFile(GetPluginUserDir() + "\\\\TuneStatus\\\\TuneStatus.ini");
  //Obslugiwane odtwarzacze
  AutoModeCheckListBoxPreview->Clear();
  for(int Count=0;Count<15;Count++)
  {
	//Pobieranie elementow
	UnicodeString PlayerID = Ini->ReadString("AutoMode",("Player"+IntToStr(Count+1)),"");
	if(!PlayerID.IsEmpty())
	{
	  //Pobieranie statusu wlaczenia elementu
	  UnicodeString Enabled = PlayerID;
	  Enabled = Enabled.Delete(1,Enabled.Pos(";"));
	  ////Odkodowywanie ID odtwarzacza
	  PlayerID = PlayerID.Delete(PlayerID.Pos(";"),PlayerID.Length());
	  if(PlayerID==1) PlayerID = "Winamp/AIMP/KMPlayer";
	  else if(StrToInt(PlayerID)==2) PlayerID = "Foobar2000";
	  else if(StrToInt(PlayerID)==3) PlayerID = "Windows Media Player";
	  else if(StrToInt(PlayerID)==4) PlayerID = "VUPlayer";
	  else if(StrToInt(PlayerID)==5) PlayerID = "XMPlay";
	  else if(StrToInt(PlayerID)==6) PlayerID = "Media Player Classic";
	  else if(StrToInt(PlayerID)==7) PlayerID = "iTunes";
	  else if(StrToInt(PlayerID)==8) PlayerID = "ALSong";
	  else if(StrToInt(PlayerID)==9) PlayerID = "Wtyczki (np. AQQ Radio)";
	  else if(StrToInt(PlayerID)==10) PlayerID = "Screamer Radio";
	  else if(StrToInt(PlayerID)==11) PlayerID = "aTunes";
	  else if(StrToInt(PlayerID)==12) PlayerID = "Songbird";
	  else if(StrToInt(PlayerID)==13) PlayerID = "Last.fm Player";
	  else if(StrToInt(PlayerID)==14) PlayerID = "VLC media player";
	  else if(StrToInt(PlayerID)==15) PlayerID = "Spotify";
	  //Dodawnie odtwarzacza do listy
	  AutoModeCheckListBoxPreview->Items->Add(PlayerID);
	  AutoModeCheckListBoxPreview->Checked[Count] = StrToBool(Enabled);
	}
  }
  AutoModeListText = AutoModeCheckListBoxPreview->Items->Text;
  //Wyglad opisu
  PreviewStatusMemo->Text = UTF8ToUnicodeString((IniStrToStr(Ini->ReadString("Settings", "Status", "Obecnie s³ucham: CC_TUNESTATUS"))).w_str());
  //Opoznienie ustawiania nowego opisu
  SetStatusSpin->Value = Ini->ReadInteger("Settings", "SetStatusDelay", 5);
  //Ignorowanie wtyczek w opoznieniu ustawiania nowego opisu
  IgnorePluginsCheckBox->Checked = Ini->ReadBool("Settings", "IgnorePlugins", true);
  //Nie ustawianie opisu przy stanie niewidocznym
  BlockInvisibleCheckBox->Checked = Ini->ReadBool("Settings", "BlockInvisible", true);
  //Ustawianie opisu tylko w sieci Jabber
  SetOnlyInJabberCheckBox->Checked = Ini->ReadBool("Settings", "SetOnlyInJabber", false);
  //Wlaczanie dzialnia wtyczki wraz z uruchomieniem
  EnableOnStartCheckBox->Checked = Ini->ReadBool("Settings", "EnableOnStart", false);
  ////Pokazywanie przycisku szybkiego wlaczania/wylaczania dzialnia wtyczki
  FastAccessCheckBox->Checked = Ini->ReadBool("Settings", "FastAccess", true);
  //Usuwanie nazwy radiostacji przy pobieraniu utworu z wtyczki AQQ Radio
  CutRadiostationNameCheckBox->Checked = Ini->ReadBool("Settings", "CutRadiostationName", true);
  //Usuwanie adresow URL z pobranych danych
  CutWWWCheckBox->Checked = Ini->ReadBool("Settings", "CutWWW", false);
  //Automatyczne wylaczanie dzialania wtyczki przy bezczynnosci
  AutoTurnOffCheckBox->Checked = Ini->ReadBool("Settings", "AutoTurnOff", false);
  AutoTurnOffSpin->Enabled = AutoTurnOffCheckBox->Checked;
  //Czas automatycznego wylaczania dzialania wtyczki przy bezczynnosci
  AutoTurnOffSpin->Value = Ini->ReadInteger("Settings", "AutoTurnOffDelay", 15);
  //Blokowanie danych z filmow wideo
  MovieExceptionCheckBox->Checked = Ini->ReadBool("Settings", "MovieException", true);
  //User Tune - powiadomienie o aktualnych sluchanych utworach przez inne kontakty
  UserTuneNotificationCheckBox->Checked = Ini->ReadBool("UserTune", "Notification", false);
  UserTuneNotificationSpin->Enabled = UserTuneNotificationCheckBox->Checked;
  UserTuneExceptionButton->Enabled = UserTuneNotificationCheckBox->Checked;
  //User Tune - czas wyswietlania chmurki informacyjnej
  UserTuneNotificationSpin->Value = Ini->ReadInteger("UserTune", "Cloud", 6);
  //User Tune - informowanie o aktualnym odtwarzanym przez nas utworze
  UserTuneSendCheckBox->Checked = Ini->ReadBool("UserTune", "Send", false);
  UserTuneSendLabel->Enabled = UserTuneSendCheckBox->Checked;
  UserTuneSendSpin->Enabled = UserTuneSendCheckBox->Checked;
  IgnoreCoreUserTuneCheckBox->Enabled = UserTuneSendCheckBox->Checked;
  //User Tune - opoznienie wysylania nowego odtwarzanego utworu
  UserTuneSendSpin->Value = Ini->ReadInteger("UserTune", "SendDelay", 5);
  if(UserTuneSendSpin->Value<4) UserTuneSendSpin->Value = 4;
  //User Tune - wymuszanie dzialania z Winamp oraz Foobar2000
  IgnoreCoreUserTuneCheckBox->Checked = Ini->ReadBool("UserTune", "IgnoreCore", false);

  delete Ini;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aSaveSettingsExecute(TObject *Sender)
{
  //Ustawianie domyslnego opisu
  if(!PreviewStatusMemo->Text.Pos("CC_TUNESTATUS"))
   PreviewStatusMemo->Text = "Obecnie s³ucham: CC_TUNESTATUS";

  TIniFile *Ini = new TIniFile(GetPluginUserDir() + "\\\\TuneStatus\\\\TuneStatus.ini");
  //Zapis ustawien trybu automatycznego
  for(int Count=0;Count<15;Count++)
  {
	UnicodeString Player = AutoModeCheckListBoxPreview->Items->Strings[Count];
	if(Player=="Winamp/AIMP/KMPlayer") Player = 1;
	else if(Player=="Foobar2000") Player = 2;
	else if(Player=="Windows Media Player") Player = 3;
	else if(Player=="VUPlayer") Player = 4;
	else if(Player=="XMPlay") Player = 5;
	else if(Player=="Media Player Classic") Player = 6;
	else if(Player=="iTunes") Player = 7;
	else if(Player=="ALSong") Player = 8;
	else if(Player=="Wtyczki (np. AQQ Radio)") Player = 9;
	else if(Player=="Screamer Radio") Player = 10;
	else if(Player=="aTunes") Player = 11;
	else if(Player=="Songbird") Player = 12;
	else if(Player=="Last.fm Player") Player = 13;
	else if(Player=="VLC media player") Player = 14;
	else if(Player=="Spotify") Player = 15;
	int Enabled = AutoModeCheckListBoxPreview->Checked[Count];
	Ini->WriteString("AutoMode", ("Player"+IntToStr(Count+1)), (Player+";"+IntToStr(Enabled)));
  }
  //Status
  ShortString StatusShort = UTF8EncodeToShortString(PreviewStatusMemo->Text);
  Ini->WriteString("Settings", "Status", StrToIniStr(StatusShort.operator AnsiString()));
  //SetOnlyInJabber
  Ini->WriteBool("Settings", "SetOnlyInJabber", SetOnlyInJabberCheckBox->Checked);
  //BlockInvisible
  Ini->WriteBool("Settings", "BlockInvisible", BlockInvisibleCheckBox->Checked);
  //EnableOnStart
  Ini->WriteBool("Settings", "EnableOnStart", EnableOnStartCheckBox->Checked);
  //FastAccess
  Ini->WriteBool("Settings", "FastAccess", FastAccessCheckBox->Checked);
  DestroyTuneStatusFastOperation();
  if(FastAccessCheckBox->Checked) BuildTuneStatusFastOperation();
  //SetStatusDelay
  Ini->WriteInteger("Settings", "SetStatusDelay", SetStatusSpin->Value);
  //IgnorePlugins
  Ini->WriteBool("Settings", "IgnorePlugins", IgnorePluginsCheckBox->Checked);
  //CutRadiostationName
  Ini->WriteBool("Settings", "CutRadiostationName", CutRadiostationNameCheckBox->Checked);
  //CutWWW
  Ini->WriteBool("Settings", "CutWWW", CutWWWCheckBox->Checked);
  //AutoTurnOff
  Ini->WriteBool("Settings", "AutoTurnOff", AutoTurnOffCheckBox->Checked);
  //AutoTurnOffDelay
  Ini->WriteInteger("Settings", "AutoTurnOffDelay", AutoTurnOffSpin->Value);
  //MovieException
  Ini->WriteBool("Settings", "MovieException", MovieExceptionCheckBox->Checked);
  //Notification
  Ini->WriteBool("UserTune", "Notification", UserTuneNotificationCheckBox->Checked);
  //Cloud
  Ini->WriteInteger("UserTune", "Cloud", UserTuneNotificationSpin->Value);
  //Send
  Ini->WriteBool("UserTune", "Send", UserTuneSendCheckBox->Checked);
  //SendDelay
  Ini->WriteInteger("UserTune", "SendDelay", UserTuneSendSpin->Value);
  //IgnoreCore
  Ini->WriteBool("UserTune", "IgnoreCore", IgnoreCoreUserTuneCheckBox->Checked);

  delete Ini;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aResetSettingsExecute(TObject *Sender)
{
  //Usuwanie wszystkich elementow
  AutoModeCheckListBoxPreview->Clear();
  //Dodawanie elementow na nowo
  AutoModeCheckListBoxPreview->Items->Add("Winamp/AIMP/KMPlayer");
  AutoModeCheckListBoxPreview->Items->Add("Foobar2000");
  AutoModeCheckListBoxPreview->Items->Add("Windows Media Player");
  AutoModeCheckListBoxPreview->Items->Add("VLC media player");
  AutoModeCheckListBoxPreview->Items->Add("Spotify");
  AutoModeCheckListBoxPreview->Items->Add("VUPlayer");
  AutoModeCheckListBoxPreview->Items->Add("XMPlay");
  AutoModeCheckListBoxPreview->Items->Add("Media Player Classic");
  AutoModeCheckListBoxPreview->Items->Add("iTunes");
  AutoModeCheckListBoxPreview->Items->Add("ALSong");
  AutoModeCheckListBoxPreview->Items->Add("Wtyczki (np. AQQ Radio)");
  AutoModeCheckListBoxPreview->Items->Add("Screamer Radio");
  AutoModeCheckListBoxPreview->Items->Add("aTunes");
  AutoModeCheckListBoxPreview->Items->Add("Songbird");
  AutoModeCheckListBoxPreview->Items->Add("Last.fm Player");
  //Zaznaczanie/odznaczanie odpowiednich checkbox'ow
  for(int Count=0;Count<11;Count++)
   AutoModeCheckListBoxPreview->Checked[Count] = true;
  for(int Count=11;Count<15;Count++)
   AutoModeCheckListBoxPreview->Checked[Count] = false;
  //Wlaczenie przycisku zastosuj
  SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aAllowApplyExecute(TObject *Sender)
{
  //Wlaczenie przycisku zastosuj
  SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aExitExecute(TObject *Sender)
{
  //Zamkniecie formy
  Close();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aSelectAllExecute(TObject *Sender)
{
  //Zaznaczenie calego tekstu w polu
  PreviewStatusMemo->SelectAll();
}
//---------------------------------------------------------------------------


