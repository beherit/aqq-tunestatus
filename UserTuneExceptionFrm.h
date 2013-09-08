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
#include "sBevel.hpp"
#include "sButton.hpp"
#include "sLabel.hpp"
#include "sListBox.hpp"
#include "sSkinManager.hpp"
#include "sSkinProvider.hpp"
#include "sTabControl.hpp"
//---------------------------------------------------------------------------
class TUserTuneExceptionForm : public TForm
{
__published:	// IDE-managed Components
	TsBevel *Bevel;
	TsButton *SaveButton;
	TsTabControl *TabControl;
	TsListBox *JIDListBox;
	TsButton *AddButton;
	TsButton *DeleteButton;
	TActionList *ActionList;
	TAction *aExit;
	TAction *aSaveSettings;
	TAction *aReadSettings;
	TsButton *CancelButton;
	TsSkinProvider *sSkinProvider;
	TsLabel *InfoLabel;
	void __fastcall aExitExecute(TObject *Sender);
	void __fastcall SaveButtonClick(TObject *Sender);
	void __fastcall AddButtonClick(TObject *Sender);
	void __fastcall JIDListBoxClick(TObject *Sender);
	void __fastcall DeleteButtonClick(TObject *Sender);
	void __fastcall aReadSettingsExecute(TObject *Sender);
	void __fastcall aSaveSettingsExecute(TObject *Sender);
	void __fastcall CancelButtonClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TUserTuneExceptionForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TUserTuneExceptionForm *UserTuneExceptionForm;
//---------------------------------------------------------------------------
#endif
