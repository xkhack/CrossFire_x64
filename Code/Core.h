#pragma once
#include <stdafx.h>
namespace Core {
	extern PVOID TargetPawn;
	void Hook_Remotekill();
	EXTERN_C void AimBot();
	bool IsVisible(D3DXVECTOR3 From, D3DXVECTOR3 To);
	namespace Vector
	{
		D3DXVECTOR3 Add(D3DXVECTOR3 point1, D3DXVECTOR3 point2);
		D3DXVECTOR3 Subtract(D3DXVECTOR3 point1, D3DXVECTOR3 point2);
		D3DXVECTOR3 Square(D3DXVECTOR3 vector);
		float ScreenDis(D3DXVECTOR3 vector, float GameX, float GameY);
		float Get2DDis(D3DXVECTOR3 vector, float GameX, float GameY);
		float GetDistance(D3DXVECTOR3 x, D3DXVECTOR3 y);
	};
};