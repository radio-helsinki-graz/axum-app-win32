//---------------------------------------------------------------------------

#include <vcl.h>
#include "EthernetAdapterSelectionFrm.h"
#pragma hdrstop
USERES("AxumMeters.res");
USEFORM("PPMMeterFrm.cpp", PPMMeterForm);
USEFORM("EthernetAdapterSelectionFrm.cpp", EthernetAdapterSelectionForm);
USELIB("..\MambanetStackDLL\MambaNetStack.lib");
USELIB("mbn.lib");
//---------------------------------------------------------------------------
extern TEthernetAdapterSelectionForm *EthernetAdapterSelectionForm;

//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
   try
   {
       Application->Initialize();
       Application->Title = "Axum Meters (MambaNet)";

       EthernetAdapterSelectionForm = new TEthernetAdapterSelectionForm(Application);

       Application->CreateForm(__classid(TPPMMeterForm), &PPMMeterForm);
     Application->Run();
   }
   catch (Exception &exception)
   {
       Application->ShowException(&exception);
   }
   return 0;
}
//---------------------------------------------------------------------------
