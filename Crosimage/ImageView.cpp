//ImageView.cpp by Kostya Kozachuck as neurocod
#include "pch.h"
#include "ImageView.h"
#include "ThumbModel.h"
#include "ThumbView.h"
#include "ThumbCache.h"
#include "CApplication.h"
#include "CMainWindow.h"
#include "TxtLnkProcessor.h"

ImageView::ImageView(ThumbModel*parent, ThumbView*view, QString file): _parent(parent) {
	_parentView = view;
	_bShowOther = true;
	_bFitScreen = true;
	QFileInfo info(file);
	ASSERT(info.dir().absolutePath()==parent->dir().absolutePath());
	setAttribute(Qt::WA_DeleteOnClose);
	showFullScreen();
	connect(parent, SIGNAL(modelReset()), SLOT(onModelReset()) );
	_files = _parent->files();
	_indexInParent = _files.indexOf(info.absoluteFilePath());
	if(-1==_indexInParent)
		_indexInParent = 0;
	navigate();
	//setContextMenuPolicy(Qt::ActionsContextMenu);
	{
		setMouseTracking(true);
		_timeMouseMoved = QTime::currentTime();
		New<QTimer> timer(this);
		connect(timer, SIGNAL(timeout()), SLOT(onTimer()) );
		timer->start(500);
	}
	{
		Action a(tr("Copy path"), QKeySequence("Ctrl+C"));
		a.connectClicks(this, SLOT(copyPath()));
		addAction(a);
	}
	{
		Action a(tr("Open linked file"));
		a.connectClicks(this, SLOT(openLinkedFile()));
		_actOpenLinkedFile = a;
		addAction(a);
	}
	{
		Action a(tr("Close this window"), QList<QKeySequence>() << QKeySequence("Esc") << QKeySequence("Alt+Up"));
		a.connectClicks(this, SLOT(close()));
		addAction(a);
	}
	{
		Action a(tr("First image"), QKeySequence("Home"));
		QObject::connect(a, &QAction::triggered, this, [=]() {
				this->_indexInParent = 0;
				this->navigate();
			});
		addAction(a);
	}
	{
		Action a(tr("Last image"), QKeySequence("End"));
		QObject::connect(a, &QAction::triggered, this, [=]() {
				this->_indexInParent = this->_parent->items().count()-1;
				this->navigate();
			});
		addAction(a);
	}
	{
		Action a(tr("Prev image"), QKeySequence("PgUp"));
		QObject::connect(a, &QAction::triggered, this, [=]() {
				this->navigate(-1);
			});
		addAction(a);
	}
	{
		Action a(tr("Next image"), QList<QKeySequence>() << QKeySequence("PgDown") << QKeySequence("Space"));
		QObject::connect(a, &QAction::triggered, this, [=]() {
				this->navigate(1);
			});
		addAction(a);
	}
	{
		Action a(tr("Scale +"), QKeySequence("+"));
		QObject::connect(a, &QAction::triggered, this, [=]() {
			//this->navigate(1);
		});
		addAction(a);
	}
	{
		Action a(tr("Scale -"), QKeySequence("-"));
		QObject::connect(a, &QAction::triggered, this, [=]() {
			//this->navigate(1);
		});
		addAction(a);
	}
	{
		Action a(tr("Scale 1:1"), QKeySequence("="));
		QObject::connect(a, &QAction::triggered, this, [=]() {
			//this->navigate(1);
		});
		addAction(a);
	}
	{
		Action a(tr("Fullscreen/Normal"), QKeySequence("F"));
		QObject::connect(a, &QAction::triggered, this, [=]() {
				if(this->isFullScreen())
					this->showNormal();
				else
					this->showFullScreen();
			});
		addAction(a);
	}
	{
		Action a(tr("Single/multiple images"), QList<QKeySequence>() << QKeySequence("1") << QKeySequence("Ctrl+Down"));
		QObject::connect(a, &QAction::triggered, this, [=]() {
				this->_bShowOther = !this->_bShowOther;
				this->navigate();
			});
		addAction(a);
	}
	{
		Action a(tr("Fit screen/Normal size"), QList<QKeySequence>() << QKeySequence("*") << QKeySequence("Ctrl+Up"));
		QObject::connect(a, &QAction::triggered, this, [=]() {
				this->_bFitScreen = !this->_bFitScreen;
				this->navigate();
			});
		addAction(a);
	}
	{
		Action a(tr("View externally"), QList<QKeySequence>() << QKeySequence("F3") << QKeySequence("Alt+1"));
		a.connectClicks(this, SLOT(viewExternally()));
		addAction(a);
	}
	{
		Action a(tr("Edit externally"), QKeySequence("Alt+2"));
		a.connectClicks(this, SLOT(editExternally()));
		addAction(a);
	}
	addActions(CApplication::s_inst->globalActions());
}
ImageView::~ImageView() {
	if(_parentView && !_images.isEmpty()) {
		_parentView->select(_images[0].file);
	}
}
void ImageView::onModelReset() {
	//зачем это надо? отображаемая папка изменится, фигня
	///_indexInParent = 0;
	///navigate(0);
}
void ImageView::paintEvent(QPaintEvent * event) {
	auto rc = rect();
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
	navigate(event->delta()>0 ? -1 : 1);
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