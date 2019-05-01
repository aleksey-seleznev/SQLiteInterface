//---------------------------------------------------------------------------

#ifndef SQLiteInterfaceH
#define SQLiteInterfaceH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "SQLiteMethods.h"
#include "VirtualTrees.hpp"
#include <Vcl.Dialogs.hpp>
//---------------------------------------------------------------------------
class TMainTree : public TForm
{
__published:	// IDE-managed Components
	TVirtualStringTree *VirtualStringTree;
	TButton *DeleteRows;
	TButton *AddRow;
	TButton *ClearTable;
	TFileOpenDialog *OpenFileDialog;
	TButton *BrowsePath;
	TButton *UpdateTable;
	TLabel *Label;
	TEdit *Path;
	void __fastcall OnGetText(TBaseVirtualTree *Sender, PVirtualNode Node, TColumnIndex Column,
          TVSTTextType TextType, UnicodeString &CellText);
	void __fastcall DeleteRowsClick(TObject *Sender);
	void __fastcall AddRowClick(TObject *Sender);
	void __fastcall OnEdited(TBaseVirtualTree *Sender, PVirtualNode Node, TColumnIndex Column);
	void __fastcall OnNewText(TBaseVirtualTree *Sender, PVirtualNode Node, TColumnIndex Column,
		  UnicodeString NewText);
	void __fastcall ClearTableClick(TObject *Sender);
	void __fastcall BrowsePathClick(TObject *Sender);
	void __fastcall UpdateTableClick(TObject *Sender);




private:	// User declarations
public:		// User declarations
	__fastcall TMainTree(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TMainTree *MainTree;
//---------------------------------------------------------------------------
#endif
