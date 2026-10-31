#include "pch.h"
#include "DirectorySorting.h"

#include <QCollator>

bool compareByNameAndNumber(const QString & s1, const QString & s2) {
	thread_local QCollator collator;
	collator.setNumericMode(true);
	return collator.compare(s1, s2)<0;
}
