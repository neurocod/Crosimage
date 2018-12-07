//MenuButton.cpp by Kostya Kozachuck as neurocod - 2014.04.25 18:09:35
#include "pch.h"
#include "MenuButton.h"

MenuButton::MenuButton(QWidget*parent): _btn(parent), _menu(_btn) {
	_btn->setMenu(_menu);
	_btn->setPopupMode(QToolButton::MenuButtonPopup);
}
MenuButton& MenuButton::operator<<(QAction*act) {
	_menu << act;
	if(_setIconByFirstAction) {
		_setIconByFirstAction = false;
		_btn.icon = act->icon();
		_btn.toolTip = act->toolTip();
		QObject::connect(_btn, &QToolButton::clicked, act, &QAction::trigger);
	}
	return *this;
}
MenuButton& MenuButton::operator<<(const QList<QAction *> & li) {
	for(auto a: li)
		*this << a;
	return *this;
}