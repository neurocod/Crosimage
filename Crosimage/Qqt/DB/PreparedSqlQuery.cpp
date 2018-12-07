//PreparedSqlQuery.cpp by Kostya Kozachuck as neurocod
//BSD license https://github.com/neurocod/Qqt
#include "pch.h"
#include "PreparedSqlQuery.h"
#include "DbWorker.h"

using namespace Qqt;
QAtomicInt PreparedSqlQuery::s_totalExecs;
PreparedSqlQuery::PreparedSqlQuery(DbWorker& parent, const QString & queryToPrepare):
	_strQueryToPrepare(queryToPrepare),
	_parent(parent)
{
	_parent.prepareLater(this);
}
PreparedSqlQuery::~PreparedSqlQuery() {
	_parent.remove(this);
}
StringStatus PreparedSqlQuery::exec() {
	++s_totalExecs;
	auto ret = prepareOnce();
	if(!ret.ok())
		return ret;
	QElapsedTimer timer;
	timer.start();
	ret = execNoTimer();
	auto ms = timer.elapsed();
	if(ms > 1000) {
		//qDebug() << QString("very long SQL query (%1 ms) is %2").arg(ms).arg(executedQuery()) << "\n\n";
	}
	return ret;
}
StringStatus PreparedSqlQuery::execNoTimer() {
	if(__super::exec())
		return true;
	return StringStatus(lastError().text());
}
StringStatus PreparedSqlQuery::execOrTrace() {
	auto ret = exec();
	if(!ret.ok())
		_parent.traceLastError(*this);
	return ret;
}
WriteStatus PreparedSqlQuery::prepareOnceOrTrace() {
	auto ret = prepareOnce();
	if(!ret.ok())
		_parent.traceLastError(*this);
	return ret;
}
StringStatus PreparedSqlQuery::prepareOnce() {
	if(_isPrepared)
		return true;
	const QSqlDatabase & db = _parent.db();
	if(!db.isOpen() || !db.isValid()) {
		return StringStatus(QObject::tr("Trying to prepare query while database is not open\n"));
	}
	QSqlQuery & q = *this;
	q = QSqlQuery(db);
	if(__super::prepare(_strQueryToPrepare)) {
		_isPrepared = true;
		_parent.remove(this);
		return true;
	}
	return WriteStatus(QObject::tr("Can't prepare statement %1: %2").arg(_strQueryToPrepare, lastError().text()));
}