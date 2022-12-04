//FileWriter.h by Kostya Kozachuck as neurocod - 22.10.2011 18:07:18
#pragma once

class FileWriter {
	public:
		static WriteStatus write(const QString & fileName, const QByteArray& arr);
		static WriteStatus write8bit(const QString & fileName, const QString & strFileData);
		static WriteStatus writeUnicode(const QString & fileName, const QString & strFileData, const QStringConverter::Encoding encoding);
		static WriteStatus removeFile(const QString & path);
		static WriteStatus formatError(const QString & file, const QString & error);
		static WriteStatus formatError(const QFile & file);
	private:
		FileWriter();
};