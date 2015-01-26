//MicrosoftLnkReader.h by Kostya Kozachuck as neurocod - 2015.01.26 16:59:22
#pragma once

class MicrosoftLnkReader {
	public:
		static QString targetOrSame(const QString & path);
		template<class T>
		static QString printf(const QString & str, T t);
};