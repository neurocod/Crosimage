﻿//ThumbDelegate.cpp by Kostya Kozachuck as neurocod
#include "pch.h"
#include "ThumbDelegate.h"
#include "ThumbModel.h"
#include "ThumbCache.h"

ThumbDelegate::ThumbDelegate(ThumbModel*parent): QStyledItemDelegate(parent) {
	m_parent = parent;
	ASSERT(parent);
}
void ThumbDelegate::paint(QPainter* painter, const QStyleOptionViewItem & option, const QModelIndex & index)const {
	auto item = m_parent->itemAt(index);
	if(!item) {
		__super::paint(painter, option, index);
		return;
	}
	auto img = ThumbCache::instance().get(*item);
	auto pt = option.rect.topLeft();
	if(const bool center = 1) {
		int diffX = ThumbModel::s_nThumbW - img.width();
		if(diffX>0) {
			if(index.column()==0 && index.model()->columnCount()>1)
				pt.rx() += diffX;
			else if(index.model()->columnCount()>1 && index.column()==index.model()->columnCount()-1)
				;
			else
				pt.rx() += diffX/2;
		}
	}
	painter->drawImage(pt, img);
	paintTextWhereMoreSpace(painter, item->fileName(), option, img.size());
	if(option.state & QStyle::State_Selected) {
		QPen pen(Qt::red);
		pen.setWidth(2);
		pen.setStyle(Qt::DashLine);
		painter->setPen(pen);
		auto rc = option.rect;
		rc.adjust(1, 1,-1,-1);
		painter->drawRect(rc);
	}
}
void ThumbDelegate::paintTextWhereMoreSpace(QPainter* painter, const QString & str, const QStyleOptionViewItem & option,
	const QSize & szImg)const
{
	QPen pen(option.palette.color(QPalette::Text));
	painter->setPen(pen);
	auto rc = option.rect;
	auto fontH = option.fontMetrics.height();
	int szFreeRight = option.rect.width() - szImg.width();
	int szFreeBottom = option.rect.height() - szImg.height();
	if(szFreeBottom<=fontH && szFreeRight<=ThumbModel::s_nThumbW/3) {
		if(rc.height() > fontH)
			rc.setTop(rc.bottom()-fontH);
		QColor clr = option.palette.color(QPalette::Base);
		clr.setAlpha(128);
		painter->fillRect(rc, clr);
		painter->drawText(rc, Qt::TextWrapAnywhere, str);
		return;
	}
	if(szFreeBottom>=szFreeRight) {
		rc.setTop(rc.top() + szImg.height());
	} else {
		rc.setLeft(rc.left() + szImg.width());
	}
	painter->drawText(rc, Qt::TextWrapAnywhere, str);
}