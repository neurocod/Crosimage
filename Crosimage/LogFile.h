//LogFile.h by neurocod
#pragma once

#define LOG_LINE LogFile::debug() << __FILE__ <<  __LINE__ << Qt::endl;

class LogFile {
	public:
		LogFile(const QString & additionalName=QString(), bool appendTextStart=true);
		virtual ~LogFile();
		static LogFile& instance();
		static QDebug debug();
		static void log(const QString & str);
		QDebug debug_();
		static void test();
		static QString filePath(const QString & additionalName=QString());
		QFile& file() { return _file; }
	protected:
		QFile _file;
};
class LogMark {
	QString _name;
	public:
		LogMark(const QString & name);
		~LogMark();
};