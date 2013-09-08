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
#include "sButton.hpp"
#include "sPageControl.hpp"
#include "sSkinManager.hpp"
#include "sSkinProvider.hpp"
#include "sCheckListBox.hpp"
#include "sLabel.hpp"
#include "sListBox.hpp"
#include "sBevel.hpp"
#include "sComboBox.hpp"
#include "sMemo.hpp"
#include "sEdit.hpp"
#include "sSpinEdit.hpp"
#include "sCheckBox.hpp"
//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:	// IDE-managed Components
		TsButton *SaveButton;
		TActionList *ActionList;
		TAction *aWinampDown;
		TAction *aFoobarDown;
		TAction *aMPCDown;
	TTimer *AutoModeTimer;
		TAction *aLastFMDown;
		TAction *aWMPDown;
		TAction *aVUPlayerDown;
		TAction *aXMPlayDown;
		TAction *aCutSongNumber;
		TAction *aLoadSettings;
		TAction *aSaveSettings;
	TAction *aAutoMode;
		TsMemo *StatusMemo;
		TAction *aSetStatusLooks;
		TsComboBox *TagsBox;
		TsMemo *PreviewStatusMemo;
		TTimer *SetStatusTimer;
		TsLabel *SetStatuslLabel;
		TAction *aiTunesDown;
		TsMemo *SongFromFile;
		TAction *aALSongDown;
		TAction *aPluginAQQRadioDown;
		TAction *aSongbirdDown;
	TsLabel *AutoModeInfoLabel;
	TsButton *ResetButton;
		TAction *aaTunesDown;
		TAction *aCutWWW;
		TAction *aScreamerRadioDown;
		TTimer *AutoTurnOffTimer;
		TsBevel *Bevel3;
		TsBevel *Bevel2;
		TAction *aResetSettings;
		TPopupMenu *FoobarPopupMenu;
		TMenuItem *FoobarDownload;
		TPopupMenu *WMPPopupMenu;
		TMenuItem *WMPDownload;
		TPopupMenu *iTunesPopupMenu;
		TMenuItem *iTunesDownload;
		TPopupMenu *SongbirdPopupMenu;
		TMenuItem *SongbirdDownload;
		TsBevel *Bevel4;
	TTimer *UserTuneTimer;
		TTimer *SetUserTuneTimer;
		TsLabel *UserTuneLabel;
		TsLabel *UserTuneSLabel;
		TsBevel *Bevel5;
		TsLabel *UserTuneSendLabel;
		TAction *aCutWWWUserTune;
		TTimer *AllowUserTuneTimer;
		TsPageControl *PageControl;
		TsTabSheet *HandlingTabSheet;
		TsTabSheet *StatusTabSheet;
		TsTabSheet *OtherTabSheet;
		TsTabSheet *UserTuneTabSheet;
	TsCheckListBox *AutoModeCheckListBox;
	TsCheckListBox *AutoModeCheckListBoxPreview;
		TsCheckBox *RunPluginCheckBox;
		TsCheckBox *EnableOnStartCheckBox;
		TsCheckBox *FastAccessCheckBox;
		TsCheckBox *SetOnlyInJabberCheckBox;
		TsCheckBox *BlockInvisibleCheckBox;
		TsSpinEdit *SetStatusSpin;
		TsCheckBox *IgnorePluginsCheckBox;
		TsCheckBox *CutRadiostationNameCheckBox;
		TsCheckBox *CutWWWCheckBox;
		TsCheckBox *AutoTurnOffCheckBox;
		TsSpinEdit *AutoTurnOffSpin;
		TsCheckBox *UserTuneNotificationCheckBox;
		TsSpinEdit *UserTuneNotificationSpin;
		TsCheckBox *UserTuneSendCheckBox;
		TsSpinEdit *UserTuneSendSpin;
		TsCheckBox *IgnoreCoreUserTuneCheckBox;
		TTimer *GetStatusTimer;
		TsBevel *Bevel;
		TAction *aExit;
		TsButton *UserTuneExceptionButton;
	TAction *aUserTuneAutoMode;
		TsBevel *Bevel1;
		TsCheckBox *MovieExceptionCheckBox;
		TTimer *StateChangeTimer;
		TAction *aSelectAll;
		TsButton *OKButton;
		TAction *aAllowApply;
		TsSkinManager *sSkinManager;
		TsSkinProvider *sSkinProvider;
		TAction *aPageControlSheetChange;
		void __fastcall aWinampDownExecute(TObject *Sender);
		void __fastcall aFoobarDownExecute(TObject *Sender);
		void __fastcall aMPCDownExecute(TObject *Sender);
		void __fastcall aLastFMDownExecute(TObject *Sender);
		void __fastcall aWMPDownExecute(TObject *Sender);
		void __fastcall AutoModeTimerTimer(TObject *Sender);
		void __fastcall SaveButtonClick(TObject *Sender);
		void __fastcall FormShow(TObject *Sender);
		void __fastcall aVUPlayerDownExecute(TObject *Sender);
		void __fastcall aXMPlayDownExecute(TObject *Sender);
		void __fastcall aCutSongNumberExecute(TObject *Sender);
		void __fastcall aLoadSettingsExecute(TObject *Sender);
		void __fastcall aSaveSettingsExecute(TObject *Sender);
		void __fastcall aAutoModeExecute(TObject *Sender);
		void __fastcall aSetStatusLooksExecute(TObject *Sender);
		void __fastcall TagsBoxSelect(TObject *Sender);
		void __fastcall TagsBoxCloseUp(TObject *Sender);
		void __fastcall TagsBoxDropDown(TObject *Sender);
		void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
		void __fastcall SetStatusTimerTimer(TObject *Sender);
		void __fastcall aiTunesDownExecute(TObject *Sender);
		void __fastcall aALSongDownExecute(TObject *Sender);
		void __fastcall aPluginAQQRadioDownExecute(TObject *Sender);
		void __fastcall aSongbirdDownExecute(TObject *Sender);
		void __fastcall SongbirdDownloadClick(TObject *Sender);
		void __fastcall WMPDownloadClick(TObject *Sender);
		void __fastcall iTunesDownloadClick(TObject *Sender);
		void __fastcall FoobarDownloadClick(TObject *Sender);
		void __fastcall aaTunesDownExecute(TObject *Sender);
		void __fastcall AutoModeCheckListBoxPreviewDragDrop(TObject *Sender, TObject *Source, int X, int Y);
		void __fastcall AutoModeCheckListBoxPreviewDragOver(TObject *Sender, TObject *Source, int X, int Y, TDragState State, bool &Accept);
		void __fastcall aCutWWWExecute(TObject *Sender);
		void __fastcall aScreamerRadioDownExecute(TObject *Sender);
		void __fastcall AutoTurnOffTimerTimer(TObject *Sender);
		void __fastcall aResetSettingsExecute(TObject *Sender);
		void __fastcall AutoModeCheckListBoxPreviewMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
		void __fastcall UserTuneTimerTimer(TObject *Sender);
		void __fastcall SetUserTuneTimerTimer(TObject *Sender);
		void __fastcall aCutWWWUserTuneExecute(TObject *Sender);
		void __fastcall AllowUserTuneTimerTimer(TObject *Sender);
		void __fastcall RunPluginCheckBoxClick(TObject *Sender);
		void __fastcall AutoTurnOffCheckBoxClick(TObject *Sender);
		void __fastcall UserTuneNotificationCheckBoxClick(TObject *Sender);
		void __fastcall UserTuneSendCheckBoxClick(TObject *Sender);
		void __fastcall GetStatusTimerTimer(TObject *Sender);
		void __fastcall aExitExecute(TObject *Sender);
		void __fastcall UserTuneExceptionButtonClick(TObject *Sender);
		void __fastcall aUserTuneAutoModeExecute(TObject *Sender);
		void __fastcall StateChangeTimerTimer(TObject *Sender);
		void __fastcall aSelectAllExecute(TObject *Sender);
		void __fastcall OKButtonClick(TObject *Sender);
		void __fastcall aAllowApplyExecute(TObject *Sender);
		void __fastcall AutoModeCheckListBoxPreviewClick(TObject *Sender);
		void __fastcall AutoModeCheckListBoxPreviewClickCheck(TObject *Sender);
		void __fastcall FormCreate(TObject *Sender);
		void __fastcall aPageControlSheetChangeExecute(TObject *Sender);
	void __fastcall OtherTabSheetShow(TObject *Sender);
	void __fastcall UserTuneTabSheetShow(TObject *Sender);
private:	// User declarations
public:		// User declarations
		bool JustEnabled;
		int SetStatusDelay;
		UnicodeString StartStatus;
		UnicodeString TempStatus;
		UnicodeString Status;
		bool SetOnlyInJabberChk;
		bool BlockInvisibleChk;
		__fastcall TMainForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif
