//---------------------------------------------------------------------------
#ifndef MainFrmH
#define MainFrmH
#include "LMDBaseEdit.hpp"
#include "LMDButtonControl.hpp"
#include "LMDCheckBox.hpp"
#include "LMDCheckListBox.hpp"
#include "LMDControl.hpp"
#include "LMDCustomBevelPanel.hpp"
#include "LMDCustomCheckBox.hpp"
#include "LMDCustomCheckListBox.hpp"
#include "LMDCustomControl.hpp"
#include "LMDCustomEdit.hpp"
#include "LMDCustomExtSpinEdit.hpp"
#include "LMDCustomImageListBox.hpp"
#include "LMDCustomListBox.hpp"
#include "LMDCustomMaskEdit.hpp"
#include "LMDCustomPanel.hpp"
#include "LMDCustomPanelFill.hpp"
#include "LMDCustomParentPanel.hpp"
#include "LMDCustomSheetControl.hpp"
#include "LMDPageControl.hpp"
#include "LMDSpinEdit.hpp"
#include "XPMan.hpp"
#include <ActnList.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <StdCtrls.hpp>
#include <Menus.hpp>
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
        TXPMan *XPMan;
        TAction *aVUPlayerDown;
        TAction *aXMPlayDown;
        TAction *aCutSongNumber;
        TAction *aReadSettings;
        TAction *aSaveSettings;
        TAction *aAutoDown;
        TLMDPageControl *LMDPageControl;
        TLMDTabSheet *SettingsTabSheet;
        TLMDCheckBox *EnableFastOnOffCheckBox;
        TLMDCheckBox *EnablePluginOnStartCheckBox;
        TLMDCheckBox *SetOnlyInJabberCheckBox;
        TLMDCheckBox *RunPluginCheckBox;
        TMemo *StatusMemo;
        TAction *aSetStatusLooks;
        TComboBox *TagsBox;
        TLabel *TagRequestLabel;
        TLMDTabSheet *StatusLooksTabSheet;
        TMemo *PreviewStatusMemo;
        TLabel *SongTimerIntervalLabel1;
        TLMDSpinEdit *SongTimerSpin;
        TTimer *SongTimer;
        TLabel *SongTimerIntervalLabel2;
        TLabel *TagInfoLabel1;
        TAction *aiTunesDown;
        TMemo *SongFromFile;
        TAction *aALSongDown;
        TLabel *TagInfoLabel2;
        TAction *aPluginAQQRadioDown;
        TLMDCheckBox *CutRadiostationNameCheckBox;
        TLMDCheckBox *DisableSongTimerCheckBox;
        TAction *aSongbirdDown;
        TLMDTabSheet *HandlingTabSheet;
        TLMDCheckListBox *AutoDownCheckListBox;
        TLMDCheckListBox *AutoDownCheckListBoxPreview;
        TBevel *Bevel3;
        TLabel *AutoDownInfoLabel;
        TButton *ResetButton;
        TAction *aaTunesDown;
        TLabel *AutoDownInfoLabel2;
        TAction *aCutWWW;
        TLMDCheckBox *CutWWWCheckBox;
        TAction *aScreamerRadioDown;
        TTimer *TurnOffTimer;
        TLMDCheckBox *TimeTurnOffCheckBox;
        TLMDSpinEdit *TimeTurnOffSpin;
        TBevel *Bevel1;
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
        TLMDCheckBox *BlockInvisibleCheckBox;
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
        void __fastcall RunPluginCheckBoxChange(TObject *Sender);
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
        void __fastcall AutoDownCheckListBoxPreviewDragDrop(
          TObject *Sender, TObject *Source, int X, int Y);
        void __fastcall AutoDownCheckListBoxPreviewDragOver(
          TObject *Sender, TObject *Source, int X, int Y, TDragState State,
          bool &Accept);
        void __fastcall aCutWWWExecute(TObject *Sender);
        void __fastcall aScreamerRadioDownExecute(TObject *Sender);
        void __fastcall TurnOffTimerTimer(TObject *Sender);
        void __fastcall TimeTurnOffCheckBoxChange(TObject *Sender);
        void __fastcall aResetSettingsExecute(TObject *Sender);
        void __fastcall AutoDownCheckListBoxPreviewMouseMove(
          TObject *Sender, TShiftState Shift, int X, int Y);
private:	// User declarations
public:		// User declarations
        bool JustEnabled;
        int IntervalValue;
        AnsiString opis_pocz;
        AnsiString opisTMP;
        bool SetOnlyInJabberCheck;
        __fastcall TMainForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif
