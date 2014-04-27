//DbWorker.h by Kostya Kozachuck as neurocod
//BSD license https://github.com/neurocod/Qqt
#pragma once
#include "PreparedSqlQuery.h"
class DbTransaction;

class DbWorker: public QObject {
	public:
		DbWorker(QObject*parent = 0);
		virtual ~DbWorker() {}
		void setDB(const QSqlDatabase & db);
		QSqlDatabase db()const;
		bool initSqlOnce();

		void prepareLater(PreparedSqlQuery*q);
		void remove(PreparedSqlQuery*q);
		bool m_bShowErrors;
		StringStatus lastError()const;
	protected:
		friend class DbTransaction;
		QSqlDatabase m_db;
		QList<PreparedSqlQuery*> m_listToPrepare;
		bool m_bQueriesPrepared;
		bool m_bShowErrorsInTracer;//otherwise - in message box

		bool prepareQueriesOnce();
		virtual bool connectToDbOnce() { return m_db.isOpen(); }//reimplement
		bool prepareOrTrace(QSqlQuery & q, const QString & statement);//trace last error if fail
		bool execOrTrace(QSqlQuery & q);
		bool execOrTrace(QSqlQuery & q, const QString & statement);
		void traceLastError(const QSqlDatabase & q);
		void traceLastError(const QSqlQuery & q);
		void maybeShowLastError(const QString & title, const QString & text);
		void maybeShowLastError(const QString & text);

		bool deleteAllRecords(const QString & strTableName);
		virtual void maybeTransaction();
		virtual void maybeCommit();
		virtual void rollback();
		struct Transaction {
			Transaction() { count = 0; isOpen = false; isResolved = false; }
			int count;//recursion count - commit only on top level, rollback at any
			StringStatus status;//ok==active and no rollback
			bool isOpen;
			bool isResolved;//commit or rollback
			bool startedAndOk()const { return isOpen && status.ok(); }
		} m_transaction;
};