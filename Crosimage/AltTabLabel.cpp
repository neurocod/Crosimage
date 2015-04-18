//AltTabLabel.cpp by Kostya Kozachuck as neurocod
#include "pch.h"
#include "AltTabLabel.h"
#include "AltTabView.h"

AltTabLabel::AltTabLabel(AltTabView*parent, QWidget*wToActivate) {
	_parent = parent;
	_widToActivate = wToActivate;
	setText(_widToActivate->windowTitle());
	setMouseTracking(1);
	setFocusPolicy(Qt::WheelFocus);
}
AltTabLabel::~AltTabLabel() {
	_parent->deleted(this);
}
void AltTabLabel::keyPressEvent(QKeyEvent * event) {
	if(event->key()==Qt::Key_Tab || event->key()==Qt::Key_Escape) {
		__super::keyPressEvent(event);
		return;
	}
	_parent->clicked(_widToActivate);
}
void AltTabLabel::mousePressEvent(QMouseEvent * event) {
	__super::mousePressEvent(event);
	_parent->clicked(_widToActivate);
}
void AltTabLabel::mouseMoveEvent(QMouseEvent * event) {
	__super::mouseMoveEvent(event);
	_parent->select(this);
}
void AltTabLabel::paintEvent(QPaintEvent * e) {
	__super::paintEvent(e);
	if(hasFocus()) {
		QPainter p(this);
		p.setPen(QPen(Qt::red, 4));
		p.setBrush(Qt::NoBrush);
		p.drawRect(0,0, width(), height());
	}
}