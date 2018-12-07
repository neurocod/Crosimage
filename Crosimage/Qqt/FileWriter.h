//FileWriter.h by Kostya Kozachuck as neurocod - 22.10.2011 18:07:18
#pragma once

class FileWriter {
	public:
		static WriteStatus write(IN const QString & fileName, IN const QByteArray& arr);
		static WriteStatus write8bit(IN const QString & fileName, IN const QString & strFileData);
		static WriteStatus writeUnicode(IN const QString & fileName, IN const QString & strFileData, const char*codec = "UTF-16");
		static WriteStatus removeFile(const QString & path);
		static WriteStatus formatError(const QString & file, const QString & error);
		static WriteStatus formatError(const QFile & file);
	private:
		FileWriter();
};