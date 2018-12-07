//JsonWriter.cpp by Konstantine Kozachuck as neurocod - 2018.06.29 16:58:08
#include "pch.h"
#include "JsonWriter.h"

QByteArray JsonWriter::toJson(const QJsonObject & j) {
	QJsonDocument doc(j);
	const QByteArray arr = doc.toJson(QJsonDocument::Indented);
	auto lines = arr.split('\n');
	QByteArray ret;
	for(int i = 0; i<lines.count(); ) {
		auto line = lines.at(i);
		++i;
		int spaces = 0;
		for(char ch : line) {
			if(ch==' ')
				spaces++;
			else
				break;
		}
		int tabs = spaces/4;
		if(tabs>0) {
			line.remove(0, tabs*4);
			QByteArray btabs;
			btabs.fill('\t', tabs);
			ret.append(btabs);
		}
		ret.append(line);
		if(i!=lines.count())
			ret.append('\n');
	}
	return ret;
}
QString JsonWriter::toJson2(const QJsonObject & v) {
	//JsonWriter w;
	//w.add(v);
	//return w._s;
	return toJson(v);
}
void JsonWriter::n() {

}
void JsonWriter::dec() {

}
void JsonWriter::tab() {

}
//void JsonWriter::add(const QJsonObject&j);
//void JsonWriter::add(const QJsonValue &j);
//void JsonWriter::add(const QJsonArray &j);