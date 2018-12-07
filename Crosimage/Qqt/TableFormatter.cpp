//TableFormatter.cpp by Kostya Kozachuck as neurocod - 08.04.2012 22:55:57
#include "pch.h"
#include "TableFormatter.h"

QString TableFormatter::toHtml()const {
	QString ret;
	if(border<=0)
		ret = "<table>\n";
	else
		ret = QString("<table border=\"%1\">").arg(border);
	QList<Tr> rows;
	if(!horizontalHeader.isEmpty()) {
		Tr tr;
		for(QVariant v: horizontalHeader) {
			Td td;
			td.v = v;
			tr << td;
		}
		rows << tr;
	}
	rows << _rows;
	if(!_lastRow.isEmpty())
		rows <<  _lastRow;
	for(Tr tr: rows) {
		ret += "<tr>";
		for(Td td: tr) {
			ret += "<td>";
			QString str = td.bIsHtml ? td.v.toString() : td.v.toString().toHtmlEscaped();
			if(td.clrText.isValid()) {
				str = QString("<font color=\"%1\">%2</font>").arg(Qqt::toString(td.clrText)).arg(str);
			}
			ret += str;
			ret += "</td>";
		}
		ret += "</tr>\n";
	}

	ret += "</table>";
	return ret;
}
QString TableFormatter::toPlainText()const {
	QString ret;

	QList<Tr> rows;
	if(!horizontalHeader.isEmpty()) {
		Tr tr;
		for(QVariant v: horizontalHeader) {
			Td td;
			td.v = v;
			tr << td;
		}
		rows << tr;
	}
	rows << _rows;
	if(!_lastRow.isEmpty())
		rows <<  _lastRow;
	bool bFirstRow = true;
	for(Tr tr: rows) {
		if(bFirstRow) {
			bFirstRow = false;
		} else {
			ret += "\r\n";
		}
		bool bFirstCol = true;
		for(Td td: tr) {
			if(bFirstCol) {
				bFirstCol = false;
			} else {
				ret += '\t';
			}
			if(td.bIsHtml) {
				qCritical() << QObject::tr("Error: can't convert html->txt");
				WIP;
			}
			ret += td.v.toString();
		}
	}
	return ret;
}
QString TableFormatter::toMonospace(bool bUseUnicode)const {
	//TODO: use 0x2534 ┴
	QString ret;
	QList<Tr> rows;
	if(!horizontalHeader.isEmpty()) {
		Tr tr;
		for(QVariant v: horizontalHeader) {
			Td td;
			td.v = v;
			tr << td;
		}
		rows << tr;
	}
	rows << _rows;
	if(!_lastRow.isEmpty())
		rows <<  _lastRow;
	QList<int> columnsWidth;//in chars
	{
		for(Tr tr: rows) {
			int col = 0;
			for(Td td: tr) {
				if(columnsWidth.size()==col)
					columnsWidth << 0;
				int & width = columnsWidth[col];
				int curWidth = td.v.toString().size();
				width = qMax(width, curWidth);
				col++;
			}
		}
	}
	QString strTotalWidthLine;
	if(border) {
		int totalWidth = 1;
		for(int width: columnsWidth) {
			totalWidth += width;
		}
		totalWidth += columnsWidth.size();
		if(bUseUnicode)
			strTotalWidthLine = QString(QChar(0x2500)).repeated(totalWidth);
		else
			strTotalWidthLine = QString('-').repeated(totalWidth);
		ret += QString('_').repeated(totalWidth) + "\r\n";
	}
	int row = 0;
	QChar verticalLineChar = (bUseUnicode) ? QChar(0x2502) : '|';
	for(Tr tr: rows) {
		if(row!=0) {
			ret += "\r\n";
		}
		if(border)
			ret += verticalLineChar;
		bool bFirstCol = true;
		int col = 0;
		for(Td td: tr) {
			if(bFirstCol) {
				bFirstCol = false;
			} else
				if(!border)
					ret += '\t';
			if(td.bIsHtml) {
				qCritical() << QObject::tr("Error: can't convert html->txt");
				WIP;
			}
			QString str = td.v.toString();
			ret += str;
			int additionalSize = columnsWidth[col]-str.length();
			if(additionalSize>0)
				ret += str.fill((bUseUnicode ? QChar(0x00A0) : ' '), additionalSize);
			col++;
			if(border)
				ret += verticalLineChar;
		}
		if(row==0 && border && !horizontalHeader.isEmpty()) {
			ret += "\r\n" + strTotalWidthLine;
		}
		row++;
	}
	if(border) {
		//0x2514==└
		ret += "\r\n" + QString(QChar(0x2514)) + strTotalWidthLine;
		ret.chop(2);
		ret += QChar(0x2518);//┘
	}
	return ret;
}
void TableFormatter::finalize() {
	if(!_lastRow.isEmpty()) {
		_rows <<  _lastRow;
		_lastRow.clear();
	}
}
void TableFormatter::setColor(int row, int col, const QColor & clr) {
	td(row, col).clrText = clr;
}
void TableFormatter::setBackgroundColor(int row, int col, const QColor & clr) {
	td(row, col).clrBackground = clr;
}
//static
TableFormatter::Tr TableFormatter::parse(const QString & _line) {
	QString line = _line;
	Tr tr;
	if(line.endsWith('|'))
		line.truncate(line.length()-1);
	if(line.startsWith('|'))
		line.remove(0, 1);
	QStringList li = line.split('|');
	for(auto str: li) {
		Td td;
		td.v = str.trimmed();
		tr << td;
	}
	return tr;
}
/*void TableFormatter::loadFromHtml(const QString & html) {
	msgBox(html.toHtmlEscaped());
}*/
void TableFormatter::loadFromPlainText(const QString & _originalText) {
	QString originalText = _originalText;
	originalText.remove('\r');
	QStringList lines = originalText.split("\n");
	bool bNextRowIsHeader = false;
	bool bHeaderMet = false;
	for(auto strLine: lines) {
		if(strLine == QString('-').repeated(strLine.length())) {
			if(!bHeaderMet) {
				bNextRowIsHeader = true;
			}
			continue;
		}
		Tr tr = parse(strLine);
		if(bNextRowIsHeader) {
			bNextRowIsHeader = false;
			bHeaderMet = true;
			for(auto td: tr)
				horizontalHeader << td.v;
			continue;
		}
		_rows << tr;
	}
}
#ifdef QT_WIDGETS_LIB
QTableView* TableFormatter::toWidget()const {
	auto view = new QTableView;
	auto model = new QStandardItemModel(view);
	toModel(model);
	view->setModel(model);
	view->setSortingEnabled(true);
	if(model->rowCount()<1000)
		view->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	view->resize(1024, 600);
	return view;
}
void TableFormatter::toModel(QStandardItemModel *model)const {
	model->removeRows(0, model->rowCount());
	QList<Tr> rows = _rows;
	if(!_lastRow.isEmpty())
		rows <<  _lastRow;

	int nColumns = horizontalHeader.count();
	for(Tr tr: rows)
		nColumns = qMax(nColumns, tr.count());
	model->setColumnCount(nColumns);
	for(int col=0; col<horizontalHeader.count(); ++col) {
		model->setHorizontalHeaderItem(col, new QStandardItem(horizontalHeader[col].toString()));
	}

	model->setRowCount(rows.count());
	for(int nRow = 0; nRow<rows.count(); ++nRow) {
		const Tr & tr = rows[nRow];
		for(int nCol = 0; nCol<tr.count(); ++nCol) {
			Td td = tr[nCol];
			auto item = new QStandardItem;
			item->setData(td.v, Qt::DisplayRole);
			if(td.clrBackground.isValid())
				item->setData(td.clrBackground, Qt::BackgroundRole);
			if(td.clrText.isValid())
				item->setData(td.clrText, Qt::ForegroundRole);
			model->setItem(nRow, nCol, item);
		}
	}
}
void TableFormatter::loadFromClipboard() {
	const QClipboard *clipboard = QApplication::clipboard();
	const QMimeData *mimeData = clipboard->mimeData();
	/*if (mimeData->hasHtml()) {
		loadFromHtml(mimeData->html());
	} else */if (mimeData->hasText()) {
		loadFromPlainText(mimeData->text());
	}
}
#endif //QT_WIDGETS_LIB