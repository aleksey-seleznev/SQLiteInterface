#ifndef SQLITEMETHODS_H
#define SQLITEMETHODS_H
#include <string>
#include "sqlite3.h"
#include "SQLiteInterface.h"

namespace SQLiteInterface {
struct SQLiteNode
{
	int id;
	UnicodeString url;
	UnicodeString title;
	int vcount;
};

class SQLite
{
private:
	sqlite3 * Database;
	sqlite3_stmt * pStatement;
	std::string fileName;
	UnicodeString errorMessage;
    bool connectionStatus;
public:
	SQLite();
	~SQLite();
    bool IsOpen();
	void SQLiteClose();
	void SQLiteFinalize();
    wchar_t * GetLastError();
	int SQLiteOpen(AnsiString & databasePath);
	int SQLitePrepare(std::string & Query);
	friend int SQLiteSelect(SQLite &, SQLiteNode *);
	friend int SQLiteExec(std::string & Query, SQLite & SQLiteData);
	friend int SQLiteExec(std::string & Query,
									SQLite & SQLiteData, int & callBackParameter);
};
}
#endif
