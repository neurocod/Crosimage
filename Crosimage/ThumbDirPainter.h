//ThumbDirPainter.h by Kostya Kozachuck as neurocod - 2013.10.21 07:07:26
#pragma once

class ThumbDirPainter {
	public:
		static QImage compose(const QList<QImage> & images);
		static QImage dirStub();
		static QImage subDirThumb();
	protected:
		static inline bool intersects(const QRect & rc, const QList<QRect> & rects);
		static void paint(QPainter & painter, int x, int y, const QImage & image, QList<QRect> & used);
		static QImage paint(const QList<QImage> & images, const QList<QRect> & used);
		static void generateVariants(const QList<QImage> & images, const QList<QRect> & used, QList<QImage> & rets);
		static int goodPixels(const QImage & img);
		static QRgb s_rgbDir;
		static void concatAndWriteAllImages(QString path, const QList<QImage> & images, int keyImage);
};