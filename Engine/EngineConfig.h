#ifndef _ENGINECONFIG_H_
#define _ENGINECONFIG_H_

namespace EngineConfig {

	class DisplayConfig {
	public:
		static const int SampleCount = 1;
		static const bool UseMultiSample = SampleCount > 1;
		static const int ScreenWidth;
		static const int ScreenHeight;
		static const float ScreenFarPlane;
		static const float ScreenNearPlaen;
		static const bool UseVSync = true;
		static const bool UseFullScreen = false;
	};
};

#endif