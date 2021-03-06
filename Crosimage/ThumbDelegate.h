﻿//ThumbDelegate.h by Kostya Kozachuck as neurocod
#pragma once
class ThumbModel;

class ThumbDelegate: public QStyledItemDelegate {
	public:
		ThumbDelegate(ThumbModel*parent);
		virtual ~ThumbDelegate() {}
		virtual void paint(QPainter* painter, const QStyleOptionViewItem & option, const QModelIndex & index)const override;
	protected:
		ThumbModel* _parent;
		void paintTextWhereMoreSpace(QPainter* painter, const QString & str, const QStyleOptionViewItem & option, const QSize & szSmall)const;
};