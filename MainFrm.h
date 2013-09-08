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
//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:	// IDE-managed Components
        TButton *OkButton;
        TCheckBox *RunPluginCheckBox;
        TActionList *ActionList;
        TAction *aWinampDown;
        TAction *aFoobarDown;
        TAction *aWMP64Down;
        TAction *aMPCDown;
        TAction *aPreSufFix;
        TTimer *Timer;
        TEdit *Preview;
        TEdit *PrefixEdit;
        TEdit *SuffixEdit;
        TCheckBox *PrefixCheckBox;
        TCheckBox *SuffixCheckBox;
        TLabel *PreviewLabel;
        TLabel *PrefixLabel;
        TLabel *SuffixLabel;
        TRadioButton *WinampDownRadio;
        TRadioButton *FoobarDownRadio;
        TRadioButton *WMP64DownRadio;
        TRadioButton *MPCDownRadio;
        TRadioButton *LastFMDownRadio;
        TAction *aLastFMDown;
        TAction *aWMP7_11Down;
        TRadioButton *WMP7_11DownRadio;
        TLabel *WMP7_11Label;
        TXPMan *XPMan;
        TAction *aVUPlayerDown;
        TRadioButton *VUPlayerDownRadio;
        TRadioButton *XMPlayDownRadio;
        TAction *aXMPlayDown;
        TAction *aCutSongNumber;
        TLabel *NewLineLabel;
        TCheckBox *SetOnlyInJabberCheckBox;
        TAction *aReadSettings;
        TCheckBox *EnablePluginOnStartCheckBox;
        TAction *aSaveSettings;
        TPageControl *PageControl;
        TTabSheet *HandlingTabSheet;
        TTabSheet *SettingsTabSheet;
        TCheckBox *EnableFastOnOffCheckBox;
        TRadioButton *AutoDownRadio;
        TAction *aAutoDown;
        void __fastcall aWinampDownExecute(TObject *Sender);
        void __fastcall aFoobarDownExecute(TObject *Sender);
        void __fastcall aWMP64DownExecute(TObject *Sender);
        void __fastcall aMPCDownExecute(TObject *Sender);
        void __fastcall aPreSufFixExecute(TObject *Sender);
        void __fastcall aLastFMDownExecute(TObject *Sender);
        void __fastcall aWMP7_11DownExecute(TObject *Sender);
        void __fastcall TimerTimer(TObject *Sender);
        void __fastcall RunPluginCheckBoxClick(TObject *Sender);
        void __fastcall OkButtonClick(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall aVUPlayerDownExecute(TObject *Sender);
        void __fastcall aXMPlayDownExecute(TObject *Sender);
        void __fastcall aCutSongNumberExecute(TObject *Sender);
        void __fastcall SetOnlyInJabberCheckBoxClick(TObject *Sender);
        void __fastcall aReadSettingsExecute(TObject *Sender);
        void __fastcall aSaveSettingsExecute(TObject *Sender);
        void __fastcall EnableFastOnOffCheckBoxClick(TObject *Sender);
        void __fastcall aAutoDownExecute(TObject *Sender);
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
