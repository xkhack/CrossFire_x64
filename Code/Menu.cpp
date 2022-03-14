#include <stdafx.h>
#include <AutoMessage.h>
#include <iltmessage.h>
#include <MsgIDs.h>
#define random_2(a,b) ((rand()%(b-a+1))+a)

HRESULT(*ResetOriginal)(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters) = nullptr;
HRESULT(*PresentOriginal)(IDirect3DDevice9* pDevice, RECT* pSourceRect, RECT* pDestRect, HWND hDestWindowOverride, RGNDATA* pDirtyRegion) = nullptr;
DWORD(__fastcall* AIManagerOriginal)(AIClientObjectManager* AIClientObject) = nullptr;
WNDPROC oWndProc = nullptr;
HWND Window;
bool ShowMenu = true;
IDirect3DDevice9* g_pd3dDevice = NULL;
//float g_Height, g_Width;
PVOID lastTarget;
CLTClientShell* m_CLTClientShell = nullptr;
WorldAddress* m_World = nullptr;
WeaponMger* m_Weapon = nullptr;
AIClientObjectManager* AIManager;
std::vector<AIClientObjectManager*> AIObjects;
EXTERN_C uint64_t WINAPI _ZwUserGetKeyState_Win7(_In_ int nVirtKey);
EXTERN_C uint64_t _NtUserGetAsyncKeyState(_In_ int nVirtKey);
static SHORT WINAPI GetKeyState3(_In_ int vKey)
{
	static auto lpNtUserGetAsyncKeyStateIndex = 0;
	static FARPROC pfnNtUserGetKeyState = nullptr;
	auto hm = GetModuleHandleA(("win32u.dll"));

	if (hm) //Win10
	{
		pfnNtUserGetKeyState = GetProcAddress(hm, ("NtUserGetKeyState"));
		if (pfnNtUserGetKeyState == NULL) {
			return 0;
		}

		return ((decltype(&GetKeyState3))pfnNtUserGetKeyState)(vKey);
	}
	else { //Win7

		auto Init = GetModuleHandleA(("user32.dll"));
		if (Init == NULL) {
			return 0;
		}
		return (SHORT)_ZwUserGetKeyState_Win7(vKey);
	}
}
static SHORT WINAPI GetAsyncKeyState2(_In_ int vKey)
{
	static auto lpNtUserGetAsyncKeyStateIndex = 0;
	static FARPROC lpNtUserGetAsyncKeyState = nullptr;
	auto hm = GetModuleHandleA(("win32u.dll"));

	if (hm) //Win10
	{
		lpNtUserGetAsyncKeyState = GetProcAddress(hm, ("NtUserGetAsyncKeyState"));
		if (lpNtUserGetAsyncKeyState == NULL) {
			return 0;
		}

		return ((decltype(&_NtUserGetAsyncKeyState))lpNtUserGetAsyncKeyState)(vKey);
	}
	else { //Win7

		auto Init = GetModuleHandleA(("user32.dll"));
		return (SHORT)_NtUserGetAsyncKeyState(vKey);
	}
}
std::string string_To_UTF8(const std::string& str)
{
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

	wchar_t* pwBuf = new wchar_t[nwLen + 1];
	ZeroMemory(pwBuf, nwLen * 2 + 2);

	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char* pBuf = new char[nLen + 1];
	ZeroMemory(pBuf, nLen + 1);

	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr(pBuf);

	delete[]pwBuf;
	delete[]pBuf;

	pwBuf = NULL;
	pBuf = NULL;

	return retStr;
}
void DrawBox(const float x, const float y, const float width, const float height, const uint32_t color, float thickness)
{
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(x, y), ImVec2(x + width, y), color, thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(x, y), ImVec2(x, y + height), color, thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(x, y + height), ImVec2(x + width, y + height), color, thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(x + width, y), ImVec2(x + width, y + height), color, thickness);
}
void FillRectangle(const float x, const float y, const float width, const float hight, const uint32_t color)
{
	const float end_y = y + hight;
	for (float curr_y = y; curr_y < end_y; ++curr_y)
	{
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(x, curr_y), ImVec2(x + width, curr_y), color, 1.5f);
	}
}
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProcHook(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (msg == WM_KEYUP) {
		if (wParam == VK_INSERT) {
			ShowMenu = !ShowMenu;
			ImGui::GetIO().MouseDrawCursor = ShowMenu;
		}
	}
	ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
	return CallWindowProcA(oWndProc, hWnd, msg, wParam, lParam);
}
void DrawLine(int x1, int y1, int x2, int y2, ImVec4 color, int thickness){
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), ImGui::ColorConvertFloat4ToU32(color), thickness);
}
void Draw2DLine_Shu(int x, int y, int width, int length, ImVec4 color)
{
	DrawLine(x, y, x, y + length, color, width);
}
void Draw2DLine_Heng(int x, int y, int length, int height, ImVec4 color)
{
	DrawLine(x, y, x + length, y, color, height);
}
void DrawSuSuPro(int CenterX, int CenterY, ImVec4 color)
{
	int Agcol = 2;
	Draw2DLine_Heng(CenterX - 14, CenterY, 8, 1, color);
	Draw2DLine_Shu(CenterX, CenterY - 12, 1, 8, color);
	Draw2DLine_Heng(CenterX + 6, CenterY, 8, 1, color);
	Draw2DLine_Shu(CenterX, CenterY + 6, 1, 8, color);
	DrawLine(CenterX - (14 + 16), CenterY - 15, CenterX - (14 + 16) + 10, CenterY - 20, color, 2);  // LeftUpTanger
	DrawLine(CenterX + 6 + 8 + 16 - Agcol, CenterY - 15, CenterX + 6 + 8 + 16 - Agcol - 10, CenterY - 20, color, 2); // RightUpTanger
	Draw2DLine_Shu(CenterX - (14 + 16), CenterY - 15, 2, 28, color);  // LeftStrgight
	Draw2DLine_Shu(CenterX + 6 + 8 + 16 - Agcol, CenterY - 15, 2, 28, color); // RightStrgight
	DrawLine(CenterX - (14 + 16), CenterY - 15 + 28, CenterX - (14 + 16) + 10, CenterY - 15 + 28 + 5, color, 2);  //LeftDownTanger
	DrawLine(CenterX + 6 + 8 + 17 - Agcol, CenterY - 15 + 28, CenterX + 6 + 8 + 16 - Agcol - 10, CenterY - 15 + 28 + 5, color, 2); // RightDownTanger
}
bool World2Screen(D3DXVECTOR3* InOut){
	D3DXVECTOR3 vScreen;
	D3DXVECTOR3 PlayerPos(InOut->x, InOut->y, InOut->z);
	D3DVIEWPORT9 viewPort = { 0 };
	D3DXMATRIX projection, view, world;
	g_pd3dDevice->GetTransform(D3DTS_VIEW, &view);
	g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &projection);
	g_pd3dDevice->GetTransform(D3DTS_WORLD, &world);
	g_pd3dDevice->GetViewport(&viewPort);
	g_Height = (float)viewPort.Height;
	g_Width = (float)viewPort.Width;
	D3DXVec3Project(&vScreen, &PlayerPos, &viewPort, &projection, &view, &world);
	if (vScreen.z <= 1.0f) {
		*InOut = vScreen;
		return true;
	}
	return false;
}
bool Bone2RECT(ModelInstance* pModelInstance, BoxRect* out) {
	D3DXVECTOR3 Head , Pelvis;
	if (pModelInstance->GetBonePos("M-bone Head", Head)){
		Head.y += 40.f;
		if (pModelInstance->GetBonePos("M-bone Pelvis", Pelvis)) {
			Pelvis.y -= 160.f;
			if (World2Screen(&Head) && World2Screen(&Pelvis)) {
				out->bottom = Pelvis.y - Head.y;
				out->right = out->bottom / 2;
				out->left = Head.x - out->bottom / 4;
				out->top = Head.y;
				return true;
			}
		}
	}
	return false;
}
void DrawSkeleton(ModelInstance* ModelInstance, DWORD Color) {
	D3DXVECTOR3 Head, Neck, Spine1, Spine, Pelvis, bone, LThigh, LCalf, LFoot, LToe0, RThigh, RCalf, RFoot, RToe0, RClavicle, RUpperArm, RForearm, RHand, LClavicle, LUpperArm, LForearm, LHand;
	if (ModelInstance->GetBonePos(xorstr("M-bone Head"), Head) &&
		ModelInstance->GetBonePos(xorstr("M-bone Neck"), Neck) &&
		ModelInstance->GetBonePos(xorstr("M-bone Spine1"), Spine1) &&
		ModelInstance->GetBonePos(xorstr("M-bone Spine"), Spine) &&
		ModelInstance->GetBonePos(xorstr("M-bone Pelvis"), bone) &&
		ModelInstance->GetBonePos(xorstr("M-bone"), Pelvis) &&
		ModelInstance->GetBonePos(xorstr("M-bone L Thigh"), LThigh) &&
		ModelInstance->GetBonePos(xorstr("M-bone L Calf"), LCalf) &&
		ModelInstance->GetBonePos(xorstr("M-bone L Foot"), LFoot) &&
		ModelInstance->GetBonePos(xorstr("M-bone L Toe0"), LToe0) &&
		ModelInstance->GetBonePos(xorstr("M-bone R Thigh"), RThigh) &&
		ModelInstance->GetBonePos(xorstr("M-bone R Foot"), RFoot) &&
		ModelInstance->GetBonePos(xorstr("M-bone R Calf"), RCalf) &&
		ModelInstance->GetBonePos(xorstr("M-bone R Toe0"), RToe0) &&
		ModelInstance->GetBonePos(xorstr("M-bone R Clavicle"), RClavicle) &&
		ModelInstance->GetBonePos(xorstr("M-bone R UpperArm"), RUpperArm) &&
		ModelInstance->GetBonePos(xorstr("M-bone R Forearm"), RForearm) &&
		ModelInstance->GetBonePos(xorstr("M-bone R Hand"), RHand) &&
		ModelInstance->GetBonePos(xorstr("M-bone L Clavicle"), LClavicle) &&
		ModelInstance->GetBonePos(xorstr("M-bone L UpperArm"), LUpperArm) &&
		ModelInstance->GetBonePos(xorstr("M-bone L Forearm"), LForearm) &&
		ModelInstance->GetBonePos(xorstr("M-bone L Hand"), LHand)) {
		if (World2Screen(&Head) &&
			World2Screen(&Neck) &&
			World2Screen(&Spine1) &&
			World2Screen(&Spine) &&
			World2Screen(&bone) &&
			World2Screen(&Pelvis) &&
			World2Screen(&LThigh) &&
			World2Screen(&LCalf) &&
			World2Screen(&LFoot) &&
			World2Screen(&LToe0) &&
			World2Screen(&RThigh) &&
			World2Screen(&RCalf) &&
			World2Screen(&RFoot) &&
			World2Screen(&RToe0) &&
			World2Screen(&RClavicle) &&
			World2Screen(&RUpperArm) &&
			World2Screen(&RForearm) &&
			World2Screen(&RHand) &&
			World2Screen(&LClavicle) &&
			World2Screen(&LUpperArm) &&
			World2Screen(&LForearm) &&
			World2Screen(&LHand)) {
			ImGui::GetOverlayDrawList()->AddLine(ImVec2{ Head.x, Head.y }, ImVec2{ Neck.x, Neck.y }, Color);
			ImGui::GetOverlayDrawList()->AddLine(ImVec2{ Spine1.x, Spine1.y }, ImVec2{ Neck.x, Neck.y }, Color);
			ImGui::GetOverlayDrawList()->AddLine(ImVec2{ Spine.x, Spine.y }, ImVec2{ Spine1.x, Spine1.y }, Color);
			ImGui::GetOverlayDrawList()->AddLine(ImVec2{ Pelvis.x, Pelvis.y }, ImVec2{ Spine.x, Spine.y }, Color);
			ImGui::GetOverlayDrawList()->AddLine(ImVec2{ bone.x, bone.y }, ImVec2{ Pelvis.x, Pelvis.y }, Color);
			ImGui::GetOverlayDrawList()->AddLine(ImVec2{ LThigh.x, LThigh.y }, ImVec2{ bone.x, bone.y }, Color);
			ImGui::GetOverlayDrawList()->AddLine(ImVec2{ LCalf.x, LCalf.y }, ImVec2{ LThigh.x, LThigh.y }, Color);
			ImGui::GetOverlayDrawList()->AddLine(ImVec2{ LFoot.x, LFoot.y }, ImVec2{ LCalf.x, LCalf.y }, Color);
			ImGui::GetOverlayDrawList()->AddLine(ImVec2{ LToe0.x, LToe0.y }, ImVec2{ LFoot.x, LFoot.y }, Color);
			ImGui::GetOverlayDrawList()->AddLine(ImVec2{ RThigh.x, RThigh.y }, ImVec2{ bone.x, bone.y }, Color);
			ImGui::GetOverlayDrawList()->AddLine(ImVec2{ RCalf.x, RCalf.y }, ImVec2{ RThigh.x, RThigh.y }, Color);
			ImGui::GetOverlayDrawList()->AddLine(ImVec2{ RFoot.x, RFoot.y }, ImVec2{ RCalf.x, RCalf.y }, Color);
			ImGui::GetOverlayDrawList()->AddLine(ImVec2{ RToe0.x, RToe0.y }, ImVec2{ RFoot.x, RFoot.y }, Color);
			ImGui::GetOverlayDrawList()->AddLine(ImVec2{ RClavicle.x, RClavicle.y }, ImVec2{ Neck.x, Neck.y }, Color);
			ImGui::GetOverlayDrawList()->AddLine(ImVec2{ RUpperArm.x, RUpperArm.y }, ImVec2{ RClavicle.x, RClavicle.y }, Color);
			ImGui::GetOverlayDrawList()->AddLine(ImVec2{ RForearm.x, RForearm.y }, ImVec2{ RUpperArm.x, RUpperArm.y }, Color);
			ImGui::GetOverlayDrawList()->AddLine(ImVec2{ RHand.x, RHand.y }, ImVec2{ RForearm.x, RForearm.y }, Color);
			ImGui::GetOverlayDrawList()->AddLine(ImVec2{ LClavicle.x, LClavicle.y }, ImVec2{ Neck.x, Neck.y }, Color);
			ImGui::GetOverlayDrawList()->AddLine(ImVec2{ LUpperArm.x, LUpperArm.y }, ImVec2{ LClavicle.x, LClavicle.y }, Color);
			ImGui::GetOverlayDrawList()->AddLine(ImVec2{ LForearm.x, LForearm.y }, ImVec2{ LUpperArm.x, LUpperArm.y }, Color);
			ImGui::GetOverlayDrawList()->AddLine(ImVec2{ LHand.x, LHand.y }, ImVec2{ LForearm.x, LForearm.y }, Color);
		}
	}
}
void colorPicker(const char* name, float color[4]) noexcept
{
	ImGui::PushID(name);
	ImGui::TextUnformatted(name);
	ImGui::SameLine(0.0f, 5.0f);
	bool openPopup = ImGui::ColorButton("##btn", ImVec4{ color[0],color[1],color[2],color[3] });
	if (openPopup)
		ImGui::OpenPopup("##popup");

	if (ImGui::BeginPopup("##popup")) {
		ImGui::ColorPicker4("##picker", color);

		ImGui::EndPopup();
	}
	ImGui::PopID();
}
void colorPicker(const char* name, ImVec4& color) noexcept{
	colorPicker(name, &color.x);
}
void DrawMenu() {
	//int  AimPointcount = random_2(0, 9);
	if (ShowMenu) {
		ImGui::SetNextWindowSize(ImVec2(250.f, 380.f), ImGuiCond_Once);
		ImGui::Begin(xorstr(u8"INS 隐藏显示菜单")); // start open
		if (ImGui::CollapsingHeader(xorstr(u8"视觉"))) {
			ImGui::Checkbox(xorstr(u8"显示准星"), &Setting.ESP.CrossHairs);
			ImGui::Checkbox(xorstr(u8"显示方框"), &Setting.ESP.Box);
			ImGui::Checkbox(xorstr(u8"显示骨骼"), &Setting.ESP.Skeleton);
			ImGui::Checkbox(xorstr(u8"显示射线"), &Setting.ESP.Ray);
			ImGui::Checkbox(xorstr(u8"显示血量"), &Setting.ESP.Health);
			ImGui::Checkbox(xorstr(u8"显示血条"), &Setting.ESP.HealthBar);
			ImGui::Checkbox(xorstr(u8"显示名字"), &Setting.ESP.Name);
			ImGui::Checkbox(xorstr(u8"显示杀敌"), &Setting.ESP.Kill);
			ImGui::Checkbox(xorstr(u8"显示武器"), &Setting.ESP.WeaponName);
			colorPicker(xorstr(u8"可视颜色"), Setting.ESP.VisibleColor);
			colorPicker(xorstr(u8"不可视颜色"), Setting.ESP.NotVisibleColor);
		}
		if (ImGui::CollapsingHeader(xorstr(u8"瞄准"))) {
			ImGui::Checkbox(xorstr(u8"子弹追踪"), &Setting.AimBot.SilentTracking);
			//ImGui::Checkbox(xorstr(u8"自动瞄准"), &Setting.AimBot.SilentTracking);
			if (Setting.AimBot.SilentTracking || Setting.AimBot.MouseAim) {
				ImGui::Checkbox(xorstr(u8"360 范围"), &Setting.AimBot.Aim360);
				ImGui::SameLine(0.0f, 5.0f);
				ImGui::Checkbox(xorstr(u8"瞄准范围##checkbox"), &Setting.AimBot.DrawFOV);
			}
			const char* Points[] = { u8" 头", u8" 胸", u8" 腰",u8"未知",u8"未知1" ,u8"未知2" ,u8"未知3" ,u8"未知4" ,u8"未知5" ,u8"未知6" };
			ImGui::Combo(xorstr(u8"瞄准部位"), &Setting.AimBot.AimPoint, Points, 10);

			if (!Setting.AimBot.SilentTracking || !Setting.AimBot.MouseAim) {
				ImGui::SliderFloat(xorstr(u8"瞄准范围##slider"), &Setting.AimBot.AimFOV, 0.0f, 1000.0f, xorstr("%.2f"));
			}
			//ImGui::SliderFloat(xorstr(u8"瞄准速率##slider"), &Setting.AimBot.AimSpeed, 1.0f, 20.0f, xorstr("%.2f"));

		}
		
		if (ImGui::CollapsingHeader(xorstr(u8"物品"))) {
			ImGui::Checkbox(xorstr(u8"地面武器"), &Setting.Item.GroundWeapon);
			ImGui::Checkbox(xorstr(u8"地面补给"), &Setting.Item.GroundSupply);
		}
		ImGui::End();
	}
}
void DrawFun() {
	float currentDistance = NULL;
	float currentcenterDistance;
	PVOID closestPawn = NULL;
	float closestDistance = FLT_MAX;
	D3DXVECTOR3 From, To;
	if (m_CLTClientShell->InGame) {
		for (int i = 0; i < 30; i++) {
			BoxRect Box;
			if (Setting.ESP.CrossHairs) {
				//准星
				DrawSuSuPro(g_Width / 2, g_Height / 2, ImVec4{ 1.f,0.f,0.f,1.f });
			}
			if (Setting.AimBot.DrawFOV) {
				//准星
				ImGui::GetOverlayDrawList()->AddCircle(ImVec2(g_Width / 2, g_Height / 2), Setting.AimBot.AimFOV, ImGui::ColorConvertFloat4ToU32(ImVec4{ 1.0f,1.f,1.f,1.f }), 20, 1.0f);
			}
			auto APlayer = m_CLTClientShell->GetPlayer(i);
			auto MPlayer = m_CLTClientShell->GetLocal();
			if (!APlayer) continue;
			if (!APlayer->IsAlive()) continue;
			auto APlayerModelInstance = APlayer->pModelInstance;
			if (!APlayerModelInstance) continue;
			if (m_CLTClientShell->TeamCheck(i)) continue;
			auto Color = ImGui::GetColorU32({ Setting.ESP.NotVisibleColor[0], Setting.ESP.NotVisibleColor[1], Setting.ESP.NotVisibleColor[2], 1.0f });

			uint8_t R, G, B;
			R = Setting.Vision.GlowNotVisibleColor[0] * 255;
			G = Setting.Vision.GlowNotVisibleColor[1] * 255;
			B = Setting.Vision.GlowNotVisibleColor[2] * 255;
			auto IsTar = lastTarget == APlayer;
			MPlayer->pModelInstance->GetBonePos("M-bone Head", From);
			APlayer->pModelInstance->GetBonePos("M-bone Head", To);

			if (Core::IsVisible(From, To)) {
				Color = IsTar ? ImGui::GetColorU32({ 1.0f, 1.0f, 0.0f, 1.0f }) : ImGui::GetColorU32({ Setting.ESP.VisibleColor[0], Setting.ESP.VisibleColor[1], Setting.ESP.VisibleColor[2], 1.0f });
				R = Setting.Vision.GlowVisibleColor[0] * 255;
				G = Setting.Vision.GlowVisibleColor[1] * 255;
				B = Setting.Vision.GlowVisibleColor[2] * 255;
				if (Setting.AimBot.SilentTracking || Setting.AimBot.MouseAim) {
					if (Setting.AimBot.Aim360) {
						auto differ = To - From;
						auto dist = differ.x * differ.x + differ.y * differ.y + differ.z * differ.z;
						if (dist < closestDistance) {
							closestDistance = dist;
							closestPawn = APlayer;
						}
					}
					else {
						if (World2Screen(&To)) {
							if (Core::Vector::ScreenDis(To, g_Width / 2, g_Height / 2) < Setting.AimBot.AimFOV) {
								currentcenterDistance = Core::Vector::Get2DDis(To, g_Width / 2, g_Height / 2);
								if (currentDistance == NULL) {
									currentDistance = currentcenterDistance;
									closestPawn = APlayer;
								}
								if (currentcenterDistance < currentDistance) {
									currentDistance = currentcenterDistance;
									closestPawn = APlayer;
								}
							}
						}
					}
				}
			}
			if (Bone2RECT(APlayerModelInstance, &Box)) {
				if (Setting.ESP.Box) {
					//方框
					ImGui::GetOverlayDrawList()->AddRect(ImVec2{ Box.left,Box.top }, ImVec2{ Box.left + Box.right,Box.top + Box.bottom }, Color, 1.f);
				}
				if (Setting.ESP.Skeleton) {
					//骨骼
					DrawSkeleton(APlayerModelInstance, Color);
				}
				if (Setting.ESP.Ray) {
					//射线
					ImGui::GetOverlayDrawList()->AddLine(ImVec2{ g_Width / 2,0.f }, ImVec2{ Box.left + Box.right / 2 ,Box.top }, Color, 1.f);
				}
				
				if (Setting.ESP.Health || Setting.ESP.Kill || Setting.ESP.Name || Setting.ESP.WeaponName) {
					char textname[0xFF] = { 0 };
					if (Setting.ESP.Health) {
						char modified[0xFF] = { 0 };
						snprintf(modified, sizeof(modified),("H:%d "), APlayer->HP);
						strcat(textname, modified);
					}

					if (Setting.ESP.Kill) {
						char modified[0xFF] = { 0 };
						snprintf(modified, sizeof(modified),("K:%d "), APlayer->Kill);
						strcat(textname, modified);
					}
					if (Setting.ESP.Name) {
						char modified[0xFF] = { 0 };
						snprintf(modified, sizeof(modified),("%s "), APlayer->Name);
						strcat(textname, modified);
					}
					if (Setting.ESP.WeaponName) {
						char modified[0xFF] = { 0 };
						snprintf(modified, sizeof(modified),("%s "), APlayer->pCharaFx->pWeaponName->WeaponName);
						strcat(textname, modified);

					}
					auto centerTop = ImVec2(Box.left + Box.right / 2, Box.top);
					auto size = ImGui::GetFont()->CalcTextSizeA(ImGui::GetOverlayDrawList()->_Data->FontSize, FLT_MAX, 0, string_To_UTF8(textname).c_str());
					ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(centerTop.x - size.x / 2.0f, centerTop.y - size.y + 3.0f), ImVec2(centerTop.x + size.x / 2.0f, centerTop.y), ImGui::GetColorU32({ 0.0f, 0.0f, 0.0f, 0.4f }));
					ImGui::GetOverlayDrawList()->AddText(ImVec2(centerTop.x - size.x / 2.0f, centerTop.y - size.y), 0xFFFFFFFF, string_To_UTF8(textname).c_str());
				}
				if (Setting.ESP.HealthBar) {
					auto Health = APlayer->HP;
					D3DCOLOR DrawColor;
					if (Health >= 80.f && Health <= 100.f) { DrawColor = 0x9E00FF00; }
					if (Health < 80.f && Health >= 50.f) { DrawColor = 0x9EFFFF00; }
					if (Health < 50.f) { DrawColor = 0x9EFF0000; }
					int length = (int)(Health * 50 / 100);
					ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2{ (Box.left + Box.right / 2) - length / 2, Box.top + Box.bottom + 7 }, ImVec2{ ((Box.left + Box.right / 2) - length / 2) + length, Box.top + Box.bottom + 7 + 2 }, DrawColor);
				}
				if (Setting.ESP.IsC4) {
					//C4
				}
			}
		}
		
			
		for (int i = 0; i < m_World->Count; i++) {
			auto Item = m_World->Item;
			if (!Item) continue;
			auto Object = Item->Object[i];
			if (!Object) continue;
			auto Object_j = Object->Object_j;
			auto Pos = Object->Pos;
			if (!Object_j) continue;
			if (World2Screen(&Pos)) {
				auto ObjectName = Object_j->GetWeaponName(m_Weapon);
				auto centerTop = TVector2(Pos.x, Pos.y);
				if (Object_j->Type == 3 && Setting.Item.GroundWeapon) {
					auto size = ImGui::GetFont()->CalcTextSizeA(ImGui::GetOverlayDrawList()->_Data->FontSize, FLT_MAX, 0, string_To_UTF8(ObjectName).c_str());
					ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(centerTop.x - size.x / 2.0f, centerTop.y - size.y + 3.0f), ImVec2(centerTop.x + size.x / 2.0f, centerTop.y), ImGui::GetColorU32({ 0.0f, 0.0f, 0.0f, 0.4f }));
					ImGui::GetOverlayDrawList()->AddText(ImVec2(centerTop.x - size.x / 2.0f, centerTop.y - size.y), 0xFF00FFFF, string_To_UTF8(ObjectName).c_str());
				}
				if (Object_j->Type == 4 || Object_j->Type == 22) {
					auto size = ImGui::GetFont()->CalcTextSizeA(ImGui::GetOverlayDrawList()->_Data->FontSize, FLT_MAX, 0, string_To_UTF8(ObjectName).c_str());
					ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(centerTop.x - size.x / 2.0f, centerTop.y - size.y + 3.0f), ImVec2(centerTop.x + size.x / 2.0f, centerTop.y), ImGui::GetColorU32({ 1.0f, 0.0f, 0.0f, 0.4f }));
					ImGui::GetOverlayDrawList()->AddText(ImVec2(centerTop.x - size.x / 2.0f, centerTop.y - size.y), 0xFF0000FF, string_To_UTF8(ObjectName).c_str());
				}
				if (Object_j->Type == 7 && Setting.Item.GroundSupply) {
					auto size = ImGui::GetFont()->CalcTextSizeA(ImGui::GetOverlayDrawList()->_Data->FontSize, FLT_MAX, 0, xorstr(u8"补给箱"));
					ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(centerTop.x - size.x / 2.0f, centerTop.y - size.y + 3.0f), ImVec2(centerTop.x + size.x / 2.0f, centerTop.y), ImGui::GetColorU32({ 0.0f, 0.0f, 0.0f, 0.4f }));
					ImGui::GetOverlayDrawList()->AddText(ImVec2(centerTop.x - size.x / 2.0f, centerTop.y - size.y), 0xFFFFFFFF, xorstr(u8"补给箱"));
				}
			}
		}


		for (auto APlayer : AIObjects) {
			auto MPlayer = m_CLTClientShell->GetLocal();
			if (!APlayer) continue;
			if (!APlayer->IsAlive()) continue;
			auto APlayerModelInstance = (ModelInstance*)APlayer->pModelInstance;
			if (!APlayerModelInstance) continue;
			auto Color = 0xFF00FF00;
			auto IsTar = lastTarget == APlayer;
			MPlayer->pModelInstance->GetBonePos(xorstr("M-bone Head"), From);
			APlayer->pModelInstance->GetBonePos(xorstr("M-bone Head"), To);
			if (Core::IsVisible(From, To)) {
				Color = IsTar ? 0xFF00FFFF : 0xFF0000FF;
				if (Setting.AimBot.SilentTracking) {

					if (World2Screen(&To)) {
						if (Core::Vector::ScreenDis(To, g_Width / 2, g_Height / 2) < Setting.AimBot.AimFOV) {
							currentcenterDistance = Core::Vector::Get2DDis(To, g_Width / 2, g_Height / 2);
							if (currentDistance == NULL) {
								currentDistance = currentcenterDistance;
								closestPawn = APlayer;
							}
							if (currentcenterDistance < currentDistance) {
								currentDistance = currentcenterDistance;
								closestPawn = APlayer;
							}
						}
					}

				}
			}
			BoxRect Box;
			if (Bone2RECT(APlayerModelInstance, &Box)) {
				if (Setting.ESP.Box) {
					ImGui::GetOverlayDrawList()->AddRect(ImVec2{ Box.left, Box.top, }, ImVec2{ Box.left + Box.right, Box.top + Box.bottom }, Color);
				}
				if (Setting.ESP.Skeleton) {
					DrawSkeleton(APlayerModelInstance, Color);
				}
				if (Setting.ESP.Ray) {
					ImGui::GetOverlayDrawList()->AddLine(ImVec2{ g_Width / 2,0.f }, ImVec2{ Box.left + Box.right / 2 ,Box.top }, Color, 1.f);
				}
				if (Setting.ESP.Health || Setting.ESP.Kill || Setting.ESP.Name || Setting.ESP.WeaponName) {
					char textname[0xFF] = { 0 };
					if (Setting.ESP.Health) {
						char modified[0xFF] = { 0 };
						snprintf(modified, sizeof(modified), xorstr("H:%d "), APlayer->HP);
						strcat(textname, modified);
					}

					/*if (Setting.ESP.Kill) {
						char modified[0xFF] = { 0 };
						snprintf(modified, sizeof(modified), "K:%X ", APlayer);
						strcat(textname, modified);
					}
					iif (Setting.ESP.Name) {
						char modified[0xFF] = { 0 };
						snprintf(modified, sizeof(modified), "%s ", APlayer->Name);
						strcat(textname, modified);
					}
					if (Setting.ESP.WeaponName) {
						char modified[0xFF] = { 0 };
						snprintf(modified, sizeof(modified), "%s ", APlayer->pCharaFx->pWeaponName->WeaponName);
						strcat(textname, modified);

					}*/
					auto centerTop = ImVec2(Box.left + Box.right / 2, Box.top);
					auto size = ImGui::GetFont()->CalcTextSizeA(ImGui::GetOverlayDrawList()->_Data->FontSize, FLT_MAX, 0, string_To_UTF8(textname).c_str());
					ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(centerTop.x - size.x / 2.0f, centerTop.y - size.y + 3.0f), ImVec2(centerTop.x + size.x / 2.0f, centerTop.y), ImGui::GetColorU32({ 0.0f, 0.0f, 0.0f, 0.4f }));
					ImGui::GetOverlayDrawList()->AddText(ImVec2(centerTop.x - size.x / 2.0f, centerTop.y - size.y), 0xFFFFFFFF, string_To_UTF8(textname).c_str());
				}
			}
		}
		lastTarget = closestPawn;
		if ((Setting.AimBot.SilentTracking || Setting.AimBot.MouseAim) && closestPawn && GetAsyncKeyState2(1) && GetForegroundWindow() == Window) {
			Core::TargetPawn = closestPawn;
		}
		else {
			Core::TargetPawn = nullptr;
		}
		std::vector <AIClientObjectManager*>().swap(AIObjects);
	}
}
HRESULT __stdcall PresentHook(IDirect3DDevice9* pDevice, RECT* pSourceRect, RECT* pDestRect, HWND hDestWindowOverride, RGNDATA* pDirtyRegion) {
	if (g_pd3dDevice == NULL){
		g_pd3dDevice = pDevice;
		//=====================================================================================
		oWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(Window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProcHook)));
		ImGui::CreateContext();
		//=====================================================================================
		ImGuiIO& io = ImGui::GetIO();
		io.DeltaTime = 1.0f / 60.0f;
		D3DDEVICE_CREATION_PARAMETERS d3dcp{ 0 };
		pDevice->GetCreationParameters(&d3dcp);
		ImFont* font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\msyh.ttc", 15.f, NULL, io.Fonts->GetGlyphRangesChineseFull());
		if (font == NULL) {
			font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\msyh.ttf", 15.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
		}
		ImGui_ImplWin32_Init(Window);
		ImGui_ImplDX9_Init(pDevice);
		ImGui::StyleColorsLightBlue();//初始化窗口风格
		m_World = g_World.GetInstance();
		m_Weapon = g_Weapon.GetInstance();
		m_CLTClientShell = Base.GetInstance()->pCLTClientShell;
	}
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	DrawMenu();
	DrawFun();
	//Core::AimBot();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	return PresentOriginal(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}
