//CrSettings.h by Kostya Kozachuck as neurocod - 2016.02.19 17:49:55
#pragma once

class CrSettings {
	public:
		CrSettings();
		virtual ~CrSettings() {}

		static const CrSettings & inst();
		static void setInst(const CrSettings & cr);
		static void load();

		RestrictedValueTT<int, 20, 1000> _thumbW;
		RestrictedValueTT<int, 20, 1000> _thumbH;
		QString _shutdownCommand;
	protected:
		static CrSettings _inst;

		void update(bool save);
};