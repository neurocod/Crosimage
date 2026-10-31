//SqliteDb.h by Kostya Kozachuck as neurocod
//BSD license https://github.com/neurocod/Qqt
#pragma once
#include "DbWorker.h"

class SqliteDb: public DbWorker {
	public:
		SqliteDb(QObject*parent = 0);

		QVariant lastInsertedId();
		bool openDB(const QString & strFile, const QString & strConnection = "", const QString & host = "localhost");
	protected:
		PreparedSqlQuery _queryLastInsertedId;
};