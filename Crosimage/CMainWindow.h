//CMainWindow.h by Kostya Kozachuck as neurocod
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
		ThumbView* view()const { return m_view; }
		void prioritizeThumbs();

		static void beforeQuit();
		static bool s_loadingComplete;
		static int instancesCount() { return s_inst.count(); }
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
		static QList<CMainWindow*> s_inst;//also focus order

		MenuButton m_menu;
		int m_nInst;
		QString m_rootDir;
		void createFileTreePanel();
		void goSibling(bool next);
		ThumbModel* m_model;//������������ ��� ���������� "������� �����", ����� �� ��������� �������������� ����������
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