//CMainWindow.cpp by Kostya Kozachuck as neurocod
#include "pch.h"
#include "CMainWindow.h"
#include "ThumbView.h"
#include "ThumbModel.h"
#include "ThumbWorker.h"
#include "ImageView.h"
#include "CApplication.h"

QList<CMainWindow*> CMainWindow::s_inst;
bool CMainWindow::s_loadingComplete = false;
class CComboBox: public QComboBox {
	public:
		virtual void enterEvent(QEvent * event)override {
			setFocus(Qt::MouseFocusReason);
		}
};
CMainWindow::CMainWindow(QWidget *parent, Qt::WindowFlags flags): QMainWindow(parent, flags) {
	setAttribute(Qt::WA_DeleteOnClose);
	m_model = new ThumbModel(this);
	m_view = new ThumbView(m_model);
	m_nInst = s_inst.count();
	s_inst << this;
	setWindowTitle();
	Widget w;
	VBoxLayout00 lay(w);
	{
		HBoxLayout00 lay2(lay);

		QSize szBtn(24, 24);

		lay2 << m_menu;

		ToolButton btnGoUp("", tr("Parent dir"), QIcon(":/qt-project.org/styles/commonstyle/images/up-32.png"), QKeySequence("Alt+Up"));
		btnGoUp.iconSize = szBtn;
		btnGoUp.connectClicks(this, SLOT(goUp()));
		lay2 << btnGoUp;

		ToolButton btnGoBack("", tr("Go back"), QIcon(":/qt-project.org/styles/commonstyle/images/left-32.png"), QKeySequence::Back);
		btnGoBack.iconSize = szBtn;
		btnGoBack.connectClicks(this, SLOT(goBack()));
		lay2 << btnGoBack;

		ToolButton btnGoFwd("", tr("Go forward"), QIcon(":/qt-project.org/styles/commonstyle/images/right-32.png"), QKeySequence::Forward);
		btnGoFwd.iconSize = szBtn;
		btnGoFwd.connectClicks(this, SLOT(goFwd()));
		lay2 << btnGoFwd;

		ToolButton btnRefresh("", tr("Refresh"), QKeySequence("F5"), QIcon(":/qt-project.org/styles/commonstyle/images/refresh-32.png"));
		btnRefresh.iconSize = szBtn;
		btnRefresh.connectClicks(m_model, SLOT(refresh()));
		lay2 << btnRefresh;
		{
			ToolButton b("", tr("Show prev sibling directory"), QIcon(":/qt-project.org/styles/commonstyle/images/up-32.png"));
			b.iconSize = szBtn;
			b.connectClicks(this, SLOT(goUpSibling()));
			lay2 << b;
		}
		{
			ToolButton b("", tr("Show next sibling directory"), QIcon(":/qt-project.org/styles/commonstyle/images/down-32.png"));
			b.iconSize = szBtn;
			b.connectClicks(this, SLOT(goDownSibling()));
			lay2 << b;
		}
		{
			ToolButton b("", tr("Show in Explorer"), QIcon(":/images/forge/32/folder-horizontal.png"));
			b.iconSize = szBtn;
			b.connectClicks(this, SLOT(openInExplorer()));
			lay2 << b;
		}
		m_boxSortBy->addItem(tr("Time"), QVariant(QDir::Time));
		m_boxSortBy->addItem(tr("Name"), QVariant(QDir::Name));
		m_boxSortBy->addItem(tr("Size"), QVariant(QDir::Size));
		m_boxSortBy->addItem(tr("Type"), QVariant(QDir::Type));
		m_boxSortBy.sizeAdjustPolicy = QComboBox::AdjustToContents;
		m_boxSortBy->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
		connect(m_boxSortBy, SIGNAL(currentIndexChanged(int)), SLOT(sortingUpdated()) );
		lay2 << m_boxSortBy;

		m_checkSortReversed.text = tr("Reversed");
		m_checkSortReversed.sizePolicy = QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
		m_checkSortReversed.connectClicks(this, SLOT(sortingUpdated()));
		lay2 << m_checkSortReversed;

		m_editPath = new CComboBox();
		m_editPath->setEditable(1);
		m_editPath->setMaxVisibleItems(30);
		{
			QCompleter *completer = new QCompleter(this);
			auto dirModel = new QDirModel(completer);
			dirModel->setFilter(QDir::Dirs|QDir::NoDotAndDotDot);
			completer->setModel(dirModel);
			m_editPath->setCompleter(completer);
		}
		lay2 << m_editPath;
		connect(m_editPath, SIGNAL(currentTextChanged(const QString &)), SLOT(go(QString)) );
	}
	lay << m_view;
	setCentralWidget(w);

	//createFileTreePanel();
	showFullScreen();
	updateSettings(false);
	{
		Action a(tr("New window"), QKeySequence("Ctrl+N"), QIcon(":/qt-project.org/styles/commonstyle/images/file-16.png"));
		QObject::connect(a, &QAction::triggered, this, [=]() {
				new CMainWindow;
			});
		addAction(a);
	}
	{
		Action a(tr("Toggle fullscreen"), QKeySequence("F11"));
		QObject::connect(a, &QAction::triggered, this, [=]() {
				if(windowState()==Qt::WindowFullScreen)
					showMaximized();
				else
					showFullScreen();
			});
		addAction(a);
	}
	addActions(CApplication::s_inst->globalActions());
	m_menu << actions();
}
CMainWindow::~CMainWindow() {
	bool b = s_inst.removeOne(this);
	ASSERT(b);
	updateSettings(true);
}
void CMainWindow::updateSettings(bool save) {
	auto li = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
	if(!li.isEmpty())
		m_rootDir = li[0];
	Settings sett;
	sett.beginGroup("MainWindow");
	sett.beginGroup(toString(m_nInst));
	if(save) {
		QString path = m_model->dir().absolutePath();
		sett.save("dir", path);
	} else {
		QString path = m_rootDir;
		sett.load("dir", path);
		go(path);
	}
}
void CMainWindow::createFileTreePanel() {
	QFileSystemModel* dirModel = new QFileSystemModel(this);
	dirModel->setFilter(QDir::Dirs|QDir::NoDotAndDotDot);//QDir::AllDirs QDir::Drives
	//setIconProvider QFileIconProvider
	dirModel->setRootPath(QDir::separator());

	QTreeView* dirView = new QTreeView(this);
	dirView->setModel(dirModel);
	for(int i=1; i < dirModel->columnCount(); ++i) { //hide unnecessary columns
		dirView->hideColumn(i);
	}
	QDockWidget* dock = new QDockWidget(tr("File tree"), this);
	dock->setAllowedAreas(Qt::AllDockWidgetAreas);
	dock->setFeatures(QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetFloatable);
	addDockWidget(Qt::LeftDockWidgetArea, dock);
	dock->setWidget(dirView);
}
void CMainWindow::goUp() {
	QDir dir(m_model->dir());
	if(!dir.cdUp())
		return;
	go(dir.absolutePath());
}
void CMainWindow::goBack() {
	if(m_navigateBack.isEmpty())
		return;
	QString str = m_navigateBack.last();;
	m_navigateBack.removeLast();
	go(str, 1);
}
void CMainWindow::goFwd() {
	if(m_navigateFwd.isEmpty())
		return;
	QString str = m_navigateFwd.first();;
	m_navigateFwd.removeFirst();
	go(str, -1);
}
void CMainWindow::go(const QString & _path, int addCurrentToHistory) {
	QFileInfo file(_path);
	if(!file.exists())
		return;
	QString path = QDir::toNativeSeparators(file.canonicalFilePath());
	if(path==m_rootDir && !path.endsWith('/') && !path.endsWith('\\'))
		path += '\\';
	if(file.isDir()) {
		const QString oldDir = m_model->dir().absolutePath();
		if(oldDir==file.absoluteFilePath())
			return;
		if(!oldDir.isEmpty()) {
			if(addCurrentToHistory<0)
				m_navigateBack << oldDir;
			else if(addCurrentToHistory>0)
				m_navigateFwd.prepend(oldDir);
		}
		m_view->selectLater(oldDir);
		m_model->setDir(path);
		setWindowTitle();
		if(needSetTextToLineEdit(path))
			m_editPath->setCurrentText(path);
	} else {
		display(path);
	}
}
void CMainWindow::go(const QString & path) {
	go(path, -1);
}
void CMainWindow::display(const QString & file) {
	QImage im;
	if(QImageReader(file).read(&im)) {
		new ImageView(m_model, m_view, file);
	} else {
		QDesktopServices::openUrl(QUrl::fromLocalFile(file));
	}
}
CMainWindow* CMainWindow::instance(QWidget*child) {
	for(auto p = child; p; p = p->parentWidget()) {
		if(auto ret = qobject_cast<CMainWindow*>(p))
			return ret;
	}
	return 0;
}
void CMainWindow::goUpSibling() {
	goSibling(false);
}
void CMainWindow::goDownSibling() {
	goSibling(true);
}
bool compareByNameAndNumber(const QString & s1, const QString & s2) {
	static QCollator collator;
	collator.setNumericMode(true);
	return collator.compare(s1, s2)<=0;
}
void CMainWindow::goSibling(bool next) {
	QDir parentDir = m_model->dir().absoluteFilePath("..");
	auto siblings = parentDir.entryList(QDir::AllDirs|QDir::NoDotAndDotDot);
	if(siblings.isEmpty()) {
		ASSERT(0);
		return;
	}
	qSort(siblings.begin(), siblings.end(), compareByNameAndNumber);
	auto index = siblings.indexOf(m_model->dir().dirName());
	if(-1==index) {
		ASSERT(0);//when this happens?
		index = 0;
	}
	index += (next ? 1 : -1);
	index = qBound(0, index, siblings.count()-1);
	QString strNew = parentDir.absoluteFilePath(siblings[index]);
	go(strNew);
}
void CMainWindow::openInExplorer() {
	QString file;
	auto index = m_view->selectionModel()->currentIndex();
	if(index.isValid()) {
		file = index.data().toString();
		FileFacility::showDirWithFile(file);
	} else {
		FileFacility::openUrl(m_model->dir());
	}
}
bool CMainWindow::needSetTextToLineEdit(QString str)const {
	if(!m_editPath->hasFocus())
		return true;
	QString str2 = m_editPath->currentText();
	if(str.length()>str2.length())
		qSwap(str, str2);
	if(str+QDir::separator() == str2)
		return false;
	return true;
}
void CMainWindow::focusInEvent(QFocusEvent * event) {
	onFocus();
	__super::focusInEvent(event);
}
void CMainWindow::enterEvent(QEvent *e) {
	__super::enterEvent(e);
	onFocus();
}
void CMainWindow::onFocus() {
	if(!s_loadingComplete)
		return;
	s_inst.removeOne(this);
	s_inst.prepend(this);	
}
void CMainWindow::beforeQuit() {
	for(int i=0; i<s_inst.count(); ++i) {
		s_inst[i]->m_nInst = i;
	}
}
void CMainWindow::setWindowTitle() {
	QString str = //qApp->applicationName() + " " +
		toString(m_nInst) + m_model->dir().absolutePath();
	setWindowTitle(str);
	str = toString(m_nInst);
	setWindowIconText(str);
}
void CMainWindow::sortingUpdated() {
	int index = m_boxSortBy->currentIndex();
	auto v = m_boxSortBy->itemData(index);
	QDir::SortFlags sort = (QDir::SortFlag)v.toInt();;
	if(m_checkSortReversed.checked)
		sort |= QDir::Reversed;
	m_model->setSortFlags(sort);
}
void CMainWindow::prioritizeThumbs() {
	m_view->prioritizeThumbs();
}