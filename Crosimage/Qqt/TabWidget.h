//TabWidget.h by Kostya Kozachuck as neurocod - 08.12.2012 13:28:55
#pragma once

class TabWidget: public WidgetPropertyRedirects {
	public:
		template<typename... Args>
		TabWidget(Args...args) : WidgetPropertyRedirects(new QTabWidget) {
			d = staticCast<QTabWidget*>();
			currentIndex.init(d);
			documentMode.init(d);
			elideMode.init(d);
			iconSize.init(d);
			movable.init(d);
			tabPosition.init(d);
			tabShape.init(d);
			tabsClosable.init(d);
			usesScrollButtons.init(d);

			setStyle();
			CtorProcessorT<TabWidget> p(*this);
			p.process_(args...);
		}

		//count : const int
		PROPERTY_REDIRECTV(QTabWidget, int, currentIndex, currentIndex, setCurrentIndex);
		PROPERTY_REDIRECTV(QTabWidget, bool, documentMode, documentMode, setDocumentMode);
		PROPERTY_REDIRECTV(QTabWidget, Qt::TextElideMode, elideMode, elideMode, setElideMode);
		PROPERTY_REDIRECT (QTabWidget, QSize, iconSize, iconSize, setIconSize);
		PROPERTY_REDIRECTV(QTabWidget, bool, movable, isMovable, setMovable);
		PROPERTY_REDIRECTV(QTabWidget, QTabWidget::TabPosition, tabPosition, tabPosition, setTabPosition);
		PROPERTY_REDIRECTV(QTabWidget, QTabWidget::TabShape, tabShape, tabShape, setTabShape);
		PROPERTY_REDIRECTV(QTabWidget, bool, tabsClosable, tabsClosable, setTabsClosable);
		PROPERTY_REDIRECTV(QTabWidget, bool, usesScrollButtons, usesScrollButtons, setUsesScrollButtons);

		void setStyle();
		TabWidget& operator<<(QWidget*tab);
		EMBED_QPOINTER_AND_CAST(QTabWidget)
};