#include <stdafx.h>
SETTINGS Setting = { 0 };
namespace SettingsHelper {
	VOID SaveSettings() {
		CHAR path[0xFF];
		GetTempPathA(sizeof(path) / sizeof(path[0]), path);
		strcat(path, ("fnambt.settings"));

		auto file = fopen(path, ("wb"));
		if (file) {
			fwrite(&Setting, sizeof(Setting), 1, file);
			fclose(file);
		}
	}

	VOID ResetSettings() {
		Setting = { 0 };
		Setting.ESP.Box = true;
		Setting.ESP.CrossHairs = true;
		Setting.ESP.Skeleton = true;
		Setting.AimBot.SilentTracking = FALSE;
		Setting.AimBot.AimPoint = { 0 };
		Setting.AimBot.AimFOV = 100.f;
		Setting.AimBot.AimSpeed = 1.0f;
		Setting.ESP.VisibleColor[0] = 1.0f;
		Setting.ESP.VisibleColor[1] = 0.0f;
		Setting.ESP.VisibleColor[2] = 0.0f;
		Setting.ESP.NotVisibleColor[0] = 0.0f;
		Setting.ESP.NotVisibleColor[1] = 1.0f;
		Setting.ESP.NotVisibleColor[2] = 0.0f;


		Setting.Vision.GlowVisibleColor[0] = 1.0f;
		Setting.Vision.GlowVisibleColor[1] = 0.0f;
		Setting.Vision.GlowVisibleColor[2] = 0.0f;
		Setting.Vision.GlowNotVisibleColor[0] = 0.0f;
		Setting.Vision.GlowNotVisibleColor[1] = 1.0f;
		Setting.Vision.GlowNotVisibleColor[2] = 0.0f;


		Setting.Vision.GlowHeight = 1.8f;

		SaveSettings();
	}
	VOID Initialize() {
		CHAR path[0xFF] = { 0 };
		GetTempPathA(sizeof(path) / sizeof(path[0]), path);
		strcat(path, ("fnambt.settings"));

		auto file = fopen(path, ("rb"));
		if (file) {
			fseek(file, 0, SEEK_END);
			auto size = ftell(file);

			if (size == sizeof(Setting)) {
				fseek(file, 0, SEEK_SET);
				fread(&Setting, sizeof(Setting), 1, file);
				fclose(file);
			}
			else {
				fclose(file);
				ResetSettings();
			}
		}
		else {
			ResetSettings();
		}
	}
}