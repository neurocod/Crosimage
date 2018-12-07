//Label.h by Kostya Kozachuck as neurocod - 29.02.2012 1:30:10
#pragma once

class Label: public WidgetPropertyRedirects {
	public:
		template<typename ... Args>
		Label(Args ... args): WidgetPropertyRedirects(new QLabel) {
			d = staticCast<QLabel*>();
			alignment.init(d);
			indent.init(d);
			margin.init(d);
			openExternalLinks.init(d);
			scaledContents.init(d);
			text.init(d);
			textFormat.init(d);
			textInteractionFlags.init(d);
			wordWrap.init(d);

			CtorProcessorT<Label> p(*this);
			p.process_(args...);
		}
		void set(const QImage & img);
		void set(const QPixmap& img);
		void set(const QString& str);
		void operator=(const QImage & img);
		void operator=(const QPixmap& img);
		void operator=(const QString& str);

		PROPERTY_REDIRECTV(QLabel, Qt::Alignment, alignment, alignment, setAlignment);
		PROPERTY_REDIRECTV(QLabel, int, indent, indent, setIndent);
		PROPERTY_REDIRECTV(QLabel, int, margin, margin, setMargin);
		PROPERTY_REDIRECTV(QLabel, bool, openExternalLinks, openExternalLinks, setOpenExternalLinks);
		//PROPERTY_REDIRECTV(QLabel, QPixmap, pixmap, setPixmap, pixmap);
		PROPERTY_REDIRECTV(QLabel, bool, scaledContents, hasScaledContents, setScaledContents);
		//selectedText : const QString
		PROPERTY_REDIRECT (QLabel, QString, text, text, setText);
		PROPERTY_REDIRECTV(QLabel, Qt::TextFormat, textFormat, textFormat, setTextFormat);
		PROPERTY_REDIRECTV(QLabel, Qt::TextInteractionFlags, textInteractionFlags, textInteractionFlags, setTextInteractionFlags);
		PROPERTY_REDIRECTV(QLabel, bool, wordWrap, wordWrap, setWordWrap);

		EMBED_QPOINTER_AND_CAST(QLabel)
};