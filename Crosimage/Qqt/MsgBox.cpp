//MsgBox.cpp by Kostya Kozachuck as neurocod - 23.12.2011 6:34:33
#include "pch.h"
#include "MsgBox.h"


MsgBox::Btn msgBox(const QString & t1) {
	MsgBox box(t1, QCoreApplication::applicationName());
	return box.execReturnBtn();
}
void MsgBox::setCustomStyle() {
	setTextInteractionFlags(Qt::TextBrowserInteraction);
	//setStyleSheet("QMessageBox{messagebox-text-interaction-flags:15}");//Qt::TextInteractionFlags
}
int MsgBox::display(QtMsgType type, const QMessageLogContext & context, const QString &msg) {
	struct SelectableLineEdit: public QLineEdit {
		SelectableLineEdit(const QString &contents, QWidget *parent = 0): QLineEdit(contents, parent) {
			setReadOnly(true);
			setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
		}
		virtual void mousePressEvent(QMouseEvent *e)override {
			QLineEdit::mousePressEvent(e);
			selectAll();
		}
	};
	struct TextBrowser: public QTextBrowser {
		TextBrowser() {
			setTabStopDistance(QFontMetrics(font()).width(QLatin1Char('x')) * 4);
		}
		void onWrapLines(bool b) {
			setLineWrapMode(b ? QTextEdit::WidgetWidth : QTextEdit::NoWrap);
		}
	};
	struct Box: public QDialog {
		CheckBox _decodedNewLine;
		CheckBox _removedBraces;
		CheckBox _decodedBraces;
		CheckBox _decodedTabs;
		CheckBox _wrapLines;
		QLineEdit* _line = 0;
		QLineEdit* _function = 0;
		TextBrowser* _message = 0;
		QLineEdit* _file = 0;
		QString _copy;
		
		void setMsg(const QString & msg) {
			QString s = msg;
			if(s.contains("\\n")) {
				s.replace("\\n", "\n");
				_decodedNewLine.checked = true;
			} else {
				_decodedNewLine.visible = false;
			}
			if(s.contains("\\t")) {
				s.replace("\\t", "\t");
				_decodedTabs.checked = true;
			} else {
				_decodedTabs.visible = false;
			}
			if(s.contains("\\\"")) {
				s.replace("\\\"", "\"");
				_decodedBraces.checked = true;
			} else {
				_decodedBraces.visible = false;
			}
			if(s.startsWith('"') && s.endsWith('"')) {
				s.remove(0, 1);
				if(s.endsWith('"'))
					s.truncate(s.length()-1);
				_removedBraces.checked = true;
			} else {
				_removedBraces.visible = false;
			}
			
			_message->setText(s);
		}
		Box(QtMsgType type, const QMessageLogContext & context, const QString &msg):
			QDialog(0, Qt::Dialog| Qt::WindowMaximizeButtonHint| Qt::WindowCloseButtonHint)
		{
			auto sz = QApplication::desktop()->size();
			const QSize want(1024, 768);
			if(sz.width()>want.width() && sz.height()>want.height())
				resize(want);
			else
				showMaximized();
			setWindowTitle(qApp->applicationName());
			_line = new SelectableLineEdit(QString::number(context.line));
			_function = new SelectableLineEdit(context.function);
			_file = new SelectableLineEdit(context.file);
			_message = new TextBrowser;
			VBoxLayout lay = this;
			FormLayout form;
			lay << form;
			form->addRow(tr("Function:"), _function);
			form->addRow(tr("File:"), _file);
			form->addRow(tr("Line:"), _line);
			form->addRow(tr("Message:"), _message);
			_decodedNewLine.text = tr("Decoded \\n");
			_decodedBraces.text = tr("Replaced \\\" to \"");
			_decodedTabs.text = tr("Replaced \\t with tabs");
			_removedBraces.text = tr("Removed \"surrounding braces\""); 
			_decodedNewLine.enabled = false;
			_decodedBraces.enabled = false;
			_removedBraces.enabled = false;
			_decodedTabs.enabled = false;
			_wrapLines.text = tr("Wrap lines");
			_wrapLines.connectClicks(_message, &TextBrowser::onWrapLines);
			if(!_message->toPlainText().contains('\n'))
				_wrapLines->click();
			lay << _wrapLines;
			lay << _decodedNewLine;
			lay << _decodedBraces;
			lay << _removedBraces;
			lay << _decodedTabs;
			setMsg(msg);
			auto box = new DialogButtonBox(this);
			lay << box;
			auto ok = box->addOk();
			connect(ok, &QAbstractButton::clicked, this, &QDialog::accept);
			auto exit = box->addCancel();
			exit->setText(tr("&Exit app"));
			connect(exit, &QAbstractButton::clicked, qApp, &QCoreApplication::quit);
			{
				_copy = QString("%1:%2 %3:\n%4")
					.arg(_file->text())
					.arg(_line->text())
					.arg(_function->text())
					.arg(_message->toPlainText());
				PushButton copy(tr("Copy text"), QIcon(":/images/copy.png"));
				copy.connectClicksF([&]() { qApp->clipboard()->setText(_copy); });
				box->addButton(copy, QDialogButtonBox::ActionRole);
			}
		}
	};
	Box box(type, context, msg);
	return box.exec();
}
