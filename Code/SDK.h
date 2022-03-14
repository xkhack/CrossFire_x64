#pragma once
#include "stdafx.h"
#include <iltmessage.h>
template< class type > type GetVFunction(PVOID base, SIZE_T index)
{
	ULONG_PTR* vtablefunc = *(PDWORD_PTR*)base;
	return (type)(vtablefunc[index]);
}
template<class T>
struct TVector2 {
	T x, y;



	TVector2()
	{}
	TVector2(const T x, const T y)
		: x(x), y(y)
	{}
};
struct IntersectInfo
{
	D3DXVECTOR3 vImpactPos;
	char spacer00[32];
	__int64 hObjImpact;
	unsigned long unk1;
	unsigned long unk2;
	unsigned long unk3;
};
class IntersectQuery {
public:
	IntersectQuery() {
		m_Flags = 0;
		m_FilterFn = 0;
		m_PolyFilterFn = 0;
		m_FilterActualIntersectFn = 0;
		m_pActualIntersectUserData = NULL;
		m_pUserData = NULL;
	}
	D3DXVECTOR3 m_From;
	D3DXVECTOR3 m_To;
	uint32_t m_Flags;
	void* m_FilterFn;
	__int64 m_FilterActualIntersectFn;
	__int64 m_PolyFilterFn;
	void* m_pUserData;
	void* m_pActualIntersectUserData;
};
struct ALTRotation {
	float w, x, z, y;

};
struct BoxRect {
	float left;
	float top;
	float right;
	float bottom;
};


class CPlayerClnt
{
public:
	char pad_0000[716]; //0x0000
	int32_t Ammo; //0x02CC
	int32_t MaxAmmo; //0x02D0
	int32_t VisualAmmo; //0x02D4
	char pad_02D8[16]; //0x02D8
	class PlayerViewManager* PlayerViewMgr; //0x02E8
	char pad_02F0[80]; //0x02F0
	class SkillBlinkDash* pSkillBlinkDash; //0x0338
	char pad_0340[608]; //0x0340
	float Spacer; //0x05A0
	float Yaw; //0x05A4
	float Pitch; //0x05A8
	char pad_05AC[64]; //0x05AC
	float Spacer1; //0x05EC
	D3DXVECTOR3 LocalPos; //0x05F0
	char pad_05FC[88]; //0x05FC
	D3DXVECTOR3 Gravity; //0x0654
}; //Size: 0x1394


class CSKickingUI
{
public:
	char pad_0000[16]; //0x0000
	int64_t IsKick; //0x0010
	int64_t Reason; //0x0018
	int64_t KickOut; //0x0020
	char KickerName[16]; //0x0028
	char KickedName[16]; //0x0038
	char pad_0040[268]; //0x0048
	int32_t Approve; //0x0154
	int32_t Opposition; //0x0158
}; //Size: 0x015C

class CGameUI
{
public:
	char pad_0000[8]; //0x0000
	class N00000B0B* N00000AED; //0x0008
	char pad_0010[8]; //0x0010
	class CGui* pCGui; //0x0018
	class CRadarUI* pCRadarUI; //0x0020
	class CStatsGui* pCStatsGui; //0x0028
	class CSetWeaponUI* pCSetWeaponUI; //0x0030
	char pad_0038[27984];//0x0038
	CSKickingUI* pCSKickingUI;//0x6D88
}; //Size: 0x6D90

class ModeNodeArrayInfo {
public:
	char pad_0000[268];//0x0000
	char BoneNmae[32];//0x10C
};

class ModeNodeArray {
public:
	ModeNodeArrayInfo* Bone[200];
};

class ModelProperties
{
public:
	char* CharacterLTBName; //0x0000
	int64_t SomeKindOfIndex; //0x0008
	char pad_0010[8]; //0x0010
	ModeNodeArray* pMNArr; //0x0018
	int32_t NodeCount; //0x0020
}; //Size: 0x0088

class CD3DRenderStyle
{
public:
	char pad_0000[2128]; //0x0000
}; //Size: 0x0850

