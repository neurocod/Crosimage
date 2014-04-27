//ThumbWorker.cpp by Kostya Kozachuck as neurocod
#include "pch.h"
#include "ThumbWorker.h"
#include "ThumbModel.h"
#include "ThumbDirPainter.h"
#include "ImgDbWorker.h"

ThumbWorker & ThumbWorker::instance() {
	static ThumbWorker p;
	return p;
}
ThumbWorker::ThumbWorker() {
	m_nThumbnailsCreated = 0;
	m_nFilesRead = 0;
	m_bNeedExit = false;
	m_bStarted = false;
	moveToThread(this);
	start();
	while(!m_bStarted)
		msleep(1);
}
void ThumbWorker::exit() {
	m_bNeedExit = true;
	__super::exit();
	wait(10000);
}
void ThumbWorker::makeFirst(const QString & path) {
	//return;
	QMutexLocker lock(&m_lock);
	int index = m_queue.indexOf(path);
	if(-1==index || 0==index) {
		return;
	}
	m_queue.QList::swap(0, index);
}
void ThumbWorker::takeFile(const QString & path) {
	QMutexLocker lock(&m_lock);
	m_queue.prepend(path);
}
void ThumbWorker::maybeUpdate(bool innerCall, const QString & path, const QList<QImage> & images) {
	if(innerCall || m_bNeedExit)
		return;
	emit update(path, ThumbDirPainter::compose(images));
}
void ThumbWorker::maybeUpdate(bool innerCall, const QString & path, const QImage & image) {
	if(innerCall)
		return;
	emit update(path, image);
}
QImage ThumbWorker::processNextFile(const QString & path, bool innerCall) {
	if(m_bNeedExit)
		return QImage();
	m_nThumbnailsCreated++;
	QFileInfo info(path);
	QImage ret;
	if(ImgDbWorker::thumbnail(info, ret)) {
		maybeUpdate(innerCall, path, ret);
		return ret;
	}
	if(!info.isDir()) {
		ret = thumb(path);
		maybeUpdate(innerCall, path, ret);
		writeToDb(innerCall, info, ret);
		return ret;
	}
	maybeUpdate(innerCall, path, ThumbDirPainter::dirStub());
	QDir dir(path);
	QList<QImage> images;
	QStringList subDirs;
	const int targImagesCount = 6;
	for(QDirIterator it(path); images.size()<targImagesCount && it.hasNext(); ) {
		if(m_bNeedExit)
			return QImage();
		QString sub = it.next();
		if(it.fileName()=="." || it.fileName()=="..")
			continue;
		m_nFilesRead++;
		auto img = thumb(sub);
		if(img.isNull()) {
			if(subDirs.count()<targImagesCount && it.fileInfo().isDir())
				subDirs << sub;
		} else {
			images << img;
			maybeUpdate(innerCall, path, images);//чтобы видеть промежуточные результаты и почему так долго считает
		}
	}
	int nDirs = targImagesCount - images.count();
	nDirs = qMin(nDirs, subDirs.count());
	nDirs = qMin(nDirs, 4);//c учетом идеальных пропорций, больше и не будет показано на экране
	if(!innerCall && nDirs>0) {
		auto images2 = images;
		for(int i=0; i<nDirs; ++i) {
			images2 << ThumbDirPainter::subDirThumb();
		}
		maybeUpdate(innerCall, path, images2);
	}
	for(int i=0; i<nDirs; ++i) {
		if(m_bNeedExit)
			return QImage();
		if(innerCall) {
			images << ThumbDirPainter::subDirThumb();
		} else {
			images << processNextFile(subDirs[i], true);
			maybeUpdate(innerCall, path, images);
		}
	}
	ret = ThumbDirPainter::compose(images);
	maybeUpdate(innerCall, path, ret);
	writeToDb(innerCall, info, ret);
	return ret;
}
QImage ThumbWorker::thumb(const QString & path) {
	QImageReader reader(path);
	QImage img;
	QImage thumb;
	if(reader.read(&img)) {
		if(img.width()>ThumbModel::s_nThumbW || img.height()>ThumbModel::s_nThumbH)
			thumb = img.scaled(ThumbModel::s_nThumbW, ThumbModel::s_nThumbH, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		else
			thumb = img;
	}
	return thumb;
}
void ThumbWorker::run() {
	m_bStarted = true;
	while(!m_bNeedExit) {
		QString path;
		{
			QMutexLocker lock(&m_lock);
			if(!m_queue.isEmpty()) {
				path = m_queue.dequeue();
			}
		}
		if(path.isEmpty())
			msleep(10);
		else
			processNextFile(path);
	}
}
void ThumbWorker::writeToDb(bool innerCall, const QFileInfo & info, const QImage & image) {
	if(innerCall || m_bNeedExit)
		return;
	ImgDbWorker::setThumbnail(info, image);
}