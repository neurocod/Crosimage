//DirDb.h by Kostya Kozachuck as neurocod
#pragma once
#include "Qqt/DB/SqliteDb.h"

//accessed from non-gui thread
class DirDb: public SqliteDb {
	public:
		static DirDb& instance(const QDir & dir);
		static DirDb& instance(const QFileInfo & file);
		virtual ~DirDb();
		ReadStatus thumbnail(const QFileInfo & file, OUT QImage & img);
		WriteStatus setThumbnail(const QFileInfo & file, const QImage & img);
		static WriteStatus setThumbnailS(const QFileInfo & file, const QImage & img);
		WriteStatus setRating(const QFileInfo & file, int n);
		void freeCacheMemory();
		static const QString dbFileName;
		static const QString dbFileName2;
	protected:
		DirDb(const QDir & dir);

		QString _dbPath;
		PreparedSqlQuery _qThumbGet;
		PreparedSqlQuery _qThumbGetAll;
		PreparedSqlQuery _qThumbSet;
		PreparedSqlQuery _qThumbSetRating;

		virtual ReadStatus connectToDbOnce()override;
		bool maybeInstallDb();
		ReadStatus readAllToCache();
		struct Item {
			Item(const QString & name): _name(name) { }
			const QString _name;
			QByteArray _thumb;
			QDateTime _modified;
			int _rating = 0;
			QByteArray _showSettings;
			QImage _iThumb;
		};
		static bool compareByRating(const Item * i1, const Item * i2);
		QList<Item*> _items;
		QMap<QString, Item*> _byName;
		Item* getOrCreate(const QString & name);
};