class BoneArray//Not array of pointer rather D3DXVECTOR4[NodeCount]
{
public:
	D3DXMATRIX BoneMatrix[200];//Its not really a D3DXMATRIX it's a different data type
}; //Size: 0x0068

class ModelInstanceType {
public:
	int32_t Types; //0x128
};

class Zombie {
public:
	char pad_0000[8]; //0x0000
	D3DXVECTOR3 Min; //0x0008
	D3DXVECTOR3 Max; //0x0014
	char pad_0020[240]; //0x0020
	D3DXVECTOR3 ModelScale; //0x0110
	int32_t ModelType; //0x011C
	int32_t ValidModel; //0x0120
	char pad_0124[4]; //0x0124
	class ModelInstanceType* pModelInstanceType; //0x0128
	char pad_0130[4]; //0x0130
	D3DXVECTOR3 N00000D6E; //0x0134
	char pad_0140[24]; //0x0140
	float Height; //0x0158
	D3DXVECTOR3 Position; //0x015C
	D3DXVECTOR3 Rotation; //0x0168
	char pad_0174[4]; //0x0174
	float Velocity; //0x0178
	char pad_017C[8]; //0x017C
	D3DXVECTOR3 GlobalForceOverride; //0x0184
	char pad_0190[5352]; //0x0190
	ModelProperties* pModelProperties; //0x1678
	char pad_1680[104]; //0x1680
	class CD3DRenderStyle* pCD3DRenderStyle; //0x16E8
	char pad_16F0[4488]; //0x16F0
	BoneArray* pBoneArray; //0x2878

	int32_t GetBoneIndex(const char* Name) {
		if (!this->pModelProperties) return NULL;
		auto BoneNode = this->pModelProperties->pMNArr;
		auto BoneNum = this->pModelProperties->NodeCount;
		for (int32_t i = 0; i < BoneNum; i++) {
			if (strstr(BoneNode->Bone[i]->BoneNmae, Name)) {
				return i;
			}
		}
		return NULL;
	}

	bool GetBonePos(const char* Name, D3DXVECTOR3& out) {
		auto boneindex = GetBoneIndex(Name);
		if (!this->pBoneArray)return false;
		auto Bone = this->pBoneArray->BoneMatrix[boneindex];
		out = D3DXVECTOR3(Bone._14, Bone._24, Bone._34);
		return true;
	}
};


class ZombieModelInstance
{
public:
	int32_t Type; //0x0000
	char pad_0004[68988]; //0x0004
	class Zombie* pZombie; //0x10D80
}; //Size: 0x10D78

class ModelInstance
{
public:
	char pad_0000[8]; //0x0000
	D3DXVECTOR3 Min; //0x0008
	D3DXVECTOR3 Max; //0x0014
	char pad_0020[240]; //0x0020
	D3DXVECTOR3 ModelScale; //0x0110
	int32_t ModelType; //0x011C
	int32_t ValidModel; //0x0120
	char pad_0124[4]; //0x0124
	ZombieModelInstance* pModelInstanceType; //0x0128
	char pad_0130[4]; //0x0130
	D3DXVECTOR3 N00000D6E; //0x0134
	char pad_0140[24]; //0x0140
	float Height; //0x0158
	D3DXVECTOR3 Position; //0x015C
	char pad_168[4]; //0x168
	float Rotationy; //0x16C
	char pad_170[4]; //0x170
	float Rotationx; //0x0174
	float Velocity; //0x0178
	char pad_017C[8]; //0x017C
	D3DXVECTOR3 GlobalForceOverride; //0x0184
	char pad_0190[5352]; //0x0190
	ModelProperties* pModelProperties; //0x1678
	char pad_1680[104]; //0x1680
	class CD3DRenderStyle* pCD3DRenderStyle; //0x16E8
	char pad_16F0[4488]; //0x16F0
	BoneArray* pBoneArray; //0x2878
	bool IsZombie() {
		if (!this->pModelInstanceType) return false;
		if (this->pModelInstanceType->Type == 1) {
			if (!this->pModelInstanceType->pZombie) return false;
			if (!this->pModelInstanceType->pZombie->pModelInstanceType) return false;
			if (this->pModelInstanceType->pZombie->pModelInstanceType->Types == 8) {
				return true;
			}
		}
		return false;
	}

