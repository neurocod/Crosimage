//ItemDelegateProxy.cpp by Kostya Kozachuck as neurocod
#include "pch.h"
#include "ItemDelegateProxy.h"

void ItemDelegateProxy::destroyEditor(QWidget * editor, const QModelIndex & index)const {
	_d->destroyEditor(editor, index);
}
bool ItemDelegateProxy::editorEvent(QEvent * event, QAbstractItemModel * model, const QStyleOptionViewItem & option, const QModelIndex & index) {
	return _d->editorEvent(event, model, option, index);
}
bool ItemDelegateProxy::helpEvent(QHelpEvent * event, QAbstractItemView * view, const QStyleOptionViewItem & option, const QModelIndex & index) {
	return _d->helpEvent(event, view, option, index);
}
void ItemDelegateProxy::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index)const {
	_d->paint(painter, option, index);
}
void ItemDelegateProxy::setEditorData(QWidget * editor, const QModelIndex & index)const {
	_d->setEditorData(editor, index);
}
void ItemDelegateProxy::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index)const {
	_d->setModelData(editor, model, index);
}
QSize ItemDelegateProxy::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index)const {
	return _d->sizeHint(option, index);
}
void ItemDelegateProxy::updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index)const {
	_d->updateEditorGeometry(editor, option, index);
}