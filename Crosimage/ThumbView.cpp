//ThumbView.cpp by Kostya Kozachuck as neurocod
#include "pch.h"
#include "ThumbView.h"
#include "ThumbModel.h"
#include "ThumbDelegate.h"
#include "CMainWindow.h"
#include "CApplication.h"

ThumbView::ThumbView(ThumbModel*m) {
	m_model = m;
	auto pal = palette();
	pal.setColor(QPalette::Base, Qt::black);
	pal.setColor(QPalette::Text, Qt::white);
	setPalette(pal);
	setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
	verticalScrollBar()->setSingleStep(40);
	horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	horizontalHeader()->setDefaultSectionSize(ThumbModel::s_nThumbW);
	verticalHeader()->setDefaultSectionSize(ThumbModel::s_nThumbH);
	horizontalHeader()->hide();
	setItemDelegate(new ThumbDelegate(m));
	setModel(m);
	connect(this, SIGNAL(doubleClicked(const QModelIndex &)), SLOT(onDoubleClicked(const QModelIndex &)) );
	connect(m_model, SIGNAL(modelReset()), SLOT(onModelReset()) );
	{
		Action a(tr("Show fullscreen"));
		a->setShortcuts(QList<QKeySequence>() << QKeySequence("Enter") << QKeySequence("Return"));
		a.connectClicks(this, SLOT(onDoubleClicked()));
		addAction(a);
	}
	{
		Action a(tr("Edit externally"), QKeySequence("Alt+2"));
		a.connectClicks(this, SLOT(editExternally()));
		addAction(a);
	}
	{
		Action a(tr("View externally"), QKeySequence("Alt+1"));
		a.connectClicks(this, SLOT(viewExternally()));
		addAction(a);
	}
	setContextMenuPolicy(Qt::ActionsContextMenu);
}
void ThumbView::onDoubleClicked() {
	onDoubleClicked(currentIndex());
}
void ThumbView::onDoubleClicked(const QModelIndex & index) {
	auto item = m_model->itemAt(index);
	if(!item)
		return;
	CMainWindow::instance(this)->go(item->absoluteFilePath());
}
QString ThumbView::selectedItemFilePath() {
	auto index = currentIndex();
	if(!index.isValid())
		return QString();
	auto item = m_model->itemAt(index);
	if(!item)
		return QString();
	auto path = item->absoluteFilePath();
	return path;
}
void ThumbView::select(QString file) {
	QFileInfo info(file);
	int nIndex = 0;
	for(auto item: m_model->items()) {
		QString path = item->filePath();
		if(info==*item) {
			auto index = m_model->indexByIntIndex(nIndex);
			setCurrentIndex(index);
			break;
		}
		nIndex++;
	}
}
void ThumbView::onModelReset() {
	if(!m_fileToSelect.isEmpty())	
		select(m_fileToSelect);
}
void ThumbView::selectLater(QString file) {
	m_fileToSelect = file;	
}
void ThumbView::viewExternally() {
	auto s = selectedItemFilePath();
	CApplication::viewExternally(s);
}
void ThumbView::editExternally() {
	auto s = selectedItemFilePath();
	CApplication::editExternally(s);
}
void ThumbView::prioritizeThumbs() {
	auto r = rect();
	auto p0 = r.topLeft();
	auto p1 = r.bottomRight();
	auto vh = verticalHeader();
	auto hh = horizontalHeader();
	int row = vh->logicalIndexAt(p0.y());
	int col = hh->logicalIndexAt(p0.x());
	int rows = vh->logicalIndexAt(p1.y());
	int cols = hh->logicalIndexAt(p1.x());
	if(rows<0)
		rows = m_model->rowCount();
	if(cols<0)
		cols = m_model->columnCount();
	if(row>=0 && col>=0 && row<=rows && col<=cols) {
		auto i0 = m_model->index(row, col);
		auto i1 = m_model->index(rows, cols);
		emit m_model->dataChanged(i0, i1);
	}
}
void ThumbView::resizeEvent(QResizeEvent *event) {
	__super::resizeEvent(event);
	int w = event->size().width();
	int cols = qMax<int>(1, w/ThumbModel::s_nThumbW);
	m_model->setColumnCount(cols);
}