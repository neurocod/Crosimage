//ImgDbWorker.h by Kostya Kozachuck as neurocod
#pragma once
#include "Qqt/DB/SqliteDb.h"

class ImgDbWorker: public SqliteDb {
	public:
		virtual ~ImgDbWorker() {}
		static ReadStatus thumbnail(const QFileInfo & file, OUT QImage & img);
		static WriteStatus setThumbnail(const QFileInfo & file, const QImage & img);
		static const QString dbFileName;
	protected:
		static ImgDbWorker* instance(const QDir & dir);
		static ImgDbWorker* instance(const QFileInfo & file);
		ImgDbWorker(const QDir & dir);

		PreparedSqlQuery _qThumbGet;
		PreparedSqlQuery _qThumbGetAll;
		PreparedSqlQuery _qThumbSet;
		QString _dbPath;
		virtual ReadStatus connectToDbOnce()override;
		bool maybeInstallDb();
		ReadStatus thumbnail_(const QFileInfo & file, OUT QImage & img);
		WriteStatus setThumbnail_(const QFileInfo & file, const QImage & img);
		ReadStatus readAll();
};