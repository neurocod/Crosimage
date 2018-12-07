//WidgetAction.cpp by Kostya Kozachuck as neurocod - 2014.09.21 14:22:40
#include "pch.h"
#include "WidgetAction.h"

QSet<WidgetAction*> WidgetAction::s_inst;
WidgetAction::~WidgetAction() {
	bool b = s_inst.remove(this);
	ASSERT(b);
}
WidgetAction::WidgetAction(QObject*parent, const QString & text, Qt::ToolButtonStyle toolButtonStyle):
	QWidgetAction(parent),
	_text(text),
	_toolButtonStyle(toolButtonStyle)
{
	s_inst << this;
	auto timer = new QTimer(this);
	timer->setSingleShot(true);
	connect(timer, &QTimer::timeout, this, &WidgetAction::addMyActionsToParent);
	connect(timer, &QTimer::timeout, timer, &QObject::deleteLater);
	timer->start(1);
}
struct WidgetAction::CreatedWidget: public QToolBar {
	CreatedWidget(QWidget*parent): QToolBar(parent) { }
	void closeParentMenu() {
		QMenu*menu = 0;
		for(QWidget*wparent = this; wparent; wparent = wparent->parentWidget()) {
			if(menu = qobject_cast<QMenu*>(wparent))
				break;
		}
		if(!menu)
			return;
		menu->close();
	}
};
WidgetAction & WidgetAction::operator<<(QAction*a) {
	return add(a);
}
WidgetAction & WidgetAction::add(QAction*a) {
	return add(a, _toolButtonStyle);
}
WidgetAction & WidgetAction::add(QAction*a, Qt::ToolButtonStyle style) {
	Pair p{a, style};
	_actions << p;
	return *this;
}
QWidget* WidgetAction::createWidget(QWidget * parent) {
	if(!isVisible())
		return 0;
	auto bar = new CreatedWidget(parent);
	bar->setToolButtonStyle(_toolButtonStyle);
	if(!_text.isEmpty())
		bar->addWidget(new QLabel(_text));
	for(Pair & p: _actions) {
		ToolButton b(p._act, p._style);
		bar->addWidget(b);
		if(_checkableActionsCloseMenuToo || !p._act->isCheckable()) {
			connect(p._act, &QAction::triggered, bar, &CreatedWidget::closeParentMenu);
		}
	}
	return bar;
}
QList<QAction*> WidgetAction::removeActionsContainedByMe(const QList<QAction*> & actions) {
	QList<QAction*> ret;
	QSet<QAction*> toRemove;
	for(auto a: actions) {
		auto candidate = static_cast<WidgetAction*>(a);
		if(!s_inst.contains(candidate)) {
			continue;
		}
		for(Pair & p2: candidate->_actions) {
			toRemove << p2._act;
		}
	}
	for(auto a: actions) {
		if(!toRemove.contains(a))
			ret << a;
	}
	return ret;
}
void WidgetAction::addMyActionsToParent() {
	for(auto o = parent(); o; o = o->parent()) {
		if(auto w = qobject_cast<QWidget*>(o)) {
			for(auto p: _actions)
				w->addAction(p._act);
			break;
		}
	}
}