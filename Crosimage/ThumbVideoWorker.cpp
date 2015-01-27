//ThumbVideoWorker.cpp by Kostya Kozachuck as neurocod - 2015.01.26 11:06:12
#include "pch.h"
#include "ThumbVideoWorker.h"
#include "ThumbWorker.h"

QStringList ThumbVideoWorker::s_extensions = {".avi", ".flv", ".mkv", ".avi", ".wmv", ".mp4", ".mpg", ".mov"};
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
QImage ThumbVideoWorker::thumbFromVideo(ThumbWorker&worker, const QString & path, int secsOffset) {
	QStringList params;
	static QString tempFile;
	if(tempFile.isEmpty()) {
		//QUuid::createUuid().toString()+".jpg"
		tempFile = QDir(QDir::tempPath()).absoluteFilePath("Crosimage_ffmpeg.jpg");
	}
	if(QFile::exists(tempFile)) {
		QFile file(tempFile);
		bool b = file.remove();
		ASSERT(b);
	}
	//there is other adwices (like mencoder) in http://habrahabr.ru/post/171213/
	//run: "C:\Program Files\ffmpeg\bin\ffmpeg.exe" -ss 00:00:01 -i "video/path.avi" -f image2 -vframes 1 "image/file.jpg"
	QString program = "C:\\Program Files\\ffmpeg\\bin\\ffmpeg.exe";
	QTime time(secsOffset/3600, secsOffset/60, secsOffset);
	QString strTime = time.toString("HH:mm:ss");
	params << "-ss" << strTime << "-i" << path << "-f" << "image2" << "-vframes" << "1" << tempFile;
	int code = QProcess::execute(program, params);
	if(QFile::exists(tempFile))
		return worker.thumb(tempFile);
	return QImage();
}
QImage ThumbVideoWorker::thumbFromVideo(ThumbWorker&worker, const QString & path) {
	QList<int> secs = { 1, 5 };
	const int step = 3;
	for(int iter = 0; iter<4; ++iter) {
		secs << secs.last() + step;
	}

	TopResult<qreal, QImage> ret;
	for(int sec: secs) {
		QImage img = thumbFromVideo(worker, path, sec);
		if(img.isNull())
			break;
		qreal factor = colorMonopolization(img);
		if(ret.addMinKey(factor, img)) {
			emit worker.maybeUpdate(false, path, img);
		}
		if(factor<0.5)
			break;
	}
	return ret.value();
}
qreal ThumbVideoWorker::colorMonopolization(const QImage & img) {
	int cx = img.size().width();
	int cy = img.size().height();
	int pixels = cx*cy;
	if(pixels<=0)
		return false;
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
	qSort(colors, colors+sz);
	int sum = 0;
	for(int i = 0; i<3; ++i) {
		int pixelCount = colors[sz-1-i];
		sum += pixelCount;
	}
	qreal ret = sum;
	ret /= pixels;
	return ret;
}