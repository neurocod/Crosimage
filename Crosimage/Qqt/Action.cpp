//Action.cpp by Kostya Kozachuck as neurocod - 11.11.2011 22:08:23
#include "pch.h"
#include "Action.h"

void Action::connectClicksS(QObject*obj, const char* slot, Qt::ConnectionType conn) {
	QObject::connect(d, SIGNAL(triggered(bool)), obj, slot, conn);
}
//static
void Action::insertToBegin(QWidget*w, QAction*act) {
	ASSERT(w && act);
	QList<QAction*> li = w->actions();
	if(li.isEmpty()) {
		w->addAction(act);
	} else {
		w->insertAction(li[0], act);
	}
}
//static
void Action::insertToBegin(QWidget*w, const QList<QAction*> & lact) {
	ASSERT(w);
	QList<QAction*> li = w->actions();
	if(li.isEmpty()) {
		w->insertActions(0, lact);
	} else {
		w->insertActions(li[0], lact);
	}
}