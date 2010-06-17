object EthernetAdaptorSelectionForm: TEthernetAdaptorSelectionForm
  Left = 275
  Top = 140
  Width = 410
  Height = 126
  Caption = 'EthernetAdaptorSelectionForm'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object GroupBox1: TGroupBox
    Left = 8
    Top = 8
    Width = 385
    Height = 73
    Caption = 'MambaNet over Ethernet (Level 2)'
    TabOrder = 0
    object cbNetCardList: TComboBox
      Left = 6
      Top = 16
      Width = 371
      Height = 21
      Style = csDropDownList
      Anchors = [akLeft, akTop, akRight]
      ItemHeight = 13
      Sorted = True
      TabOrder = 0
      Items.Strings = (
        '--- Manager not initialized ---')
    end
    object About_Btn: TButton
      Left = 12
      Top = 42
      Width = 75
      Height = 25
      Caption = 'About ...'
      TabOrder = 1
    end
    object Open_Btn: TButton
      Left = 224
      Top = 42
      Width = 73
      Height = 25
      Caption = 'Open'
      TabOrder = 2
    end
    object Close_Btn: TButton
      Left = 304
      Top = 42
      Width = 73
      Height = 25
      Caption = 'Close'
      Enabled = False
      TabOrder = 3
    end
  end
end
