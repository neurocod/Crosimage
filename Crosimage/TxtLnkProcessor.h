//TxtLnkProcessor.h by Kostya Kozachuck as neurocod - 2015.01.26 20:00:49
#pragma once

//*.txtlnk file extension - unicode text file with file link inside, maybe with relative path
class TxtLnkProcessor {
	public:
		static const QString extensionWithDot;
		static void shellExecute(const QString & path);
		static QString pathFromFile(const QString & path);
		static QString pathFromFileOrSame(const QString & path);
		static QString existingLinkedFileFromParentDir(const QString & path);
};