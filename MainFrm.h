//---------------------------------------------------------------------------
#ifndef MainFrmH
#define MainFrmH
#include "cspin.h"
#include <ActnList.hpp>
#include <Buttons.hpp>
#include <CheckLst.hpp>
#include <Classes.hpp>
#include <ComCtrls.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include <StdCtrls.hpp>
//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:	// IDE-managed Components
		TButton *SaveButton;
		TActionList *ActionList;
		TAction *aWinampDown;
		TAction *aFoobarDown;
		TAction *aMPCDown;
		TTimer *Timer;
		TAction *aLastFMDown;
		TAction *aWMPDown;
		TAction *aVUPlayerDown;
		TAction *aXMPlayDown;
		TAction *aCutSongNumber;
		TAction *aReadSettings;
		TAction *aSaveSettings;
		TAction *aAutoDown;
		TMemo *StatusMemo;
		TAction *aSetStatusLooks;
		TComboBox *TagsBox;
		TMemo *PreviewStatusMemo;
		TLabel *SongTimerIntervalLabel1;
		TTimer *SongTimer;
		TLabel *SongTimerIntervalLabel2;
		TAction *aiTunesDown;
		TMemo *SongFromFile;
		TAction *aALSongDown;
		TAction *aPluginAQQRadioDown;
		TAction *aSongbirdDown;
		TLabel *AutoDownInfoLabel;
		TButton *ResetButton;
		TAction *aaTunesDown;
		TLabel *AutoDownInfoLabel2;
		TAction *aCutWWW;
		TAction *aScreamerRadioDown;
		TTimer *TurnOffTimer;
		TBevel *Bevel3;
		TBevel *Bevel2;
		TAction *aResetSettings;
		TPopupMenu *FoobarPopupMenu;
		TMenuItem *FoobarDownload;
		TPopupMenu *WMPPopupMenu;
		TMenuItem *WMPDownload;
		TPopupMenu *iTunesPopupMenu;
		TMenuItem *iTunesDownload;
		TPopupMenu *SongbirdPopupMenu;
		TMenuItem *SongbirdDownload;
		TLabel *UserTuneTimeNLabel;
		TBevel *Bevel4;
		TTimer *UserTuneTimer;
		TTimer *UserTuneSongTimer;
		TLabel *UserTuneLabel;
		TLabel *UserTuneSLabel;
		TBevel *Bevel5;
		TLabel *UserTuneTimeSLabel1;
		TLabel *UserTuneTimeSLabel2;
		TAction *aCutWWWUserTune;
		TTimer *AllowUserTuneNTimer;
		TPageControl *PageControl;
		TTabSheet *HandlingTabSheet;
		TTabSheet *StatusTabSheet;
		TTabSheet *SettingsTabSheet;
		TTabSheet *UserTuneTabSheet;
		TCheckListBox *AutoDownCheckListBox;
		TCheckListBox *AutoDownCheckListBoxPreview;
		TCheckBox *RunPluginCheckBox;
		TCheckBox *EnablePluginOnStartCheckBox;
		TCheckBox *EnableFastOnOffCheckBox;
		TCheckBox *SetOnlyInJabberCheckBox;
		TCheckBox *BlockInvisibleCheckBox;
		TCSpinEdit *SongTimerSpin;
		TCheckBox *DisableSongTimerCheckBox;
		TCheckBox *CutRadiostationNameCheckBox;
		TCheckBox *CutWWWCheckBox;
		TCheckBox *TimeTurnOffCheckBox;
		TCSpinEdit *TimeTurnOffSpin;
		TCheckBox *EnableUserTuneNCheckBox;
		TCSpinEdit *UserTuneNSpin;
		TCheckBox *EnableUserTuneSCheckBox;
		TCSpinEdit *UserTuneSSpin;
		TCheckBox *EnableAQQUserTuneSCheckBox;
		TTimer *GetStatusTimer;
		TBevel *Bevel;
		TAction *aExit;
		TButton *UserTuneExceptionButton;
		TAction *aAutoDownUserTune;
		TBevel *Bevel1;
		TCheckBox *MovieExceptionCheckBox;
		void __fastcall aWinampDownExecute(TObject *Sender);
		void __fastcall aFoobarDownExecute(TObject *Sender);
		void __fastcall aMPCDownExecute(TObject *Sender);
		void __fastcall aLastFMDownExecute(TObject *Sender);
		void __fastcall aWMPDownExecute(TObject *Sender);
		void __fastcall TimerTimer(TObject *Sender);
		void __fastcall SaveButtonClick(TObject *Sender);
		void __fastcall FormShow(TObject *Sender);
		void __fastcall aVUPlayerDownExecute(TObject *Sender);
		void __fastcall aXMPlayDownExecute(TObject *Sender);
		void __fastcall aCutSongNumberExecute(TObject *Sender);
		void __fastcall aReadSettingsExecute(TObject *Sender);
		void __fastcall aSaveSettingsExecute(TObject *Sender);
		void __fastcall aAutoDownExecute(TObject *Sender);
		void __fastcall aSetStatusLooksExecute(TObject *Sender);
		void __fastcall TagsBoxSelect(TObject *Sender);
		void __fastcall TagsBoxCloseUp(TObject *Sender);
		void __fastcall TagsBoxDropDown(TObject *Sender);
		void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
		void __fastcall PreviewStatusMemoChange(TObject *Sender);
		void __fastcall SongTimerTimer(TObject *Sender);
		void __fastcall aiTunesDownExecute(TObject *Sender);
		void __fastcall aALSongDownExecute(TObject *Sender);
		void __fastcall aPluginAQQRadioDownExecute(TObject *Sender);
		void __fastcall aSongbirdDownExecute(TObject *Sender);
		void __fastcall SongbirdDownloadClick(TObject *Sender);
		void __fastcall WMPDownloadClick(TObject *Sender);
		void __fastcall iTunesDownloadClick(TObject *Sender);
		void __fastcall FoobarDownloadClick(TObject *Sender);
		void __fastcall aaTunesDownExecute(TObject *Sender);
		void __fastcall AutoDownCheckListBoxPreviewClick(TObject *Sender);
		void __fastcall AutoDownCheckListBoxPreviewDragDrop(TObject *Sender, TObject *Source, int X, int Y);
		void __fastcall AutoDownCheckListBoxPreviewDragOver(TObject *Sender, TObject *Source, int X, int Y, TDragState State, bool &Accept);
		void __fastcall aCutWWWExecute(TObject *Sender);
		void __fastcall aScreamerRadioDownExecute(TObject *Sender);
		void __fastcall TurnOffTimerTimer(TObject *Sender);
		void __fastcall aResetSettingsExecute(TObject *Sender);
		void __fastcall AutoDownCheckListBoxPreviewMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
		void __fastcall UserTuneTimerTimer(TObject *Sender);
		void __fastcall UserTuneSongTimerTimer(TObject *Sender);
		void __fastcall aCutWWWUserTuneExecute(TObject *Sender);
		void __fastcall AllowUserTuneNTimerTimer(TObject *Sender);
		void __fastcall RunPluginCheckBoxClick(TObject *Sender);
		void __fastcall TimeTurnOffCheckBoxClick(TObject *Sender);
		void __fastcall EnableUserTuneNCheckBoxClick(TObject *Sender);
		void __fastcall EnableUserTuneSCheckBoxClick(TObject *Sender);
		void __fastcall GetStatusTimerTimer(TObject *Sender);
		void __fastcall aExitExecute(TObject *Sender);
		void __fastcall UserTuneExceptionButtonClick(TObject *Sender);
		void __fastcall aAutoDownUserTuneExecute(TObject *Sender);

private:	// User declarations
public:		// User declarations
        bool JustEnabled;
        int IntervalValue;
		UnicodeString opis_pocz;
		UnicodeString opisTMP;
		UnicodeString opis;
        bool SetOnlyInJabberCheck;
        bool BlockInvisibleCheck;
        __fastcall TMainForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif
