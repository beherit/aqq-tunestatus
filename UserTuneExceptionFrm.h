//---------------------------------------------------------------------------

#ifndef UserTuneExceptionFrmH
#define UserTuneExceptionFrmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <ActnList.hpp>
//---------------------------------------------------------------------------
class TUserTuneExceptionForm : public TForm
{
__published:	// IDE-managed Components
	TBevel *Bevel;
	TButton *SaveButton;
	TTabControl *TabControl;
	TListBox *JIDListBox;
	TButton *AddButton;
	TButton *DeleteButton;
	TActionList *ActionList;
	TAction *aExit;
	TAction *aSaveSettings;
	TAction *aReadSettings;
	TLabel *InfoLabel;
	TButton *CancelButton;
	void __fastcall aExitExecute(TObject *Sender);
	void __fastcall SaveButtonClick(TObject *Sender);
	void __fastcall AddButtonClick(TObject *Sender);
	void __fastcall JIDListBoxClick(TObject *Sender);
	void __fastcall DeleteButtonClick(TObject *Sender);
	void __fastcall aReadSettingsExecute(TObject *Sender);
	void __fastcall aSaveSettingsExecute(TObject *Sender);
	void __fastcall CancelButtonClick(TObject *Sender);

private:	// User declarations
public:		// User declarations
	__fastcall TUserTuneExceptionForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TUserTuneExceptionForm *UserTuneExceptionForm;
//---------------------------------------------------------------------------
#endif
