//CMainWindow.h by Kostya Kozachuck as neurocod - 29.03.2013 8:16:28
#pragma once
class ThumbModel;
class ThumbView;

class CMainWindow : public QMainWindow {
	Q_OBJECT
	public:
		CMainWindow(QWidget *parent = 0, Qt::WindowFlags flags = 0);
		virtual ~CMainWindow();
		static CMainWindow* instance(QWidget*child);
		void go(const QString & dir, int addCurrentToHistory);//-1=history, 0=not add, 1==add to fwd
		void display(const QString & file);
		static void beforeQuit();
		static bool s_loadingComplete;
		static QList<CMainWindow*> s_inst;//also focus order
		ThumbView* view()const { return m_view; }
		void prioritizeThumbs();
	public slots:
		void go(const QString & path);
		void goUp();
		void goUpSibling();
		void goDownSibling();
		void goBack();
		void goFwd();
		void openInExplorer();
		void sortingUpdated();
	protected:
		virtual void focusInEvent(QFocusEvent * event)override;
		virtual void enterEvent(QEvent *e)override;
		void onFocus();
		void updateSettings(bool save);
	private:
		MenuButton m_menu;
		int m_nInst;
		QString m_rootDir;
		void createFileTreePanel();
		void goSibling(bool next);
		ThumbModel* m_model;//используется как переменная "текущая папка", чтобы не создавать дополнительную переменную
		ThumbView* m_view;
		QComboBox* m_editPath;
		QStringList m_navigateBack;
		QStringList m_navigateFwd;
		ComboBox m_boxSortBy;
		CheckBox m_checkSortReversed;
		bool needSetTextToLineEdit(QString str)const;
		using QWidget::setWindowTitle;
		void setWindowTitle();
};