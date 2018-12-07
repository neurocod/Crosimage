//HttpRespond.cpp by Kostya Kozachuck as neurocod - 2015.11.29 07:01:38
#include "pch.h"
#include "HttpRespond.h"

HttpRespond::HttpRespond() {
}
void HttpRespond::write(QIODevice*device) {
	QByteArray headers = "HTTP/1.1 ";
	headers += QByteArray::number(_code) + "\r\n";
	const char* strContentLength = "Content-Length";
	if(_body.isEmpty()) {
		_headers.remove(strContentLength);
	} else {
		_headers[strContentLength] = QByteArray::number(_body.length());
	}
	for(auto it = _headers.cbegin(); it!=_headers.cend(); ++it) {
		headers += it.key() + ": " + it.value() + "\r\n";
	}
	headers += "\r\n";
	device->write(headers);
	device->write(_body);
}
void HttpRespond::addHeader(const QByteArray & arr) {
	int index = arr.indexOf(':');
	ASSERT(index!=-1);
	if(index==-1)
		return;
	auto name = arr.left(index);
	auto value = arr.mid(index+1);
	_headers[name] = value;
}