HRESULT __stdcall ResetHook(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters) {
	ImGui_ImplDX9_InvalidateDeviceObjects();
	ImGui_ImplDX9_CreateDeviceObjects();
	return ResetOriginal(pDevice,pPresentationParameters);
}
DWORD __fastcall AIObjectHook(AIClientObjectManager* AIManager) {
	AIObjects.push_back(AIManager);
	return AIManagerOriginal(AIManager);
}
bool Render::Initialize() {
	Window = FindWindow(xorstr(L"CrossFire"), xorstr(L"穿越火线"));
	// 初始化D3D
	IDirect3D9* d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
	if (d3d9 == NULL) {
		MessageBox(NULL, xorstr(L"创建D3D失败"), xorstr(L""), MB_OK);
		return false;
	}
	D3DPRESENT_PARAMETERS pp = {};
	pp.Windowed = TRUE;
	pp.SwapEffect = D3DSWAPEFFECT_COPY;

	IDirect3DDevice9* device;
	if (FAILED(d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, Window, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &pp, &device))) {
		MessageBox(NULL, xorstr(L"创建设备失败"), xorstr(L""), MB_OK);
		d3d9->Release();
		return false;
	}
	auto Reset = (*(void***)device)[16]; // Reset是IDirect3DDevice9第17个函数
	auto Present = (*(void***)device)[17]; // Present是IDirect3DDevice9第18个函数
	if (MH_CreateHook(Reset, ResetHook, reinterpret_cast<PVOID*>(&ResetOriginal)) != MH_OK) {
		return false;
	}
	if (MH_CreateHook(Present, PresentHook, reinterpret_cast<PVOID*>(&PresentOriginal)) != MH_OK) {
		return false;
	}
	if (MH_CreateHook((PVOID)AIObject, AIObjectHook, reinterpret_cast<PVOID*>(&AIManagerOriginal)) != MH_OK) {
		return false;
	}
	MH_EnableHook(MH_ALL_HOOKS);
	// 释放
	d3d9->Release();
	device->Release();
	return true;
}
