//ImgDbWorker.h by Kostya Kozachuck as neurocod - 2013.10.29 16:07:06
#pragma once
#include "../../../qtest/SqliteDb.h"

class ImgDbWorker: public SqliteDb {
	public:
		virtual ~ImgDbWorker() {}
		static bool thumbnail(const QFileInfo & file, OUT QImage & img);
		static bool setThumbnail(const QFileInfo & file, const QImage & img);
		static const QString dbFileName;
	protected:
		static ImgDbWorker* instance(const QDir & dir);
		static ImgDbWorker* instance(const QFileInfo & file);
		ImgDbWorker(const QDir & dir);

		PreparedSqlQuery m_qThumbGet;
		PreparedSqlQuery m_qThumbGetAll;
		PreparedSqlQuery m_qThumbSet;
		QString m_dbPath;
		virtual bool connectToDbOnce()override;
		bool maybeInstallDb();
		bool _thumbnail(const QFileInfo & file, OUT QImage & img);
		bool _setThumbnail(const QFileInfo & file, const QImage & img);
		bool readAll();
};