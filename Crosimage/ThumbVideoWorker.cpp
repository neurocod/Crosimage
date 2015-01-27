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
	const int sz = 512;//2**(3 rgb*3 bits per pixel)
	QMap<QRgb, int> stat;
	int colors[sz];
	for(int & c: colors)
		c = 0;
	const int quantizationBits = 5;
	auto adr = img.scanLine(0);
	QImage img2 = img;
	img2.save("d:\\v\\test.jpg");
	ASSERT(img.format()==QImage::Format_RGB32);
	for(int y = 0; y<cy; ++y) {
		auto p = img.scanLine(y);
		int x = 0;
		for(auto end = p + cx; p<end; ++p) {
			if(x==100 && y==90)
				int t = 3;
			const QRgb rgb = *p;
			int r = qRed(rgb) >> quantizationBits;
			int g = qGreen(rgb) >> quantizationBits;
			int b = qBlue(rgb) >> quantizationBits;
			QRgb rgb2 = r + (g<<3) + (b<<6);
			ASSERT(rgb2<sz && r<=8 && g<=8 && b<=8);
			colors[rgb2]++;
			QRgb rgbPosterized = qRgb(r<<quantizationBits, g<<quantizationBits, b<<quantizationBits);
			stat[rgbPosterized] = colors[rgb2];
			img2.setPixel(x, y, rgbPosterized);
			++x;
		}
	}
	img2.save("d:\\v\\test2.jpg");
	qSort(colors, colors+sz);
	int sum = 0;
	QString str;
	for(int i = 0; i<3; ++i) {
		int pixelCount = colors[sz-1-i];
		QColor clr = stat.keys(pixelCount).first();
		str += toString(clr) + '\n';
		sum += pixelCount;
	}
	qreal ret = sum;
	ret /= pixels;
	return ret;
}