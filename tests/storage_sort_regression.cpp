#include "DirectorySorting.h"
#include "pch.h"
#include "DirDb.h"

#include <stdexcept>
#include <thread>

namespace {
void require(bool condition, const char *message) {
	if(!condition)
		throw std::runtime_error(message);
}

// Frozen writer from SqliteDb at 9377ecb. The test seeds an existing database
// with this format before opening it through the current DirDb implementation.
QByteArray legacyDateBytes(const QDateTime &date) {
	QByteArray bytes;
	QDataStream stream(&bytes, QIODevice::WriteOnly);
	stream << date;
	return bytes;
}

// Preserve even the old reader's handling of incomplete input. Changing that
// behavior requires a separate validation/migration decision, not this move.
QDateTime legacyDateFromVariant(const QVariant &value) {
	QDateTime date;
	if(value.canConvert(QVariant::ByteArray)) {
		const QByteArray bytes = value.toByteArray();
		QDataStream stream(bytes);
		stream >> date;
	}
	return date;
}

class InspectableDirDb: public DirDb {
public:
	explicit InspectableDirDb(const QDir &dir): DirDb(dir) {}
	QDateTime modified(const QString &name)const {
		auto item = _byName.value(name);
		require(item != nullptr, "missing cached database item");
		return item->_modified;
	}
	QList<int> ratings()const {
		QList<int> result;
		for(auto item: _items)
			result << item->_rating;
		return result;
	}
};

void checkStorage() {
	QTemporaryDir temporary;
	require(temporary.isValid(), "could not create storage fixture");
	const QDir directory(temporary.path());
	const QString databasePath = directory.filePath(DirDb::dbFileName);
	const QDateTime date(QDate(2024, 2, 29), QTime(12, 34, 56, 789), QTimeZone::UTC);
	const QString seedConnection = QStringLiteral("storage-regression-seed");
	{
		auto database = QSqlDatabase::addDatabase("QSQLITE", seedConnection);
		database.setDatabaseName(databasePath);
		require(database.open(), "could not open seed database");
		QSqlQuery query(database);
		require(query.exec("CREATE TABLE files (name TEXT PRIMARY KEY, thumb BLOB, "
			"modified BLOB, rating INTEGER, show_settings BLOB)"), "could not create legacy schema");
		require(query.prepare("INSERT INTO files (name, modified, rating) VALUES (?, ?, ?)"),
			"could not prepare legacy records");
		const QList<QByteArray> blobs = {legacyDateBytes(date), QByteArray(""), QByteArray::fromHex("01")};
		const QList<int> ratings = {1, 10, -2};
		for(int index = 0; index < blobs.size(); ++index) {
			query.bindValue(0, QString::number(index));
			query.bindValue(1, blobs[index]);
			query.bindValue(2, ratings[index]);
			require(query.exec(), "could not insert legacy record");
		}
	}
	QSqlDatabase::removeDatabase(seedConnection);
	{
		InspectableDirDb database(directory);
		require(database.modified("0") == date, "legacy modified BLOB did not survive loading");
		require(database.modified("1") == legacyDateFromVariant(QByteArray("")),
			"empty BLOB decoding differs from the legacy reader");
		require(database.modified("2") == legacyDateFromVariant(QByteArray::fromHex("01")),
			"truncated BLOB decoding differs from the legacy reader");
		require(database.ratings() == QList<int>({10, 1, -2}), "cached records are not ordered by descending rating");

		QFile imageFile(directory.filePath("new-image.png"));
		require(imageFile.open(QIODevice::WriteOnly), "could not create thumbnail source");
		imageFile.close();
		const QFileInfo info(imageFile.fileName());
		QImage image(2, 2, QImage::Format_RGB32);
		image.fill(Qt::red);
		require(database.setThumbnail(info, image).ok(), "could not write new thumbnail");
		QSqlQuery query(database.db());
		require(query.exec("SELECT modified FROM files WHERE name='new-image.png'") && query.next(),
			"could not read the stored timestamp");
		require(query.value(0).toByteArray() == legacyDateBytes(info.lastModified()),
			"writing a thumbnail changed the legacy timestamp encoding");
	}
	require(!QSqlDatabase::contains(databasePath), "database connection survived its owner");
}

void checkSorting() {
	QStringList names = {"image10.png", "image2.png", "image1.png"};
	std::sort(names.begin(), names.end(), compareByNameAndNumber);
	require(names == QStringList({"image1.png", "image2.png", "image10.png"}),
		"natural sorting no longer orders numeric names");
	QCollator legacyComparator;
	legacyComparator.setNumericMode(true);
	const QStringList samples = {"A2", "a10", "folder1", "folder01", "z", QString::fromUtf8("папка2")};
	for(const auto &left: samples)
		for(const auto &right: samples)
			require(compareByNameAndNumber(left, right) == (legacyComparator.compare(left, right) < 0),
				"the extracted comparator changed collation semantics");
	bool sortedOnWorker = false;
	std::thread worker([&sortedOnWorker]() {
		sortedOnWorker = compareByNameAndNumber("folder2", "folder10")
			&& !compareByNameAndNumber("folder10", "folder2");
	});
	worker.join();
	require(sortedOnWorker, "natural sorting failed on another thread");
}
}

int main(int argc, char **argv) {
	QCoreApplication application(argc, argv);
	QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
	try {
		checkSorting();
		checkStorage();
		qInfo() << "Storage and sorting regressions passed";
		return 0;
	} catch(const std::exception &error) {
		qCritical() << error.what();
		return 1;
	}
}
