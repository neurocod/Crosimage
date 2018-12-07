//Object.cpp by Kostya Kozachuck as neurocod - 11.03.2012 21:18:53
#include "pch.h"
#include "Object.h"

Object::Object(QObject*parent) : ObjectPropertyRedirects(new QObject(parent)) {
	d = objectName.destination();
}