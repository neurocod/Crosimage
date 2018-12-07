#pragma once
#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include <QtWidgets/QtWidgets>
#include "preprocessor.h"
#include "New.h"
template<class... Args, class T, class R>
auto resolve(R(T::*m)(Args...)) -> decltype(m) {
	return m;
}
template<class T, class R>
auto resolve(R(T::*m)(void)) -> decltype(m) {
	return m;
}
//to get address of an overloaded function, write:
//resolve<parameters>(&ClassName::funcName)
//resolve<>(&ClassName::funcName) if params are void
#include "ByteArrayWithDataStream.h"
#include "Uid.h"
#include "JsonValueReader.h"
#include "JsonWriter.h"
#include "IntIterator.h"
#include "TopResult.h"
#include "PropertyRedirect.h"
#include "NLStyle.h"
#include "FileFacility.h"

#include "ToString.h"
#include "FileReader.h"
#include "FileWriter.h"
#include "Settings.h"
#include "SignalEmitter.h"
#include "Thread.h"
