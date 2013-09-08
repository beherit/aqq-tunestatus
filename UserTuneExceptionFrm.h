//---------------------------------------------------------------------------
#ifndef UserTuneExceptionFrmH
#define UserTuneExceptionFrmH
#define WM_ALPHAWINDOWS (WM_USER + 666)
//---------------------------------------------------------------------------
#include "sBevel.hpp"
#include "sButton.hpp"
#include "sLabel.hpp"
#include "sListBox.hpp"
#include "sSkinProvider.hpp"
#include "sTabControl.hpp"
#include <System.Actions.hpp>
#include <System.Classes.hpp>
#include <Vcl.ActnList.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.StdCtrls.hpp>
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
	TAction *aLoadSettings;
	TsButton *CancelButton;
	TsSkinProvider *sSkinProvider;
	TsLabel *InfoLabel;
	void __fastcall aExitExecute(TObject *Sender);
	void __fastcall SaveButtonClick(TObject *Sender);
	void __fastcall AddButtonClick(TObject *Sender);
	void __fastcall JIDListBoxClick(TObject *Sender);
	void __fastcall DeleteButtonClick(TObject *Sender);
	void __fastcall aLoadSettingsExecute(TObject *Sender);
	void __fastcall aSaveSettingsExecute(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TUserTuneExceptionForm(TComponent* Owner);
	void __fastcall WMTransparency(TMessage &Message);
	BEGIN_MESSAGE_MAP
	MESSAGE_HANDLER(WM_ALPHAWINDOWS,TMessage,WMTransparency);
	END_MESSAGE_MAP(TForm)
};
//---------------------------------------------------------------------------
extern PACKAGE TUserTuneExceptionForm *UserTuneExceptionForm;
//---------------------------------------------------------------------------
#endif
