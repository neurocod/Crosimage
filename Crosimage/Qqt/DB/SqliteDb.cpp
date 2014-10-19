//SqliteDb.cpp by Kostya Kozachuck as neurocod
//BSD license https://github.com/neurocod/Qqt
#include "pch.h"
#include "SqliteDb.h"

SqliteDb::SqliteDb(QObject*parent): DbWorker(parent), _queryLastInsertedId(this, "SELECT last_insert_rowid()") {
}
//static
QVariant SqliteDb::toVariantByteArray(const QDateTime & dt) {
	QByteArray arr;
	{
		QBuffer buf(&arr);
		buf.open(QIODevice::WriteOnly);
		QDataStream stream(&buf);
		stream << dt;
	}
	return QVariant(arr);
}
//static
QDateTime SqliteDb::dateTimeFromVariant(const QVariant & v) {
	ASSERT(0);//not tested
	QDateTime dt;
	if(v.canConvert(QVariant::ByteArray)) {
		QByteArray arr = v.toByteArray();
		QBuffer buf(&arr);
		buf.open(QIODevice::ReadOnly);
		QDataStream stream(&buf);
		stream >> dt;
	}
	return dt;
}
QVariant SqliteDb::lastInsertedId() {
	if(!execOrTrace(_queryLastInsertedId)) {
		return QVariant();
	}
	if(!_queryLastInsertedId.next()) {
		traceLastError(_queryLastInsertedId);
		return QVariant();
	}
	QVariant id = _queryLastInsertedId.value(0);
	return id;
}
bool SqliteDb::openDB(const QString & strFile, const QString & strConnection, const QString & host) {
	_db = QSqlDatabase::addDatabase("QSQLITE", strConnection);
	_db.setHostName(host);
	_db.setDatabaseName(strFile);
	if (!_db.open()) {
		traceLastError(_db);
		return false;
	}
	return true;
}