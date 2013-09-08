//---------------------------------------------------------------------------

#ifndef MainFrmH
#define MainFrmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ActnList.hpp>
#include <ExtCtrls.hpp>
#include "XPMan.hpp"
#include <ComCtrls.hpp>
#include "LMDControl.hpp"
#include "LMDCustomBevelPanel.hpp"
#include "LMDCustomControl.hpp"
#include "LMDCustomPanel.hpp"
#include "LMDCustomPanelFill.hpp"
#include "LMDCustomParentPanel.hpp"
#include "LMDCustomSheetControl.hpp"
#include "LMDPageControl.hpp"
#include "LMDButtonControl.hpp"
#include "LMDCheckBox.hpp"
#include "LMDCustomCheckBox.hpp"
#include "LMDRadioButton.hpp"
#include "LMDBaseEdit.hpp"
#include "LMDCustomEdit.hpp"
#include "LMDCustomExtSpinEdit.hpp"
#include "LMDCustomMaskEdit.hpp"
#include "LMDExtSpinEdit.hpp"
#include "LMDSpinEdit.hpp"
//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:	// IDE-managed Components
        TButton *SaveButton;
        TActionList *ActionList;
        TAction *aWinampDown;
        TAction *aFoobarDown;
        TAction *aWMP64Down;
        TAction *aMPCDown;
        TTimer *Timer;
        TAction *aLastFMDown;
        TAction *aWMP7_11Down;
        TXPMan *XPMan;
        TAction *aVUPlayerDown;
        TAction *aXMPlayDown;
        TAction *aCutSongNumber;
        TAction *aReadSettings;
        TAction *aSaveSettings;
        TAction *aAutoDown;
        TLMDPageControl *LMDPageControl;
        TLMDTabSheet *HandlingLMDTabSheet;
        TLMDTabSheet *SettingsLMDTabSheet;
        TLMDCheckBox *EnableFastOnOffCheckBox;
        TLMDCheckBox *EnablePluginOnStartCheckBox;
        TLMDCheckBox *SetOnlyInJabberCheckBox;
        TLMDRadioButton *WinampDownRadio;
        TLMDRadioButton *FoobarDownRadio;
        TLMDRadioButton *LastFMDownRadio;
        TLMDRadioButton *WMP7_11DownRadio;
        TLMDRadioButton *WMP64DownRadio;
        TLMDRadioButton *VUPlayerDownRadio;
        TLMDRadioButton *XMPlayDownRadio;
        TLMDRadioButton *MPCDownRadio;
        TLMDRadioButton *AutoDownRadio;
        TLMDCheckBox *RunPluginCheckBox;
        TMemo *StatusMemo;
        TAction *aSetStatusLooks;
        TComboBox *TagsBox;
        TLabel *TagRequestLabel;
        TLMDTabSheet *StatusLooksLMDTabSheet;
        TMemo *PreviewStatusMemo;
        TLabel *SongTimerIntervalLabel1;
        TLMDSpinEdit *SongTimerSpin;
        TTimer *SongTimer;
        TLabel *SongTimerIntervalLabel2;
        TLMDRadioButton *iTunesDownRadio;
        TLabel *TagInfoLabel1;
        TAction *aiTunesDown;
        TMemo *SongFromFile;
        TLMDRadioButton *ALSongDownRadio;
        TAction *aALSongDown;
        TLabel *TagInfoLabel2;
        TBevel *Bevel;
        TLMDRadioButton *PluginAQQRadioDownRadio;
        TAction *aPluginAQQRadioDown;
        void __fastcall aWinampDownExecute(TObject *Sender);
        void __fastcall aFoobarDownExecute(TObject *Sender);
        void __fastcall aWMP64DownExecute(TObject *Sender);
        void __fastcall aMPCDownExecute(TObject *Sender);
        void __fastcall aLastFMDownExecute(TObject *Sender);
        void __fastcall aWMP7_11DownExecute(TObject *Sender);
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
