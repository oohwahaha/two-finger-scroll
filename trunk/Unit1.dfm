object Form1: TForm1
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsDialog
  Caption = 'Settings - TwoFingerScroll'
  ClientHeight = 288
  ClientWidth = 280
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnClose = FormClose
  PixelsPerInch = 96
  TextHeight = 13
  object ok: TButton
    Left = 117
    Top = 255
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    ModalResult = 1
    TabOrder = 0
    OnClick = okClick
  end
  object cancel: TButton
    Left = 198
    Top = 255
    Width = 75
    Height = 25
    Cancel = True
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 1
    OnClick = cancelClick
  end
  object pages: TPageControl
    Left = 8
    Top = 8
    Width = 265
    Height = 241
    ActivePage = generalTab
    TabOrder = 2
    object generalTab: TTabSheet
      Caption = 'General'
      ImageIndex = 2
      object startWithWindows: TCheckBox
        Left = 16
        Top = 16
        Width = 113
        Height = 17
        Caption = 'Start with Windows'
        TabOrder = 0
      end
    end
    object scrollTab: TTabSheet
      Caption = 'Scrolling'
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object scrollLinearAccLabel: TLabel
        Left = 32
        Top = 109
        Width = 86
        Height = 13
        Caption = 'Scroll acceleration'
        Enabled = False
      end
      object scrollCircular: TRadioButton
        Left = 16
        Top = 179
        Width = 97
        Height = 17
        Caption = 'Circular (future)'
        Enabled = False
        TabOrder = 4
        OnClick = scrollLinearClick
      end
      object scrollLinearAcc: TTrackBar
        Left = 32
        Top = 128
        Width = 209
        Height = 37
        Enabled = False
        Max = 100
        Min = 30
        Frequency = 5
        Position = 40
        TabOrder = 3
      end
      object scrollLinearEdge: TCheckBox
        Left = 32
        Top = 78
        Width = 129
        Height = 17
        Caption = 'Keep scrolling on edges'
        Enabled = False
        TabOrder = 2
      end
      object scrollLinear: TRadioButton
        Left = 16
        Top = 47
        Width = 49
        Height = 17
        Caption = 'Linear'
        Checked = True
        TabOrder = 1
        TabStop = True
        OnClick = scrollLinearClick
      end
      object scrollOff: TRadioButton
        Left = 16
        Top = 16
        Width = 33
        Height = 17
        Caption = 'Off'
        TabOrder = 0
        OnClick = scrollLinearClick
      end
    end
    object tapTab: TTabSheet
      Caption = 'Tapping'
      ImageIndex = 1
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object tapFunctionLabel: TLabel
        Left = 32
        Top = 52
        Width = 41
        Height = 13
        Caption = 'Function'
        Enabled = False
      end
      object tapFunction: TComboBox
        Left = 32
        Top = 71
        Width = 121
        Height = 19
        Style = csOwnerDrawFixed
        Enabled = False
        ItemHeight = 13
        ItemIndex = 2
        TabOrder = 1
        Text = 'Right button'
        Items.Strings = (
          'Left button'
          'Middle button'
          'Right button'
          'Button 4'
          'Button 5')
      end
      object tapActive: TCheckBox
        Left = 16
        Top = 16
        Width = 49
        Height = 17
        Caption = 'Active'
        TabOrder = 0
        OnClick = tapActiveClick
      end
    end
  end
  object defaults: TButton
    Left = 8
    Top = 255
    Width = 75
    Height = 25
    Caption = 'Defaults'
    TabOrder = 3
    OnClick = defaultsClick
  end
  object TrayIcon1: TTrayIcon
    Icon.Data = {
      0000010001002020100001000400E80200001600000028000000200000004000
      0000010004000000000080020000000000000000000000000000000000000000
      00008080800080C0FF0000608000FFFFFF00002020004080C000FFC0C000FF00
      FF00FF80A000406060000000A00081808000FFFFFF00FFFF4000818080000000
      0000000000000000000000000000000000000000000111000000000000000000
      0000000042600110000000000000000066002266042660110000000000000002
      2660422660426601100000000000000422660422660426601100000000000000
      4226604226604266011000000000026604226604226604266011000000000426
      6042266022266042660110000000004266042222022222022660110000000004
      2660222220222222226601100000000042660222220222222226601100000000
      0422202222202222222266011000000000422202222222222222266011000000
      0004222022222222222222660110000000004222222222222222222660110000
      0000042222222222222222226601000000000042222222222222222226010000
      0000000222222222222222222607000001111115222226222222222260010000
      1111111152222662222222440030000010000000000222266442440063300000
      0426666666222222266600222630000004222222222222224400222222600000
      0444444444444444004222222000000000000000000000000004222000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000FFFF
      FFFFFFF03FFFF3001FFFE0000FFFC00007FFC00003FF800001FF000000FF0000
      007F8000003FC000001FE000000FF0000007F8000003FC000001FE000000FF00
      0000FF800000FFC00000F8000000F0000000F0000000F0000000F0000000F000
      0001F8000C07FFFFFE1FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF}
    PopupMenu = PopupMenu1
    Visible = True
    OnDblClick = Settings1Click
    Left = 200
  end
  object PopupMenu1: TPopupMenu
    Left = 224
    object Settings1: TMenuItem
      Caption = 'Settings'
      Default = True
      OnClick = Settings1Click
    end
    object globalActive: TMenuItem
      AutoCheck = True
      Caption = 'Active'
      Checked = True
    end
    object N1: TMenuItem
      Caption = '-'
    end
    object About1: TMenuItem
      Caption = 'About'
      OnClick = About1Click
    end
    object Exit1: TMenuItem
      Caption = 'Exit'
      OnClick = Exit1Click
    end
  end
  object reactivateTimer: TTimer
    Enabled = False
    Interval = 500
    OnTimer = reactivateTimerTimer
    Left = 248
  end
end
