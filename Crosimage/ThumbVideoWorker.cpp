//ThumbVideoWorker.cpp by Kostya Kozachuck as neurocod - 2015.01.26 11:06:12
#include "pch.h"
#include "ThumbVideoWorker.h"
#include "ThumbWorker.h"

QStringList ThumbVideoWorker::s_extensions = {".avi", ".flv", ".mkv", ".avi", ".wmv", ".mp4", ".mpg"};
ThumbVideoWorker::ThumbVideoWorker() {
}
bool ThumbVideoWorker::isVideoFile(const QString & pathLowercase) {
	for(auto ext: s_extensions) {
		if(pathLowercase.endsWith(ext)) {
			return true;
		}
	}
	return false;
}
QImage ThumbVideoWorker::thumbFromVideo(ThumbWorker*worker, const QString & path, int secsOffset) {
	QStringList params;
	static QString tempFile;
	if(tempFile.isEmpty())
		tempFile = QDir(QDir::tempPath()).absoluteFilePath(QUuid::createUuid().toString()+".jpg");
	if(QFile::exists(tempFile)) {
		QFile file(tempFile);
		bool b = file.remove();
		ASSERT(b);
	}
	//there is other adwices (like mencoder) in http://habrahabr.ru/post/171213/
	QString program = "C:\\Program Files\\ffmpeg\\bin\\ffmpeg.exe";
	QTime time(secsOffset/3600, secsOffset/60, secsOffset);
	QString strTime = time.toString("HH:mm:ss");
	params << "-ss" << strTime << "-i" << path << "-f" << "image2" << "-vframes" << "1" << tempFile;
	int code = QProcess::execute(program, params);
	if(QFile::exists(tempFile))
		return worker->thumb(tempFile);
	return QImage();
}
QImage ThumbVideoWorker::thumbFromVideo(ThumbWorker*worker, const QString & path) {
	int secs = 6;
	const int step = 3;
	TopResult<qreal, QImage> ret;
	for(int iter = 0; iter<4; ++iter) {
		QImage img = thumbFromVideo(worker, path, secs);
		if(img.isNull())
			break;
		qreal factor = colorMonopolization(img);
		if(ret.addMinKey(factor, img)) {
			emit worker->maybeUpdate(false, path, img);
		}
		if(factor<0.5)
			break;
		secs += step;
	}
	return ret.value();
}
qreal ThumbVideoWorker::colorMonopolization(const QImage & img) {
	int cx = img.size().width();
	int cy = img.size().height();
	int pixels = cx*cy;
	if(pixels<=0)
		return false;
	const int sz = 512;//2**(3 rgb*3 bits per pixel)
	int colors[sz];
	for(int & c: colors)
		c = 0;
	const int quantizationBits = 5;
	for(int y = 0; y<cy; ++y) {
		for(int x = 0; x<cx; ++x) {
			QRgb rgb = img.pixel(x, y);
			int r = qRed(rgb) >> quantizationBits;
			int g = qGreen(rgb) >> quantizationBits;
			int b = qBlue(rgb) >> quantizationBits;
			rgb = r + (g<<3) + (b<<6);
			ASSERT(rgb<sz && r<=8 && g<=8 && b<=8);
			colors[rgb]++;
		}
	}
	qSort(colors, colors+sz);
	int sum = colors[sz-1] + colors[sz-2] + colors[sz-3];
	qreal ret = sum;
	ret /= pixels;
	return ret;
}