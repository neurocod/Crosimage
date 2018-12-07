//HttpRequestHeaders.cpp by Kostya Kozachuck as neurocod - 2015.11.28 05:44:56
#include "pch.h"
#include "HttpRequestHeaders.h"

HttpRequestHeaders::HttpRequestHeaders() {
	_parseStatus.setError("empty headers");
}
HttpRequestHeaders::HttpRequestHeaders(const QByteArray & headers) {
	set(headers);
}
void HttpRequestHeaders::set(const QByteArray & headers) {
	_parseStatus.setOk();
	auto lines = headers.split('\n');
	for(auto & line: lines) {
		if(line.endsWith('\r')) {
			line.truncate(line.count()-1);
		}
	}
	if(lines.count()<1) {
		_parseStatus.setError("http request lines count < 1");
		return;
	}
	QByteArray line = lines[0];
	auto parts = line.split(' ');
	if(parts.count()!=3) {
		_parseStatus.setError("http request first line doesn't contain 3 spaces");
		return;
	}
	for(auto part: parts) {
		if(part.isEmpty()) {
			_parseStatus.setError("http request first line contains empty request parts");
			return;
		}
	}
	_method = parts[0];
	_path = parts[1];
	parseUriToParams();
	_protocol = parts[2];
	bool protocolOk = _protocol!="HTTP/1.0" || _protocol=="HTTP/1.1";
	if(!protocolOk) {
		_parseStatus.setError("unsupported http protocol version");
		return;
	}
	for(int i = 1; i<lines.count(); ++i) {
		auto & line = lines[i];
		int index = line.indexOf(':');
		if(-1==index) {
			_parseStatus.setError("http header without colon");
			return;
		}
		auto name = line.left(index-1);
		auto value = line.mid(index+1);
		name = name.trimmed();
		value = value.trimmed();
		if(name.isEmpty()) {
			_parseStatus.setError("http header with empty name");
			return;
		}
		_headers[name] = value;
	}
}
void HttpRequestHeaders::parseUriToParams() {
	int pathEnd = _path.indexOf('?');
	if(-1==pathEnd)
		return;
	auto params = _path.mid(pathEnd+1);
	_path.truncate(pathEnd);
	for(auto param: params.split('&')) {
		int index = param.indexOf('=');
		if(-1==index) {
			if(!param.isEmpty())
				_params.insert(param, "");
			continue;
		}
		auto value = param.mid(index+1);
		param.truncate(index);
		_params[param] = value;
	}
}