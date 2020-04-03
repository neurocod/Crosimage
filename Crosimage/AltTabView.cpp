//AltTabView.cpp by Kostya Kozachuck as neurocod
#include "pch.h"
#include "AltTabView.h"
#include "AltTabLabel.h"
#include "CMainWindow.h"
#include "ThumbView.h"
#include "ImageView.h"

AltTabView::AltTabView() {
	setAttribute(Qt::WA_DeleteOnClose);
	_lay = new QGridLayout(this);
	_lay->setVerticalSpacing(0);
	_lay->setHorizontalSpacing(0);
	_lay->setContentsMargins(0,0,0,0);
	_nNextRow = _nNextCol = 0;
	{
		New<QTimer> timer(this);
		timer->start(500);
		connect(timer, &QTimer::timeout, this, &AltTabView::updateUI);
	}
	int count = topLevelWidgets().count();
	for(int i=2; ; ++i) {
		int test = i*i;
		if(test>=count) {
			_colCount = i;
			break;
		}
	}
	{
		Action a(QKeySequence("Esc"));
		a.connectClicks(this, &AltTabView::close);
		addAction(a);
	}
	updateUI();
}
AltTabView::~AltTabView() {
	qDeleteAll(_mains.values());//cause their dtors must unregister in this class
}
void AltTabView::showInstance() {
	New<AltTabView> w;
	w->showFullScreen();
}
void AltTabView::updateUI() {
	for(auto w: topLevelWidgets()) {
		if(_mains.contains(w))
			continue;
		auto la = new AltTabLabel(this, w);
		_mains[w] = la;
		_lay->addWidget(la, _nNextRow, _nNextCol++);
		if(_nNextCol>=_colCount) {
			_nNextCol = 0;
			_nNextRow++;
		}
	}
	int h = -1;
	int divider = qMax(_lay->rowCount(), _lay->columnCount());
	for(auto my: _mains.values()) {
		auto w = my->_widToActivate;
		if(!w) {
			delete my;
			continue;
		}
		QPixmap pix = grab(w);
		if(h<=1) {
			h = w->height() / divider;
		}
		auto la = _mains[w];
		if(h<1 || !la)
			continue;
		qreal dpiK = qApp->devicePixelRatio();
		QSize szScale = la->size();
		szScale.rwidth() *= dpiK;
		szScale.rheight() *= dpiK;
		la->setPixmap(pix.scaled(szScale, Qt::KeepAspectRatio));
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
	_selected = p;
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
	if(auto w = _mains.key(p, 0)) {
		_mains.remove(w);
	} else {
		ASSERT(0);
	}
}