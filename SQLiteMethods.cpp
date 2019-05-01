#include "SQLiteMethods.h"
using namespace SQLiteInterface;

int CallBack(void *value, int columnsCount,
								  char **rowData, char **columnsName)
{
	int *result = (int*) value;
	if (columnsCount == 1){
		if(*rowData != nullptr)
			*result = atoi(*rowData);
		else
			*result = 0;
    }
	else
		*result = columnsCount;
	return 0;
}


SQLite::SQLite()
{
	 fileName = "Specify path to database file.";
	 errorMessage = "NULL";
	 Database = nullptr;
	 pStatement = nullptr;
     connectionStatus = false;
}


SQLite::~SQLite()
{
	this->SQLiteFinalize();
	this->SQLiteClose();
}

bool SQLite::IsOpen()
{
    return this->connectionStatus;
}

void SQLite::SQLiteClose()
{
	sqlite3_close(this->Database);
}


void SQLite::SQLiteFinalize()
{
	sqlite3_finalize(this->pStatement);
}


wchar_t * SQLite::GetLastError()
{
	return this->errorMessage.c_str();
}


int SQLite::SQLiteOpen(AnsiString & databasePath)
{
	char filename[MAX_PATH];
	strcpy(filename,databasePath.c_str());
	const char * fileName = filename;
	if (int rc = sqlite3_open_v2(fileName, &Database,
							SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE ,NULL)){
		this->errorMessage =  "Error occured while opening database!";
		return 1;
	}
	else {
		connectionStatus = true;
		return 0;
	}

}


int SQLite::SQLitePrepare(std::string & Query)
{
	const char * selectQuery = Query.c_str();
	int queryResult = sqlite3_prepare_v2(this->Database ,selectQuery,
										 -1, &this->pStatement, nullptr);
	if(queryResult  != SQLITE_OK) {
		this->errorMessage = "Error in SQL syntax!";
		this->SQLiteClose();
        this->SQLiteFinalize();
		return 1;
	}
	else
		return 0;

}


int SQLiteInterface::SQLiteSelect(SQLite & SQLiteData, SQLiteNode * Node)
{
	int queryResult = sqlite3_step(SQLiteData.pStatement);
	if (queryResult == SQLITE_ROW) {
		Node->id = sqlite3_column_int(SQLiteData.pStatement, 0);
		Node->url = (UnicodeString)(char*)sqlite3_column_text(SQLiteData.pStatement, 1);
		Node->title = (UnicodeString)(char*)sqlite3_column_text(SQLiteData.pStatement, 2);
		Node->vcount = sqlite3_column_int(SQLiteData.pStatement, 3);
		return 0;
	}
	else
        return 1;
}


int SQLiteInterface::SQLiteExec(std::string & Query, SQLite & SQLiteData)
{
	const char * executeQuery = Query.c_str();
	char * errmsg;
	int queryResult = sqlite3_exec(SQLiteData.Database, executeQuery,
															nullptr, nullptr, &errmsg);
	if (queryResult == SQLITE_OK)
		return 0;
	else {
		SQLiteData.errorMessage = errmsg;
		return 1;
	}
}


int SQLiteInterface::SQLiteExec(std::string & Query,
										SQLite & SQLiteData, int & callBackParameter)
{
	const char * executeQuery = Query.c_str();
	char * errmsg;
	int queryResult = sqlite3_exec(SQLiteData.Database, executeQuery,
												CallBack, &callBackParameter, &errmsg);
	if (queryResult == SQLITE_OK)
		return 0;
	else {
		SQLiteData.errorMessage = errmsg;
		return 1;
	}
}


