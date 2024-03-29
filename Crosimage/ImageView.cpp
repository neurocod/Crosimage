﻿//ImageView.cpp by Kostya Kozachuck as neurocod
#include "pch.h"
#include "ImageView.h"
#include "ThumbModel.h"
#include "ThumbView.h"
#include "ThumbCache.h"
#include "CApplication.h"
#include "CMainWindow.h"
#include "TxtLnkProcessor.h"

template<class T>
T* findInParent(QWidget*w) {
	if (!w)
		return 0;
	for (QWidget* p = w->parentWidget(); p; p = p->parentWidget()) {
		if (T* t = dynamic_cast<T*>(p))
			return t;
	}
	return 0;
}
ImageView::ImageView(ThumbModel*parent, ThumbView*view, QString file): _parent(parent) {
	_parentView = view;
	_bShowOther = true;
	_bFitScreen = true;
	setAttribute(Qt::WA_DeleteOnClose);
	showFullScreen();
	resetAndSelect(file);
	connect(_parent, &ThumbModel::modelReset, this, [this](){ resetAndSelect({}); });
	
	//setContextMenuPolicy(Qt::ActionsContextMenu);
	{
		setMouseTracking(true);
		_timeMouseMoved = QTime::currentTime();
		New<QTimer> timer(this);
		connect(timer, &QTimer::timeout, this, &ImageView::onTimer);
		timer->start(500);
	}
	{
		Action a(tr("Copy path"), QKeySequence("Ctrl+C"));
		a.connectClicks(this, &ImageView::copyPath);
		addAction(a);
	}
	{
		Action a(tr("Go up"), QKeySequence("Alt+Up"));
		QObject::connect(a, &QAction::triggered, this, [this]() {
			if (auto w = findInParent<CMainWindow>(_parentView))
				w->goUp();
		});
		addAction(a);
	}
	{
		Action a(tr("Open linked file"));
		a.connectClicks(this, &ImageView::openLinkedFile);
		_actOpenLinkedFile = a;
		addAction(a);
	}
	{
		Action a(tr("Close this window"), QList<QKeySequence>() << QKeySequence("Esc") << QKeySequence("Enter"));
		a.connectClicks(this, &ImageView::close);
		addAction(a);
	}
	{
		Action a(tr("First image"), QKeySequence("Home"));
		QObject::connect(a, &QAction::triggered, this, [this]() {
			_indexInParent = 0;
			navigate();
		});
		addAction(a);
	}
	{
		Action a(tr("Last image"), QKeySequence("End"));
		QObject::connect(a, &QAction::triggered, this, [this]() {
			_indexInParent = _parent->items().count()-1;
			navigate();
		});
		addAction(a);
	}
	{
		Action a(tr("Prev image"), QList<QKeySequence>() << QKeySequence("PgUp") << QKeySequence("Left"));
		QObject::connect(a, &QAction::triggered, this, [this]() {
			navigate(-1);
		});
		addAction(a);
	}
	{
		Action a(tr("Next image"), QList<QKeySequence>() << QKeySequence("PgDown") << QKeySequence("Right") << QKeySequence("Space"));
		QObject::connect(a, &QAction::triggered, this, [this]() {
			navigate(1);
		});
		addAction(a);
	}
	//{
	//	Action a(tr("Scale +"), QKeySequence("+"));
	//	QObject::connect(a, &QAction::triggered, this, [this]() {
	//		//navigate(1);
	//	});
	//	addAction(a);
	//}
	//{
	//	Action a(tr("Scale -"), QKeySequence("-"));
	//	QObject::connect(a, &QAction::triggered, this, [this]() {
	//		//navigate(1);
	//	});
	//	addAction(a);
	//}
	//{
	//	Action a(tr("Scale 1:1"), QKeySequence("="));
	//	QObject::connect(a, &QAction::triggered, this, [this]() {
	//		//navigate(1);
	//	});
	//	addAction(a);
	//}
	{
		Action a(tr("Fullscreen/Normal"), QKeySequence("F"));
		QObject::connect(a, &QAction::triggered, this, [this]() {
			if(isFullScreen())
				showNormal();
			else
				showFullScreen();
		});
		addAction(a);
	}
	{
		Action a(tr("Single/multiple images"), QList<QKeySequence>() << QKeySequence("1") << QKeySequence("Ctrl+Down"));
		QObject::connect(a, &QAction::triggered, this, [this]() {
			_bShowOther = !_bShowOther;
			navigate();
		});
		addAction(a);
	}
	{
		Action a(tr("Fit screen/Normal size"), QList<QKeySequence>() << QKeySequence("*") << QKeySequence("Ctrl+Up"));
		QObject::connect(a, &QAction::triggered, this, [this]() {
			_bFitScreen = !_bFitScreen;
			navigate();
		});
		addAction(a);
	}
	{
		Action a(tr("View externally"), QList<QKeySequence>() << QKeySequence("F3") << QKeySequence("Alt+1"));
		a.connectClicks(this, &ImageView::viewExternally);
		addAction(a);
	}
	{
		Action a(tr("Edit externally"), QKeySequence("Alt+2"));
		a.connectClicks(this, &ImageView::editExternally);
		addAction(a);
	}
	{
		Action a(tr("Open in explorer"), QKeySequence("Alt+3"), QIcon(":/qt-project.org/styles/commonstyle/images/standardbutton-open-32.png"));
		a.connectClicks(this, &ImageView::openInExplorer);
		addAction(a);
	}
	{
		Action a(tr("Lock window size"), QKeySequence("L"));//to fight with MS Windows automatic resize at screen borders
		QObject::connect(a, &QAction::triggered, this, [this]() {
			auto sz = minimumSize();
			if(sz==QSize(0, 0)) { //not locked, lock
				sz = size();
				setMinimumSize(sz);
				setMaximumSize(sz);
			} else {//unlock
				setMinimumSize(QSize(0, 0));
				setMaximumSize(QSize(0xffffff, 0xffffff));
			}
		});
		addAction(a);
	}
	addActions(CApplication::s_inst->globalActions());
}
ImageView::~ImageView() {
	if(_parentView && !_images.isEmpty()) {
		_parentView->select(_images[0].file);
	}
}
void ImageView::resetAndSelect(const QString& file) {
	_files = _parent->files();
	if (_files.isEmpty() && !_parent->items().isEmpty()) { // dirs only
		close();
		return;
	}
	removeUnsupportedFiles();
	if (file.isEmpty()) {
		_indexInParent = _files.isEmpty() ? -1 : 0;
	} else {
		QFileInfo info(file);
		_indexInParent = _files.indexOf(info.absoluteFilePath());
	}
	if(-1==_indexInParent)
		_indexInParent = 0;
	navigate();
}
void ImageView::paintEvent(QPaintEvent * event) {
	const QRect rc = rect();
	QPainter painter(this);
	painter.setClipRect(rc);
	painter.fillRect(rc, Qt::black);
	if(_images.isEmpty())
		return;
	Image & img = _images[0];
	QSize sz;
	if(_bFitScreen)
		sz = rc.size();
	else
		sz = img.orig.size();
	if(img.szNeeed!=sz) {
		img.szNeeed = sz;
		img.scaled = img.orig.scaled(sz, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	}
	int x = 0;
	if(img.scaled.width()<rc.width())
		x += (rc.width() - img.scaled.width())/2;
	int y = 0;
	if(img.scaled.height()<rc.height())
		y += (rc.height()-img.scaled.height())/2;
	painter.drawPixmap(x, y, img.scaled);
	if(!_bShowOther)
		return;
	if(_images.size()>1) {
		auto & img = _images[1].orig;
		int y2 = 0;
		if(img.height()<rc.height())
			y2 = (rc.height()-img.height())/2;
		painter.drawPixmap(x-img.width(), y2, img);
	}
	if(_images.size()>2) {
		auto & img = _images[2].orig;
		int y2 = 0;
		if(img.height()<rc.height())
			y2 = (rc.height()-img.height())/2;
		painter.drawPixmap(x + _images[0].scaled.width(), y2, img);
	}
}
void ImageView::show(const QStringList & files) {
	_images.clear();
	for(auto file: files) {
		if(file.isEmpty()) {
			_images << Image();
			continue;
		}
		QImageReader reader(file);
		QImage img;
		if(!reader.read(&img)) {
			QFileInfo fileInfo(file);
			img = ThumbCache::instance().get(fileInfo, fileInfo.absoluteFilePath());
		}
		Image image;
		image.orig = QPixmap::fromImage(img);
		image.file = file;
		_images << image;
	}
	update();
}
void ImageView::navigate(int step) {
	auto files = _files;
	int test = _indexInParent + step;
	test = qBound(0, test, files.count()-1);
	if(test>=files.count())
		return;
	_indexInParent = test;
	QString path = files[test];
	QStringList li;
	li << path;
	if(_indexInParent>0)
		li << files[_indexInParent-1];
	else
		li << "";
	if(_indexInParent + 1 < files.count())
		li << files[_indexInParent+1];
	else
		li << "";
	show(li);
}
void ImageView::wheelEvent(QWheelEvent * event) {
	auto y = event->angleDelta().y();
	navigate(y >0 ? -1 : 1);
}
void ImageView::onTimer() {
	if(_timeMouseMoved.secsTo(QTime::currentTime())>=1)
		setCursor(Qt::BlankCursor);
}
void ImageView::mouseMoveEvent(QMouseEvent *e) {
	setCursor(Qt::ArrowCursor);
	_timeMouseMoved = QTime::currentTime();
	__super::mouseMoveEvent(e);
}
void ImageView::viewExternally() {
	if(!_images.isEmpty())
		CApplication::viewExternally(_images[0].file);
}
void ImageView::editExternally() {
	if(!_images.isEmpty())
		CApplication::editExternally(_images[0].file);
}
void ImageView::openInExplorer() {
	if(!_images.isEmpty())
		FileFacility::showDirWithFile(_images[0].file);
}
void ImageView::mouseDoubleClickEvent(QMouseEvent* event) {
	__super::close();
}
void ImageView::contextMenuEvent(QContextMenuEvent * event) {
	QMenu m;
	auto li = actions();
	if(findLinkedFile(centerFile()).isEmpty())
		li.removeOne(_actOpenLinkedFile);
	m.addActions(li);
	m.exec(event->pos());
}
void ImageView::openLinkedFile() {
	QString str = findLinkedFile(centerFile());
	if(str.isEmpty())
		return;
	QDesktopServices::openUrl(QUrl::fromLocalFile(str));
}
QString ImageView::findLinkedFile(QString str) {
	return TxtLnkProcessor::existingLinkedFileFromParentDir(str);
}
QString ImageView::centerFile()const {
	if(_images.isEmpty())
		return QString();
	return _images[0].file;
}
void ImageView::copyPath() {
	qApp->clipboard()->setText(centerFile());
}
void ImageView::removeUnsupportedFiles() {
	for(auto it = _files.begin(); it!=_files.end(); ) {
		QString str = *it;
		if(isSupportedFile(str))
			++it;
		else
			it = _files.erase(it);
	}
}
bool ImageView::isSupportedFile(const QString & path)const {
	auto str = path;
	int index = str.lastIndexOf('/');
	if(-1!=index)
		str = str.mid(index+1);
	index = str.lastIndexOf('\\');
	if(-1!=index)
		str = str.mid(index+1);
	index = str.lastIndexOf('.');
	if(-1!=index)
		str = str.mid(index+1);
	if(str.isEmpty())
		return false;
	if(str.endsWith("sld"))
		return false;
	static const QSet<QString> formats = []() {
		QSet<QString> ret;
		for(auto arr: QImageReader::supportedImageFormats()) {
			ret << QString::fromUtf8(arr);
		}
		return ret;
	}();
	str = str.toLower();
	return formats.contains(str);
}