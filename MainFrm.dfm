object MainForm: TMainForm
  Left = 454
  Top = 56
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'TuneStatus by Beherit'
  ClientHeight = 368
  ClientWidth = 295
  Color = clBtnFace
  Font.Charset = EASTEUROPE_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Icon.Data = {
    0000010001001010000001002000680400001600000028000000100000002000
    000001002000000000000000000000000000000000000000000000000000FFFF
    FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
    FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
    FF004C4C4C423E3E3ECC363636CD313131992F2F2F1AFFFFFF00FFFFFF00FFFF
    FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF006868
    686D727272FF858585FF8A8A8AFF636363FF3B3B3BFF2F2F2F6CFFFFFF00FFFF
    FF002F2F2F42262626CC202020CD1C1C1C991C1C1C1AFFFFFF00FFFFFF007A7A
    7A6A919191FFA9A9A9FFAEAEAEFFABABABFF8B8B8BFF4A4A4AFF353535413C3C
    3C6D535353FF6E6E6EFF767676FF4E4E4EFF262626FF1B1B1B6CFFFFFF00FFFF
    FF007E7E7E70898989FF9E9E9EFFB0B0B0FFACACACFF757575FF424242A04F4F
    4F6A737373FF919191FF989898FF969696FF767676FF353535FF1F1F1F41FFFF
    FF00FFFFFF007F7F7F427A7A7AE3737373FF898989FF9D9D9DFF484848EDFFFF
    FF005C5C5C706B6B6BFF858585FF9A9A9AFF979797FF5F5F5FFF222222A0FFFF
    FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00717171FFB1B1B1FF494949FFFFFF
    FF00FFFFFF005D5D5D42595959E3515151FF6C6C6CFF878787FF272727EDFFFF
    FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00747474FFB8B8B8FF4E4E4EFFFFFF
    FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF004F4F4FFF9A9A9AFF2D2D2DFFFFFF
    FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00797979FFBCBCBCFF545454FFFFFF
    FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00505050FFA1A1A1FF323232FFFFFF
    FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF007F7F7FFFC1C1C1FF595959FFFFFF
    FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00565656FFA5A5A5FF373737FFFFFF
    FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00858585FFC5C5C5FF5D5D5DFFFFFF
    FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF005D5D5DFFAAAAAAFF3B3B3BFFFFFF
    FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF008A8A8AFFCACACAFF5E5E5EFFFFFF
    FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF005B5B5BFFAFAFAFFF404040FFFFFF
    FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00909090FFCDCDCDFF848484FF5959
    59FF505050FF4D4D4DFF494949FF565656FF7F7F7FFFB4B4B4FF464646FFFFFF
    FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00939393FFD1D1D1FFCECECEFFCBCB
    CBFFC8C8C8FFC6C6C6FFC2C2C2FFBFBFBFFFBCBCBCFFB9B9B9FF4A4A4AFFFFFF
    FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF009C9C9CFF999999FF959595FF9191
    91FF8D8D8DFF888888FF848484FF7F7F7FFF7A7A7AFF767676FF666666FFFFFF
    FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
    FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
    0000C7FF000083C7000081830000C0810000E0C00000F8E00000F8F80000F8F8
    0000F8F80000F8F80000F8F80000F8000000F8000000F8000000FFFF0000}
  OldCreateOrder = False
  Position = poScreenCenter
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object HandlingGroupBox: TGroupBox
    Left = 6
    Top = 6
    Width = 283
    Height = 230
    Caption = 'Obs'#322'uga'
    TabOrder = 0
    object Label1: TLabel
      Left = 32
      Top = 112
      Width = 196
      Height = 13
      Caption = 'Do obs'#322'ugi potrzebny jest plugin w WMP!'
      Enabled = False
      Font.Charset = EASTEUROPE_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object WinampDownRadio: TRadioButton
      Left = 8
      Top = 20
      Width = 173
      Height = 17
      Caption = 'Pobieraj dane z Winamp/AIMP2'
      Checked = True
      TabOrder = 0
      TabStop = True
    end
    object FoobarDownRadio: TRadioButton
      Left = 8
      Top = 44
      Width = 158
      Height = 17
      Caption = 'Pobieraj dane z Foobar2000'
      TabOrder = 1
    end
    object WMP64DownRadio: TRadioButton
      Left = 8
      Top = 132
      Width = 226
      Height = 17
      Caption = 'Pobieraj dane z Windows Media Player 6.4'
      TabOrder = 4
    end
    object MPCDownRadio: TRadioButton
      Left = 8
      Top = 204
      Width = 193
      Height = 17
      Caption = 'Pobieraj dane z Media Player Classic'
      TabOrder = 5
    end
    object LastFMDownRadio: TRadioButton
      Left = 8
      Top = 68
      Width = 169
      Height = 17
      Caption = 'Pobieraj dane z Last.fm Player'
      TabOrder = 2
    end
    object WMP7_11DownRadio: TRadioButton
      Left = 8
      Top = 92
      Width = 233
      Height = 17
      Caption = 'Pobieraj dane z Windows Media Player 7-11'
      TabOrder = 3
    end
    object VUPlayerDownRadio: TRadioButton
      Left = 8
      Top = 156
      Width = 140
      Height = 17
      Caption = 'Pobieraj dane z VUPlayer'
      TabOrder = 6
    end
    object XMPlayDownRadio: TRadioButton
      Left = 8
      Top = 180
      Width = 129
      Height = 17
      Caption = 'Pobieraj dane z XMPlay'
      TabOrder = 7
    end
  end
  object SettingsGroupBox: TGroupBox
    Left = 6
    Top = 238
    Width = 283
    Height = 94
    Caption = 'Opcje'
    TabOrder = 1
    object PreviewLabel: TLabel
      Left = 8
      Top = 20
      Width = 38
      Height = 13
      Caption = 'Podgl'#261'd'
    end
    object PrefixLabel: TLabel
      Left = 8
      Top = 44
      Width = 28
      Height = 13
      Caption = 'Prefix'
    end
    object SuffixLabel: TLabel
      Left = 8
      Top = 68
      Width = 28
      Height = 13
      Caption = 'Suffix'
    end
    object Preview: TEdit
      Left = 52
      Top = 16
      Width = 221
      Height = 21
      ReadOnly = True
      TabOrder = 0
    end
    object PrefixEdit: TEdit
      Left = 52
      Top = 40
      Width = 181
      Height = 21
      TabOrder = 1
    end
    object SuffixEdit: TEdit
      Left = 52
      Top = 64
      Width = 181
      Height = 21
      TabOrder = 3
    end
    object PrefixCheckBox: TCheckBox
      Left = 240
      Top = 42
      Width = 37
      Height = 17
      Caption = 'On'
      TabOrder = 2
    end
    object SuffixCheckBox: TCheckBox
      Left = 240
      Top = 66
      Width = 37
      Height = 17
      Caption = 'On'
      TabOrder = 4
    end
  end
  object OkButton: TButton
    Left = 208
    Top = 336
    Width = 75
    Height = 25
    Caption = 'OK'
    TabOrder = 2
    OnClick = OkButtonClick
  end
  object RunPluginCheckBox: TCheckBox
    Left = 12
    Top = 340
    Width = 93
    Height = 17
    Caption = 'W'#322#261'cz wtyczk'#281
    TabOrder = 3
    OnClick = RunPluginCheckBoxClick
  end
  object ActionList: TActionList
    Left = 168
    Top = 336
    object aWinampDown: TAction
      Caption = 'aWinampDown'
      OnExecute = aWinampDownExecute
    end
    object aFoobarDown: TAction
      Caption = 'aFoobarDown'
      OnExecute = aFoobarDownExecute
    end
    object aLastFMDown: TAction
      Caption = 'aLastFMDown'
      OnExecute = aLastFMDownExecute
    end
    object aWMP7_11Down: TAction
      Caption = 'aWMP7_11Down'
      OnExecute = aWMP7_11DownExecute
    end
    object aWMP64Down: TAction
      Caption = 'aWMP64Down'
      Visible = False
      OnExecute = aWMP64DownExecute
    end
    object aVUPlayerDown: TAction
      Caption = 'aVUPlayerDown'
      OnExecute = aVUPlayerDownExecute
    end
    object aXMPlayDown: TAction
      Caption = 'aXMPlayDown'
      OnExecute = aXMPlayDownExecute
    end
    object aMPCDown: TAction
      Caption = 'aMPCDown'
      OnExecute = aMPCDownExecute
    end
    object aPreSufFix: TAction
      Caption = 'aPreSufFix'
      OnExecute = aPreSufFixExecute
    end
  end
  object Timer: TTimer
    Enabled = False
    OnTimer = TimerTimer
    Left = 104
    Top = 336
  end
  object XPMan: TXPMan
    Left = 136
    Top = 336
  end
end
