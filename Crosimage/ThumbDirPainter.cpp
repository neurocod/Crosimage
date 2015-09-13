//ThumbDirPainter.cpp by Kostya Kozachuck as neurocod
#include "pch.h"
#include "ThumbDirPainter.h"
#include "ThumbModel.h"

QRgb ThumbDirPainter::s_rgbDir = 0xFFCD52;
QColor ThumbDirPainter::dirColor() {
	return s_rgbDir;
}
QImage ThumbDirPainter::dirStub() {
	static QImage ret;
	if(ret.isNull()) {
		QSize sz(ThumbModel::s_nThumbW, ThumbModel::s_nThumbH);
		//QSize sz(1, 1);
		ret = QImage(sz, QImage::Format_RGB32);
		ret.fill(QColor(s_rgbDir));
		s_rgbDir = ret.pixel(0, 0);//update alpha
	}
	return ret;
}
QImage ThumbDirPainter::compose(const QList<QImage> & images) {
	auto ret = compose2(images);
	compactImage(ret);
	return ret;
}
QImage ThumbDirPainter::compose2(const QList<QImage> & _images) {
	if(_images.isEmpty())
		return dirStub();
	if(_images.size()==1) {
		auto ret = dirStub();
		QPainter painter(&ret);
		painter.drawImage(0, 0, _images[0]);
		return ret;
	}
	QList<QImage> images = _images;
	QSize sz2(ThumbModel::s_nThumbW/2, ThumbModel::s_nThumbH/2);
	for(auto & img: images)
		img = img.scaled(sz2, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	qSort(images.begin(), images.end(),
		[](const QImage & i1, const QImage & i2)->bool {
			//return i1.width()*i1.height()>i2.width()*i2.height();
			return i1.height()>i2.height();
		});
	QList<QRect> used;
	QList<QImage> rets;
	{
		auto ret = dirStub();
		QPainter painter(&ret);
		paint(painter, 0, 0, images[0], used);
		rets << ret;
	}
	generateVariants(images, used, rets);
	TopResult<int, int> topImage;//index->pixel
	for(int i=0; i<rets.count(); ++i) {
		const QImage & img = rets[i];
		topImage.addMaxValue(i, goodPixels(img));
	}
	//static int maxImages = 0;
	//maxImages = qMax(maxImages, rets.count());
	//concatAndWriteAllImages("e:\\test\\1.png", rets, topImage.key());
	return rets[topImage.key()];
}
void ThumbDirPainter::concatAndWriteAllImages(QString path, const QList<QImage> & rets, int keyImage) {
	if(rets.isEmpty())
		return;
	int rows = 0;
	const int cols = 5;
	while(rows*cols<rets.count()) {
		rows++;
	}
	QSize sz(cols*ThumbModel::s_nThumbW, rows*ThumbModel::s_nThumbH);
	QImage back(sz, QImage::Format_RGB32);
	back.fill(Qt::black);
	QPainter painter(&back);
	painter.setPen(QPen(Qt::red, 3));
	painter.setFont(QFont("Arial", 12));
	int nImage = 0;
	for(int y=0; y<rows; ++y) {
		for(int x=0; x<cols; ++x) {
			if(nImage>=rets.count())
				break;
			int xx = x*ThumbModel::s_nThumbW;
			int yy = y*ThumbModel::s_nThumbH;
			painter.drawImage(xx, yy, rets[nImage]);
			painter.drawText(xx, yy+ThumbModel::s_nThumbH/4, toString(goodPixels(rets[nImage])));
			if(keyImage==nImage)
				painter.drawRect(QRect(xx, yy, ThumbModel::s_nThumbW, ThumbModel::s_nThumbH));
			nImage++;
		}
	}
	ASSERT(nImage>=rets.count());
	QImageWriter writer(path);
	writer.write(back);
}
bool ThumbDirPainter::intersects(const QRect & rc, const QList<QRect> & rects) {
	for(auto r: rects)
		if(r.intersects(rc))
			return true;
	return false;
}
void ThumbDirPainter::paint(QPainter & painter, int x, int y, const QImage & image, QList<QRect> & used) {
	painter.drawImage(x, y, image);
	used << QRect(x,y, image.width(), image.height());
}
void ThumbDirPainter::generateVariants(const QList<QImage> & images, const QList<QRect> & _used, QList<QImage> & rets) {
	int startingIndex = _used.count();
	if(startingIndex>=images.count()) {
		return;
	}
	if(startingIndex==5)
		int t=3;
	const QImage & image = images[startingIndex];
	for(auto rc: _used) {
		{
			auto pt = rc.topRight();
			pt.rx()++;
			QRect rcTest(pt, image.size());
			if(rcTest.left()<ThumbModel::s_nThumbW
				&& rcTest.bottom()<ThumbModel::s_nThumbH
				&& !intersects(rcTest, _used))
			{
				auto used = _used;
				used << rcTest;
				auto ret = paint(images, used);
				rets << ret;
				generateVariants(images, used, rets);
			}
		}
		{
			auto pt = rc.bottomLeft();
			pt.ry()++;
			QRect rcTest(pt, image.size());
			if(rcTest.left()<ThumbModel::s_nThumbW
				&& rcTest.bottom()<ThumbModel::s_nThumbH
				&& !intersects(rcTest, _used))
			{
				auto used = _used;
				used << rcTest;
				auto ret = paint(images, used);
				rets << ret;
				generateVariants(images, used, rets);
			}
		}
	}
}
int ThumbDirPainter::goodPixels(const QImage & img) {
	int ret = 0;
	const int w = img.width();
	for(int y=0; y<img.height(); ++y) {
		for(int x=0; x<w; ++x) {
			auto rgb = img.pixel(x, y);
			if(rgb!=s_rgbDir)
				ret++;
		}
	}
	return ret;
}
QImage ThumbDirPainter::paint(const QList<QImage> & images, const QList<QRect> & used) {
	auto ret = dirStub();
	QPainter painter(&ret);
	for(int i=0; i<used.count(); ++i) {
		painter.drawImage(used[i].topLeft(), images[i]);
	}
	return ret;
}
QImage ThumbDirPainter::subDirThumb() {
	static QImage ret;
	if(ret.isNull()) {
		QSize sz(ThumbModel::s_nThumbW, ThumbModel::s_nThumbH);
		ret = QImage(sz, QImage::Format_RGB32);
		ret.fill(QColor(s_rgbDir));
		QPainter painter(&ret);
		painter.setPen(QPen(Qt::black, 3));
		painter.drawRoundedRect(QRect(0, 0, sz.width(), sz.height()), 10, 10);
	}
	return ret;
}
void ThumbDirPainter::compactImage(QImage & img) {
	int newH = img.height();
	for(int y = img.height()-1; y>1; --y) {
		bool foundOtherColor = false;
		QRgb* line = (QRgb*)img.scanLine(y);
		QRgb* end = line + img.width();
		for(auto x = line; x!=end; ++x) {
			if(*x!=s_rgbDir) {
				foundOtherColor = true;
				break;
			}
		}
		if(foundOtherColor)
			break;
		newH--;
	}
	int pixelsToLeaveAtTop = 2;
	if(newH + pixelsToLeaveAtTop < img.height())
		img = img.copy(0, 0, img.width(), newH+pixelsToLeaveAtTop);
}