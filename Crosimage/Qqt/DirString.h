//DirString.h by Kostya Kozachuck as neurocod - 12.08.2011 18:28:30
#pragma once

//ensures there is always / at the end of the path, if it's not empty
//default state is empty
class DirString {
	public:
		DirString();
		DirString(const QString & path);
		virtual ~DirString() {}

		static QString visualStudioSolutionDir();//if launched from Visual Studio

		operator QString()const { return _str; }
		static int length(const QStringList & li);
		bool isEmpty()const;
		DirString& operator+=(const QString & str);
	protected:
		QString _str;

		void correctSlash();
};