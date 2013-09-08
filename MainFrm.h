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
//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:	// IDE-managed Components
        TButton *OkButton;
        TActionList *ActionList;
        TAction *aWinampDown;
        TAction *aFoobarDown;
        TAction *aWMP64Down;
        TAction *aMPCDown;
        TTimer *Timer;
        TAction *aLastFMDown;
        TAction *aWMP7_11Down;
        TLabel *WMP7_11Label;
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
        void __fastcall aWinampDownExecute(TObject *Sender);
        void __fastcall aFoobarDownExecute(TObject *Sender);
        void __fastcall aWMP64DownExecute(TObject *Sender);
        void __fastcall aMPCDownExecute(TObject *Sender);
        void __fastcall aLastFMDownExecute(TObject *Sender);
        void __fastcall aWMP7_11DownExecute(TObject *Sender);
        void __fastcall TimerTimer(TObject *Sender);
        void __fastcall RunPluginCheckBoxClick(TObject *Sender);
        void __fastcall OkButtonClick(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall aVUPlayerDownExecute(TObject *Sender);
        void __fastcall aXMPlayDownExecute(TObject *Sender);
        void __fastcall aCutSongNumberExecute(TObject *Sender);
        void __fastcall SetOnlyInJabberCheckBoxClick(TObject *Sender);
        void __fastcall aReadSettingsExecute(TObject *Sender);
        void __fastcall aSaveSettingsExecute(TObject *Sender);
        void __fastcall aAutoDownExecute(TObject *Sender);
        void __fastcall EnableFastOnOffCheckBoxChange(TObject *Sender);
        void __fastcall RunPluginCheckBoxChange(TObject *Sender);
        void __fastcall aSetStatusLooksExecute(TObject *Sender);
        void __fastcall TagsBoxSelect(TObject *Sender);
        void __fastcall TagsBoxCloseUp(TObject *Sender);
        void __fastcall TagsBoxDropDown(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations
public:		// User declarations
        AnsiString opis_pocz;
        AnsiString opisTMP;
        __fastcall TMainForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif
