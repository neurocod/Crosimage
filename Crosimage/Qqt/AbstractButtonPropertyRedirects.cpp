//AbstractButtonPropertyRedirects.cpp by Kostya Kozachuck as neurocod - 16.09.2011 1:16:38
#include "pch.h"
#include "AbstractButtonPropertyRedirects.h"

AbstractButtonPropertyRedirects::AbstractButtonPropertyRedirects(QAbstractButton*d): WidgetPropertyRedirects(d) {
	autoExclusive.init(d);
	autoRepeat.init(d);
	autoRepeatDelay.init(d);
	autoRepeatInterval.init(d);
	checkable.init(d);
	checked.init(d);
	down.init(d);
	icon.init(d);
	iconSize.init(d);
	shortcut.init(d);
	text.init(d);
}
void AbstractButtonPropertyRedirects::connectClicksS(QObject*obj, const char* slot, Qt::ConnectionType type) {
	QObject::connect(icon.destination(), SIGNAL(clicked(bool)), obj, slot, type);
}
void AbstractButtonPropertyRedirects::set(const QIcon & i) {
	icon = i;
}
void AbstractButtonPropertyRedirects::addShortcutToTooltip() {
	QKeySequence keySeq = shortcut;
	QString key = keySeq.toString();
	if(key.isEmpty())
		return;
	QString str = toolTip;
	if(!str.isEmpty())
		str += '\n';
	str += key;
	toolTip = str;
}