//GroupBox.h by Kostya Kozachuck as neurocod - 11.03.2012 10:29:36
#pragma once

class GroupBox: public WidgetPropertyRedirects {
	MAYBE_SUPER(WidgetPropertyRedirects)
	public:
		template<typename... Args>
		GroupBox(Args... args): WidgetPropertyRedirects(new QGroupBox) {
			d = staticCast<QGroupBox*>();
			checkable.init(d);
			checked.init(d);
			flat.init(d);
			title.init(d);

			CtorProcessor p(*this);
			p.process_(args...);
		}

		void connectClicksS(QObject*obj, const char * slot) {
			QObject::connect(d, SIGNAL(clicked(bool)), obj, slot);
		}
		//alignment: incompatible type, use redirect
		//PROPERTY_REDIRECTV(QGroupBox, Qt::Alignment, alignment, alignment, setAlignment);
		PROPERTY_REDIRECTV(QGroupBox, bool, checkable, isCheckable, setCheckable);
		PROPERTY_REDIRECTV(QGroupBox, bool, checked, isChecked, setChecked);
		PROPERTY_REDIRECTV(QGroupBox, bool, flat, isFlat, setFlat);
		PROPERTY_REDIRECT (QGroupBox, QString, title, title, setTitle);
		EMBED_QPOINTER_AND_CAST(QGroupBox)
	protected:
		struct CtorProcessor: public CtorProcessorT<GroupBox> {
			CtorProcessor(GroupBox&d) : CtorProcessorT(d) {}
			virtual void process(const QString & str) {
				switch(_nStr) {
				case 0:
					_d->setTitle(str);
					break;
				case 1:
					_d->setToolTip(str);
					break;
				default:
					ASSERT(0);
				}
				_nStr++;
			}
		};
};