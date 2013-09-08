object MainForm: TMainForm
  Left = 432
  Top = 116
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'TuneStatus by Beherit'
  ClientHeight = 300
  ClientWidth = 290
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
  object OkButton: TButton
    Left = 206
    Top = 271
    Width = 75
    Height = 25
    Caption = 'OK'
    Enabled = False
    TabOrder = 0
    OnClick = OkButtonClick
  end
  object LMDPageControl: TLMDPageControl
    Left = 4
    Top = 4
    Width = 284
    Height = 265
    TabOrder = 1
    Options = [toNoTabIfEmpty, toShowBorder, toShowFocusRect, toShowAccelChar]
    RaggedRight = False
    DockOrientation = doNoOrient
    DefaultPage = HandlingLMDTabSheet
    ActivePage = HandlingLMDTabSheet
    object HandlingLMDTabSheet: TLMDTabSheet
      Left = 4
      Top = 27
      Width = 276
      Height = 234
      Bevel.Mode = bmCustom
      Caption = 'Obs'#322'uga'
      object WMP7_11Label: TLabel
        Left = 30
        Top = 98
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
        Transparent = True
      end
      object WinampDownRadio: TLMDRadioButton
        Left = 6
        Top = 6
        Width = 221
        Height = 18
        Caption = 'Pobieraj dane z Winamp/AIMP2/KMPlayer'
        Alignment.Alignment = agTopLeft
        Alignment.Spacing = 4
        AutoSize = True
        Checked = False
        TabOrder = 0
        Transparent = True
      end
      object FoobarDownRadio: TLMDRadioButton
        Left = 6
        Top = 30
        Width = 158
        Height = 18
        Caption = 'Pobieraj dane z Foobar2000'
        Alignment.Alignment = agTopLeft
        Alignment.Spacing = 4
        AutoSize = True
        Checked = False
        TabOrder = 1
        Transparent = True
      end
      object LastFMDownRadio: TLMDRadioButton
        Left = 6
        Top = 54
        Width = 169
        Height = 18
        Caption = 'Pobieraj dane z Last.fm Player'
        Alignment.Alignment = agTopLeft
        Alignment.Spacing = 4
        AutoSize = True
        Checked = False
        TabOrder = 2
        Transparent = True
      end
      object WMP7_11DownRadio: TLMDRadioButton
        Left = 6
        Top = 78
        Width = 232
        Height = 18
        Caption = 'Pobieraj dane z Windows Media Player 7-11'
        Alignment.Alignment = agTopLeft
        Alignment.Spacing = 4
        AutoSize = True
        Checked = False
        TabOrder = 3
        Transparent = True
      end
      object WMP64DownRadio: TLMDRadioButton
        Left = 6
        Top = 118
        Width = 226
        Height = 18
        Caption = 'Pobieraj dane z Windows Media Player 6.4'
        Alignment.Alignment = agTopLeft
        Alignment.Spacing = 4
        AutoSize = True
        Checked = False
        TabOrder = 4
        Transparent = True
      end
      object VUPlayerDownRadio: TLMDRadioButton
        Left = 6
        Top = 142
        Width = 143
        Height = 18
        Caption = 'Pobieraj dane z VUPlayer'
        Alignment.Alignment = agTopLeft
        Alignment.Spacing = 4
        AutoSize = True
        Checked = False
        TabOrder = 5
        Transparent = True
      end
      object XMPlayDownRadio: TLMDRadioButton
        Left = 6
        Top = 166
        Width = 134
        Height = 18
        Caption = 'Pobieraj dane z XMPlay'
        Alignment.Alignment = agTopLeft
        Alignment.Spacing = 4
        AutoSize = True
        Checked = False
        TabOrder = 6
        Transparent = True
      end
      object MPCDownRadio: TLMDRadioButton
        Left = 6
        Top = 190
        Width = 196
        Height = 18
        Caption = 'Pobieraj dane z Media Player Classic'
        Alignment.Alignment = agTopLeft
        Alignment.Spacing = 4
        AutoSize = True
        Checked = False
        TabOrder = 7
        Transparent = True
      end
      object AutoDownRadio: TLMDRadioButton
        Left = 6
        Top = 214
        Width = 126
        Height = 18
        Caption = 'Okre'#347'l automatycznie'
        Alignment.Alignment = agTopLeft
        Alignment.Spacing = 4
        AutoSize = True
        Checked = True
        TabOrder = 8
        Transparent = True
      end
    end
    object StatusLooksLMDTabSheet: TLMDTabSheet
      Left = 4
      Top = 27
      Width = 276
      Height = 234
      Bevel.Mode = bmCustom
      Caption = 'Wygl'#261'd opisu'
      object TagRequestLabel: TLabel
        Left = 11
        Top = 210
        Width = 251
        Height = 13
        Caption = 'W opisie musi wyst'#261'pi'#263' tag CC_TUNESTATUS!'
        Enabled = False
        Font.Charset = EASTEUROPE_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
        Transparent = True
      end
      object StatusMemo: TMemo
        Left = 8
        Top = 32
        Width = 258
        Height = 170
        Lines.Strings = (
          'TuneStatus CC_PLUGINVERSION'
          'http://www.beherit.za.pl/?page_id=118'
          ''
          '~ CC_TUNESTATUS ~')
        MaxLength = 512
        ScrollBars = ssVertical
        TabOrder = 1
      end
      object PreviewStatusMemo: TMemo
        Left = 8
        Top = 32
        Width = 258
        Height = 170
        MaxLength = 512
        ScrollBars = ssVertical
        TabOrder = 2
        OnChange = PreviewStatusMemoChange
      end
      object TagsBox: TComboBox
        Left = 8
        Top = 4
        Width = 257
        Height = 21
        Style = csDropDownList
        ItemHeight = 13
        TabOrder = 0
        OnCloseUp = TagsBoxCloseUp
        OnDropDown = TagsBoxDropDown
        OnSelect = TagsBoxSelect
        Items.Strings = (
          'CC_TUNESTATUS        (pobrany utw'#243'r)'
          'CC_STARTSTATUS      (opis pocz'#261'tkowy)'
          'CC_PLUGINVERSION   (wersja TuneStatus)'
          'CC_AQQVERSION        (wersja AQQ)'
          'CC_PLAYERNAME        (nazwa odtwarzacza)'
          'CC_SAMPLERATE        (samplerate)'
          'CC_BITRATE                (birate)'
          'CC_CHANNELS             (ilo'#347#263' kana'#322#243'w)'
          'CC_SONGLENGTH       (d'#322'ugo'#347#263' utworu [mm:ss])')
      end
    end
    object SettingsLMDTabSheet: TLMDTabSheet
      Left = 4
      Top = 27
      Width = 276
      Height = 234
      Bevel.Mode = bmCustom
      Caption = 'Inne opcje'
      object SongTimerIntervalLabel1: TLabel
        Left = 10
        Top = 80
        Width = 78
        Height = 13
        Caption = 'Ustawiaj opis po'
        Transparent = True
      end
      object SongTimerIntervalLabel2: TLabel
        Left = 144
        Top = 80
        Width = 108
        Height = 13
        Caption = 'od rozpocz'#281'cia utworu'
        Transparent = True
      end
      object EnableFastOnOffCheckBox: TLMDCheckBox
        Left = 6
        Top = 54
        Width = 258
        Height = 18
        Caption = 'Pokazuj przycisk szybkiego w'#322#261'czenia/wy'#322#261'czenia'
        Alignment.Alignment = agTopLeft
        Alignment.Spacing = 4
        AutoSize = True
        TabOrder = 0
        Transparent = True
        OnChange = EnableFastOnOffCheckBoxChange
        Checked = True
        State = cbChecked
      end
      object EnablePluginOnStartCheckBox: TLMDCheckBox
        Left = 6
        Top = 30
        Width = 188
        Height = 18
        Caption = 'W'#322#261'czaj wtyczk'#281' przy starcie AQQ'
        Alignment.Alignment = agTopLeft
        Alignment.Spacing = 4
        AutoSize = True
        TabOrder = 1
        Transparent = True
      end
      object SetOnlyInJabberCheckBox: TLMDCheckBox
        Left = 6
        Top = 6
        Width = 227
        Height = 18
        Caption = 'Ustawiaj opis tylko na kontach sieci Jabber'
        Alignment.Alignment = agTopLeft
        Alignment.Spacing = 4
        AutoSize = True
        TabOrder = 2
        Transparent = True
      end
      object SongTimerSpin: TLMDSpinEdit
        Left = 92
        Top = 77
        Width = 45
        Height = 21
        Bevel.Mode = bmWindows
        Caret.BlinkRate = 530
        TabOrder = 3
        Transparent = True
        AutoSelect = True
        Alignment = taRightJustify
        CustomButtons = <>
        Suffix = ' s'
        MinValue = 1
        MaxValue = 30
        Value = 5
        Value = 5
      end
    end
  end
  object RunPluginCheckBox: TLMDCheckBox
    Left = 12
    Top = 276
    Width = 94
    Height = 18
    Caption = 'W'#322#261'cz wtyczk'#281
    Alignment.Alignment = agTopLeft
    Alignment.Spacing = 4
    AutoSize = True
    TabOrder = 2
    Transparent = True
    OnChange = RunPluginCheckBoxChange
  end
  object ActionList: TActionList
    Left = 176
    Top = 272
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
    object aAutoDown: TAction
      Caption = 'aAutoDown'
      OnExecute = aAutoDownExecute
    end
    object aCutSongNumber: TAction
      Caption = 'aCutSongNumber'
      OnExecute = aCutSongNumberExecute
    end
    object aReadSettings: TAction
      Caption = 'aReadSettings'
      OnExecute = aReadSettingsExecute
    end
    object aSaveSettings: TAction
      Caption = 'aSaveSettings'
      OnExecute = aSaveSettingsExecute
    end
    object aSetStatusLooks: TAction
      Caption = 'aSetStatusLooks'
      OnExecute = aSetStatusLooksExecute
    end
  end
  object Timer: TTimer
    Enabled = False
    OnTimer = TimerTimer
    Left = 112
    Top = 272
  end
  object XPMan: TXPMan
    Left = 144
    Top = 272
  end
  object SongTimer: TTimer
    Enabled = False
    Interval = 5000
    OnTimer = SongTimerTimer
    Left = 80
    Top = 272
  end
end
