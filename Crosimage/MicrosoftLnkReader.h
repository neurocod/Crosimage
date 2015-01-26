//MicrosoftLnkReader.h by Kostya Kozachuck as neurocod - 2015.01.26 16:59:22
#pragma once

class MicrosoftLnkReader {
	public:
		static QString targetOrSame(const QString & path);
		StringStatus loadAndParse(const QString & path);
		template<class T>
		static QString printf(const QString & str, T t);

		static const int Signature = 0x4C000000;
		quint32 _signature;
		unsigned char GUID[16];//The current GUID for shortcuts is 01 14 02 00 00 00 00 00 C0 00 00 00 00 00 46, It may change in the future.
		quint32 ShortcutFlags;//Shortcut flags are explained below
		quint32 TargetFileFlags;//Target file flags - Flags are explained below
		unsigned char CreationTime[8];//Windows FILETIME structure
		unsigned char LastAccessTime[8];//FILETIME
		unsigned char ModificationTime[8];//FILETIME
		quint32 FileLength;//The length of the target file. 0 if the target is not a file.
		//This value is used to find the target when the link is broken.
		quint32 IconNumber;//If the file has a custom icon (set by the flags bit 6),
		//then this long integer indicates the index of the icon to use. Otherwise it is zero.
		quint32 ShowWindow;//SW_SHOWNORMAL=1, SW_SHOWMAXIMIZED=3, SW_SHOWMINNOACTIVE=7, all other values MUST be treated as SW_SHOWNORMAL
		quint32 HotKey;//The hot key assigned for this shortcut
		quint32 Reserved1;//Reserved - Always 0
		quint32 Reserved2;//Reserved - Always 0
};