//DbWorker.h by Kostya Kozachuck as neurocod
//BSD license https://github.com/neurocod/Qqt
#pragma once
#include "PreparedSqlQuery.h"
class DbTransaction;

class DbWorker: public QObject {
	public:
		DbWorker(QObject*parent = 0);
		~DbWorker();
		const QSqlDatabase & db()const;
		WriteStatus initSqlOnce();
		void close();

		using PreparedSqlQuery = Qqt::PreparedSqlQuery;
		void prepareLater(PreparedSqlQuery*q);
		void remove(PreparedSqlQuery*q);
		bool _showErrors;
		StringStatus lastError()const;
		void traceLastError(const QSqlDatabase & q);
		void traceLastError(const QSqlQuery & q);
		void maybeShowLastError(const QString & title, const QString & text);
		void maybeShowLastError(const QString & text);
		bool transactionStartedAndOk()const;
	protected:
		friend class DbTransaction;
		QSqlDatabase _db;
		QSet<PreparedSqlQuery*> _setToPrepare;
		bool _showErrorsInTracer;//otherwise - in message box

		WriteStatus prepareQueriesOnce();
		virtual WriteStatus connectToDbOnce() { return _db.isOpen(); }//reimplement

		WriteStatus deleteAllRecords(const QString & strTableName);
		virtual void maybeTransaction();
		virtual StringStatus maybeCommit();
		virtual void rollback();
		struct Transaction: public StringStatus {//ok==active and no rollback
			int count = 0;//recursion count - commit only on top level, rollback at any
			bool isOpen = false;
			bool isResolved = false;//commited or rollbacked
			bool startedAndOk()const { return isOpen && ok(); }
		} _transaction;
};