//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "PPMMeterFrm.h"
#include "EthernetAdapterSelectionFrm.h"
#define MBN_VARARG
#include "mbn.h"
#include <stdio.h>
#include <dos.h>


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "AnalogClock"
#pragma link "MeterPanel"
#pragma link "GradientFill"
#pragma link "ExtendedPanel"
#pragma link "VUMeter"
#pragma link "HNPSManager"
#pragma link "HNAdapter"
#pragma link "HNQueue"
#pragma link "HNAdapterConfig"
#pragma link "HNPacket"
#pragma link "HNUserFilter"
#pragma link "BitmapButton"
#pragma resource "*.dfm"

TPPMMeterForm *PPMMeterForm;

#define PCB_MAJOR_VERSION        1
#define PCB_MINOR_VERSION        0

#define FIRMWARE_MAJOR_VERSION   1
#define FIRMWARE_MINOR_VERSION   0

#define MANUFACTURER_ID          1      //D&R
#define PRODUCT_ID               21     //Axum-PPM-Meter

struct mbn_node_info thisnode = {
  0, 0,
  "Axum PPM Meters DSP (Windows)", "Axum-PPM-Meter",
  MANUFACTURER_ID, PRODUCT_ID, 0,
  0, 0,
  FIRMWARE_MAJOR_VERSION, FIRMWARE_MINOR_VERSION,
};

void mError(struct mbn_handler *mbn, int code, char *msg);
void mOnlineStatus(struct mbn_handler *mbn, unsigned long addr, char valid);
int mSetActuatorData(struct mbn_handler *mbn, unsigned short object, union mbn_data data);

//---------------------------------------------------------------------------
__fastcall TPPMMeterForm::TPPMMeterForm(TComponent* Owner)
   : TForm(Owner)
{
  MeterData[0] = -50;
  MeterData[1] = -50;
  MeterData[2] = -50;
  MeterData[3] = -50;
  MambaNetAddress = 0x00000000;
  Valid = 0;
}
//---------------------------------------------------------------------------


void __fastcall TPPMMeterForm::FormShow(TObject *Sender)
{
  char err[MBN_ERRSIZE], ifdesc[512];
  struct mbn_if_ethernet *n;
  struct mbn_interface *itf;
  struct mbn_object objects[200];

  objects[0] = MBN_OBJ("Meter 1 Left dB",
                       MBN_DATATYPE_NODATA,
                       MBN_DATATYPE_FLOAT, 2, -50.0, 10.0, 0.0, 0.0);
  objects[1] = MBN_OBJ("Meter 1 Right dB",
                       MBN_DATATYPE_NODATA,
                       MBN_DATATYPE_FLOAT, 2, -50.0, 10.0, 0.0, 0.0);
  objects[2] = MBN_OBJ("Meter 2 Left dB",
                       MBN_DATATYPE_NODATA,
                       MBN_DATATYPE_FLOAT, 2, -50.0, 10.0, 0.0, 0.0);
  objects[3] = MBN_OBJ("Meter 2 Right dB",
                       MBN_DATATYPE_NODATA,
                       MBN_DATATYPE_FLOAT, 2, -50.0, 10.0, 0.0, 0.0);
  objects[4] = MBN_OBJ("Display 1",
                       MBN_DATATYPE_NODATA,
                       MBN_DATATYPE_OCTETS, 8, 0, 127, 0, "");
  objects[5] = MBN_OBJ("Float edit",
                       MBN_DATATYPE_NODATA,
                       MBN_DATATYPE_FLOAT, 2, -140.0, 10.0, -140.0, -140.0);

  for (int cntSwitch=0; cntSwitch<70; cntSwitch++)
  {
    char desc[32];
    sprintf(desc,"Switch %d", cntSwitch+1);

    objects[6+cntSwitch] = MBN_OBJ(desc,
                           MBN_DATATYPE_STATE, 1, 1, 0, 1, 0,
                           MBN_DATATYPE_STATE, 1, 0, 1, 0, 0);
  }

  thisnode.NumberOfObjects = 76;

  iflist = mbnEthernetIFList(err);

  for (n=iflist; n!=NULL; n=n->next)
  {
    EthernetAdapterSelectionForm->AdapterComboBox->Items->AddObject(n->desc, (TObject *)n->name);
  }

  if (EthernetAdapterSelectionForm->ShowModal() ==  mrOk)
  {
    thisnode.UniqueIDPerProduct = EthernetAdapterSelectionForm->CSpinEdit1->Value;

    if (EthernetAdapterSelectionForm->EthernetRadioButton->Checked)
    {
      if(EthernetAdapterSelectionForm->AdapterComboBox->ItemIndex < 0)
        return;
      if((itf = mbnEthernetOpen((char *)EthernetAdapterSelectionForm->AdapterComboBox->Items->Objects[EthernetAdapterSelectionForm->AdapterComboBox->ItemIndex], err)) == NULL)
        return;
    }
    else
    {  //TCPIP
      if((itf = mbnTCPOpen(EthernetAdapterSelectionForm->URLEdit->Text.c_str(), NULL, NULL, NULL, err)) == NULL)
        return;
    }

    mbn = mbnInit(&thisnode, objects, itf, err);

    mbnSetErrorCallback(mbn, mError);
    mbnSetOnlineStatusCallback(mbn, mOnlineStatus);
    mbnSetSetActuatorDataCallback(mbn, mSetActuatorData);

    mbnStartInterface(itf, err);
  }
}

