//DbWorker.cpp by Kostya Kozachuck as neurocod
//BSD license https://github.com/neurocod/Qqt
#include "pch.h"
#include "DbWorker.h"
#include <QSqlError>

DbWorker::DbWorker(QObject*parent):
	QObject(parent),
	m_bShowErrors(true),
	m_bShowErrorsInTracer(true),
	m_bQueriesPrepared(false)
{
}
void DbWorker::setDB(const QSqlDatabase & db) {
	ASSERT(!m_db.isOpen() && db.isOpen());
	m_bQueriesPrepared = false;
	m_db = db;
}
bool DbWorker::execOrTrace(QSqlQuery & q, const QString & statement) {
	bool ret = q.exec(statement);
	if(!ret) {
		traceLastError(q);
	}
	return ret;
}
bool DbWorker::execOrTrace(QSqlQuery & q) {
	bool ret = q.exec();
	if(!ret) {
		traceLastError(q);
	}
	return ret;
}
void DbWorker::maybeShowLastError(const QString & title, const QString & text) {
	if(!m_bShowErrors)
		return;
	qDebug() << (title + " " + text);
	//msgBox(title, text);
	/*if(m_bShowErrorsInTracer) {
		QString str = QString("%1\n%2")
			.arg(title)
			.arg(text);
		reportError(str);
	} else {
		QMessageBox::warning(0, title, text, QMessageBox::Ok, QMessageBox::Ok);
	}*/
}
void DbWorker::maybeShowLastError(const QString & text) {
	maybeShowLastError(QObject::tr("Database error"), text);
}
void DbWorker::traceLastError(const QSqlDatabase & q) {
	maybeShowLastError(q.lastError().text());
}
void DbWorker::traceLastError(const QSqlQuery & q) {
	QString text = QObject::tr("%1\nError query: %2")
		.arg(q.lastError().text())
		.arg(q.lastQuery());
	maybeShowLastError(QObject::tr("Database query error"), text);
}
bool DbWorker::prepareQueriesOnce() {
	if(!m_db.isOpen() || !m_db.isValid()) {
		maybeShowLastError(
			QObject::tr("Trying to prepare queries while database is not open\n"));
		return false;
	}
	if(!m_bQueriesPrepared) {
		for(PreparedSqlQuery* q: m_listToPrepare) {
			q->setDB(m_db);
			if(!prepareOrTrace(*q, q->queryToPrepare())) {
				return false;
			}
		}
		m_bQueriesPrepared = true;
	}
	return m_bQueriesPrepared;
}
bool DbWorker::prepareOrTrace(QSqlQuery & q, const QString & statement) {
	bool ret = q.prepare(statement);
	if(!ret) {
		traceLastError(q);
	}
	return ret;
}
bool DbWorker::deleteAllRecords(const QString & strTableName) {
	//seems like DELETE FROM name can't be prepared
	QSqlQuery query(m_db);
	if(!prepareOrTrace(query,
			QString("DELETE FROM %1;").arg(strTableName)
			)) {
		return false;
	}
	//query.bindValue(":t", strTableName);
	if(!query.exec()) {
		traceLastError(query);
		return false;
	}
	return true;
}
StringStatus DbWorker::lastError()const {
	auto err = m_db.lastError();
	if(err.type()==QSqlError::NoError)
		return true;
	return StringStatus(false, err.text());
}
void DbWorker::maybeTransaction() {
	if(1==m_transaction.count) {
		ASSERT(!m_transaction.isOpen);
		m_transaction.isOpen = true;
		m_transaction.isResolved = false;
		m_transaction.status = StringStatus(true);
		if(!m_db.transaction()) {
			m_transaction.status = lastError();
		}
	} else {
		ASSERT(m_transaction.isOpen);
	}
}
void DbWorker::maybeCommit() {
	if(!m_transaction.isOpen || 1!=m_transaction.count)
		return;
	m_transaction.isOpen = false;
	m_transaction.isResolved = true;
	if(!m_db.commit()) {
		m_transaction.status = lastError();
	}
}
void DbWorker::rollback() {
	if(!m_transaction.isOpen)
		return;
	m_transaction.status = StringStatus(false, tr("rolled back"));
	m_transaction.isOpen = false;
	m_transaction.isResolved = true;
	if(!m_db.rollback()) {
		m_transaction.status = lastError();
	}
}
bool DbWorker::initSqlOnce() {
	if(!connectToDbOnce()) {
		return false;
	}
	return prepareQueriesOnce();
}
void DbWorker::prepareLater(PreparedSqlQuery*q) {
	if(m_listToPrepare.contains(q)) {
		ASSERT(0);
		return;
	}
	m_listToPrepare << q;
}
void DbWorker::remove(PreparedSqlQuery*q) {
	if(1!=m_listToPrepare.removeAll(q)) {
		ASSERT(0);
	}
}
QSqlDatabase DbWorker::db()const {
	return m_db;
}