//JsonWriter.h by Konstantine Kozachuck as neurocod - 2018.06.29 16:58:08
#pragma once

class JsonWriter {
	public:
		static QByteArray toJson(const QJsonObject & j);//spaces are replaced with tabs
		static QString toJson2(const QJsonObject & v);
	protected:
		JsonWriter() {}
		int _indent = 0;
		QString _s;
		void n();//newline
		void dec();//decrement indents
		void tab();//increment indents

		void add(const QJsonObject&j);
		void add(const QJsonValue &j);
		void add(const QJsonArray &j);
};