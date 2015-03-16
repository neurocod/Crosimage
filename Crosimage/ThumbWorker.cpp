﻿//ThumbWorker.cpp by Kostya Kozachuck as neurocod
#include "pch.h"
#include "ThumbWorker.h"
#include "ThumbModel.h"
#include "ThumbDirPainter.h"
#include "ImgDbWorker.h"
#include "ThumbVideoWorker.h"
#include "TxtLnkProcessor.h"

ThumbWorker & ThumbWorker::instance() {
	static ThumbWorker p;
	return p;
}
ThumbWorker::ThumbWorker() {
	_nThumbnailsCreated = 0;
	_nFilesRead = 0;
	_bNeedExit = false;
	_bStarted = false;
	moveToThread(this);
	start();
	while(!_bStarted)
		msleep(1);
}
void ThumbWorker::exit() {
	_bNeedExit = true;
	__super::exit();
	wait(10000);
}
void ThumbWorker::makeFirst(const QString & path) {
	Job job = {path};
	QMutexLocker lock(&_lock);
	int index = _queue.indexOf(job);
	if(-1==index || 0==index) {
		return;
	}
	_queue.QList::swap(0, index);
}
void ThumbWorker::takeFile(const QString & path, bool updateAnyway) {
	Job job;
	job._path = path;
	job._updateAnyway = updateAnyway;
	QMutexLocker lock(&_lock);
	_queue.prepend(job);
}
void ThumbWorker::maybeUpdate(bool innerCall, const QString & path, const QList<QImage> & images) {
	if(innerCall || _bNeedExit)
		return;
	emit updated(path, ThumbDirPainter::compose(images));
}
void ThumbWorker::maybeUpdate(bool innerCall, const QString & path, const QImage & image) {
	if(innerCall)
		return;
	emit updated(path, image);
}
QImage ThumbWorker::processNextFile(const QString & path, bool updateAnyway, bool innerCall) {
	if(_bNeedExit)
		return QImage();
	_nThumbnailsCreated++;
	QFileInfo info(path);
	QImage ret;
	if(!updateAnyway && ImgDbWorker::thumbnail(info, ret).ok()) {
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
	QStringList subDirs, nullImages;
	const int targImagesCount = 6;
	for(QDirIterator it(path); images.size()<targImagesCount && it.hasNext(); ) {
		if(_bNeedExit)
			return QImage();
		QString sub = it.next();
		if(it.fileName()=="." || it.fileName()=="..")
			continue;
		_nFilesRead++;
		auto img = thumb(sub);
		if(img.isNull()) {
			if(it.fileInfo().isDir() && subDirs.count()<targImagesCount)
				subDirs << sub;
			if(it.fileInfo().isFile() && nullImages.count()<targImagesCount)
				nullImages << sub;
		} else {
			images << img;
			maybeUpdate(innerCall, path, images);//to see intermediate results and why is it takes so long
		}
	}
	{	//subDirs
		int nDirs = targImagesCount - images.count();
		nDirs = qMin(nDirs, subDirs.count());
		nDirs = qMin(nDirs, 4);//c учетом идеальных пропорций, больше и не будет показано на экране
		if(!innerCall && nDirs>0) {//show subdirs loading
			auto images2 = images;
			for(int i = 0; i<nDirs; ++i) {
				images2 << ThumbDirPainter::subDirThumb();
			}
			maybeUpdate(innerCall, path, images2);
		}
		for(int i = 0; i<nDirs; ++i) {
			if(_bNeedExit)
				return QImage();
			if(innerCall) {
				images << ThumbDirPainter::subDirThumb();
			} else {
				images << processNextFile(subDirs[i], updateAnyway, true);
				maybeUpdate(innerCall, path, images);
			}
		}
	}
	//TODO: ask QFileIconProvider in gui thread
	//{	//nullImages
	//	int count = targImagesCount - images.count();
	//	count = qMin(count, nullImages.count());
	//	for(int i = 0; i<count; ++i) {
	//		int x = ThumbModel::s_nThumbW/2;
	//		int y = ThumbModel::s_nThumbH/2;
	//		images << _iconProvider.icon(nullImages[i]).pixmap(x, y).toImage();
	//	}
	//}
	ret = ThumbDirPainter::compose(images);
	maybeUpdate(innerCall, path, ret);
	writeToDb(innerCall, info, ret);
	return ret;
}
QImage ThumbWorker::thumb(const QString & path) {
	QString pathLowercase = path.toLower();
	if(pathLowercase.endsWith(TxtLnkProcessor::extensionWithDot)) {
		QString path2 = TxtLnkProcessor::pathFromFileOrSame(path);
		if(!path2.endsWith(TxtLnkProcessor::extensionWithDot))
			return thumb(path2);
	}
	if(ThumbVideoWorker::isVideoFile(pathLowercase))
		return ThumbVideoWorker::thumbFromVideo(*this, path);
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
	_bStarted = true;
	while(!_bNeedExit) {
		Job job;
		{
			QMutexLocker lock(&_lock);
			if(!_queue.isEmpty()) {
				job = _queue.dequeue();
			}
		}
		if(job._path.isEmpty())
			msleep(10);
		else
			processNextFile(job._path, job._updateAnyway);
	}
}
void ThumbWorker::writeToDb(bool innerCall, const QFileInfo & info, const QImage & image) {
	if(innerCall || _bNeedExit)
		return;
	ImgDbWorker::setThumbnail(info, image);
}