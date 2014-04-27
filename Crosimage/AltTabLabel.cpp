//AltTabLabel.cpp by Kostya Kozachuck as neurocod - 2014.01.09 14:05:36
#include "pch.h"
#include "AltTabLabel.h"
#include "AltTabView.h"

AltTabLabel::AltTabLabel(AltTabView*parent, QWidget*wToActivate) {
	m_parent = parent;
	m_widToActivate = wToActivate;
	setText(m_widToActivate->windowTitle());
	setMouseTracking(1);
	setFocusPolicy(Qt::WheelFocus);
}
AltTabLabel::~AltTabLabel() {
	m_parent->deleted(this);
}
void AltTabLabel::keyPressEvent(QKeyEvent * event) {
	if(event->key()==Qt::Key_Tab || event->key()==Qt::Key_Escape) {
		__super::keyPressEvent(event);
		return;
	}
	m_parent->clicked(m_widToActivate);
}
void AltTabLabel::mousePressEvent(QMouseEvent * event) {
	__super::mousePressEvent(event);
	m_parent->clicked(m_widToActivate);
}
void AltTabLabel::mouseMoveEvent(QMouseEvent * event) {
	__super::mouseMoveEvent(event);
	m_parent->select(this);
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