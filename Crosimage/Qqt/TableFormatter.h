//TableFormatter.h by Kostya Kozachuck as neurocod - 08.04.2012 22:55:56
#pragma once

//TODO: loadFromHtml
class TableFormatter {
	public:
		struct Td { //Td = Table data = cell
			QVariant v;
			bool bIsHtml = false;
			QColor clrText;
			QColor clrBackground;
		};
		using Tr = QList<Td>;//Tr = table row

		QString toHtml()const;
		QString toPlainText()const;
		QString toMonospace(bool bUseUnicode=true)const;
#ifdef QT_WIDGETS_LIB
		QTableView* toWidget()const;
		void toModel(QStandardItemModel *model)const;
		void loadFromClipboard();
#endif
		//void loadFromHtml(const QString & html);
		void loadFromPlainText(const QString & text);

		int border;
		QVariantList horizontalHeader;
		TableFormatter() {
			border = 0;
		}
		TableFormatter & operator <<(decltype(endl)) {
			newLine();
			return *this;
		}
		template<class T>
		TableFormatter & operator <<(T t) {
			Td td;
			td.v = t;
			_lastRow << td;
			return *this;
		}
		template<class T>
		TableFormatter & addHtml(T t) {
			Td td;
			td.v = t;
			td.bIsHtml = true;
			_lastRow << td;
			return *this;
		}
		void newLine() {
			_rows << _lastRow;
			_lastRow.clear();
		}
		void finalize();
		int rowCount()const { return _rows.count(); }
		void setColor(int row, int col, const QColor & clr);
		void setBackgroundColor(int row, int col, const QColor & clr);
		Td& td(int row, int col) { return (_rows[row])[col]; }
		int columns(int row)const { return _rows[row].count(); }
		QList<Tr> rows() {
			finalize();
			return _rows;
		}
	protected:
		Tr _lastRow;
		QList<Tr> _rows;
		static Tr parse(const QString & line);
};