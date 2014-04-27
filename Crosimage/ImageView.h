//ImageView.h by Kostya Kozachuck as neurocod
#pragma once
class ThumbModel;
class ThumbView;

class ImageView: public QWidget {
	Q_OBJECT
	public:
		ImageView(ThumbModel*parent, ThumbView*view, QString file);
		virtual ~ImageView();
	public slots:
		void onModelReset();
		void onTimer();
		void editExternally();
		void viewExternally();
	protected:
		virtual void paintEvent(QPaintEvent * event)override;
		virtual void wheelEvent(QWheelEvent * event)override;
		virtual void mouseMoveEvent(QMouseEvent *event)override;

		QStringList m_files;
		QPointer<ThumbModel> m_parent;
		QPointer<ThumbView> m_parentView;
		struct Image {
			QString file;
			QPixmap orig;
			QPixmap scaled;
			QSize szScaled;
		};
		bool m_bFitScreen;
		bool m_bShowOther;
		QList<Image> m_images;//center, left, tight
		int m_indexInParent;
	
		void navigate(int step=0);
		void show(const QStringList & files);
		QTime m_timeMouseMoved;
};