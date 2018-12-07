//HttpRespond.h by Kostya Kozachuck as neurocod - 2015.11.29 07:01:38
#pragma once
#include "HttpStatusCode.h"

class HttpRespond {
	public:
		HttpRespond();
		virtual ~HttpRespond() {}
		void write(QIODevice*device);//non-const - can change Content-Length header
		void addHeader(const QByteArray & arr);

		int _code = Http_200_OK;
		QMap<QByteArray, QByteArray> _headers;
		QByteArray _body;
};