//ItemDelegateProxy.h by Kostya Kozachuck as neurocod
#pragma once

struct ItemDelegateProxy: public QAbstractItemDelegate {
	QAbstractItemDelegate*_d = 0;
	ItemDelegateProxy(QAbstractItemDelegate*example): _d(example) {
		ASSERT(example);
	}
	virtual void destroyEditor(QWidget * editor, const QModelIndex & index)const override;
	virtual bool editorEvent(QEvent * event, QAbstractItemModel * model, const QStyleOptionViewItem & option, const QModelIndex & index)override;
	virtual bool helpEvent(QHelpEvent * event, QAbstractItemView * view, const QStyleOptionViewItem & option, const QModelIndex & index)override;
	virtual void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index)const override;
	virtual void setEditorData(QWidget * editor, const QModelIndex & index)const override;
	virtual void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index)const override;
	virtual QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index)const override;
	virtual void updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index)const override;
};