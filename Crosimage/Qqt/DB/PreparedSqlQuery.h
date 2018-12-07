//PreparedSqlQuery.h by Kostya Kozachuck as neurocod
//BSD license https://github.com/neurocod/Qqt
#pragma once
#include <QSqlQuery>
#include <QSqlError>
class DbWorker;

namespace Qqt {
class PreparedSqlQuery: public QSqlQuery {
	MAYBE_SUPER(QSqlQuery)
	public:
		PreparedSqlQuery(DbWorker& parent, const QString & queryToPrepare);
		virtual ~PreparedSqlQuery();

		static QAtomicInt s_totalExecs;//for debug
		StringStatus exec();
		StringStatus prepareOnce();
		StringStatus execOrTrace();
		WriteStatus prepareOnceOrTrace();
		const QString _strQueryToPrepare;
	protected:
		DbWorker& _parent;
		bool _isPrepared = false;

		StringStatus execNoTimer();
};
class AbstractSqlTable {
	public:
		virtual StringStatus maybeCreateTable(const QStringList& exising) = 0;
};
}//Qqt