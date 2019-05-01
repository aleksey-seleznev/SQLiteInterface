//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "SQLiteInterface.h"
#include "SQLiteMethods.h"
#include "sqlite3.h"
#include <vector>
#include <algorithm>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "VirtualTrees"
#pragma resource "*.dfm"
TMainTree *MainTree;
using namespace SQLiteInterface;
SQLite dbHist;
//---------------------------------------------------------------------------
__fastcall TMainTree::TMainTree(TComponent* Owner)
	: TForm(Owner)
{

}
//---------------------------------------------------------------------------


void __fastcall TMainTree::OnGetText(TBaseVirtualTree *Sender, PVirtualNode Node,
		  TColumnIndex Column, TVSTTextType TextType, UnicodeString &CellText)
{
	if(!Node)	return;
	SQLiteNode *nodeData = (SQLiteNode *)Sender->GetNodeData(Node);
	switch(Column)
	{
		case 0: CellText = nodeData->id;
				break;
		case 1: CellText = nodeData->url;
				break;
		case 2: CellText = nodeData->title;
				break;
		case 3: CellText = nodeData->vcount;
				break;
	}
}
//---------------------------------------------------------------------------


void __fastcall TMainTree::DeleteRowsClick(TObject *Sender)
{
	if (!dbHist.IsOpen()) {
		Application->MessageBox(L"Specify path to history database.", L"Warning", MB_OK);
		return;
	}
	if (!VirtualStringTree->SelectedCount) {
        Application->MessageBox(L"Select rows to delete.", L"Warning", MB_OK);
		return;
	}
	std::string transaction;
	std::string insertQuery = "insert into temp (id) values ();";
	std::size_t shift = insertQuery.find(");");
	std::string createTableQuery = "create table if not exists temp (id int);";
	std::string clearQuery = "delete from temp;";
	std::vector<int> indexVector;
	if(SQLiteExec(createTableQuery, dbHist)){
		Application->MessageBox(dbHist.GetLastError(), L"Warning", MB_OK);
		return;
    }
	PVirtualNode selectedNode = VirtualStringTree->GetFirstSelected();
	SQLiteNode *nodeData = (SQLiteNode *)VirtualStringTree->GetNodeData(selectedNode);
	indexVector.push_back(nodeData->id);
	SQLiteExec(clearQuery, dbHist);
	for (int i = 0; i < VirtualStringTree->SelectedCount - 1; i++) {
		selectedNode = VirtualStringTree->GetNextSelected(selectedNode);
		SQLiteNode *nodeData = (SQLiteNode *)VirtualStringTree->GetNodeData(selectedNode);
		indexVector.push_back(nodeData->id);
	}
	VirtualStringTree->DeleteSelectedNodes();
	transaction = "BEGIN TRANSACTION";
	SQLiteExec(transaction, dbHist);
	for (std::vector<int>::iterator it = indexVector.begin();
												it != indexVector.end(); ++it){
		std::string subString = std::to_string(*it);
		std::string tempQuery = insertQuery;
		tempQuery.insert(shift, subString);
		SQLiteExec(tempQuery, dbHist);
	}
    transaction = "END TRANSACTION";
	SQLiteExec(transaction, dbHist);
	std::string deleteQuery = "delete from urls \
							   where id in (select id from temp);";
	if (SQLiteExec(deleteQuery, dbHist)){
		Application->MessageBox(dbHist.GetLastError(), L"Warning", MB_OK);
		return;
	}

}
//---------------------------------------------------------------------------


void __fastcall TMainTree::AddRowClick(TObject *Sender)
{
	if (!dbHist.IsOpen()) {
		Application->MessageBox(L"Specify path to history database.", L"Warning", MB_OK);
		return;
	}
	std::string maxQuery = "select max(id) from urls;";
	int maxId = 0;
	SQLiteExec(maxQuery, dbHist, maxId);
	PVirtualNode createdNode = VirtualStringTree->AddChild(VirtualStringTree->RootNode);
	SQLiteNode *nodeData = (SQLiteNode *)VirtualStringTree->GetNodeData(createdNode);
	nodeData->id = ++maxId;
	VirtualStringTree->FocusedNode = createdNode;
	VirtualStringTree->Selected[createdNode] = true;
	VirtualStringTree->EditNode(createdNode, 1);
	std::string createQuery = "insert into urls (id) values ();";
	std::size_t shift = createQuery.find(");");
	createQuery.insert(shift, std::to_string(maxId));
	if (SQLiteExec(createQuery, dbHist)){
		Application->MessageBox(dbHist.GetLastError(), L"Warning", MB_OK);
		return;
	}

}
//---------------------------------------------------------------------------


