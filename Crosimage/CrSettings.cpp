//CrSettings.cpp by Kostya Kozachuck as neurocod - 2016.02.19 17:49:55
#include "pch.h"
#include "CrSettings.h"

CrSettings CrSettings::_inst;
const CrSettings & CrSettings::inst() {
	return _inst;
}
void CrSettings::load() {
	_inst.update(false);
}
void CrSettings::setInst(const CrSettings & cr) {
	_inst = cr;
	_inst.update(true);
}
CrSettings::CrSettings(): _thumbW(325), _thumbH(325) {
	_clrBase = Qt::black;
	_clrText = Qt::white;
}
void CrSettings::update(bool save) {
	Settings sett;
	sett.update(save, "shutdownCommand", _shutdownCommand);
	sett.updateRestricted(save, "thumbW", _thumbW);
	sett.updateRestricted(save, "thumbH", _thumbH);
}