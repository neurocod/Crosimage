//PreparedSqlQuery.cpp by Kostya Kozachuck as neurocod
//BSD license https://github.com/neurocod/Qqt
#include "pch.h"
#include "PreparedSqlQuery.h"
#include "DbWorker.h"

PreparedSqlQuery::PreparedSqlQuery(DbWorker*parent, const QString & strQueryToPrepare):
		m_strQueryToPrepare(strQueryToPrepare),
		m_parent(parent) {
	ASSERT(m_parent);
	m_parent->prepareLater(this);
}
void PreparedSqlQuery::setDB(QSqlDatabase db) {
	QSqlQuery & q = *this;
	q = QSqlQuery(db);
}
PreparedSqlQuery::~PreparedSqlQuery() {
	m_parent->remove(this);
}