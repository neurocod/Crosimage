//FormLayout.h by Kostya Kozachuck as neurocod - 28.06.2012 21:59:10
#pragma once
#include "Layout.h"

class FormLayout: public Layout {
	public:
		FormLayout(): Layout(new QFormLayout()) {
			d = staticCast<QFormLayout*>();

			fieldGrowthPolicy.init(d);
			formAlignment.init(d);
			horizontalSpacing.init(d);
			labelAlignment.init(d);
			rowWrapPolicy.init(d);
			verticalSpacing.init(d);
		}
		FormLayout(QWidget*parent): FormLayout() {
			parent->setLayout(d);
		}
		FormLayout(QBoxLayout*parent): FormLayout() {
			parent->addLayout(d);
		}

		QLabel* addHtmlRow(const QString & strHtml, QWidget *field=0);
		FormLayout& operator << (QWidget*w);
		FormLayout& operator << (const QString & str);

		PROPERTY_REDIRECTV(QFormLayout, QFormLayout::FieldGrowthPolicy, fieldGrowthPolicy, fieldGrowthPolicy, setFieldGrowthPolicy);
		PROPERTY_REDIRECTV(QFormLayout, Qt::Alignment, formAlignment, formAlignment, setFormAlignment);
		PROPERTY_REDIRECTV(QFormLayout, int, horizontalSpacing, horizontalSpacing, setHorizontalSpacing);
		PROPERTY_REDIRECTV(QFormLayout, Qt::Alignment, labelAlignment, labelAlignment, setLabelAlignment);
		PROPERTY_REDIRECTV(QFormLayout, QFormLayout::RowWrapPolicy, rowWrapPolicy, rowWrapPolicy, setRowWrapPolicy);
		PROPERTY_REDIRECTV(QFormLayout, int, verticalSpacing, verticalSpacing, setVerticalSpacing);

		EMBED_QPOINTER_AND_CAST(QFormLayout)
	protected:
		void init();
};