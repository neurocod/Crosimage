//ThumbView.h by Kostya Kozachuck as neurocod
#pragma once
class ThumbModel;

class ThumbView: public QTableView {
	Q_OBJECT
	public:
		ThumbView(ThumbModel*m);
		virtual ~ThumbView() {}
		void select(const QString & file);
		void selectLater(const QString & file);
		void prioritizeThumbs();
	public slots:
		void onDoubleClicked(const QModelIndex & index);
		void onDoubleClicked();
		void editExternally();
		void viewExternally();
		void onModelReset();
		void rebuildThumbnail();
		void deleteFile();
	protected:
		ThumbModel* _model;
		QString _fileToSelect;
		QString selectedItemFilePath();

		virtual void resizeEvent(QResizeEvent *event)override;
		virtual int sizeHintForRow(int row)const override;
};