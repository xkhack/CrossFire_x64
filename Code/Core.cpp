#include <stdafx.h>
namespace Core {
	PVOID TargetPawn = nullptr;
	bool m_Remotekill(IntersectQuery* pQuery, IntersectInfo* pInfo) {
		if (Core::TargetPawn){
			D3DXVECTOR3 Pos;
			//Setting.AimBot.AimPoint = random_2(1, 10);
			if (room.GetInstance()->type == 1) {
				auto Pawn = (AIClientObjectManager*)Core::TargetPawn;
				if (Setting.AimBot.AimPoint == 0) {
					Pawn->pModelInstance->GetBonePos("M-bone Head", Pos);
				}
				else if (Setting.AimBot.AimPoint == 1) {
					Pawn->pModelInstance->GetBonePos("M-bone Spine1", Pos);
				}
				else if (Setting.AimBot.AimPoint == 2) {
					Pawn->pModelInstance->GetBonePos("M-bone Pelvis", Pos);
				}
				else if (Setting.AimBot.AimPoint == 3) {
					Pawn->pModelInstance->GetBonePos("M-bone R Foot", Pos);
				}
				else if (Setting.AimBot.AimPoint == 4) {
					Pawn->pModelInstance->GetBonePos("M-bone R Thigh", Pos);
				}
				else if (Setting.AimBot.AimPoint == 5) {
					Pawn->pModelInstance->GetBonePos("M-bone L Toe0", Pos);
				}
				else if (Setting.AimBot.AimPoint == 6) {
					Pawn->pModelInstance->GetBonePos("M-bone L Foot", Pos);
				}
				else if (Setting.AimBot.AimPoint == 7) {
					Pawn->pModelInstance->GetBonePos("M-bone L Calf", Pos);
				}
				else if (Setting.AimBot.AimPoint == 8) {
					Pawn->pModelInstance->GetBonePos("M-bone L Thigh", Pos);
				}
				else if (Setting.AimBot.AimPoint == 9) {
					Pawn->pModelInstance->GetBonePos("M-bone Spine", Pos);
				}
				else if (Setting.AimBot.AimPoint == 10) {
					Pawn->pModelInstance->GetBonePos("M-bone Neck", Pos);
				}
			}
			else {
				auto Pawn = (Player*)Core::TargetPawn;
				if (Setting.AimBot.AimPoint == 0) {
					Pawn->pModelInstance->GetBonePos("M-bone Head", Pos);
				}
				else if (Setting.AimBot.AimPoint == 1) {
					Pawn->pModelInstance->GetBonePos("M-bone Spine1", Pos);
				}
				else if (Setting.AimBot.AimPoint == 2) {
					Pawn->pModelInstance->GetBonePos("M-bone Pelvis", Pos);
				}
				else if (Setting.AimBot.AimPoint == 3) {
					Pawn->pModelInstance->GetBonePos("M-bone R Foot", Pos);
				}
				else if (Setting.AimBot.AimPoint == 4) {
					Pawn->pModelInstance->GetBonePos("M-bone R Thigh", Pos);
				}
				else if (Setting.AimBot.AimPoint == 5) {
					Pawn->pModelInstance->GetBonePos("M-bone L Toe0", Pos);
				}
				else if (Setting.AimBot.AimPoint == 6) {
					Pawn->pModelInstance->GetBonePos("M-bone L Foot", Pos);
				}
				else if (Setting.AimBot.AimPoint == 7) {
					Pawn->pModelInstance->GetBonePos("M-bone L Calf", Pos);
				}
				else if (Setting.AimBot.AimPoint == 8) {
					Pawn->pModelInstance->GetBonePos("M-bone L Thigh", Pos);
				}
				else if (Setting.AimBot.AimPoint == 9) {
					Pawn->pModelInstance->GetBonePos("M-bone Spine", Pos);
				}
				else if (Setting.AimBot.AimPoint == 10) {
					Pawn->pModelInstance->GetBonePos("M-bone Neck", Pos);
				}
			}
			Pos += D3DXVECTOR3(0.0f, 15.0f, 0.0f);
			pQuery->m_To = Pos;
			pQuery->m_Flags = 7;
			g_IntersectSegment(*pQuery, pInfo);
		}
		return g_IntersectSegment(*pQuery, pInfo);
	}
	void AimBot() {
		if (Core::TargetPawn && Setting.AimBot.SilentTracking) {
			D3DXVECTOR3 Pos;   //etting.AimBot.SilentTracking

			auto Pawn = (Player*)Core::TargetPawn;
			if (Setting.AimBot.AimPoint == 0) {
				Pawn->pModelInstance->GetBonePos(xorstr("M-bone Head"), Pos);
				Pos += D3DXVECTOR3(0.0f, 15.0f, 0.0f);
				//Pos.y += 40.f;
				Pos.y -= 8.f;
			}
			else {
				Pawn->pModelInstance->GetBonePos(xorstr("M-bone Spine"), Pos);
			}

			if (World2Screen(&Pos)) {
				float x, y;
				float GameX, GameY;
				float Speed = (float)Setting.AimBot.AimSpeed;
				GameX = g_Width / 2;
				GameY = g_Height / 2;
				x = (Pos.x - GameX) / Speed;
				y = (Pos.y - GameY) / Speed;
				INPUT input;
				input.type = INPUT_MOUSE;
				input.mi.dx = x;
				input.mi.dy = y;
				input.mi.mouseData = 0;
				input.mi.dwFlags = MOUSEEVENTF_HWHEEL | MOUSEEVENTF_MOVE;   //MOUSEEVENTF_ABSOLUTE 代表决对位置  MOUSEEVENTF_MOVE代表移动事件
				input.mi.time = 0;
				input.mi.dwExtraInfo = 0;

				SendInput(1, &input, sizeof(INPUT));
			}
		}
	}
	bool IsVisible(D3DXVECTOR3 From, D3DXVECTOR3 To) {
		IntersectQuery pQuery;
		IntersectInfo pInfo;
		pQuery.m_From = From;
		pQuery.m_To = To;
		return !g_IntersectSegment(pQuery, &pInfo);
	}
	void Hook_Remotekill() {
		g_CLTClient->IntersectSegment = (uint64_t)m_Remotekill;
	}
	namespace Vector
	{
		D3DXVECTOR3 Add(D3DXVECTOR3 point1, D3DXVECTOR3 point2)
		{
			D3DXVECTOR3 vector{ 0, 0, 0 };
			vector.x = point1.x + point2.x;
			vector.y = point1.y + point2.y;
			vector.z = point1.z + point2.z;
			return vector;
		}

