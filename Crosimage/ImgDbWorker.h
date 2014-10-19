//ImgDbWorker.h by Kostya Kozachuck as neurocod
#pragma once
#include "Qqt/DB/SqliteDb.h"

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

		PreparedSqlQuery _qThumbGet;
		PreparedSqlQuery _qThumbGetAll;
		PreparedSqlQuery _qThumbSet;
		QString _dbPath;
		virtual bool connectToDbOnce()override;
		bool maybeInstallDb();
		bool _thumbnail(const QFileInfo & file, OUT QImage & img);
		bool _setThumbnail(const QFileInfo & file, const QImage & img);
		bool readAll();
};