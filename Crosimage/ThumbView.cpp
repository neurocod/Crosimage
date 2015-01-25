//ThumbView.cpp by Kostya Kozachuck as neurocod
#include "pch.h"
#include "ThumbView.h"
#include "ThumbModel.h"
#include "ThumbDelegate.h"
#include "CMainWindow.h"
#include "CApplication.h"
#include "ThumbCache.h"

ThumbView::ThumbView(ThumbModel*m) {
	_model = m;
	auto pal = palette();
	pal.setColor(QPalette::Base, Qt::black);
	pal.setColor(QPalette::Text, Qt::white);
	setPalette(pal);
	setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
	horizontalHeader()->hide();
	horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	horizontalHeader()->setDefaultSectionSize(ThumbModel::s_nThumbW);
	verticalScrollBar()->setSingleStep(40);
	verticalHeader()->setDefaultSectionSize(ThumbModel::s_nThumbH);
	verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	verticalHeader()->hide();
	setItemDelegate(new ThumbDelegate(m));
	setModel(m);
	connect(this, SIGNAL(doubleClicked(const QModelIndex &)), SLOT(onDoubleClicked(const QModelIndex &)) );
	connect(_model, SIGNAL(modelReset()), SLOT(onModelReset()) );
	{
		Action a(tr("Show fullscreen"));
		a->setShortcuts(QList<QKeySequence>() << QKeySequence("Enter") << QKeySequence("Return"));
		a.connectClicks(this, SLOT(onDoubleClicked()));
		addAction(a);
	}
	{
		Action a(tr("View externally"), QKeySequence("Alt+1"));
		a.connectClicks(this, SLOT(viewExternally()));
		addAction(a);
	}
	{
		Action a(tr("Edit externally"), QKeySequence("Alt+2"));
		a.connectClicks(this, SLOT(editExternally()));
		addAction(a);
	}
	{
		Action a(tr("Rebuild thumbnail"));
		a.connectClicks(this, SLOT(rebuildThumbnail()));
		addAction(a);
	}
	setContextMenuPolicy(Qt::ActionsContextMenu);
}
void ThumbView::onDoubleClicked() {
	onDoubleClicked(currentIndex());
}
void ThumbView::onDoubleClicked(const QModelIndex & index) {
	auto item = _model->itemBy(index);
	if(!item)
		return;
	CMainWindow::instance(this)->go(item->absoluteFilePath());
}
QString ThumbView::selectedItemFilePath() {
	auto index = currentIndex();
	if(!index.isValid())
		return QString();
	auto item = _model->itemBy(index);
	if(!item)
		return QString();
	auto path = item->absoluteFilePath();
	return path;
}
void ThumbView::select(QString file) {
	QFileInfo info(file);
	int nIndex = 0;
	for(auto item: _model->items()) {
		QString path = item->filePath();
		if(info==*item) {
			auto index = _model->indexByIntIndex(nIndex);
			setCurrentIndex(index);
			break;
		}
		nIndex++;
	}
}
void ThumbView::onModelReset() {
	if(!_fileToSelect.isEmpty())
		select(_fileToSelect);
}
void ThumbView::selectLater(QString file) {
	_fileToSelect = file;
}
void ThumbView::viewExternally() {
	auto path = selectedItemFilePath();
	if(path.isEmpty())
		return;
	CApplication::viewExternally(path);
}
void ThumbView::editExternally() {
	auto path = selectedItemFilePath();
	if(path.isEmpty())
		return;
	CApplication::editExternally(path);
}
void ThumbView::rebuildThumbnail() {
	auto path = selectedItemFilePath();
	if(path.isEmpty())
		return;
	ThumbCache::instance().rebuild(path);
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
		rows = _model->rowCount();
	if(cols<0)
		cols = _model->columnCount();
	if(row>=0 && col>=0 && row<=rows && col<=cols) {
		auto i0 = _model->index(row, col);
		auto i1 = _model->index(rows, cols);
		emit _model->dataChanged(i0, i1);
	}
}
void ThumbView::resizeEvent(QResizeEvent *event) {
	__super::resizeEvent(event);
	int w = event->size().width();
	int cols = qMax<int>(1, w/ThumbModel::s_nThumbW);
	_model->setColumnCount(cols);
}
int ThumbView::sizeHintForRow(int row)const {
	return _model->rowHeight(row);
}