﻿//ThumbVideoWorker.cpp by Kostya Kozachuck as neurocod - 2015.01.26 11:06:12
#include "pch.h"
#include "ThumbVideoWorker.h"
#include "ThumbWorker.h"

bool ThumbVideoWorker::isVideoFile(const QString & pathLowercase) {
	static const QStringList list = {".avi", ".flv", ".mkv", ".wmv", ".mp4", ".mpg", ".mov", ".vob"};
	const QString chromeDownload = ".crdownload";
	QString path = pathLowercase;
	if(path.endsWith(chromeDownload))
		path.truncate(path.length()-chromeDownload.length());
	for(const QString & ext: list) {
		if(path.endsWith(ext)) {
			return true;
		}
	}
	return false;
}
QImage ThumbVideoWorker::thumbFromVideo(ThumbWorker&worker, const QString & path, int secsOffset) {
	QStringList params;
	static QString tempFile;
	if(tempFile.isEmpty()) {
		tempFile = QDir(QDir::tempPath()).absoluteFilePath("Crosimage_ffmpeg.jpg");
	}
	if(QFile::exists(tempFile)) {
		QFile file(tempFile);
		bool b = file.remove();
		ASSERT(b);
	}
	//there are other choices (like mencoder) described in http://habrahabr.ru/post/171213/
	//run: "C:\Program Files\ffmpeg\bin\ffmpeg.exe" -ss 00:00:01 -i "video/path.avi" -f image2 -vframes 1 "image/file.jpg"
	static QString program;
	if (program.isEmpty()) {
		program = R"(C:\Program Files\ffmpeg\bin\ffmpeg.exe)";
		if (!QFile::exists(program)) {
			qWarning() << QObject::tr("Program %1 does not exist").arg(program);
		}
	}
	const QTime time(secsOffset/3600, secsOffset/60, secsOffset);
	QString strTime = time.toString("HH:mm:ss");
	params << "-ss" << strTime << "-i" << path << "-f" << "image2" << "-vframes" << "1" << tempFile;
	const int code = QProcess::execute(program, params);
	if(QFile::exists(tempFile))
		return worker.thumb(tempFile);
	return QImage();
}
QImage ThumbVideoWorker::thumbFromVideo(ThumbWorker&worker, const QString & path) {
	// seconds where video thumbnails are get from:
	QList<int> secs = { 1, 5 };
	int step = 3;
	for(int iter = 0; iter<6; ++iter) {
		secs << secs.last() + step;
		step++;
	}

	TopResult<qreal, QImage> ret;
	for(const int sec: secs) {
		QImage img = thumbFromVideo(worker, path, sec);
		if(img.isNull())
			break;
		const qreal factor = colorMonopolization(img);
		if(ret.addMinKey(factor, img)) {
			emit worker.maybeUpdate(false, path, img);
		}
		if(factor<0.5)
			break;
	}
	return ret.value();
}
qreal ThumbVideoWorker::colorMonopolization(const QImage & img) {
	const int cx = img.size().width();
	const int cy = img.size().height();
	const int pixels = cx*cy;
	if(pixels<=0)
		return 0;
	const int quantizationBits = 5;
	const int sz = 512;//2**(3 rgb*3 bits per pixel)
	int colors[sz];
	for(int & c: colors)
		c = 0;
	ASSERT(img.format()==QImage::Format_RGB32);
	for(int y = 0; y<cy; ++y) {
		QRgb* p = (QRgb*)img.scanLine(y);
		for(auto end = p + cx; p<end; ++p) {
			const QRgb rgb = *p;
			int r = qRed(rgb) >> quantizationBits;
			int g = qGreen(rgb) >> quantizationBits;
			int b = qBlue(rgb) >> quantizationBits;
			QRgb rgb2 = r + (g<<3) + (b<<6);
			ASSERT(rgb2<sz && r<=8 && g<=8 && b<=8);
			colors[rgb2]++;
		}
	}
	std::sort(colors, colors+sz);
	int sum = 0;
	for(int i = 0; i<3; ++i) {
		int pixelCount = colors[sz-1-i];
		sum += pixelCount;
	}
	qreal ret = sum;
	ret /= pixels;
	return ret;
}