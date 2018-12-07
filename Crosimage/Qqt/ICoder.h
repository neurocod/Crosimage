//ICoder.h by Kostya Kozachuck as neurocod - 13.08.2009 22:23:50
#pragma once

//temporary solution class, only xoring
class ICoder {
	public:
		ICoder() {}
		virtual ~ICoder() {}
		static void setKey(const QString & str);
		static QString code(IN const QString & strSrc);
		static void code(IN const QByteArray & arrSrc, OUT QByteArray & arrOut);
		static void code(IN const QByteArray & arrSrc, OUT QByteArray & arrOut, IN const QByteArray & arrKey);
		static bool decode(IN const QString & strSrc, OUT QString & strDest);
		static bool decode(IN const QByteArray & arrSrc, OUT QByteArray & arrDest);
		static bool decode(IN const QByteArray & arrSrc, OUT QByteArray & arrDest, IN const QByteArray & arrKey);
		static void makeXor(IN OUT QByteArray & arr, IN const QString & strXorKey);
		static void makeXor(IN OUT QByteArray & arr, IN const QByteArray & arrXorKey);
		static void test();
	protected:
		static QByteArray s_arrKey;
};