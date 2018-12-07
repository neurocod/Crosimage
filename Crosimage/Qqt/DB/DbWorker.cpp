//DbWorker.cpp by Kostya Kozachuck as neurocod
//BSD license https://github.com/neurocod/Qqt
#include "pch.h"
#include "DbWorker.h"

DbWorker::DbWorker(QObject*parent):
	QObject(parent),
	_showErrors(true),
	_showErrorsInTracer(true)
{
}
DbWorker::~DbWorker() {
	_db.close();
	QString str = _db.connectionName();
	_db = QSqlDatabase();
	if(!str.isEmpty())
		QSqlDatabase::removeDatabase(str);
}
bool DbWorker::transactionStartedAndOk()const {
	return _transaction.startedAndOk();
}
void DbWorker::maybeShowLastError(const QString & title, const QString & text) {
	if(!_showErrors)
		return;
	qDebug() << (title + ' ' + text);
	//msgBox(title, text);
	/*if(_showErrorsInTracer) {
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
WriteStatus DbWorker::prepareQueriesOnce() {
	while(!_setToPrepare.isEmpty()) {
		PreparedSqlQuery* q = *_setToPrepare.begin();
		auto ret = q->prepareOnceOrTrace();
		if(!ret.ok())
			return ret;
	}
	return true;
}
WriteStatus DbWorker::deleteAllRecords(const QString & strTableName) {
	//seems like DELETE FROM name can't be prepared
	PreparedSqlQuery query(*this, QStringLiteral("DELETE FROM %1;").arg(strTableName));
	//query.bindValue(QStringLiteral(":t", strTableName);
	return query.execOrTrace();
}
StringStatus DbWorker::lastError()const {
	auto err = _db.lastError();
	if(err.type()==QSqlError::NoError)
		return true;
	return StringStatus(err.text());
}
void DbWorker::maybeTransaction() {
	if(1==_transaction.count) {
		ASSERT(!_transaction.isOpen);
		_transaction.isOpen = true;
		_transaction.isResolved = false;
		_transaction.setOk();
		if(!_db.transaction()) {
			_transaction.set(lastError());
		}
	} else {
		ASSERT(_transaction.isOpen);
	}
}
StringStatus DbWorker::maybeCommit() {
	if(!_transaction.isOpen || 1!=_transaction.count)
		return true;
	_transaction.isOpen = false;
	_transaction.isResolved = true;
	if(!_db.commit()) {
		_transaction.set(lastError());
		return lastError();
	}
	return true;
}
void DbWorker::rollback() {
	if(!_transaction.isOpen)
		return;
	_transaction.setError(tr("rolled back"));
	_transaction.isOpen = false;
	_transaction.isResolved = true;
	if(!_db.rollback()) {
		_transaction.set(lastError());
	}
}
void DbWorker::close() {
	_db.close();
}
WriteStatus DbWorker::initSqlOnce() {
	auto status = connectToDbOnce();
	if(!status.ok())
		return status;
	return prepareQueriesOnce();
}
void DbWorker::prepareLater(PreparedSqlQuery*q) {
	if(_setToPrepare.contains(q)) {
		ASSERT(0);
		return;
	}
	_setToPrepare << q;
}
void DbWorker::remove(PreparedSqlQuery*q) {
	_setToPrepare.remove(q);
}
const QSqlDatabase & DbWorker::db()const {
	return _db;
}