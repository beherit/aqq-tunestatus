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
//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:	// IDE-managed Components
        TGroupBox *HandlingGroupBox;
        TGroupBox *SettingsGroupBox;
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
        TLabel *Label1;
        TXPMan *XPMan1;
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
private:	// User declarations
public:		// User declarations
        __fastcall TMainForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif
