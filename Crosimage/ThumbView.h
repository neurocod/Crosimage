//ThumbView.h by Kostya Kozachuck as neurocod
#pragma once
#include "ThumbModel.h"

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
		void onDoubleClicked0();
		void editExternally();
		void viewExternally();
		void openInExplorer();
		void onModelReset();
		void rebuildThumbnail();
		void clearThumbnail();
		void deleteFile();
		void renameCurrentFile();
		void copyPath();
		void newDir();
		void moveFiles();
	protected:
		ThumbModel* _model;
		QString _fileToSelect;
		QList<ThumbModel::Item*> selectedItems()const;
		QString selectedItemFilePath();

		virtual void resizeEvent(QResizeEvent *event)override;
		virtual void keyPressEvent(QKeyEvent*event)override;
		virtual int sizeHintForRow(int row)const override;
};