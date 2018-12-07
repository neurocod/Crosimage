//Uid.h by Konstantine Kozachuck as neurocod - 2018.07.01 21:51:54
#pragma once

#ifdef _DEBUG
#define DEBUG_UUID
#endif

#ifdef DEBUG_UUID
struct Uid: public QUuid {
	Uid() {}
	Uid(const QString &s);
	Uid(const char*c);
	Uid(const QUuid &id);
	void operator=(const QUuid&id);
	bool operator==(const QUuid&id)const;
	
	QByteArray _s;//just for debug
};
#else
using Uid = QUuid;
#endif
using UidList = QList<Uid>;
enum UidLength {
	ToStringUidLength = 38//like {00000000-0000-0000-0000-000000000000}
};