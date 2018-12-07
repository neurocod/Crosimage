//Menu.h by Kostya Kozachuck as neurocod - 16.09.2011 2:01:02
#pragma once

class Menu: public WidgetPropertyRedirects {
	public:
		//вроде баг в Qt 5.3RC если вызывать QMenu::setParent когда оно уже создано и родитель QToolButton - тогда кнопка рисует текст
		//template<typename ... Args>
		Menu(QWidget*parent=0): WidgetPropertyRedirects(new QMenu(parent)) {
			d = staticCast<QMenu*>();

			icon.init(d);
			title.init(d);
			tearOffEnabled.init(d);
			separatorsCollapsible.init(d);

			//struct CtorProcessor : public CtorProcessorT<Menu> {
			//	CtorProcessor(Menu&d): CtorProcessorT(d) {}
			//	void process(const QString & str) {
			//		_d->setTitle(str);
			//	}
			//} p(*this);
			//p.process_(args...);
		}
		void showUnder(QWidget*w);
		Menu& operator<<(QAction*act);

		PROPERTY_REDIRECT(QMenu, QIcon, icon, icon, setIcon);
		PROPERTY_REDIRECT(QMenu, QString, title, title, setTitle);
		PROPERTY_REDIRECTV(QMenu, bool, tearOffEnabled, isTearOffEnabled, setTearOffEnabled);
		PROPERTY_REDIRECTV(QMenu, bool, separatorsCollapsible, separatorsCollapsible, setSeparatorsCollapsible);

		EMBED_QPOINTER_AND_CAST(QMenu)
};