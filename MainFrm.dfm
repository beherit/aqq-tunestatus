object MainForm: TMainForm
  Left = 616
  Top = 102
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'TuneStatus by Beherit'
  ClientHeight = 359
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
    Top = 330
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
    Height = 325
    TabOrder = 1
    Options = [toNoTabIfEmpty, toShowBorder, toShowFocusRect, toShowAccelChar]
    RaggedRight = False
    DockOrientation = doNoOrient
    DefaultPage = HandlingTabSheet
    ActivePage = HandlingTabSheet
    object HandlingTabSheet: TLMDTabSheet
      Left = 4
      Top = 27
      Width = 276
      Height = 294
      Bevel.Mode = bmCustom
      Caption = 'Obs'#322'uga'
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
        Cursor = crHelp
        Hint = 
          'Dla lepszego dzia'#322'ania zaleca si'#281' zainstalowa'#263' wtyczk'#281' do Foobar' +
          '2000'
        Caption = 'Pobieraj dane z Foobar2000'
        Alignment.Alignment = agTopLeft
        Alignment.Spacing = 4
        AutoSize = True
        Checked = False
        ParentShowHint = False
        PopupMenu = FoobarPopupMenu
        ShowHint = True
        TabOrder = 1
        Transparent = True
      end
      object LastFMDownRadio: TLMDRadioButton
        Left = 6
        Top = 246
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
      object WMPDownRadio: TLMDRadioButton
        Left = 6
        Top = 54
        Width = 207
        Height = 18
        Cursor = crHelp
        Hint = 'Dla wersji WMP powy'#380'ej 7 do ob'#322'sugi potrzebny jest plugin w WMP'
        Caption = 'Pobieraj dane z Windows Media Player'
        Alignment.Alignment = agTopLeft
        Alignment.Spacing = 4
        AutoSize = True
        Checked = False
        ParentShowHint = False
        PopupMenu = WMPPopupMenu
        ShowHint = True
        TabOrder = 3
        Transparent = True
      end
      object VUPlayerDownRadio: TLMDRadioButton
        Left = 6
        Top = 78
        Width = 143
        Height = 18
        Caption = 'Pobieraj dane z VUPlayer'
        Alignment.Alignment = agTopLeft
        Alignment.Spacing = 4
        AutoSize = True
        Checked = False
        TabOrder = 4
        Transparent = True
      end
      object XMPlayDownRadio: TLMDRadioButton
        Left = 6
        Top = 102
        Width = 134
        Height = 18
        Caption = 'Pobieraj dane z XMPlay'
        Alignment.Alignment = agTopLeft
        Alignment.Spacing = 4
        AutoSize = True
        Checked = False
        TabOrder = 5
        Transparent = True
      end
      object MPCDownRadio: TLMDRadioButton
        Left = 6
        Top = 126
        Width = 196
        Height = 18
        Caption = 'Pobieraj dane z Media Player Classic'
        Alignment.Alignment = agTopLeft
        Alignment.Spacing = 4
        AutoSize = True
        Checked = False
        TabOrder = 6
        Transparent = True
      end
      object AutoDownRadio: TLMDRadioButton
        Left = 6
        Top = 270
        Width = 126
        Height = 18
        Caption = 'Okre'#347'l automatycznie'
        Alignment.Alignment = agTopLeft
        Alignment.Spacing = 4
        AutoSize = True
        Checked = True
        TabOrder = 7
        Transparent = True
      end
      object iTunesDownRadio: TLMDRadioButton
        Left = 6
        Top = 150
        Width = 131
        Height = 18
        Cursor = crHelp
        Hint = 'Do obs'#322'ugi potrzebny jest plugin w iTunes'
        Caption = 'Pobieraj dane z iTunes'
        Alignment.Alignment = agTopLeft
        Alignment.Spacing = 4
        AutoSize = True
        Checked = False
        ParentShowHint = False
        PopupMenu = iTunesPopupMenu
        ShowHint = True
        TabOrder = 8
        Transparent = True
      end
      object ALSongDownRadio: TLMDRadioButton
        Left = 6
        Top = 174
        Width = 136
        Height = 18
        Caption = 'Pobieraj dane z ALSong'
        Alignment.Alignment = agTopLeft
        Alignment.Spacing = 4
        AutoSize = True
        Checked = False
        TabOrder = 9
        Transparent = True
      end
      object PluginAQQRadioDownRadio: TLMDRadioButton
        Left = 6
        Top = 198
        Width = 191
        Height = 18
        Caption = 'Pobieraj dane z wtyczki AQQ Radio'
        Alignment.Alignment = agTopLeft
        Alignment.Spacing = 4
        AutoSize = True
        Checked = False
        TabOrder = 10
        Transparent = True
      end
      object SongbirdDownRadio: TLMDRadioButton
        Left = 6
        Top = 222
        Width = 142
        Height = 18
        Cursor = crHelp
        Hint = 
          'Do obs'#322'ugi potrzebne jest rozszerzebie Birdtitle zainstalowane w' +
          ' Songbird'
        Caption = 'Pobieraj dane z Songbird'
        Alignment.Alignment = agTopLeft
        Alignment.Spacing = 4
        AutoSize = True
        Checked = False
        ParentShowHint = False
        PopupMenu = SongbirdPopupMenu
        ShowHint = True
        TabOrder = 11
        Transparent = True
      end
    end
    object StatusLooksTabSheet: TLMDTabSheet
      Left = 4
      Top = 27
      Width = 276
      Height = 294
      Bevel.Mode = bmCustom
      Caption = 'Wygl'#261'd opisu'
      object TagRequestLabel: TLabel
        Left = 11
        Top = 240
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
        Left = 67
        Top = 260
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
        Left = 47
        Top = 274
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
        Left = 8
        Top = 32
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
          '--------Tylko dla Winamp/AIMP2/KMPlayer--------'
          'CC_SAMPLERATE        (samplerate)'
          'CC_BITRATE                (birate)'
          'CC_CHANNELS             (ilo'#347#263' kana'#322#243'w)'
          'CC_SONGLENGTH       (d'#322'ugo'#347#263' utworu [mm:ss])')
      end
      object SongFromFile: TMemo
        Left = 8
        Top = 184
        Width = 257
        Height = 25
        TabStop = False
        ReadOnly = True
        TabOrder = 3
        Visible = False
        WordWrap = False
      end
      object PreviewStatusMemo: TMemo
        Left = 8
        Top = 32
        Width = 258
        Height = 201
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
      Height = 294
      Bevel.Mode = bmCustom
      Caption = 'Opcje'
      DesignSize = (
        276
        294)
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
      object Bevel2: TBevel
        Left = 8
        Top = 154
        Width = 258
        Height = 4
        Anchors = [akLeft, akTop, akRight]
        Shape = bsTopLine
      end
      object Bevel1: TBevel
        Left = 8
        Top = 82
        Width = 258
        Height = 4
        Anchors = [akLeft, akTop, akRight]
        Shape = bsTopLine
      end
      object Bevel3: TBevel
        Left = 8
        Top = 204
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
        Left = 34
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
    end
    object AdvancetAutoTabSheet: TLMDTabSheet
      Left = 4
      Top = 27
      Width = 276
      Height = 294
      Bevel.Mode = bmCustom
      Caption = 'Tryb automatyczny'
      object AutoDownUpButton: TLMDSpeedButton
        Left = 216
        Top = 72
        Width = 25
        Height = 25
        Enabled = False
        OnClick = AutoDownUpButtonClick
        ImageList = ImageList
        UseCaption = False
      end
      object AutoDownDownButton: TLMDSpeedButton
        Left = 216
        Top = 104
        Width = 25
        Height = 25
        Enabled = False
        OnClick = AutoDownDownButtonClick
        ImageList = ImageList
        ImageIndex = 1
      end
      object AutoDownInfoLabel: TLabel
        Left = 20
        Top = 8
        Width = 231
        Height = 52
        Alignment = taCenter
        Caption = 
          'Mo'#380'esz tutaj okre'#347'li'#263' hierarchie trybu automatycznego oraz wy'#322#261'c' +
          'zy'#263' z szukania poszczeg'#243'lne odtwarzacze co mo'#380'e wp'#322'yn'#261#263' na szybk' +
          'o'#347#263' dzia'#322'ania AQQ.'
        Transparent = True
        WordWrap = True
      end
      object AutoDownCheckListBox: TLMDCheckListBox
        Left = 32
        Top = 72
        Width = 177
        Height = 180
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
          'AQQ Radio'
          'Songbird'
          'Last.fm Player')
        ItemIndex = -1
        Layout.Alignment = agTopLeft
      end
      object ResetButton: TButton
        Left = 32
        Top = 256
        Width = 75
        Height = 25
        Caption = 'Resetuj'
        TabOrder = 2
        OnClick = ResetButtonClick
      end
      object AutoDownCheckListBoxPreview: TLMDCheckListBox
        Left = 32
        Top = 72
        Width = 177
        Height = 180
        ItemHeight = 16
        TabOrder = 1
        OnSelect = AutoDownCheckListBoxPreviewSelect
        Items.Strings = (
          'Winamp/AIMP2/KMPlayer'
          'Foobar2000'
          'Windows Media Player'
          'VUPlayer'
          'XMPlay'
          'Media Player Classic'
          'iTunes'
          'ALSong'
          'AQQ Radio'
          'Songbird'
          'Last.fm Player')
        ItemIndex = -1
        Layout.Alignment = agTopLeft
      end
    end
  end
  object RunPluginCheckBox: TLMDCheckBox
    Left = 12
    Top = 335
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
    Top = 328
    object aWinampDown: TAction
      Category = 'Players'
      Caption = 'aWinampDown'
      OnExecute = aWinampDownExecute
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
    object aALSongDown: TAction
      Category = 'Players'
      Caption = 'aALSongDown'
      OnExecute = aALSongDownExecute
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
    object aCutSongNumber: TAction
      Category = 'Status'
      Caption = 'aCutSongNumber'
      OnExecute = aCutSongNumberExecute
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
  end
  object Timer: TTimer
    Enabled = False
    OnTimer = TimerTimer
    Left = 112
    Top = 328
  end
  object XPMan: TXPMan
    Left = 144
    Top = 328
  end
  object SongTimer: TTimer
    Enabled = False
    Interval = 5000
    OnTimer = SongTimerTimer
    Left = 80
    Top = 328
  end
  object SongbirdPopupMenu: TPopupMenu
    Left = 248
    Top = 256
    object SongbirdDownload: TMenuItem
      Caption = 'Pobierz rozszerzenie'
      OnClick = SongbirdDownloadClick
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
  object FoobarPopupMenu: TPopupMenu
    Left = 248
    Top = 160
    object FoobarDownload: TMenuItem
      Caption = 'Pobierz wtyczk'#281
      OnClick = FoobarDownloadClick
    end
  end
  object ImageList: TImageList
    DrawingStyle = dsTransparent
    Masked = False
    Left = 248
    Top = 288
    Bitmap = {
      494C000602000400040010001000FFFFFFFFFF00FFFFFFFFFFFFFFFF424D3600
      0000000000003600000028000000100000004000000001002000000000000010
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000001D63402D1D6340961D6340F61D6340A51D63403C000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00001D6340391F6643FF53B27FFF52D08AFF2EAB69FF1E6742FF1D6340450000
      0000000000000000000000000000000000000000000000000000000000001D63
      40391F6643FF65BF8EFF74DDA2FF41D182FF34CE7CFF26B56CFF1E6742FF1D63
      40450000000000000000000000000000000000000000000000001D63403C1F66
      43FF73C397FF87E3AFFF6DDC9EFF3CD181FF32D07DFF28D27CFF1CC172FF1D67
      42FF1D63404B000000000000000000000000000000001D63403C206643FF82CC
      A3FF9BE9BDFF87E3B0FF69DC9DFF3BD281FF30D27EFF26D47DFF1DD87EFF13D1
      7BFF1D6742FF1D63404B00000000000000001D634030206643FF8FD1ACFFADEE
      CAFF9BE8BDFF87E3B0FF70DEA1FF39D382FF2ED37FFF24D67EFF1BDA80FF13DF
      82FF12D079FF1C6843FF1D63404E000000001D63409993D1AFFFBEF2D6FFAEEE
      CBFF9BE9BDFF5CB886FF71DEA2FF3ED585FF2DD480FF1BB56EFF1ADC81FF13E1
      84FF0EE586FF11D07AFF1D6340B7000000001D6340F9CCF5DEFFBEF2D6FFAEEE
      CBFF69BC90FF1D6340FF70DFA2FF45D789FF2DD480FF1D6340FF1BB06AFF13E1
      84FF0EE586FF0BE889FF1D6340FF000000001D6340D8B9EBD0FFBFF2D6FF75BF
      98FF1D6340961D6340FF71DEA2FF4CD78EFF2ED480FF1D6340FF1D63409619AE
      68FF0EE486FF0BE888FF1D6340FF000000001D63406F2F7653FF3E8661FF1D63
      40961D6340241D6340FF71DEA2FF54D993FF2FD37FFF1D6340FF1D63400C1D63
      40961B824FFF1A824FFF1D63409600000000000000001D6340151D63402A0000
      0000000000001D6340FF73DEA2FF5BD996FF31D17EFF1D6340FF000000000000
      00001D63402D1D63402D00000000000000000000000000000000000000000000
      0000000000001D6340FF73DDA2FF5DD895FF34CF7CFF1D6340FF000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000001D6340FF75DCA2FF5FD794FF38CE7DFF1D6340FF000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000001D6340FF76DCA1FF60D594FF3CCC7DFF1D6340FF000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000001D63409650AB7AFF5FD190FF2F9F63FF1D634096000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000001D6340211D63408A1D6340F31D63408A1D63401E000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000001D6340211D63408A1D6340F31D63408A1D63401E000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000001D63409650AB7AFF5FD190FF2F9F63FF1D634096000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000001D6340FF76DCA1FF60D594FF3CCC7DFF1D6340FF000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000001D6340FF75DCA2FF5FD794FF38CE7DFF1D6340FF000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000001D6340FF73DDA2FF5DD895FF34CF7CFF1D6340FF000000000000
      000000000000000000000000000000000000000000001D6340151D63402A0000
      0000000000001D6340FF73DEA2FF5BD996FF31D17EFF1D6340FF000000000000
      00001D63402D1D63402D00000000000000001D63406F2F7653FF3E8661FF1D63
      40961D6340241D6340FF71DEA2FF54D993FF2FD37FFF1D6340FF1D63400C1D63
      40961B824FFF1A824FFF1D634096000000001D6340D8B9EBD0FFBFF2D6FF75BF
      98FF1D6340961D6340FF71DEA2FF4CD78EFF2ED480FF1D6340FF1D63409619AE
      68FF0EE486FF0BE888FF1D6340FF000000001D6340F9CCF5DEFFBEF2D6FFAEEE
      CBFF69BC90FF1D6340FF70DFA2FF45D789FF2DD480FF1D6340FF1BB06AFF13E1
      84FF0EE586FF0BE889FF1D6340FF000000001D63409993D1AFFFBEF2D6FFAEEE
      CBFF9BE9BDFF5CB886FF71DEA2FF3ED585FF2DD480FF1BB56EFF1ADC81FF13E1
      84FF0EE586FF11D07AFF1D6340B7000000001D634030206643FF8FD1ACFFADEE
      CAFF9BE8BDFF87E3B0FF70DEA1FF39D382FF2ED37FFF24D67EFF1BDA80FF13DF
      82FF12D079FF1C6843FF1D63404E00000000000000001D63403C206643FF82CC
      A3FF9BE9BDFF87E3B0FF69DC9DFF3BD281FF30D27EFF26D47DFF1DD87EFF13D1
      7BFF1D6742FF1D63404B000000000000000000000000000000001D63403C1F66
      43FF73C397FF87E3AFFF6DDC9EFF3CD181FF32D07DFF28D27CFF1CC172FF1D67
      42FF1D63404B0000000000000000000000000000000000000000000000001D63
      40391F6643FF65BF8EFF74DDA2FF41D182FF34CE7CFF26B56CFF1E6742FF1D63
      4045000000000000000000000000000000000000000000000000000000000000
      00001D6340391F6643FF53B27FFF52D08AFF2EAB69FF1E6742FF1D6340450000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000001D63402D1D6340961D6340F61D6340A51D63403C000000000000
      000000000000000000000000000000000000424D3E000000000000003E000000
      2800000010000000400000000100010000000000000100000000000000000000
      000000000000000000000000FFFFFF0000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000AAAA000055550000AAAA000055550000
      AAAA000055550000AAAA000055550000AAAA000055550000AAAA000055550000
      AAAA000055550000AAAA00005555000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000}
  end
end