		D3DXVECTOR3 Subtract(D3DXVECTOR3 point1, D3DXVECTOR3 point2)
		{
			D3DXVECTOR3 vector{ 0, 0, 0 };
			vector.x = point1.x - point2.x;
			vector.y = point1.y - point2.y;
			vector.z = point1.z - point2.z;
			return vector;
		}

		D3DXVECTOR3 Square(D3DXVECTOR3 vector)
		{
			return D3DXVECTOR3{ vector.x * vector.x, vector.y * vector.y, vector.z * vector.z };
		}
		float ScreenDis(D3DXVECTOR3 vector, float GameX, float GameY) {
			D3DXVECTOR3 Vec;
			Vec.x = vector.x - GameX;
			Vec.y = vector.y - GameY;
			float dis = sqrtf(Vec.x * Vec.x + Vec.y * Vec.y);
			return dis;
		}
		float Get2DDis(D3DXVECTOR3 vector, float GameX, float GameY) {
			D3DXVECTOR3 Vec;
			Vec.x = GameX - vector.x;
			Vec.y = GameY - vector.y;
			float dis = sqrtf(powf(Vec.x, 2) + powf(Vec.y, 2));
			return dis;
		}
		float GetDistance(D3DXVECTOR3 x, D3DXVECTOR3 y)
		{
			auto z = Vector::Subtract(x, y);
			return sqrtf(z.x * z.x + z.y * z.y + z.z * z.z);
		}
	}
}

