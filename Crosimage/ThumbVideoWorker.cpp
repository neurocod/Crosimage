//ThumbVideoWorker.cpp by Kostya Kozachuck as neurocod - 2015.01.26 11:06:12
#include "pch.h"
#include "ThumbVideoWorker.h"
#include "ThumbWorker.h"

QStringList ThumbVideoWorker::s_extensions = {".avi", ".flv", ".mkv", ".avi", ".wmv", ".mp4", ".mpg"};
ThumbVideoWorker::ThumbVideoWorker() {
}
bool ThumbVideoWorker::isVideoFile(const QString & path) {
	auto str = path.toLower();
	for(auto ext: s_extensions) {
		if(str.endsWith(ext)) {
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
	QImage ret;
	int secs = 6;
	int step = 3;
	for(int iter = 0; iter<4; ++iter) {
		ret = thumbFromVideo(worker, path, secs);
		if(ret.isNull() || isGoodThumb(ret))
			break;
		emit worker->maybeUpdate(false, path, ret);
		secs += step;
	}
	return ret;
}
bool ThumbVideoWorker::isGoodThumb(const QImage & img) {
	if(img.size().isEmpty())
		return false;
	return !hasMonopolisticColors(img);
}
bool ThumbVideoWorker::isBlackOrWhite(const QImage & img) {
	if(img.size().isEmpty())
		return false;
	int whitePixels = 0;
	int blackPixels = 0;
	int cx = img.size().width();
	int cy = img.size().height();
	int pixels = cx * cy;
	for(int y = 0; y<cy; ++y) {
		for(int x = 0; x<cx; ++x) {
			QRgb rgb = img.pixel(x, y);
			int sum = qRed(rgb)+qGreen(rgb)+qBlue(rgb);
			if(sum<64)
				blackPixels++;
			else if(sum>400)
				whitePixels++;
		}
	}
	qreal white = whitePixels;
	qreal black = blackPixels;
	white /= pixels;
	black /= pixels;
	const qreal threshld = 0.8;
	if(white>threshld || black>threshld)
		return true;
	return false;
}
bool ThumbVideoWorker::hasMonopolisticColors(const QImage & img) {
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
	qreal has = sum;
	has /= pixels;
	return has>0.6;
}