void __fastcall TPPMMeterForm::MeterReleaseTimerTimer(TObject *Sender)
{
   if (BigLeftMeterPanel1->dBPosition>-50)
   {
      if (MeterData[0] < BigLeftMeterPanel1->dBPosition)
      {
         BigLeftMeterPanel1->dBPosition-=0.25;
      }
   }

   if (BigRightMeterPanel1->dBPosition>-50)
   {
      if (MeterData[1] < BigRightMeterPanel1->dBPosition)
      {
         BigRightMeterPanel1->dBPosition-=0.25;
      }
   }

   if (BigLeftMeterPanel2->dBPosition>-50)
   {
      if (MeterData[2] < BigLeftMeterPanel2->dBPosition)
      {
         BigLeftMeterPanel2->dBPosition-=0.25;
      }
   }

   if (BigRightMeterPanel2->dBPosition>-50)
   {
      if (MeterData[3] < BigRightMeterPanel2->dBPosition)
      {
         BigRightMeterPanel2->dBPosition-=0.25;
      }
   }
}
//---------------------------------------------------------------------------

void __fastcall TPPMMeterForm::Timer2Timer(TObject *Sender)
{
  struct time T;
  gettime(&T);

  const AnsiString DateFormat = "d mmm yyyy";
  const AnsiString DayFormat = "ddd";
  const AnsiString TimeFormat = "hh:mm:ss am/pm";

  BigAnalogClock->Seconds = T.ti_sec;
  BigAnalogClock->Minutes = T.ti_min;
  BigAnalogClock->Hours = T.ti_hour;

  struct date D;
  getdate(&D);

  TDateTime Date;
  Date = EncodeTime(T.ti_hour, T.ti_min, T.ti_sec, 0);
  AnsiString Time = Date.FormatString(TimeFormat);

  Date = EncodeDate(D.da_year, D.da_mon, D.da_day);
  BigAnalogClock->TimeString = Date.FormatString(DayFormat) + " " + Time;
  BigAnalogClock->DateString = Date.FormatString(DateFormat);
  BigAnalogClock->PaintControl();
}
//---------------------------------------------------------------------------


void __fastcall TPPMMeterForm::SwitchMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
   if ((Shift.Contains(ssLeft)) && (Valid))
   {
      int ObjectNr = 1030+((TLabel *)Sender)->Caption.ToInt()-1;
      union mbn_data data;

      data.State = 1;
      mbnUpdateSensorData(mbn, ObjectNr, data);
   }
}
//---------------------------------------------------------------------------

void __fastcall TPPMMeterForm::SwitchMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
   if ((Button == Controls::mbLeft) && (Valid))
   {
      int ObjectNr = 1030+((TLabel *)Sender)->Caption.ToInt()-1;
      union mbn_data data;

      data.State = 0;
      mbnUpdateSensorData(mbn, ObjectNr, data);
   }
}
//---------------------------------------------------------------------------

void __fastcall TPPMMeterForm::Edit1Change(TObject *Sender)
{
  int ObjectNr = 1029;
  float Value = Edit1->Text.ToDouble();
  union mbn_data data;

  if (Valid)
  {
    data.Float = Value;
    mbnUpdateSensorData(mbn, ObjectNr, data);
  }
}
//---------------------------------------------------------------------------

void mError(struct mbn_handler *mbn, int code, char *msg) {
}

void mOnlineStatus(struct mbn_handler *mbn, unsigned long addr, char valid) {
  PPMMeterForm->MambaNetAddress = addr;
  PPMMeterForm->Valid = valid;
}

int mSetActuatorData(struct mbn_handler *mbn, unsigned short object, union mbn_data data) {
  if ((object>=1030) && (object<1030+70))
  {
    AnsiString ControlName = ((AnsiString)"Switch") + ((AnsiString)(object-1029));
    TLabel *SendLabel = (TLabel *)PPMMeterForm->FindComponent(ControlName);

    if (data.State)
    {
      SendLabel->Color = 0x008080FF;
    }
    else
    {
      SendLabel->Color = 0x00E0E0E0;
    }
  }
  else
  {
    switch (object)
    {
      case 1024:
      {
        float dB = data.Float+20;

        PPMMeterForm->MeterData[0] = dB;
        if (dB>PPMMeterForm->BigLeftMeterPanel1->dBPosition)
        {
          PPMMeterForm->BigLeftMeterPanel1->dBPosition = dB;
        }
      }
      break;
      case 1025:
      {
        float dB = data.Float+20;

        PPMMeterForm->MeterData[1] = dB;
        if (dB>PPMMeterForm->BigRightMeterPanel1->dBPosition)
        {
          PPMMeterForm->BigRightMeterPanel1->dBPosition = dB;
        }
      }
      break;
      case 1026:
      {
        float dB = data.Float+20;

        PPMMeterForm->MeterData[2] = dB;
        if (dB>PPMMeterForm->BigLeftMeterPanel2->dBPosition)
        {
          PPMMeterForm->BigLeftMeterPanel2->dBPosition = dB;
        }
      }
      break;
      case 1027:
      {
        float dB = data.Float+20;

        PPMMeterForm->MeterData[3] = dB;
        if (dB>PPMMeterForm->BigRightMeterPanel2->dBPosition)
        {
          PPMMeterForm->BigRightMeterPanel2->dBPosition = dB;
        }
      }
      break;
      case 1028:
      {
        PPMMeterForm->Label1->Caption = (char *)data.Octets;
      }
      break;
      case 1029:
      {
        char TempString[32];
        sprintf(TempString, "%5.1f", data.Float);
        PPMMeterForm->Edit1->Text = TempString;
      }
      break;
    }
  }
  mbnUpdateActuatorData(mbn, object, data);

  return 0;
}


void __fastcall TPPMMeterForm::FormClose(TObject *Sender,
      TCloseAction &Action)
{
  mbnFree(mbn);

  Action = caFree;
}
//---------------------------------------------------------------------------

