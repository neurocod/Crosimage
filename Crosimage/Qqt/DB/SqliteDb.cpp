//SqliteDb.cpp by Kostya Kozachuck as neurocod
//BSD license https://github.com/neurocod/Qqt
#include "pch.h"
#include "SqliteDb.h"

SqliteDb::SqliteDb(QObject*parent): DbWorker(parent), m_queryLastInsertedId(this, "SELECT last_insert_rowid()") {
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
	if(!execOrTrace(m_queryLastInsertedId)) {
		return QVariant();
	}
	if(!m_queryLastInsertedId.next()) {
		traceLastError(m_queryLastInsertedId);
		return QVariant();
	}
	QVariant id = m_queryLastInsertedId.value(0);
	return id;
}
bool SqliteDb::openDB(const QString & strFile, const QString & strConnection, const QString & host) {
	m_db = QSqlDatabase::addDatabase("QSQLITE", strConnection);
	m_db.setHostName(host);
	m_db.setDatabaseName(strFile);
	if (!m_db.open()) {
		traceLastError(m_db);
		return false;
	}
	return true;
}