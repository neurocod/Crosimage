//TabWidget.cpp by Kostya Kozachuck as neurocod - 08.12.2012 13:28:55
#include "pch.h"
#include "TabWidget.h"

TabWidget& TabWidget::operator<<(QWidget*tab) {
	d->addTab(tab, tab->windowIcon(), tab->windowTitle());
	return *this;
}
void TabWidget::setStyle() {
	d->tabBar()->setAutoFillBackground(true);
	d->setAutoFillBackground(true);
}