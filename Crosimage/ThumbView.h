//ThumbView.h by Kostya Kozachuck as neurocod - 29.03.2013 8:16:40
#pragma once
class ThumbModel;

class ThumbView: public QTableView {
	Q_OBJECT
	public:
		ThumbView(ThumbModel*m);
		virtual ~ThumbView() {}
		void select(QString file);
		void selectLater(QString file);
		void prioritizeThumbs();
	public slots:
		void onDoubleClicked(const QModelIndex & index);
		void onDoubleClicked();
		void editExternally();
		void viewExternally();
		void onModelReset();
	protected:
		ThumbModel* m_model;
		QString m_fileToSelect;
		QString selectedItemFilePath();

		virtual void resizeEvent(QResizeEvent *event)override;
};