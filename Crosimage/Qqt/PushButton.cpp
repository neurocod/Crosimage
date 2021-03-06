﻿//PushButton.cpp by Kostya Kozachuck as neurocod - 27.08.2011 21:20:17
#include "pch.h"
#include "PushButton.h"

void PushButton::setDefaultAction(QAction *action) {
	_defaultAction = action;
    if (!action)
        return;
    d->setText(action->iconText());
    d->setIcon(action->icon());
#ifndef QT_NO_TOOLTIP
    d->setToolTip(action->toolTip());
#endif
#ifndef QT_NO_STATUSTIP
    d->setStatusTip(action->statusTip());
#endif
#ifndef QT_NO_WHATSTHIS
    d->setWhatsThis(action->whatsThis());
#endif
    d->setCheckable(action->isCheckable());
    d->setChecked(action->isChecked());
    d->setEnabled(action->isEnabled());
	QObject::connect(d, &QPushButton::clicked, action, &QAction::trigger);
}