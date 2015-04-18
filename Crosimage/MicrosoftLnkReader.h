//MicrosoftLnkReader.h by Kostya Kozachuck as neurocod - 2015.01.26 16:59:22
#pragma once

//TODO: use docs (google for The_Windows_Shortcut_File_Format.pdf) and TODO parsing
//http://habrahabr.ru/post/125081/
class MicrosoftLnkReader {
	public:
		static QString targetOrSame(const QString & path);
		StringStatus loadAndParse(const QString & path);

		static const quint32 CorrectHeaderSize = 0x0000004C;
		quint32 _headerSize;
		quint8 _guid[16];
		static quint8 CorrectGuid[16];
		quint32 _shortcutFlags;//Shortcut flags are explained below
		quint32 _targetFileFlags;//Target file flags - Flags are explained below
		quint64 _creationTime;//Windows FILETIME structure
		quint64 _lastAccessTime;//FILETIME
		quint64 _modificationTime;//FILETIME
		quint32 _fileLength;//The length of the target file. 0 if the target is not a file.
		//This value is used to find the target when the link is broken.
		quint32 _iconNumber;//If the file has a custom icon (set by the flags bit 6),
		//then this long integer indicates the index of the icon to use. Otherwise it is zero.
		quint32 _showWindow;//SW_SHOWNORMAL=1, SW_SHOWMAXIMIZED=3, SW_SHOWMINNOACTIVE=7, all other values MUST be treated as SW_SHOWNORMAL
		quint16 _hotKey;//The hot key assigned for this shortcut
		quint16 _reserved1;
		quint32 _reserved2;
		quint32 _reserved3;

		QString _fileName;
		static QString read0Terminated(QDataStream & stream, bool unicode);
};