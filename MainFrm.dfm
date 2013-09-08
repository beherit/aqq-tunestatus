object MainForm: TMainForm
  Left = 161
  Top = 110
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'TuneStatus by Beherit'
  ClientHeight = 354
  ClientWidth = 309
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
    Left = 222
    Top = 324
    Width = 75
    Height = 25
    Caption = 'Zapisz'
    Enabled = False
    TabOrder = 0
    TabStop = False
    OnClick = SaveButtonClick
  end
  object PageControl: TPageControl
    Left = 6
    Top = 6
    Width = 297
    Height = 313
    ActivePage = HandlingTabSheet
    TabOrder = 1
    object HandlingTabSheet: TTabSheet
      Caption = 'Obs'#322'uga'
      object AutoDownInfoLabel2: TLabel
        Left = 45
        Top = 46
        Width = 201
        Height = 13
        Caption = '(do zmiany kolejno'#347'ci u'#380'yj Drag and Drop)'
        Enabled = False
        Transparent = True
      end
      object AutoDownInfoLabel: TLabel
        Left = 38
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
      object AutoDownCheckListBox: TCheckListBox
        Left = 65
        Top = 68
        Width = 151
        Height = 174
        ItemHeight = 13
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
          'Screamer Radio'
          'aTunes'
          'Songbird'
          'Last.fm Player')
        TabOrder = 0
        Visible = False
      end
      object AutoDownCheckListBoxPreview: TCheckListBox
        Left = 65
        Top = 68
        Width = 151
        Height = 174
        DragMode = dmAutomatic
        ItemHeight = 13
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
          'Screamer Radio'
          'aTunes'
          'Songbird'
          'Last.fm Player')
        ParentShowHint = False
        ShowHint = True
        TabOrder = 1
        OnClick = AutoDownCheckListBoxPreviewClick
        OnDragDrop = AutoDownCheckListBoxPreviewDragDrop
        OnDragOver = AutoDownCheckListBoxPreviewDragOver
        OnMouseMove = AutoDownCheckListBoxPreviewMouseMove
      end
      object ResetButton: TButton
        Left = 65
        Top = 248
        Width = 75
        Height = 25
        Action = aResetSettings
        TabOrder = 2
      end
    end
    object StatusLooksTabSheet: TTabSheet
      Caption = 'Wygl'#261'd opisu'
      ImageIndex = 1
      object TagInfoLabel2: TLabel
        Left = 58
        Top = 263
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
      object TagInfoLabel1: TLabel
        Left = 79
        Top = 244
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
      object TagRequestLabel: TLabel
        Left = 19
        Top = 217
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
      object TagsBox: TComboBox
        Left = 16
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
        Left = 16
        Top = 186
        Width = 257
        Height = 25
        TabStop = False
        ReadOnly = True
        TabOrder = 2
        Visible = False
        WordWrap = False
      end
      object StatusMemo: TMemo
        Left = 15
        Top = 33
        Width = 258
        Height = 137
        Lines.Strings = (
          '~ CC_TUNESTATUS ~')
        MaxLength = 512
        ScrollBars = ssVertical
        TabOrder = 3
        Visible = False
      end
      object PreviewStatusMemo: TMemo
        Left = 16
        Top = 33
        Width = 258
        Height = 177
        MaxLength = 512
        ScrollBars = ssVertical
        TabOrder = 1
        OnChange = PreviewStatusMemoChange
      end
    end
    object SettingsTabSheet: TTabSheet
      Caption = 'Opcje'
      ImageIndex = 2
      DesignSize = (
        289
        285)
      object Bevel3: TBevel
        Left = 9
        Top = 242
        Width = 271
        Height = 4
        Anchors = [akLeft, akTop, akRight]
        Shape = bsTopLine
      end
      object Bevel2: TBevel
        Left = 9
        Top = 53
        Width = 271
        Height = 4
        Anchors = [akLeft, akTop, akRight]
        Shape = bsTopLine
      end
      object Bevel1: TBevel
        Left = 9
        Top = 171
        Width = 271
        Height = 4
        Anchors = [akLeft, akTop, akRight]
        Shape = bsTopLine
      end
      object SongTimerIntervalLabel2: TLabel
        Left = 150
        Top = 113
        Width = 116
        Height = 13
        Caption = 's od rozpocz'#281'cia utworu'
        Transparent = True
      end
      object SongTimerIntervalLabel1: TLabel
        Left = 22
        Top = 113
        Width = 78
        Height = 13
        Caption = 'Ustawiaj opis po'
        Transparent = True
      end
      object EnablePluginOnStartCheckBox: TCheckBox
        Left = 6
        Top = 6
        Width = 280
        Height = 17
        Caption = 'Aktywuj dzia'#322'anie wtyczki wraz z uruchomieniem AQQ'
        TabOrder = 0
      end
      object EnableFastOnOffCheckBox: TCheckBox
        Left = 6
        Top = 30
        Width = 254
        Height = 17
        Caption = 'Pokazuj przycisk szybkiego w'#322#261'czenia/wy'#322#261'czenia'
        Checked = True
        State = cbChecked
        TabOrder = 1
      end
      object SetOnlyInJabberCheckBox: TCheckBox
        Left = 6
        Top = 63
        Width = 225
        Height = 17
        Caption = 'Ustawiaj opis tylko na kontach sieci Jabber'
        TabOrder = 2
      end
      object BlockInvisibleCheckBox: TCheckBox
        Left = 6
        Top = 87
        Width = 225
        Height = 17
        Caption = 'Nie ustawiaj opisu przy stanie niewidoczny'
        Checked = True
        State = cbChecked
        TabOrder = 3
      end
      object SongTimerSpin: TCSpinEdit
        Left = 106
        Top = 110
        Width = 38
        Height = 22
        MaxValue = 30
        MinValue = 4
        TabOrder = 4
        Value = 5
      end
      object DisableSongTimerCheckBox: TCheckBox
        Left = 48
        Top = 132
        Width = 193
        Height = 33
        Caption = 'Funkcja nieaktywna przy pobieraniu danych z wtyczki AQQ Radio'
        Checked = True
        State = cbChecked
        TabOrder = 5
        WordWrap = True
      end
      object CutRadiostationNameCheckBox: TCheckBox
        Left = 6
        Top = 181
        Width = 263
        Height = 26
        Caption = 
          'Ucinaj nazw'#281' radiostacji przy pobieraniu utworu z wtyczki AQQ Ra' +
          'dio'
        Checked = True
        State = cbChecked
        TabOrder = 6
        WordWrap = True
      end
      object CutWWWCheckBox: TCheckBox
        Left = 6
        Top = 213
        Width = 261
        Height = 25
        Caption = 'Wycinaj adresy stron WWW z pobranego utworu'
        TabOrder = 7
        WordWrap = True
      end
      object TimeTurnOffCheckBox: TCheckBox
        Left = 41
        Top = 252
        Width = 163
        Height = 26
        Caption = 'Wy'#322#261'cz dzia'#322'anie wtyczki gdy utw'#243'r nie zmienia si'#281' od (min)'
        TabOrder = 8
        WordWrap = True
        OnClick = TimeTurnOffCheckBoxClick
      end
      object TimeTurnOffSpin: TCSpinEdit
        Left = 210
        Top = 254
        Width = 38
        Height = 22
        MaxValue = 60
        MinValue = 5
        TabOrder = 9
        Value = 5
      end
    end
    object UserTuneTabSheet: TTabSheet
      Caption = 'User Tune'
      ImageIndex = 3
      DesignSize = (
        289
        285)
      object UserTuneTimeNLabel: TLabel
        Left = 45
        Top = 43
        Width = 155
        Height = 13
        Caption = 'Czas wy'#347'wietlania notyfikacji (s)'
        Transparent = True
      end
      object UserTuneSLabel: TLabel
        Left = 21
        Top = 110
        Width = 247
        Height = 39
        Alignment = taCenter
        Caption = 
          'Funkcja nie dzia'#322'a z odtwarzaczami Winamp, AIMP, The KMPlayer or' +
          'az Foobar2000, gdy'#380' s'#261' one domy'#347'lnie obs'#322'ugiwane przez samo AQQ'
        Enabled = False
        Transparent = True
        WordWrap = True
      end
      object UserTuneLabel: TLabel
        Left = 23
        Top = 242
        Width = 242
        Height = 39
        Alignment = taCenter
        Caption = 
          'Obs'#322'uga User Tune (XEP-0118) jest przeznaczona wy'#322#261'cznie dla sie' +
          'ci Jabber! Funkcja jest niezale'#380'na od aktywacji wtyczki (ustawia' +
          'nie opisu)!'
        Enabled = False
        Transparent = True
        WordWrap = True
      end
      object Bevel5: TBevel
        Left = 9
        Top = 235
        Width = 271
        Height = 4
        Anchors = [akLeft, akTop, akRight]
        Shape = bsTopLine
      end
      object Bevel4: TBevel
        Left = 9
        Top = 68
        Width = 271
        Height = 4
        Anchors = [akLeft, akTop, akRight]
        Shape = bsTopLine
      end
      object UserTuneTimeSLabel2: TLabel
        Left = 165
        Top = 163
        Width = 116
        Height = 13
        Caption = 's od rozpocz'#281'cia utworu'
        Transparent = True
      end
      object UserTuneTimeSLabel1: TLabel
        Left = 8
        Top = 163
        Width = 107
        Height = 13
        Caption = 'Wysy'#322'aj informacj'#281' po'
        Transparent = True
      end
      object EnableUserTuneNCheckBox: TCheckBox
        Left = 6
        Top = 6
        Width = 280
        Height = 26
        Caption = 'Powiadamiaj o aktualnie s'#322'uchanych utworach moich znajomych'
        TabOrder = 0
        WordWrap = True
        OnClick = EnableUserTuneNCheckBoxClick
      end
      object UserTuneNSpin: TCSpinEdit
        Left = 206
        Top = 40
        Width = 38
        Height = 22
        MaxValue = 30
        MinValue = 1
        TabOrder = 1
        Value = 4
      end
      object EnableUserTuneSCheckBox: TCheckBox
        Left = 6
        Top = 78
        Width = 236
        Height = 26
        Caption = 'Informuj znajomych o aktualnie s'#322'uchanym utworze muzycznym'
        TabOrder = 2
        WordWrap = True
        OnClick = EnableUserTuneSCheckBoxClick
      end
      object UserTuneSSpin: TCSpinEdit
        Left = 121
        Top = 160
        Width = 38
        Height = 22
        MaxValue = 30
        MinValue = 4
        TabOrder = 3
        Value = 5
      end
      object EnableAQQUserTuneSCheckBox: TCheckBox
        Left = 27
        Top = 188
        Width = 235
        Height = 41
        Caption = 
          'Wymuszaj dzia'#322'anie wtyczki z odtwarzaczami obs'#322'ugiwanymi przez A' +
          'QQ oraz z wtyczk'#261' AQQ Radio'
        TabOrder = 4
        WordWrap = True
      end
    end
  end
  object RunPluginCheckBox: TCheckBox
    Left = 14
    Top = 328
    Width = 115
    Height = 17
    Caption = 'Wtyczka aktywna'
    TabOrder = 2
    OnClick = RunPluginCheckBoxClick
  end
  object ActionList: TActionList
    Left = 99
    Top = 320
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
    object aCutSongNumber: TAction
      Category = 'Status'
      Caption = 'aCutSongNumber'
      OnExecute = aCutSongNumberExecute
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
    object aXMPlayDown: TAction
      Category = 'Players'
      Caption = 'aXMPlayDown'
      OnExecute = aXMPlayDownExecute
    end
    object aScreamerRadioDown: TAction
      Category = 'Players'
      Caption = 'aScreamerRadioDown'
      OnExecute = aScreamerRadioDownExecute
    end
    object aCutWWW: TAction
      Category = 'Status'
      Caption = 'aCutWWW'
      OnExecute = aCutWWWExecute
    end
    object aCutWWWUserTune: TAction
      Category = 'Status'
      Caption = 'aCutWWWUserTune'
      OnExecute = aCutWWWUserTuneExecute
    end
    object aaTunesDown: TAction
      Category = 'Players'
      Caption = 'aaTunesDown'
      OnExecute = aaTunesDownExecute
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
    Left = 67
    Top = 320
  end
  object SongTimer: TTimer
    Enabled = False
    Interval = 5000
    OnTimer = SongTimerTimer
    Left = 35
    Top = 320
  end
  object TurnOffTimer: TTimer
    Enabled = False
    Interval = 900000
    OnTimer = TurnOffTimerTimer
    Left = 3
    Top = 320
  end
  object FoobarPopupMenu: TPopupMenu
    Left = 272
    Top = 120
    object FoobarDownload: TMenuItem
      Caption = 'Pobierz wtyczk'#281
      OnClick = FoobarDownloadClick
    end
  end
  object WMPPopupMenu: TPopupMenu
    Left = 272
    Top = 152
    object WMPDownload: TMenuItem
      Caption = 'Pobierz plugin'
      OnClick = WMPDownloadClick
    end
  end
  object iTunesPopupMenu: TPopupMenu
    Left = 272
    Top = 184
    object iTunesDownload: TMenuItem
      Caption = 'Pobierz plugin'
      OnClick = iTunesDownloadClick
    end
  end
  object SongbirdPopupMenu: TPopupMenu
    Left = 272
    Top = 216
    object SongbirdDownload: TMenuItem
      Caption = 'Pobierz rozszerzenie'
      OnClick = SongbirdDownloadClick
    end
  end
  object UserTuneTimer: TTimer
    Enabled = False
    OnTimer = UserTuneTimerTimer
    Left = 67
    Top = 288
  end
  object UserTuneSongTimer: TTimer
    Enabled = False
    Interval = 5000
    OnTimer = UserTuneSongTimerTimer
    Left = 35
    Top = 288
  end
  object AllowUserTuneNTimer: TTimer
    Enabled = False
    Interval = 20000
    OnTimer = AllowUserTuneNTimerTimer
    Left = 3
    Top = 288
  end
  object GetStatusTimer: TTimer
    Enabled = False
    Interval = 100
    OnTimer = GetStatusTimerTimer
    Left = 128
    Top = 320
  end
end
