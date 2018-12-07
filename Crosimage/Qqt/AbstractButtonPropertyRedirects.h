//AbstractButtonPropertyRedirects.h by Kostya Kozachuck as neurocod - 16.09.2011 1:16:37
#pragma once

class AbstractButtonPropertyRedirects: public WidgetPropertyRedirects {
	public:
		AbstractButtonPropertyRedirects(QAbstractButton*d);

		PROPERTY_REDIRECTV(QAbstractButton, bool, autoExclusive, autoExclusive, setAutoExclusive);
		PROPERTY_REDIRECTV(QAbstractButton, bool, autoRepeat, autoRepeat, setAutoRepeat);
		PROPERTY_REDIRECTV(QAbstractButton, int, autoRepeatDelay, autoRepeatDelay, setAutoRepeatDelay);
		PROPERTY_REDIRECTV(QAbstractButton, int, autoRepeatInterval, autoRepeatInterval, setAutoRepeatInterval);
		PROPERTY_REDIRECTV(QAbstractButton, bool, checkable, isCheckable, setCheckable);
		PROPERTY_REDIRECTV(QAbstractButton, bool, checked, isChecked, setChecked);
		PROPERTY_REDIRECTV(QAbstractButton, bool, down, isDown, setDown);
		PROPERTY_REDIRECT (QAbstractButton, QIcon, icon, icon, setIcon);
		PROPERTY_REDIRECT (QAbstractButton, QSize, iconSize, iconSize, setIconSize);
		PROPERTY_REDIRECT (QAbstractButton, QKeySequence, shortcut, shortcut, setShortcut);
		PROPERTY_REDIRECT (QAbstractButton, QString, text, text, setText);

		void set(const QIcon & i);
		void connectClicksS(QObject*obj, const char* slot, Qt::ConnectionType type = Qt::AutoConnection);
		template<class T1, class T2>
		void connectClicks(T1*receiver, const T2 slot, Qt::ConnectionType type = Qt::AutoConnection) {
			QAbstractButton* btn = icon.destination();
			QObject::connect(btn, &QAbstractButton::clicked, receiver, slot, type);
		}
		template<class T1>
		void connectClicksF(T1 functor) {
			QAbstractButton* btn = icon.destination();
			QObject::connect(btn, &QAbstractButton::clicked, functor);
		}
		template<class T1, class T2>
		void connectClicksF(T1 obj, T2 functor) {
			QAbstractButton* btn = icon.destination();
			QObject::connect(btn, &QAbstractButton::clicked, obj, functor);
		}
		void addShortcutToTooltip();
};