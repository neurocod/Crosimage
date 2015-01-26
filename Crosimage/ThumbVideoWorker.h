//ThumbVideoWorker.h by Kostya Kozachuck as neurocod - 2015.01.26 11:06:12
#pragma once
class ThumbWorker;

class ThumbVideoWorker {
	public:
		ThumbVideoWorker();
		virtual ~ThumbVideoWorker() {}

		static QStringList s_extensions;
		static bool isVideoFile(const QString & path);
		static QImage thumbFromVideo(ThumbWorker*worker, const QString & path);
		static QImage thumbFromVideo(ThumbWorker*worker, const QString & path, int secsOffset);
		static bool isGoodThumb(const QImage & img);
		static bool isBlackOrWhite(const QImage & img);
		static bool hasMonopolisticColors(const QImage & img);
};