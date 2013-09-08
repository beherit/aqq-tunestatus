object UserTuneExceptionForm: TUserTuneExceptionForm
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsToolWindow
  Caption = 'TuneStatus - wyj'#261'tki UserTune'
  ClientHeight = 209
  ClientWidth = 232
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poOwnerFormCenter
  OnShow = aReadSettingsExecute
  PixelsPerInch = 96
  TextHeight = 13
  object Bevel: TBevel
    Left = 0
    Top = 177
    Width = 232
    Height = 32
    Align = alBottom
    Shape = bsTopLine
    ExplicitTop = 175
    ExplicitWidth = 236
  end
  object SaveButton: TButton
    Left = 153
    Top = 181
    Width = 75
    Height = 25
    Caption = 'Zapisz'
    Enabled = False
    TabOrder = 0
    TabStop = False
    OnClick = SaveButtonClick
  end
  object TabControl: TTabControl
    AlignWithMargins = True
    Left = 6
    Top = 6
    Width = 220
    Height = 165
    Margins.Left = 6
    Margins.Top = 6
    Margins.Right = 6
    Margins.Bottom = 6
    Align = alClient
    TabOrder = 1
    ExplicitLeft = 8
    ExplicitTop = 8
    ExplicitHeight = 161
    object InfoLabel: TLabel
      Left = 29
      Top = 8
      Width = 161
      Height = 13
      Caption = 'Kontakty wy'#322#261'czone z notyfikacji:'
    end
    object JIDListBox: TListBox
      Left = 8
      Top = 27
      Width = 202
      Height = 96
      ItemHeight = 13
      TabOrder = 0
      OnClick = JIDListBoxClick
    end
    object AddButton: TButton
      Left = 8
      Top = 129
      Width = 75
      Height = 25
      Caption = 'Dodaj'
      TabOrder = 1
      OnClick = AddButtonClick
    end
    object DeleteButton: TButton
      Left = 135
      Top = 129
      Width = 75
      Height = 25
      Caption = 'Usu'#324
      Enabled = False
      TabOrder = 2
      OnClick = DeleteButtonClick
    end
  end
  object CancelButton: TButton
    Left = 72
    Top = 181
    Width = 75
    Height = 25
    Caption = 'Anuluj'
    TabOrder = 2
    OnClick = CancelButtonClick
  end
  object ActionList: TActionList
    Top = 232
    object aExit: TAction
      Caption = 'aExit'
      ShortCut = 27
      OnExecute = aExitExecute
    end
    object aSaveSettings: TAction
      Caption = 'aSaveSettings'
      OnExecute = aSaveSettingsExecute
    end
    object aReadSettings: TAction
      Caption = 'aReadSettings'
      OnExecute = aReadSettingsExecute
    end
  end
end
