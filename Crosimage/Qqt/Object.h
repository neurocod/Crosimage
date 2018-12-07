//Object.h by Kostya Kozachuck as neurocod - 11.03.2012 21:18:52
#pragma once

class ObjectPropertyRedirects {
	public:
		ObjectPropertyRedirects(QObject*d) {
			objectName.init(d);
		}
		virtual ~ObjectPropertyRedirects() {}

		PROPERTY_REDIRECT(QObject, QString, objectName, objectName, setObjectName);
		template<class T>
		T staticCast() {
			QObject*obj = objectName.destination();
			return static_cast<T>(obj);
		}
};

class Object: public ObjectPropertyRedirects {
	MAYBE_SUPER(ObjectPropertyRedirects)
	public:
		Object(QObject*parent = 0);
		EMBED_QPOINTER_AND_CAST(QObject)
};