	auto GetModelInstance() {
		if (IsZombie()) {
			return (ModelInstance*)this->pModelInstanceType->pZombie;
		}
		else {
			return this;
		}
		return this;
	}


	int32_t GetBoneIndex(const char* Name) {

		if (!this->pModelProperties) return NULL;
		auto BoneNode = this->pModelProperties->pMNArr;
		auto BoneNum = this->pModelProperties->NodeCount;
		for (int32_t i = 0; i < BoneNum; i++) {
			if (strstr(BoneNode->Bone[i]->BoneNmae, Name)) {
				return i;
			}
		}

		return NULL;
	}

	bool GetBonePos(const char* Name, D3DXVECTOR3& out) {
		if (IsZombie()) {
			return this->pModelInstanceType->pZombie->GetBonePos(Name, out);
		}
		else {
			auto boneindex = GetBoneIndex(Name);
			if (!this->pBoneArray)return false;
			auto Bone = this->pBoneArray->BoneMatrix[boneindex];
			out = D3DXVECTOR3(Bone._14, Bone._24, Bone._34);
			return true;
		}
	}

}; //Size: 0x09E0
class WeaponName
{
public:
	int64_t WeaponID; //0x0000
	int32_t Spacer; //0x0008
	int16_t Spacer1; //0x000C
	char WeaponName[100]; //0x000E
}; //Size: 0x0072
class CCharaFx
{
public:
	char pad_0000[564]; //0x0000
	int32_t IsAlive; //0x0234
	char pad_0238[69680]; //0x0238
	WeaponName* pWeaponName; //0x11268
}; //Size: 0x0048
class Player
{
public:
	class ModelInstance* pModelInstance; //0x0000
	int8_t Index; //0x0008
	int8_t Team; //0x0009
	char Name[14]; //0x000A
	CCharaFx* pCharaFx; //0x0018
	char pad_0220[8]; //0x0020
	int32_t HasC4; //0x0028
	char pad_002C[4]; //0x002C
	int32_t Rank; //0x0030
	char pad_0030[16]; //0x0034
	int32_t HP; //0x0044
	int32_t Kill; //0x0048

	bool IsAlive() {
		auto pCharFx = this->pCharaFx;
		if (IsBadReadPtr(pCharFx,8)) return false;
		if (pCharFx->IsAlive == 0){
			return true;
		}
		return false;
	}
	bool IsC4() {
		if (this->HasC4 == 1) {
			return true;
		}
		return false;
	}
};
class RoomManager {
public:
	char pad_0000[28];//0x0000
	uint32_t type;
	static auto GetInstance() {
		auto addr = *RoomManagera;
		return reinterpret_cast<RoomManager*>(addr);
	}
};

extern RoomManager room;

class CLTClientShell
{
public:
	char pad_0000[72]; //0x0000
	class CCamera* pCamera; //0x0048
	char pad_0050[72]; //0x0050
	CPlayerClnt* pPlayerBase; //0x0098
	int32_t InGame; //0x00A0
	char pad_00A8[460]; //0x00A8
	CGameUI* pGameUI; //0x026C
	char pad_0270[24]; //0x0274
	int64_t LocalIndex; //0x0290

