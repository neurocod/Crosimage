AltTabView:://AltTabView.cpp by Kostya Kozachuck as neurocod - 2014.01.09 12:54:48
#include "pch.h"
#include "AltTabView.h"
#include "AltTabLabel.h"
#include "CMainWindow.h"
#include "ThumbView.h"
#include "ImageView.h"

AltTabView::AltTabView() {
	setAttribute(Qt::WA_DeleteOnClose);
	m_lay = new QGridLayout(this);
	m_lay->setVerticalSpacing(0);
	m_lay->setHorizontalSpacing(0);
	m_lay->setContentsMargins(0,0,0,0);
	m_nNextRow = m_nNextCol = 0;
	{
		New<QTimer> timer(this);
		timer->start(500);
		connect(timer, SIGNAL(timeout()), SLOT(updateUI()) );
	}
	int count = topLevelWidgets().count();
	for(int i=2; ; ++i) {
		int test = i*i;
		if(test>=count) {
			m_colCount = i;
			break;
		}
	}
	{
		Action a(QKeySequence("Esc"));
		a.connectClicks(this, SLOT(close()));
		addAction(a);
	}
	updateUI();
}
AltTabView::~AltTabView() {
	qDeleteAll(m_mains.values());//cause their dtors must unregister in this class
}
void AltTabView::showInstance() {
	New<AltTabView> w;
	w->showFullScreen();
}
void AltTabView::updateUI() {
	for(auto w: topLevelWidgets()) {
		if(m_mains.contains(w))
			continue;
		auto la = new AltTabLabel(this, w);
		m_mains[w] = la;
		m_lay->addWidget(la, m_nNextRow, m_nNextCol++);
		if(m_nNextCol>=m_colCount) {
			m_nNextCol = 0;
			m_nNextRow++;
		}
	}
	int h = -1;
	int divider = qMax(m_lay->rowCount(), m_lay->columnCount());
	for(auto my: m_mains.values()) {
		auto w = my->m_widToActivate;
		if(!w) {
			delete my;
			continue;
		}
		auto pix = grab(w);
		if(h<=1) {
			h = w->height() / divider;
		}
		auto la = m_mains[w];
		if(h<1 || !la)
			continue;
		la->setPixmap(pix.scaled(la->size(), Qt::KeepAspectRatio));
	}	
}
QWidgetList AltTabView::topLevelWidgets() {
	QWidgetList ret;
	for(auto w: QApplication::topLevelWidgets()) {
		if(qobject_cast<CMainWindow*>(w) || qobject_cast<ImageView*>(w))
			ret << w;
	}
	return ret;
}
void AltTabView::clicked(QWidget*w) {
	w->activateWindow();
	close();
}
void AltTabView::select(AltTabLabel*p) {
	m_selected = p;
	p->setFocus(Qt::MouseFocusReason);
}
QPixmap AltTabView::grab(QWidget*w) {
	if(auto mv = qobject_cast<CMainWindow*>(w)) {
		auto v = mv->view();
		auto pix = v->grab();
		int dx = v->verticalScrollBar()->width();
		if(!v->verticalScrollBar()->isVisible())
			dx = 0;
		if(dx>0)
			pix = pix.copy(0,0, pix.width()-dx, pix.height());
		if(v->verticalHeader()->isVisible())
			pix = pix.copy(v->verticalHeader()->width(), 0, pix.width(), pix.height());
		return pix;
	}
	return w->grab();
}
void AltTabView::deleted(AltTabLabel*p) {
	if(auto w = m_mains.key(p, 0)) {
		m_mains.remove(w);
	} else {
		ASSERT(0);
	}
}