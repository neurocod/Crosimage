//HttpRequestHeaders.h by Kostya Kozachuck as neurocod - 2015.11.28 05:44:56
#pragma once

class HttpRequestHeaders {
	public:
		HttpRequestHeaders();
		HttpRequestHeaders(const QByteArray & headers);
		void set(const QByteArray & headers);
		virtual ~HttpRequestHeaders() {}

		ReadStatus _parseStatus;
		QByteArray _method;
		QByteArray _path;
		QMap<QString, QString> _params;
		QByteArray _protocol;
		QMap<QByteArray, QByteArray> _headers;
	protected:
		void parseUriToParams();
};
