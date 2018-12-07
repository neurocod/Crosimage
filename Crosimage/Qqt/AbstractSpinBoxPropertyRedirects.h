//AbstractSpinBoxPropertyRedirects.h by Kostya Kozachuck as neurocod - 11.03.2012 20:19:10
#pragma once

class AbstractSpinBoxPropertyRedirects: public WidgetPropertyRedirects {
	MAYBE_SUPER(WidgetPropertyRedirects)
	public:
		AbstractSpinBoxPropertyRedirects(QAbstractSpinBox*d) : WidgetPropertyRedirects(d) {
			accelerated.init(d);
			alignment.init(d);
			buttonSymbols.init(d);
			correctionMode.init(d);
			frame.init(d);
			keyboardTracking.init(d);
			readOnly.init(d);
			specialValueText.init(d);
			wrapping.init(d);
		}

		PROPERTY_REDIRECTV(QAbstractSpinBox, bool, accelerated, isAccelerated, setAccelerated);
		//PROPERTY_REDIRECTV(QAbstractSpinBox, const bool, acceptableInput, acceptableInput);
		PROPERTY_REDIRECTV(QAbstractSpinBox, Qt::Alignment, alignment, alignment, setAlignment);
		PROPERTY_REDIRECTV(QAbstractSpinBox, QAbstractSpinBox::ButtonSymbols, buttonSymbols, buttonSymbols, setButtonSymbols);
		PROPERTY_REDIRECTV(QAbstractSpinBox, QAbstractSpinBox::CorrectionMode, correctionMode, correctionMode, setCorrectionMode);
		PROPERTY_REDIRECTV(QAbstractSpinBox, bool, frame, hasFrame, setFrame);
		PROPERTY_REDIRECTV(QAbstractSpinBox, bool, keyboardTracking, keyboardTracking, setKeyboardTracking);
		PROPERTY_REDIRECTV(QAbstractSpinBox, bool, readOnly, isReadOnly, setReadOnly);
		PROPERTY_REDIRECT (QAbstractSpinBox, QString, specialValueText, specialValueText, setSpecialValueText);
		//PROPERTY_REDIRECTV(QAbstractSpinBox, const QString, text);
		PROPERTY_REDIRECTV(QAbstractSpinBox, bool, wrapping, wrapping, setWrapping);
};