void __fastcall TMainTree::OnEdited(TBaseVirtualTree *Sender, PVirtualNode Node, TColumnIndex Column)
{
	SQLiteNode *nodeData = (SQLiteNode *)Sender->GetNodeData(Node);
	std::string insertQuery = "update urls set = (\"\") where id = ;";
	std::size_t shift = insertQuery.find(";");
	insertQuery.insert(shift, std::to_string(nodeData->id));
	AnsiString data;
	std::string subString = "";
	shift = insertQuery.find("=");
	switch(Column)
	{
		case 1:	data = nodeData->url.c_str();
				insertQuery.insert(shift, "url");
				break;
		case 2: data = nodeData->title.c_str();
				insertQuery.insert(shift, "title");
				break;
		case 3: data = nodeData->vcount;
				insertQuery.insert(shift, "visit_count");
				break;
	}
	shift = insertQuery.find("\")");
	insertQuery.insert(shift, data.c_str());
	if (SQLiteExec(insertQuery, dbHist)){
		Application->MessageBox(dbHist.GetLastError(), L"Warning", MB_OK);
		return;
	}
}
//---------------------------------------------------------------------------


void __fastcall TMainTree::OnNewText(TBaseVirtualTree *Sender, PVirtualNode Node,
          TColumnIndex Column, UnicodeString NewText)
{
	SQLiteNode *nodeData = (SQLiteNode *) VirtualStringTree->GetNodeData(Node);
	switch(Column)
	{
		case 1: nodeData->url = NewText;
				break;
		case 2: nodeData->title = NewText;
				break;
		case 3: nodeData->vcount = NewText.ToInt();
				break;
	}
}
//---------------------------------------------------------------------------


void __fastcall TMainTree::ClearTableClick(TObject *Sender)
{
	if (!dbHist.IsOpen()) {
		Application->MessageBox(L"Specify path to history database.", L"Warning", MB_OK);
		return;
	}
	std::string clearQuery = "delete from urls;";
	SQLiteExec(clearQuery, dbHist);
    VirtualStringTree->Clear();
}
//---------------------------------------------------------------------------


void __fastcall TMainTree::BrowsePathClick(TObject *Sender)
{
	AnsiString databasePath;
	if(OpenFileDialog->Execute()){
		databasePath = OpenFileDialog->FileName;
		if (databasePath.IsEmpty()) {
			Application->MessageBox(L"Specify path to history database.", L"Warning", MB_OK);
			return;
		}
		if (dbHist.IsOpen){
			dbHist.SQLiteFinalize();
			dbHist.SQLiteClose();
		}

		if(dbHist.SQLiteOpen(databasePath)){
			Application->MessageBox(dbHist.GetLastError(), L"Warning", MB_OK);
			return;
		}
		UpdateTableClick(Sender);
	}
    Path->Text = databasePath;
}
//---------------------------------------------------------------------------


void __fastcall TMainTree::UpdateTableClick(TObject *Sender)
{
	if (!dbHist.IsOpen()) {
		Application->MessageBox(L"Specify path to history database.", L"Warning", MB_OK);
		return;
	}
	std::string selectQuery = "select * from urls;";
	std::string countQuery = "select count(*) from urls;";
	dbHist.SQLitePrepare(selectQuery);
	VirtualStringTree->Clear();
	VirtualStringTree->NodeDataSize = sizeof(SQLiteNode);
	VirtualStringTree->BeginUpdate();
	int numberOfRows = 0;
	SQLiteExec(countQuery, dbHist, numberOfRows);

	for (int i = 0; i < numberOfRows; i++) {
		PVirtualNode entryNode = VirtualStringTree->AddChild(VirtualStringTree->RootNode);
		SQLiteNode *nodeData = (SQLiteNode*) VirtualStringTree->GetNodeData(entryNode);
		SQLiteSelect(dbHist, nodeData);
	}
	VirtualStringTree->EndUpdate();
}
//---------------------------------------------------------------------------


