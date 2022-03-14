#pragma once
typedef struct {
	struct {
		bool CrossHairs;
		bool Box;
		bool Skeleton;
		bool Ray;
		bool EyeLiner;
		bool Health;
		bool HealthBar;
		bool Name;
		bool Kill;
		bool WeaponName;
		bool IsC4;
		float VisibleColor[3];
		float NotVisibleColor[3];
	}ESP;
	struct {
		bool GroundWeapon;
		bool GroundSupply;
	}Item;
	struct {
		bool Perspective;
		bool Highlight;
		bool Streak;
		bool Coloring;
		bool ShowGhost;
		bool Glow;
		float GlowVisibleColor[3];
		float GlowNotVisibleColor[3];
		float GlowHeight;
	}Vision;

	struct {
		bool SilentTracking;
		bool MouseAim;
		float MouseAimSpeed;
		float AimSpeed;
		int AimHotkey;
		float AimFOV;
		bool Aim360;
		bool DrawFOV;
		int AimPoint;
		bool RandomAimPosition;
		bool IsVisible;
	}AimBot;
	struct {





	}Weapon;
	struct {
		bool Teleport;



	}Memory;
	struct {




	}Zombie;


} SETTINGS;

extern SETTINGS Setting;
namespace SettingsHelper {
	VOID Initialize();
	VOID SaveSettings();
}