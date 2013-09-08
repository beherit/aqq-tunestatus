object MainForm: TMainForm
  Left = 565
  Top = 111
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'TuneStatus by Beherit'
  ClientHeight = 371
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
  object SaveButton: TButton
    Left = 206
    Top = 342
    Width = 75
    Height = 25
    Caption = 'Zapisz'
    Enabled = False
    TabOrder = 0
    OnClick = SaveButtonClick
  end
  object LMDPageControl: TLMDPageControl
    Left = 4
    Top = 4
    Width = 284
    Height = 336
    TabOrder = 1
    Options = [toNoTabIfEmpty, toShowBorder, toShowFocusRect, toShowAccelChar]
    RaggedRight = False
    DockOrientation = doNoOrient
    ActivePage = HandlingTabSheet
    object HandlingTabSheet: TLMDTabSheet
      Left = 4
      Top = 27
      Width = 276
      Height = 305
      Bevel.Mode = bmCustom
      Caption = 'Obs'#322'uga'
      object AutoDownInfoLabel: TLabel
        Left = 31
        Top = 5
        Width = 214
        Height = 39
        Alignment = taCenter
        Caption = 
          'Wybierz kt'#243're odtwarzacze ma obs'#322'ugiwa'#263' wtyczka oraz okre'#347'l ich ' +
          'kolejno'#347#263' na li'#347'cie co decyduje o ich priorytecie:'
        Transparent = True
        WordWrap = True
      end
      object AutoDownInfoLabel2: TLabel
        Left = 38
        Top = 44
        Width = 201
        Height = 13
        Caption = '(do zmiany kolejno'#347'ci u'#380'yj Drag and Drop)'
        Enabled = False
        Transparent = True
      end
      object AutoDownCheckListBox: TLMDCheckListBox
        Left = 50
        Top = 62
        Width = 177
        Height = 211
        ItemHeight = 16
        TabOrder = 0
        Visible = False
        Items.Strings = (
          'Winamp/AIMP2/KMPlayer'
          'Foobar2000'
          'Windows Media Player'
          'VUPlayer'
          'XMPlay'
          'Media Player Classic'
          'iTunes'
          'ALSong'
          'aTunes'
          'Screamer Radio'
          'AQQ Radio'
          'Songbird'
          'Last.fm Player')
        BtnGap = 4
        ItemIndex = -1
        Layout.Alignment = agTopLeft
      end
      object ResetButton: TButton
        Left = 50
        Top = 276
        Width = 75
        Height = 25
        Action = aResetSettings
        TabOrder = 2
      end
      object AutoDownCheckListBoxPreview: TLMDCheckListBox
        Left = 50
        Top = 62
        Width = 177
        Height = 211
        DragMode = dmAutomatic
        ItemHeight = 16
        ParentShowHint = False
        ShowHint = True
        TabOrder = 1
        OnClick = AutoDownCheckListBoxPreviewClick
        OnDragDrop = AutoDownCheckListBoxPreviewDragDrop
        OnDragOver = AutoDownCheckListBoxPreviewDragOver
        OnMouseMove = AutoDownCheckListBoxPreviewMouseMove
        Items.Strings = (
          'Winamp/AIMP2/KMPlayer'
          'Foobar2000'
          'Windows Media Player'
          'VUPlayer'
          'XMPlay'
          'Media Player Classic'
          'iTunes'
          'ALSong'
          'aTunes'
          'Screamer Radio'
          'AQQ Radio'
          'Songbird'
          'Last.fm Player')
        BtnGap = 4
        ItemIndex = -1
        Layout.Alignment = agTopLeft
      end
    end
    object StatusLooksTabSheet: TLMDTabSheet
      Left = 4
      Top = 27
      Width = 276
      Height = 305
      Bevel.Mode = bmCustom
      Caption = 'Wygl'#261'd opisu'
      object TagRequestLabel: TLabel
        Left = 13
        Top = 228
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
      object TagInfoLabel1: TLabel
        Left = 68
        Top = 256
        Width = 130
        Height = 13
        Caption = 'Przyk'#322'adowy wygl'#261'd opisu:'
        Enabled = False
        Font.Charset = EASTEUROPE_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
        Transparent = True
      end
      object TagInfoLabel2: TLabel
        Left = 48
        Top = 270
        Width = 172
        Height = 13
        Caption = 'Obecnie s'#322'ucham: CC_TUNESTATUS'
        Enabled = False
        Font.Charset = EASTEUROPE_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = [fsItalic]
        ParentFont = False
        Transparent = True
      end
      object StatusMemo: TMemo
        Left = 9
        Top = 34
        Width = 258
        Height = 137
        Lines.Strings = (
          'TuneStatus CC_PLUGINVERSION'
          'http://www.beherit.za.pl/?page_id=118'
          ''
          '~ CC_TUNESTATUS ~')
        MaxLength = 512
        ScrollBars = ssVertical
        TabOrder = 1
      end
      object TagsBox: TComboBox
        Left = 9
        Top = 6
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
          '--------Tylko dla Winamp/AIMP2/KMPlayer--------'
          'CC_SAMPLERATE        (samplerate)'
          'CC_BITRATE                (birate)'
          'CC_CHANNELS             (ilo'#347#263' kana'#322#243'w)'
          'CC_SONGLENGTH       (d'#322'ugo'#347#263' utworu [mm:ss])')
      end
      object SongFromFile: TMemo
        Left = 9
        Top = 186
        Width = 257
        Height = 25
        TabStop = False
        ReadOnly = True
        TabOrder = 3
        Visible = False
        WordWrap = False
      end
      object PreviewStatusMemo: TMemo
        Left = 9
        Top = 34
        Width = 258
        Height = 177
        MaxLength = 512
        ScrollBars = ssVertical
        TabOrder = 2
        OnChange = PreviewStatusMemoChange
      end
    end
    object SettingsTabSheet: TLMDTabSheet
      Left = 4
      Top = 27
      Width = 276
      Height = 305
      Bevel.Mode = bmCustom
      Caption = 'Opcje'
      DesignSize = (
        276
        305)
      object SongTimerIntervalLabel1: TLabel
        Left = 16
        Top = 93
        Width = 78
        Height = 13
        Caption = 'Ustawiaj opis po'
        Transparent = True
      end
      object SongTimerIntervalLabel2: TLabel
        Left = 150
        Top = 93
        Width = 108
        Height = 13
        Caption = 'od rozpocz'#281'cia utworu'
        Transparent = True
      end
      object Bevel3: TBevel
        Left = 9
        Top = 232
        Width = 258
        Height = 4
        Anchors = [akLeft, akTop, akRight]
        Shape = bsTopLine
      end
      object TimeTurnOffLabel: TLabel
        Left = 39
        Top = 245
        Width = 139
        Height = 26
        Alignment = taCenter
        Caption = 'Wy'#322#261'cz dzia'#322'anie wtyczki gdy utw'#243'r nie zmienia si'#281' od'
        Enabled = False
        Transparent = True
        WordWrap = True
      end
      object Bevel1: TBevel
        Left = 9
        Top = 152
        Width = 258
        Height = 4
        Anchors = [akLeft, akTop, akRight]
        Shape = bsTopLine
      end
      object Bevel2: TBevel
        Left = 9
        Top = 80
        Width = 258
        Height = 4
        Anchors = [akLeft, akTop, akRight]
        Shape = bsTopLine
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
        Left = 98
        Top = 90
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
      object CutRadiostationNameCheckBox: TLMDCheckBox
        Left = 6
        Top = 162
        Width = 263
        Height = 31
        Caption = 
          'Ucinaj nazw'#281' radiostacji przy pobieraniu utworu z wtyczki AQQ Ra' +
          'dio'
        Alignment.Alignment = agTopLeft
        Alignment.Spacing = 4
        AutoSize = True
        TabOrder = 4
        Transparent = True
        Checked = True
        State = cbChecked
      end
      object DisableSongTimerCheckBox: TLMDCheckBox
        Left = 39
        Top = 116
        Width = 198
        Height = 31
        Caption = 'Funkcja nieaktywna przy pobieraniu danych z wtyczki AQQ Radio'
        Alignment.Alignment = agTopLeft
        Alignment.Spacing = 4
        AutoSize = True
        TabOrder = 5
        Transparent = True
        Checked = True
        State = cbChecked
      end
      object CutWWWCheckBox: TLMDCheckBox
        Left = 5
        Top = 204
        Width = 258
        Height = 18
        Caption = 'Wycinaj adresy stron WWW z pobranego utworu'
        Alignment.Alignment = agTopLeft
        Alignment.Spacing = 4
        AutoSize = True
        TabOrder = 6
        Transparent = True
      end
      object TimeTurnOffCheckBox: TLMDCheckBox
        Left = 20
        Top = 251
        Width = 16
        Height = 15
        Alignment.Alignment = agTopLeft
        Alignment.Spacing = 4
        AutoSize = True
        TabOrder = 7
        Transparent = True
        OnChange = TimeTurnOffCheckBoxChange
      end
      object TimeTurnOffSpin: TLMDSpinEdit
        Left = 183
        Top = 249
        Width = 57
        Height = 21
        Bevel.Mode = bmWindows
        Caret.BlinkRate = 530
        Enabled = False
        TabOrder = 8
        Transparent = True
        AutoSelect = True
        Alignment = taRightJustify
        CustomButtons = <>
        Suffix = ' min'
        MinValue = 5
        MaxValue = 60
        Value = 15
        Value = 15
      end
    end
  end
  object RunPluginCheckBox: TLMDCheckBox
    Left = 12
    Top = 347
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
    Left = 179
    Top = 336
    object aCutSongNumber: TAction
      Category = 'Status'
      Caption = 'aCutSongNumber'
      OnExecute = aCutSongNumberExecute
    end
    object aWinampDown: TAction
      Category = 'Players'
      Caption = 'aWinampDown'
      OnExecute = aWinampDownExecute
    end
    object aCutWWW: TAction
      Category = 'Status'
      Caption = 'aCutWWW'
      OnExecute = aCutWWWExecute
    end
    object aFoobarDown: TAction
      Category = 'Players'
      Caption = 'aFoobarDown'
      OnExecute = aFoobarDownExecute
    end
    object aWMPDown: TAction
      Category = 'Players'
      Caption = 'aWMPDown'
      OnExecute = aWMPDownExecute
    end
    object aVUPlayerDown: TAction
      Category = 'Players'
      Caption = 'aVUPlayerDown'
      OnExecute = aVUPlayerDownExecute
    end
    object aXMPlayDown: TAction
      Category = 'Players'
      Caption = 'aXMPlayDown'
      OnExecute = aXMPlayDownExecute
    end
    object aMPCDown: TAction
      Category = 'Players'
      Caption = 'aMPCDown'
      OnExecute = aMPCDownExecute
    end
    object aiTunesDown: TAction
      Category = 'Players'
      Caption = 'aiTunesDown'
      OnExecute = aiTunesDownExecute
    end
    object aaTunesDown: TAction
      Category = 'Players'
      Caption = 'aaTunesDown'
      OnExecute = aaTunesDownExecute
    end
    object aALSongDown: TAction
      Category = 'Players'
      Caption = 'aALSongDown'
      OnExecute = aALSongDownExecute
    end
    object aScreamerRadioDown: TAction
      Category = 'Players'
      Caption = 'aScreamerRadioDown'
      OnExecute = aScreamerRadioDownExecute
    end
    object aPluginAQQRadioDown: TAction
      Category = 'Players'
      Caption = 'aPluginAQQRadioDown'
      OnExecute = aPluginAQQRadioDownExecute
    end
    object aSongbirdDown: TAction
      Category = 'Players'
      Caption = 'aSongbirdDown'
      OnExecute = aSongbirdDownExecute
    end
    object aLastFMDown: TAction
      Category = 'Players'
      Caption = 'aLastFMDown'
      OnExecute = aLastFMDownExecute
    end
    object aAutoDown: TAction
      Category = 'Players'
      Caption = 'aAutoDown'
      OnExecute = aAutoDownExecute
    end
    object aSetStatusLooks: TAction
      Category = 'Status'
      Caption = 'aSetStatusLooks'
      OnExecute = aSetStatusLooksExecute
    end
    object aReadSettings: TAction
      Category = 'Settings'
      Caption = 'aReadSettings'
      OnExecute = aReadSettingsExecute
    end
    object aSaveSettings: TAction
      Category = 'Settings'
      Caption = 'aSaveSettings'
      OnExecute = aSaveSettingsExecute
    end
    object aResetSettings: TAction
      Category = 'Settings'
      Caption = 'Resetuj'
      OnExecute = aResetSettingsExecute
    end
  end
  object Timer: TTimer
    Enabled = False
    OnTimer = TimerTimer
    Left = 115
    Top = 336
  end
  object XPMan: TXPMan
    Left = 147
    Top = 336
  end
  object SongTimer: TTimer
    Enabled = False
    Interval = 5000
    OnTimer = SongTimerTimer
    Left = 83
    Top = 336
  end
  object TurnOffTimer: TTimer
    Enabled = False
    Interval = 900000
    OnTimer = TurnOffTimerTimer
    Left = 48
    Top = 336
  end
  object FoobarPopupMenu: TPopupMenu
    Left = 248
    Top = 160
    object FoobarDownload: TMenuItem
      Caption = 'Pobierz wtyczk'#281
      OnClick = FoobarDownloadClick
    end
  end
  object WMPPopupMenu: TPopupMenu
    Left = 248
    Top = 192
    object WMPDownload: TMenuItem
      Caption = 'Pobierz plugin'
      OnClick = WMPDownloadClick
    end
  end
  object iTunesPopupMenu: TPopupMenu
    Left = 248
    Top = 224
    object iTunesDownload: TMenuItem
      Caption = 'Pobierz plugin'
      OnClick = iTunesDownloadClick
    end
  end
  object SongbirdPopupMenu: TPopupMenu
    Left = 248
    Top = 256
    object SongbirdDownload: TMenuItem
      Caption = 'Pobierz rozszerzenie'
      OnClick = SongbirdDownloadClick
    end
  end
end
