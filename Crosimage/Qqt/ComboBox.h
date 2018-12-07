//ComboBox.h by Kostya Kozachuck as neurocod - 11.03.2012 7:19:02
#pragma once

class ComboBox: public WidgetPropertyRedirects {
	MAYBE_SUPER(WidgetPropertyRedirects)
	public:
		template<typename... Args>
		ComboBox(Args... args): WidgetPropertyRedirects(new QComboBox) {
			d = staticCast<QComboBox*>();
			duplicatesEnabled.init(d);
			editable.init(d);
			frame.init(d);
			currentIndex.init(d);
			iconSize.init(d);
			insertPolicy.init(d);
			maxCount.init(d);
			maxVisibleItems.init(d);
			minimumContentsLength.init(d);
			modelColumn.init(d);
			sizeAdjustPolicy.init(d);
			currentText.init(d);

			CtorProcessorT<ComboBox> p(*this);
			p.process_(args...);
		}

		ComboBox& operator<<(const QStringList & texts);
		ComboBox& operator<<(const QString & text);
		void add(const QString & text, const QVariant & userData = QVariant());
		void add(const QIcon & icon, const QString & text, const QVariant & userData = QVariant());
		//count : const int;
		PROPERTY_REDIRECTV(QComboBox, bool, duplicatesEnabled, duplicatesEnabled, setDuplicatesEnabled);
		PROPERTY_REDIRECT (QComboBox, QString, currentText, currentText, setCurrentText);
		PROPERTY_REDIRECTV(QComboBox, bool, editable, isEditable, setEditable);
		PROPERTY_REDIRECTV(QComboBox, bool, frame, hasFrame, setFrame);
		PROPERTY_REDIRECT (QComboBox, QSize, iconSize, iconSize, setIconSize);
		PROPERTY_REDIRECTV(QComboBox, QComboBox::InsertPolicy, insertPolicy, insertPolicy, setInsertPolicy);
		PROPERTY_REDIRECTV(QComboBox, int, currentIndex, currentIndex, setCurrentIndex);
		PROPERTY_REDIRECTV(QComboBox, int, maxCount, maxCount, setMaxCount);
		PROPERTY_REDIRECTV(QComboBox, int, maxVisibleItems, maxVisibleItems, setMaxVisibleItems);
		PROPERTY_REDIRECTV(QComboBox, int, minimumContentsLength, minimumContentsLength, setMinimumContentsLength);
		PROPERTY_REDIRECTV(QComboBox, int, modelColumn, modelColumn, setModelColumn);
		PROPERTY_REDIRECTV(QComboBox, QComboBox::SizeAdjustPolicy, sizeAdjustPolicy, sizeAdjustPolicy, setSizeAdjustPolicy);
		EMBED_QPOINTER_AND_CAST(QComboBox)
};