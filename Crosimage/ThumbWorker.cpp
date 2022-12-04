//ThumbWorker.cpp by Kostya Kozachuck as neurocod
#include "pch.h"
#include "ThumbWorker.h"
#include "ThumbModel.h"
#include "ThumbDirPainter.h"
#include "DirDb.h"
#include "ThumbVideoWorker.h"
#include "TxtLnkProcessor.h"
#include "LogFile.h"

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
	auto job = new GenerateThumb(path);
	QMutexLocker lock(&_lock);
	for(int i = 0; i<_queue.count(); ++i) {
		auto j = _queue.at(i);
		if(job->equal(*j)) {
			_queue.swapItemsAt(0, i);
			return;
		}
	}
}
void ThumbWorker::setThumb(const QFileInfo & file, const QImage & img) {
	auto job = new SetThumb(file, img);
	pushBack(job);
}
void ThumbWorker::takeFile(const QString & path, bool updateAnyway) {
	auto job = new GenerateThumb(path);
	job->_updateAnyway = updateAnyway;
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
QImage ThumbWorker::generateThumb(const QString & path, bool updateAnyway, bool innerCall) {
	if(_bNeedExit)
		return QImage();
	_nThumbnailsCreated++;
	QFileInfo info(path);
	QImage ret;
	if(!updateAnyway && DirDb::instance(info).thumbnail(info, ret).ok()) {
		maybeUpdate(innerCall, path, ret);
		return ret;
	}
	if(!info.isDir()) {
		ret = thumb(path);
		maybeUpdate(innerCall, path, ret);
		if(!innerCall)
			DirDb::setThumbnailS(info, ret);
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
				images << generateThumb(subDirs[i], updateAnyway, true);
				maybeUpdate(innerCall, path, images);
			}
		}
	}
	//TODO: ask QFileIconProvider in gui thread
	//{	//nullImages
	//	int count = targImagesCount - images.count();
	//	count = qMin(count, nullImages.count());
	//	for(int i = 0; i<count; ++i) {
	//		int x = CrSettings::inst()._thumbW/2;
	//		int y = CrSettings::inst()._thumbH/2;
	//		images << _iconProvider.icon(nullImages[i]).pixmap(x, y).toImage();
	//	}
	//}
	ret = ThumbDirPainter::compose(images);
	maybeUpdate(innerCall, path, ret);
	if(!innerCall)
		DirDb::setThumbnailS(info, ret);
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
		if(img.width()>CrSettings::inst()._thumbW || img.height()>CrSettings::inst()._thumbH)
			thumb = img.scaled(CrSettings::inst()._thumbW, CrSettings::inst()._thumbH, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		else
			thumb = img;
	}
	return thumb;
}
void ThumbWorker::run() {
	_bStarted = true;
	while(!_bNeedExit) {
		Job*job = 0;
		{
			QMutexLocker lock(&_lock);
			if(!_queue.isEmpty()) {
				job = _queue.takeFirst();
			}
		}
		if(!job) {
			msleep(10);
			continue;
		}
		job->run(this);
		if(job->_delete) {
			delete job;
		} else {
			pushBack(job);
		}
	}
}
void ThumbWorker::generateRecursive(const QDir & dir) {
	bool found = false;
	{
		QMutexLocker lock(&_lock);
		for(auto job: _queue) {
			if(job->type()==Job::TypeGenerateDirRecursive) {
				found = true;
				break;
			}
		}
	}
	if(found) {
		msgBox(tr("Other recursive generation is in progress, see log file near executable file"));
		return;
	}
	auto job = new GenerateDirRecursive(dir, QDirIterator::Subdirectories);
	pushBack(job);
}
void ThumbWorker::NewRating::run(ThumbWorker*w) {
	DirDb::instance(_info).setRating(_info, _rating);
}
void ThumbWorker::GenerateThumb::run(ThumbWorker*w) {
	w->generateThumb(_path, _updateAnyway);
}
void ThumbWorker::SetThumb::run(ThumbWorker*w) {
	DirDb::setThumbnailS(_file, _img);
	w->emit updated(_file.absoluteFilePath(), _img);
}
void ThumbWorker::GenerateDirRecursive::run(ThumbWorker*w) {
	if(_dirIterator.hasNext()) {
		_delete = true;
		return;
	}
	_delete = false;
	QString path = _dirIterator.next();
	if(path.endsWith("/..") || path.endsWith("/."))
		return;
	w->generateThumb(path, true);
	QFileInfo fi(path);
	DirDb::instance(fi).freeCacheMemory();
	if(fi.isDir() || 0==(w->_nThumbnailsCreated%100)) {
		LogFile::debug() << QString("%1 %2\n").arg(w->_nThumbnailsCreated).arg(path);
	}
}
void ThumbWorker::pushBack(Job*j) {
	QMutexLocker lock(&_lock);
	_queue.append(j);
}