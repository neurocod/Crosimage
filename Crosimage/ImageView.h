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
		void openLinkedFile();
		void copyPath();
	protected:
		virtual void paintEvent(QPaintEvent * event)override;
		virtual void wheelEvent(QWheelEvent * event)override;
		virtual void mouseMoveEvent(QMouseEvent *event)override;
		virtual void contextMenuEvent(QContextMenuEvent * event)override;

		QStringList _files;
		QPointer<ThumbModel> _parent;
		QPointer<ThumbView> _parentView;
		struct Image {
			QString file;
			QPixmap orig;
			QPixmap scaled;
			QSize szNeeed;
		};
		bool _bFitScreen;
		bool _bShowOther;
		QList<Image> _images;//center, left, tight
		int _indexInParent;
		QAction* _actOpenLinkedFile =0;

		void navigate(int step=0);
		void show(const QStringList & files);
		QTime _timeMouseMoved;

		QString centerFile()const;
		static QString findLinkedFile(QString str);
};