	Player* GetPlayer(int32_t index) {
		return reinterpret_cast<Player*>((uintptr_t)this + 0x298 + (index * 0xDA0));
	}
	Player* GetLocal() {
		return GetPlayer(this->LocalIndex);
	}
	bool TeamCheck(int32_t index) {
		auto Me = GetPlayer(this->LocalIndex);
		auto Them = GetPlayer(index);
		auto Mea = Me->pModelInstance->IsZombie();
		auto Thema = Them->pModelInstance->IsZombie(); 
		if (room.GetInstance()->type == 2) {
			return (Mea == Thema);
		}
		else{
			return (Me->Team == Them->Team);
		}
	}
}; 
class CLTClient
{
public:
	char pad_0000[336];//0000
	__int64 IntersectSegment;
	uint64_t GetObjectRotation(ModelInstance* hObj, ALTRotation* Rotation) {
		typedef uint64_t(__fastcall* oGetObjectRotation)(void*, ModelInstance*, ALTRotation*);
		return GetVFunction<oGetObjectRotation>(this, 39)(this, hObj, Rotation);
	}
	uint64_t GetObjectPos(ModelInstance* hObj, D3DXVECTOR3* pPos) {
		typedef uint64_t(__fastcall* oGetObjectPos)(void*, ModelInstance*, D3DXVECTOR3*);
		return GetVFunction<oGetObjectPos>(this, 40)(this, hObj, pPos);
	}
	uint64_t SetObjectOutLine(ModelInstance* hObj, bool Status, uint8_t r, uint8_t g, uint8_t b) {
		typedef uint64_t(__fastcall* oSetObjectOutLine)(void*, ModelInstance*, uint8_t, uint8_t, uint8_t, uint8_t);
		return GetVFunction<oSetObjectOutLine>(this, 74)(this, hObj, Status, r, g, b);
	}
	uint64_t SendToServer(ILTMessage_Read* pMsg, uint32 flags) {
		typedef uint64_t(__fastcall* oSendToServer)(void*, ILTMessage_Read*, uint32);
		return GetVFunction<oSendToServer>(this, 127)(this, pMsg, flags);
	}
};
#define CLTClientoffset 0x9D8


class BaseAddress
{
public:
	class CLTClientShell* pCLTClientShell; //0x0000
	static auto GetInstance() {
		return reinterpret_cast<BaseAddress*>(pBaseAddress);
	}

}; //Size: 0x0438

class AIClientObjectManager
{
public:
	char pad_0000[208];
	Zombie* pModelInstance; // 0x00D0
	char pad_00A4[348]; //0x00D8
	int32_t IsAliveZ; //0x0234
	char pad_0204[100292];//0x238
	int32_t HP; //0x189FC
	int32_t MaxHP;//0x18A00
	bool IsAlive() {
		if (IsAliveZ == 0) {
			return true;
		}
		return false;
	}
};
class WeaponMger
{
public:
	class WeaponName* Index[0x11FF];
	static auto GetInstance() {
		DWORD64 Addr = *pWeaponMgr;
		return reinterpret_cast<WeaponMger*>(Addr);
	}
};
class CDroppedWeaponFX {
public:
	char pad_0000[82]; //0x0000
	WORD Index; //0x0052
};
class ItemType {
public:
	uint64_t Type;
	CDroppedWeaponFX* pCDroppedWeaponFX;
	const char* GetWeaponName(WeaponMger* Weap) {
		int Index = this->pCDroppedWeaponFX->Index;
		return Weap->Index[Index]->WeaponName;
	}
};
class ItemObject {
public:
	char pad_0000[296]; //0x0000
	ItemType* Object_j; //0x0128
	char pad_0130[44]; //0x0130
	D3DXVECTOR3 Pos;
};
class ItemArray
{
public:
	 ItemObject* Object[200];//0x0000
};
class WorldAddress
{
public:
	ItemArray* Item;//0x0000
	uint64_t Count;//0x0008
	static auto GetInstance() {
		return reinterpret_cast<WorldAddress*>(pWorldAddress);
	}
};
enum
{
	INTERSECT_OBJECTS = 1,
	IGNORE_NONSOLID = 2,
	INTERSECT_HPOLY = 4,
	CHECK_FROM_POINT_INSIDE_OBJECTS = 8,
	INTERSECT_MODELOBBS = 16,
};

typedef bool(WINAPIV* mIntersectSegment)(const IntersectQuery& iQuery, IntersectInfo* pInfo);
inline mIntersectSegment g_IntersectSegment = nullptr;

typedef bool(__fastcall* mGameMessagesBox)(__int64 a1, __int64 a2, __int64 a3 ,const char* txt, DWORD a4, __int64 a5, __int64 a6, __int64 a7);
inline mGameMessagesBox g_GameMessagesBox = nullptr;
inline CLTClient* g_CLTClient = nullptr;
inline float g_Height, g_Width;
extern BaseAddress Base;
extern WorldAddress g_World;
extern WeaponMger g_Weapon;
