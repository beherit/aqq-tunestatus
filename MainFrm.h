//---------------------------------------------------------------------------
// Copyright (C) 2009-2013 Krzysztof Grochocki
//
// This file is part of TuneStatus
//
// TuneStatus is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3, or (at your option)
// any later version.
//
// TuneStatus is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GNU Radio; see the file COPYING. If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street,
// Boston, MA 02110-1301, USA.
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifndef MainFrmH
#define MainFrmH
#define WM_ALPHAWINDOWS (WM_USER + 666)
//---------------------------------------------------------------------------
#include "sBevel.hpp"
#include "sButton.hpp"
#include "sCheckBox.hpp"
#include "sCheckListBox.hpp"
#include "sComboBox.hpp"
#include "sEdit.hpp"
#include "sLabel.hpp"
#include "sListBox.hpp"
#include "sMemo.hpp"
#include "sPageControl.hpp"
#include "sSkinManager.hpp"
#include "sSkinProvider.hpp"
#include "sSpinEdit.hpp"
#include <System.Actions.hpp>
#include <System.Classes.hpp>
#include <Vcl.ActnList.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.StdCtrls.hpp>
//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:	// IDE-managed Components
		TsButton *SaveButton;
		TActionList *ActionList;
		TAction *aLoadSettings;
		TAction *aSaveSettings;
		TsComboBox *TagsBox;
		TsMemo *PreviewStatusMemo;
		TsLabel *SetStatuslLabel;
		TsMemo *SongFromFile;
		TsLabel *AutoModeInfoLabel;
		TsButton *ResetButton;
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
		TsLabel *UserTuneLabel;
		TsBevel *Bevel5;
		TsLabel *UserTuneSendLabel;
		TsPageControl *PageControl;
		TsTabSheet *HandlingTabSheet;
		TsTabSheet *StatusTabSheet;
		TsTabSheet *OtherTabSheet;
		TsTabSheet *UserTuneTabSheet;
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
		TsBevel *Bevel;
		TAction *aExit;
		TsButton *UserTuneExceptionButton;
		TsBevel *Bevel1;
		TsCheckBox *MovieExceptionCheckBox;
		TAction *aSelectAll;
		TsButton *OKButton;
		TAction *aAllowApply;
		TsSkinManager *sSkinManager;
		TsSkinProvider *sSkinProvider;
		void __fastcall SaveButtonClick(TObject *Sender);
		void __fastcall FormShow(TObject *Sender);
		void __fastcall aLoadSettingsExecute(TObject *Sender);
		void __fastcall aSaveSettingsExecute(TObject *Sender);
		void __fastcall TagsBoxSelect(TObject *Sender);
		void __fastcall TagsBoxCloseUp(TObject *Sender);
		void __fastcall TagsBoxDropDown(TObject *Sender);
		void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
		void __fastcall SongbirdDownloadClick(TObject *Sender);
		void __fastcall WMPDownloadClick(TObject *Sender);
		void __fastcall iTunesDownloadClick(TObject *Sender);
		void __fastcall FoobarDownloadClick(TObject *Sender);
		void __fastcall AutoModeCheckListBoxPreviewDragDrop(TObject *Sender, TObject *Source, int X, int Y);
		void __fastcall AutoModeCheckListBoxPreviewDragOver(TObject *Sender, TObject *Source, int X, int Y, TDragState State, bool &Accept);
		void __fastcall aResetSettingsExecute(TObject *Sender);
		void __fastcall AutoModeCheckListBoxPreviewMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
		void __fastcall RunPluginCheckBoxClick(TObject *Sender);
		void __fastcall AutoTurnOffCheckBoxClick(TObject *Sender);
		void __fastcall UserTuneNotificationCheckBoxClick(TObject *Sender);
		void __fastcall UserTuneSendCheckBoxClick(TObject *Sender);
		void __fastcall aExitExecute(TObject *Sender);
		void __fastcall UserTuneExceptionButtonClick(TObject *Sender);
		void __fastcall aSelectAllExecute(TObject *Sender);
		void __fastcall OKButtonClick(TObject *Sender);
		void __fastcall aAllowApplyExecute(TObject *Sender);
		void __fastcall AutoModeCheckListBoxPreviewClick(TObject *Sender);
		void __fastcall FormCreate(TObject *Sender);
		void __fastcall OtherTabSheetShow(TObject *Sender);
		void __fastcall UserTuneTabSheetShow(TObject *Sender);
private:	// User declarations
public:		// User declarations
		__fastcall TMainForm(TComponent* Owner);
		void __fastcall WMTransparency(TMessage &Message);
		BEGIN_MESSAGE_MAP
		MESSAGE_HANDLER(WM_ALPHAWINDOWS,TMessage,WMTransparency);
		END_MESSAGE_MAP(TForm)
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif
