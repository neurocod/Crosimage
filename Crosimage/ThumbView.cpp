//ThumbView.cpp by Kostya Kozachuck as neurocod
#include "pch.h"
#include "ThumbView.h"
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
	horizontalHeader()->setDefaultSectionSize(CrSettings::inst()._thumbW);
	verticalScrollBar()->setSingleStep(40);
	verticalHeader()->setDefaultSectionSize(CrSettings::inst()._thumbH);
	verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	verticalHeader()->hide();
	setItemDelegate(new ThumbDelegate(m));
	setModel(m);
	connect(this, SIGNAL(doubleClicked(const QModelIndex &)), SLOT(onDoubleClicked(const QModelIndex &)) );
	connect(_model, SIGNAL(modelReset()), SLOT(onModelReset()) );
	{
		Action a(tr("Show fullscreen"),
			QList<QKeySequence>() << QKeySequence("F3") << QKeySequence("Enter") << QKeySequence("Return"));
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
		Action a(tr("Open in explorer"), QKeySequence("Alt+3"), QIcon(":/qt-project.org/styles/commonstyle/images/standardbutton-open-32.png"));
		a.connectClicks(this, SLOT(openInExplorer()));
		addAction(a);
	}
	{
		Action a(tr("Rebuild thumbnail"));
		a.connectClicks(this, SLOT(rebuildThumbnail()));
		addAction(a);
	}
	{
		Action a(tr("Copy path"), QKeySequence("Ctrl+C"));
		a.connectClicks(this, SLOT(copyPath()));
		addAction(a);
	}
	{
		Action a(tr("New dir"), QList<QKeySequence>() << QKeySequence("F7") << QKeySequence("Ctrl+Shift+N"));
		a.connectClicks(this, SLOT(newDir()));
		addAction(a);
	}
	{
		Action a(tr("Delete file"), QKeySequence("Shift+Del"));
		a.connectClicks(this, SLOT(deleteFile()));
		addAction(a);
	}
	{
		Action a(tr("Move files..."), QKeySequence("F6"));
		a.connectClicks(this, SLOT(moveFiles()));
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
void ThumbView::select(const QString & file) {
	QFileInfo info(file);
	QString abs = info.absoluteFilePath();
	const auto & items = _model->items();
	for(int n = 0; n<items.count(); ++n) {
		const auto & item = items.at(n);
		if(abs==item->absoluteFilePath()) {
			auto index = _model->indexByIntIndex(n);
			setCurrentIndex(index);
			break;
		}
	}
}
void ThumbView::onModelReset() {
	if(!_fileToSelect.isEmpty())
		select(_fileToSelect);
}
void ThumbView::selectLater(const QString & file) {
	_fileToSelect = file;
}
void ThumbView::viewExternally() {
	for(auto item: selectedItems()) {
		CApplication::viewExternally(item->absoluteFilePath());
	}
}
void ThumbView::editExternally() {
	for(auto item: selectedItems()) {
		CApplication::editExternally(item->absoluteFilePath());
	}
}
void ThumbView::rebuildThumbnail() {
	for(auto item: selectedItems()) {
		ThumbCache::instance().rebuild(item->absoluteFilePath());
	}
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
	int cols = qMax<int>(1, w/CrSettings::inst()._thumbW);
	_model->setColumnCount(cols);
}
int ThumbView::sizeHintForRow(int row)const {
	return _model->rowHeight(row);
}
void ThumbView::deleteFile() {
	QList<ThumbModel::Item*> items = selectedItems();
	if(items.isEmpty())
		return;
	QString str;
	if(items.size()==1) {
		str = tr("Do you realy want to delete the selected file %1?").arg(items[0]->fileName());
	} else {
		str = tr("Do you realy want to delete the %1 selected files?\n\n").arg(items.count());
		for(int i = 0; i<items.count(); ++i) {
			if(i>=10) {
				str += tr("...\nAnd %1 more").arg(items.count()-i);
				break;
			}
			str += items[i]->fileName() + '\n';
		}
	}
	//if(QMessageBox::Yes!=QMessageBox::question(this, tr("Delete file"), str, StandardButtons buttons = StandardButtons(Yes | No), StandardButton defaultButton = NoButton)[static]
}
QList<ThumbModel::Item*> ThumbView::selectedItems()const {
	QList<ThumbModel::Item*> items;
	for(auto index: selectedIndexes())
		if(auto item = _model->itemBy(index))
			items << item;
	return items;
}
void ThumbView::copyPath() {
	QList<ThumbModel::Item*> items = selectedItems();
	if(items.isEmpty())
		return;
	QString str;
	for(auto i: items) {
		if(!str.isEmpty())
			str += '\n';
		str += i->absoluteFilePath();
	}
	qApp->clipboard()->setText(str);
}
void ThumbView::newDir() {
	QString name= QInputDialog::getText(this, tr("New dir"), tr("New dir name:"));
	if(name.isEmpty())
		return;
	bool b = _model->dir().mkdir(name);
	if(b)
		_model->refresh();
}
void ThumbView::openInExplorer() {
	QString file = selectedItemFilePath();
	if(file.isEmpty()) {
		FileFacility::showDirWithFile(_model->dir().absolutePath());
	} else {
		FileFacility::showDirWithFile(file);
	}
}
void ThumbView::moveFiles() {
	auto selected = selectionModel()->selectedIndexes();
	if(selected.isEmpty())
		return;
	static QMap<QString, QString> dirsToMove;
	QString curDir = _model->dir().path();
	QString & dirToMove = dirsToMove[curDir];
	if(dirToMove.isEmpty()) {
		dirToMove = QFileDialog::getExistingDirectory(this, tr("Dir to move files:"), curDir);
	}
	if(dirToMove.isEmpty())
		return;
	QDir dir2(dirToMove);
	for(auto index: selected) {
		const QFileInfo & file = _model->itemBy(index)->fileInfo();
		QString from = file.absoluteFilePath();
		QString to = dir2.absoluteFilePath(file.fileName());
		bool b = QDir().rename(from, to);
		if(!b) {
			msgBox(tr("error moving %1 -> %2").arg(from, to));
			break;
		}
	}
	_model->refresh();
}