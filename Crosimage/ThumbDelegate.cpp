﻿//ThumbDelegate.cpp by Kostya Kozachuck as neurocod
#include "pch.h"
#include "ThumbDelegate.h"
#include "ThumbModel.h"
#include "ThumbCache.h"
#include "ThumbDirPainter.h"

ThumbDelegate::ThumbDelegate(ThumbModel*parent): QStyledItemDelegate(parent) {
	_parent = parent;
	ASSERT(parent);
}
void ThumbDelegate::paint(QPainter* painter, const QStyleOptionViewItem & option, const QModelIndex & index)const {
	auto item = _parent->itemBy(index);
	if(!item) {
		__super::paint(painter, option, index);
		return;
	}
	if(item->fileInfo().isDir())
		painter->fillRect(option.rect, ThumbDirPainter::dirColor());
	{
		QPen pen(Qt::gray);
		pen.setStyle(Qt::DashLine);
		painter->setPen(pen);
		auto rc = option.rect;
		rc.adjust(0, 0,-1,-1);
		painter->drawRect(rc);
	}
	const auto & img = item->thumbnail();
	ThumbCache::instance().maybeMakeFirst(item->absoluteFilePath());
	auto pt = option.rect.topLeft();
	if(const bool center = 1) {
		int diffX = CrSettings::inst()._thumbW - img.width();
		if(diffX>0) { // if actual thumb width is less than table thumb cell,
			if(index.column()==0 && index.model()->columnCount()>1)
				pt.rx() += diffX; // first column images have offset to the right so eye stiicks closer to the window center
			else if(index.model()->columnCount()>1 && index.column()==index.model()->columnCount()-1)
				; // last column has 0 offset so it is close to the columns on the left
			else
				pt.rx() += diffX/2; // center horizontally
		}
	}
	{
		QRect rc(0, 0, qMin(img.width(), option.rect.width()), qMin(img.height(), option.rect.height()));
		painter->drawImage(pt, img, rc);
	}
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
	if(szFreeBottom<=fontH && szFreeRight<=CrSettings::inst()._thumbW/3) {
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