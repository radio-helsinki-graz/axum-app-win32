//---------------------------------------------------------------------------

#ifndef EthernetCardSelectionFormH
#define EthernetCardSelectionFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TEthernetAdaptorSelectionForm : public TForm
{
__published:	// IDE-managed Components
   TGroupBox *GroupBox1;
   TComboBox *cbNetCardList;
   TButton *About_Btn;
   TButton *Open_Btn;
   TButton *Close_Btn;
private:	// User declarations
public:		// User declarations
   __fastcall TEthernetAdaptorSelectionForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TEthernetAdaptorSelectionForm *EthernetAdaptorSelectionForm;
//---------------------------------------------------------------------------
#endif
