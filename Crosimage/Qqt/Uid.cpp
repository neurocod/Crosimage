//Uid.cpp by Konstantine Kozachuck as neurocod - 2018.07.01 21:51:54
#include "pch.h"
#include "Uid.h"

#ifdef _DEBUG
Uid::Uid(const QString & id): QUuid(id) {
	if(id.length() == 38 && id[0] == "\"" && id[37] == "\"") {//Postgres format
		*this = QUuid("{" + id.mid(1, 36) + "}");
	}
	_s = toByteArray();
}
Uid::Uid(const QUuid & id): QUuid(id) {
	_s = toByteArray();
}
Uid::Uid(const char*c): QUuid(c) {
	_s = toByteArray();
}
void Uid::operator=(const QUuid&id) {
	QUuid::operator=(id);
	_s = toByteArray();
}
bool Uid::operator==(const QUuid&id)const {
	return QUuid::operator==(id);
}
#endif