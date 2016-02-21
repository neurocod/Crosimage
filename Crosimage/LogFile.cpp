//LogFile.cpp by neurocod
#include "pch.h"
#include "LogFile.h"

LogFile& LogFile::instance() {
	static LogFile p;
	return p;
}
LogFile::LogFile(const QString & additionalName, bool appendTextStart): _file(filePath(additionalName)) {
	if(!_file.open(QIODevice::WriteOnly|QIODevice::Append)) {
		msgBox(QObject::tr("Can't open file %1 for writing").arg(_file.fileName()));
		return;
	}
	if(appendTextStart) {
		QString str = QString("Program started at %1").arg(QDateTime::currentDateTime().toString("yyyy.MM.dd ddd hh:mm:ss.zzz"));
#ifdef _DEBUG
		str.prepend("[Debug] ");
#endif
		str.prepend("\n____________ ");
		debug_() << str << endl;
	}
}
LogFile::~LogFile() {
}
QDebug LogFile::debug() {//static
	return instance().debug_();
}
void LogFile::test() {
	LogFile::debug() << "test";
}
QDebug LogFile::debug_() {
	if(_file.isOpen())
		return QDebug(&_file);
	return QDebug(QtDebugMsg);
}
void LogFile::log(const QString & str) {
	debug() << str;
}
QString LogFile::filePath(const QString & additionalName) {//static
	QString dir = QCoreApplication::applicationDirPath();
	QString name = QCoreApplication::arguments()[0];
	name += additionalName;
	name += ".log";
	QString path = QDir(dir).absoluteFilePath(name);
#ifdef Q_OS_MACX
	{
		int index = path.indexOf(".app/");
		if(-1!=index) {
			QString str = path;
			path.truncate(index);
			path += ".log";
		}
	}
#endif
	return path;
}
//________________________________________________________________
LogMark::LogMark(const QString & name): _name(name) {
	QString str = "{ " + _name + " " + QTime::currentTime().toString("ss.zzz") + "\n";
	LogFile::debug() << str;
}
LogMark::~LogMark() {
	//QString str = "} " + _name + QTime::currentTime().toString("ss.zzz") + "\n";
	//LogFile::debug() << str;
	}