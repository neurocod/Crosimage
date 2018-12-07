//ICoder.cpp by Kostya Kozachuck as neurocod - 13.08.2009 22:23:52
#include "pch.h"
#include "ICoder.h"

QByteArray ICoder::s_arrKey("7E2129F0BB454F1BA187C36D8AFD4D42");

//static
void ICoder::setKey(const QString & str) {
	QByteArray ar;
	ar.append(str);
	s_arrKey = ar;
}
//static
void ICoder::makeXor(IN OUT QByteArray & arr, IN const QByteArray & arrXorKey) {
	if(arrXorKey.isEmpty()) {
		ASSERT(0);
		return;
	}
	const int nLen = arr.size();
	const int nXorLen = arrXorKey.size();

	int nXorIndex = 0;
	for(int i=0; i<nLen; ++i, ++nXorIndex) {
		if(nXorIndex>=nXorLen)
			nXorIndex = 0;
		char chXor = arrXorKey[nXorIndex];
		char chDest = arr[i];
		chDest = chDest ^ chXor;
		arr[i] = chDest;
	}
}
//static
void ICoder::makeXor(IN OUT QByteArray & arr, IN const QString & strXorKey) {
	QByteArray arrXor = strXorKey.toLatin1();
	makeXor(arr, arrXor);
}
//static
QString ICoder::code(IN const QString & strSrc) {
	QByteArray arrSrc, arrOut;
	arrSrc.append(strSrc);

	code(arrSrc, arrOut);
	QString ret(arrOut);
	return ret;
}
//static
void ICoder::code(IN const QByteArray & arrSrc, OUT QByteArray & arrOut) {
	code(arrSrc, arrOut, s_arrKey);
}
//static
void ICoder::code(IN const QByteArray & arrSrc, OUT QByteArray & arrOut, IN const QByteArray & arrKey) {
	//xor
	QByteArray arrXor(arrSrc);
	makeXor(arrXor, arrKey);

	//hex
	arrOut = arrXor.toHex();
}
//static
bool ICoder::decode(IN const QString & strSrc, OUT QString & strDest) {
	QByteArray arrSrc, arrDest;
	arrSrc.append(strSrc);
	bool ret = decode(arrSrc, arrDest, s_arrKey);
	if(ret) {
		strDest = arrDest;
	}
	return ret;
}
//static
bool ICoder::decode(IN const QByteArray & arrSrc, OUT QByteArray & arrDest) {
	return decode(arrSrc, arrDest, s_arrKey);
}
//static
bool ICoder::decode(IN const QByteArray & arrSrc, OUT QByteArray & arrDest, IN const QByteArray & arrKey) {
	//1 check if is hex encoded properly
	int nLen = arrSrc.count();
	if(nLen % 2) {
		return false;
	}
	for(int i=0; i<nLen; ++i) {
		char ch = arrSrc[i];
		bool isHex =
			('0'<=ch && ch<='9') ||
			('a'<=ch && ch<='f') ||
			('A'<=ch && ch<='F');
		if(!isHex) {
			//not hex-encoded properly
			return false;
		}
	}
	//hex -> byte array
	arrDest = QByteArray::fromHex(arrSrc);

	//byte -> xor
	makeXor(arrDest, arrKey);
	return true;
}
void ICoder::test() {
	{//code and decode
		QString strSrc("simple string with enough length");
		strSrc = strSrc.repeated(2);
		QString strCoded = ICoder::code(strSrc);
		strCoded = strCoded.toUpper();
		QString strDecoded;
		bool b = ICoder::decode(
			//"",
			strCoded,
			strDecoded);
		ASSERT(strDecoded == strSrc);
	}
}