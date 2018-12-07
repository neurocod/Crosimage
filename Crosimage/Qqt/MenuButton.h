//MenuButton.h by Kostya Kozachuck as neurocod - 2014.04.25 18:09:35
#pragma once

class MenuButton {
		ToolButton _btn;
		Menu _menu;//keep created after _btn
	public:
		bool _setIconByFirstAction = true;
		MenuButton(QWidget*parent = 0);
		operator QToolButton*() { return _btn; }
		QToolButton* operator->() { return _btn; }
		MenuButton& operator << (QAction*act);
		MenuButton& operator << (const QList<QAction *> & li);
};