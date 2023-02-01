#include <Windows.h>
#include <dwmapi.h>
#include <d3d11.h>
#include <iostream>
#include <string>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_win32.h>
#include <sstream>
#include <strstream>
#include "csgo/csgo.h"
#include "mem/MemMan.h"
#include <string>
#include <math.h>
#include <array>
#include <conio.h>
//CONSTANTS
int screenX = GetSystemMetrics(SM_CXSCREEN);
int screenY = GetSystemMetrics(SM_CYSCREEN);


float TriggerBotDelay = 0;
struct csgo {
	uintptr_t localPlayer = cs::dwLocalPlayer;
	uintptr_t entityList = cs::dwEntityList;
	uintptr_t team = cs::m_iTeamNum;
	uintptr_t health = cs::m_iHealth;
	uintptr_t origin = cs::m_vecOrigin;
	uintptr_t dw_view_matrix = 0x4DF0D44;
	uintptr_t b_freezeTime = cs::m_bFreezePeriod;
	uintptr_t glowObjectManager = cs::dwGlowObjectManager;
	uintptr_t glowIndex = cs::m_iGlowIndex;
	uintptr_t bDormant = cs::m_bDormant;
	uintptr_t fovStart = cs::m_iFOVStart;
	uintptr_t defaultFov = cs::m_iDefaultFOV;
	uintptr_t flashDuration = cs::m_flFlashDuration;
	uintptr_t clientState = cs::dwClientState;
	uintptr_t myWeapons = cs::m_hMyWeapons;
	uintptr_t fallBackWear = cs::m_flFallbackWear;
	uintptr_t fallBackPaintKit = cs::m_nFallbackPaintKit;
	uintptr_t fallBackSeed = cs::m_nFallbackSeed;
	uintptr_t fallBackStateTrack = cs::m_nFallbackStatTrak;
	uintptr_t itemDefinitionIndex = cs::m_iItemDefinitionIndex;
	uintptr_t itemIdHigh = cs::m_iItemIDHigh;
	uintptr_t entityQuality = cs::m_iEntityQuality;
	uintptr_t accountID = cs::m_iAccountID;
	uintptr_t originalOwnerXuidLow = cs::m_OriginalOwnerXuidLow;
	uintptr_t crossHairID = cs::m_iCrosshairId;
	uintptr_t dw_forceAttack = cs::dwForceAttack;
	uintptr_t model_ambient_min = cs::model_ambient_min;
	uintptr_t m_clrRender = cs::m_clrRender;
}csgo;

struct values {
	DWORD client;
	int hProcess;
	DWORD localPlayer;
	DWORD entityList;
}val;

struct view_matrix_t {
	float* operator[ ](int index) {
		return matrix[index];
	}

	float matrix[4][4];
};

struct Vector3
{
	float x, y, z;
};
struct Vector2 {
	float x, y;
};

Vector3 WorldToScreen(const Vector3 pos, view_matrix_t matrix) {
	float _x = matrix[0][0] * pos.x + matrix[0][1] * pos.y + matrix[0][2] * pos.z + matrix[0][3];
	float _y = matrix[1][0] * pos.x + matrix[1][1] * pos.y + matrix[1][2] * pos.z + matrix[1][3];

	float w = matrix[3][0] * pos.x + matrix[3][1] * pos.y + matrix[3][2] * pos.z + matrix[3][3];

	float inv_w = 1.f / w;
	_x *= inv_w;
	_y *= inv_w;

	float x = screenX * .5f;
	float y = screenY * .5f;

	x += 0.5f * _x * screenX + 0.5f;
	y -= 0.5f * _y * screenY + 0.5f;


	return { x,y,w };
}

std::string toString(auto str,char* &string) {
	std::stringstream ss{};
	ss << std::to_string((int)str) + "[M]";

	string = new char[ss.str().size() + 1];
	#pragma warning(suppress : 4996)
	strcpy(string, ss.str().c_str());
	return string;
}
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK window_procedure(HWND window,UINT message,WPARAM wparam,LPARAM lparam)
{
	if(ImGui_ImplWin32_WndProcHandler(window,message,wparam,lparam))
	{
		return 0L;
	}
	if(message == WM_DESTROY) {
		PostQuitMessage(0);
		return 0L;
	}
	return DefWindowProc(window, message, wparam, lparam);
}
// OPTIONS
bool visual = false;
bool misc = false;
bool TriggerBotOption = false;
bool chams = false;

void DisableAll() {
	misc = false;
	visual = false;
	TriggerBotOption = false;
	chams = false;
}
MemMan mem;
void Shoot() {
	Sleep(TriggerBotDelay);
	mem.writeMem<int>(val.client + csgo.dw_forceAttack, 6);
	Sleep(20);
	mem.writeMem<int>(val.client + csgo.dw_forceAttack, 4);
}
struct color {
	int r, g, b;
}col;
INT APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE,PSTR,INT cmd_show)
{
	WNDCLASSEXW wc{};
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = window_procedure;
	wc.hInstance = hInstance;
	wc.lpszClassName = L"sexy wall hack fr fr";

	RegisterClassExW(&wc);

	const HWND window = CreateWindowExW(
		WS_EX_TOPMOST |WS_EX_TRANSPARENT |WS_EX_LAYERED,
		wc.lpszClassName,
		L"Sexy window frfr",
		WS_POPUP,
		0, 0,
		GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
		nullptr,
		nullptr, 
		wc.hInstance,
		nullptr);
	SetLayeredWindowAttributes(window, RGB(0, 0, 0), BYTE(255), LWA_ALPHA);
	{
		RECT client_area;
		GetClientRect(window, &client_area);
		RECT window_area{};
		GetWindowRect(window, &window_area);
		POINT diff{};
		ClientToScreen(window, &diff);
		const MARGINS margins{
		window_area.left + (diff.x - window_area.left),
		window_area.top + (diff.y - window_area.top),
		client_area.right,
		client_area.bottom
		};
		DwmExtendFrameIntoClientArea(window, &margins);
	}

	DXGI_SWAP_CHAIN_DESC sd{};
	sd.BufferDesc.RefreshRate.Numerator = 60U;
	sd.BufferDesc.RefreshRate.Denominator = 1U;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.SampleDesc.Count = 1U;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 2U;
	sd.OutputWindow = window;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	constexpr D3D_FEATURE_LEVEL levels[2]{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_0
	};

	ID3D11Device* device{ nullptr };
	ID3D11DeviceContext* device_context{ nullptr };
	IDXGISwapChain* Swap_chain{ nullptr };
	ID3D11RenderTargetView* render_target_view{ nullptr };
	D3D_FEATURE_LEVEL level{};
	D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0U,
		levels,
		2U,
		D3D11_SDK_VERSION,
		&sd,
		&Swap_chain,
		&device,
		&level,
		&device_context);

	ID3D11Texture2D* back_buffer{ nullptr };
	Swap_chain->GetBuffer(0U, IID_PPV_ARGS(&back_buffer));
	if (back_buffer) {
		device->CreateRenderTargetView(back_buffer, nullptr, &render_target_view);
	}
	else
	{
		return 1;
	}
	ShowWindow(window, cmd_show);
	UpdateWindow(window);

	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(device, device_context);

	bool running = true;
	//Getting handle to a process
	 val.hProcess = mem.getProcess("csgo.exe");
	 val.client = mem.getModule(val.hProcess, "client.dll");
	 const auto engine = mem.getModule(val.hProcess, "engine.dll");

	//Activates
	 bool wallHack = false;
	 bool showMenu = false;
	 bool fovHack = false;
	 float fovValue = 0;
	 bool lines = false;
	 bool health = false;
	 bool bDistance = false;
	 bool crosshair = false;
	 bool teamESP = false;
	 bool antiFlash = false;
	 bool TriggerBot = false;
	 bool HoldTriggerBot = false;
	 bool getChar = false;
	 bool enable_chams;
	 ImGuiStyle& style = ImGui::GetStyle();
	 style.Colors[ImGuiCol_WindowBg] = ImColor(28,28,36, 250);
	 Vector3 oPunch {0,0,0};
	while(running)
	{
		MSG msg;
		while(PeekMessage(&msg,nullptr,0U,0U,PM_REMOVE)){
			Vector3 pos;
			Vector3 head;
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if(msg.message == WM_QUIT)
			{
				running = false;
			}
		}
		if(running == false)
		{
			break;
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();

		ImGui::NewFrame();
		//Logic and some kewl Stuff
		val.localPlayer = mem.readMem<uintptr_t>(val.client + csgo.localPlayer);
		const auto glowObjectManager = mem.readMem<uintptr_t>(val.client + csgo.glowObjectManager);
		val.entityList = mem.readMem<DWORD>(val.client + csgo.entityList);
		view_matrix_t vm = mem.readMem<view_matrix_t>(val.client + csgo.dw_view_matrix);
		int localTeam = mem.readMem<int>(val.localPlayer + csgo.team);
		const auto& weapons = mem.readMem<std::array <unsigned long, 8 >>(val.localPlayer + csgo.myWeapons);
		Vector3 viewAngles = mem.readMem<Vector3>((engine + csgo.clientState) + cs::dwClientState_ViewAngles);
		int m_iShotsFired = mem.readMem<int>(val.localPlayer + cs::m_iShotsFired);
		if (GetAsyncKeyState(VK_INSERT) & 1 || GetAsyncKeyState(VK_RETURN) & 1) {
			showMenu = !showMenu;
			SetWindowLong(window, GWL_EXSTYLE, GetWindowLong(window, GWL_EXSTYLE) + ~WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_LAYERED);
		}
		if (showMenu && ImGui::Begin("Penis Pills")) {
			{
				//ImGui::Checkbox("show static crosshair", &crosshair);
				ImGui::SetCursorPosY(25);
				if (ImGui::Button("Visuals")) {
					DisableAll();
					visual = !visual;
				}
				ImGui::SetCursorPos(ImVec2(70, 25));
				if (ImGui::Button("misc")) {
					DisableAll();
					misc = !misc;
				}
				ImGui::SetCursorPos(ImVec2(111, 25));
				style.Colors[ImGuiCol_Button] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
				style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
				style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
				if(ImGui::Button("TriggerBot")) {
					DisableAll();
					TriggerBotOption = !TriggerBotOption;
				}
				ImGui::SetCursorPos(ImVec2(194, 25));
				if (ImGui::Button("chams")) {
					DisableAll();
					enable_chams = true;
				}
				if (visual)
				{
					ImGui::Checkbox("team ESP", &teamESP);
					ImGui::Checkbox("show distance", &bDistance);
					ImGui::Checkbox("box", &wallHack);
					ImGui::Checkbox("lines", &lines);
					ImGui::Checkbox("show health", &health);
				}
				if (misc)
				{
					ImGui::SetCursorPosY(70);
					ImGui::Checkbox("FOV", &fovHack);
					ImGui::SetCursorPosY(140);
					ImGui::DragFloat("fov value", &fovValue, 1, 100, 160, "%.3f", 1);
					ImGui::SetCursorPosY(40);
					if (ImGui::Button("exit")) {
						ImGui::DestroyContext();
						DestroyWindow(window);
						UnregisterClassW(wc.lpszClassName, wc.hInstance);;
					}
					ImGui::SetCursorPosY(210);
					ImGui::Checkbox("Anti Flash", &antiFlash);

				}
				if (TriggerBotOption) {
					ImGui::SetCursorPosY(50);
					ImGui::Checkbox("trigger bot", &TriggerBot);
					ImGui::DragFloat("TriggerBot delay", &TriggerBotDelay, 5, 0.0, 1000.0, "%.3f", 0);
					ImGui::Checkbox("Hold shift", &getChar);

				}
				if (enable_chams) {
					ImGui::Checkbox("enable chams", &chams);
				}
				//handle exit


				/*
					 bool lines;
					 bool health;
					 bool distance;
					 bool crosshair;
					 bool teamESP;
				*/


			}
		}
		int freezeTime = mem.readMem<int>(val.client + 0x103C0);
		char* strFreezeTime;
		toString(freezeTime, strFreezeTime);
		//ImGui::Text(strFreezeTime);
		for(int i = 0; i < 64; ++i) {
			DWORD entity = mem.readMem<DWORD>(val.client + csgo.entityList + i * 0x10);
			int entityHealth = mem.readMem<int>(entity + csgo.health);
			int team = mem.readMem<int>(entity + csgo.team);
			Vector3 pos = mem.readMem<Vector3>(entity + csgo.origin);
			Vector3 head;
			head.x = pos.x;
			head.y = pos.y;
			head.z = pos.z + 75.f;
			Vector3 myLoc = mem.readMem<Vector3>(val.localPlayer + csgo.origin);
			Vector3 screenPos = WorldToScreen(pos, vm);
			Vector3 screenHead = WorldToScreen(head, vm);
			float height = screenHead.y - screenPos.y;
			float width = height / 2.4f;
			bool bDormant = mem.readMem<bool>(entity + csgo.bDormant);
			int distance = sqrt(pow(myLoc.x - pos.x,2) + pow(myLoc.y- pos.y,2) + pow(myLoc.z - pos.z,2))- 32;
			int flashDuration = mem.readMem<int>(val.localPlayer + csgo.flashDuration);
			if (antiFlash &&val.localPlayer && flashDuration > 0) {
				mem.writeMem<int>(val.localPlayer + csgo.flashDuration, 0);
			}
			//mem.writeMem<int>(val.localPlayer + cs::m_iFOV,120);
			if (entity != NULL) {

				if (chams)
				{
					if (mem.readMem<int>(entity + csgo.team) == mem.readMem<int>(val.localPlayer + csgo.team)) {
						mem.writeMem<ImColor>(entity + csgo.m_clrRender, { 253,0,0 });
					}
					else if (mem.readMem<int>(entity + csgo.team) != mem.readMem<int>(val.localPlayer + csgo.team)) {
						mem.writeMem<ImColor>(entity + csgo.m_clrRender, {255,0,55});
					}

					//model brightness
					float brightness = 25.f;
					const auto _this = static_cast<uintptr_t>(engine + csgo.model_ambient_min - 0x2c);
					mem.writeMem<std::uintptr_t>(engine + csgo.model_ambient_min, *reinterpret_cast<std::uintptr_t*>(&brightness) ^ _this);
				}
				if (TriggerBot)
				{
					const auto crosshairId = mem.readMem<int>(val.localPlayer + csgo.crossHairID);
					const auto player = mem.readMem<uintptr_t>(val.client + csgo.entityList + (crosshairId - 1) * 0x10);
					int playerHealth = mem.readMem<int>(player + csgo.health);
					int playerTeam = mem.readMem<int>(player + csgo.team);

					if (crosshairId != 0 && playerHealth > 0 && playerHealth <= 100 && playerTeam != localTeam) {
						//Sleep(500);

						bool keyPressed = false;
						if (getChar)
						{
							
							if (GetAsyncKeyState(VK_SHIFT) == -32768) {
								Shoot();
							}
						}
						else {
							Shoot();
						}
					}
				}

				if (bDormant == 0 && wallHack == true && val.entityList && val.localPlayer && screenPos.z >= 0.01f && team != localTeam && entityHealth > 0 && entityHealth < 101 && entity != val.localPlayer) {
					if (wallHack == true && val.entityList && val.localPlayer && screenPos.z >= 0.01f && team != localTeam && entityHealth > 0 && entityHealth < 101 && entity != val.localPlayer) {
						ImGui::GetBackgroundDrawList()->AddRect(ImVec2(screenPos.x , screenPos.y ), ImVec2(screenPos.x , screenPos.y), ImColor(0, 0, 0, 255), 0, 0, 2);
						//ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(screenPos.x + (width / 2.5), screenPos.y + (height / 1.1)), ImVec2(screenPos.x - (width / 1.4f), screenPos.y), ImColor(255, 0, 0, 50), 0, 0);
					}
				}
				if (bDormant == 0 && lines && val.entityList && val.localPlayer && screenPos.z >= 0.01f && team != localTeam && entityHealth > 0 && entityHealth < 101 && entity != val.localPlayer)
				{
					ImGui::GetBackgroundDrawList()->AddLine(ImVec2(screenX / 2, screenY), ImVec2(screenPos.x, screenPos.y), ImColor(255, 0, 0, 255));
				}
				if (crosshair) ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(screenX / 2, screenY / 2), 3, ImColor(0, 255, 0));
				int Ehealth = entityHealth;
				char* HealthStr;
				toString(Ehealth, HealthStr);
				if (bDormant == 0 && health && val.entityList && val.localPlayer && screenPos.z >= 0.01f && team != localTeam && entityHealth > 0 && entityHealth < 101 && entity != val.localPlayer) ImGui::GetBackgroundDrawList()->AddText(ImVec2(screenPos.x, screenPos.y + height), ImColor(0, 255, 0), HealthStr);
				char* distanceStr;
				toString(distance, distanceStr);
				if (bDormant == 0 && bDistance && val.entityList && val.localPlayer && screenPos.z >= 0.01f && team != localTeam && entityHealth > 0 && entityHealth < 101 && entity != val.localPlayer)ImGui::GetBackgroundDrawList()->AddText(ImVec2(screenPos.x, screenPos.y), ImColor(255, 0, 0), distanceStr);

				if (teamESP && screenPos.z >= 0.01f && team == localTeam && entityHealth > 0 && entityHealth < 101 && entity != val.localPlayer) {
					ImGui::GetBackgroundDrawList()->AddRect(ImVec2(screenPos.x + (width / 2.5), screenPos.y + (height / 1.1)), ImVec2(screenPos.x - (width / 1.4f), screenPos.y), ImColor(0, 0, 255, 255), 0, 0, 2);
					ImGui::GetBackgroundDrawList()->AddLine(ImVec2(screenX / 2, screenY), ImVec2(screenPos.x, screenPos.y), ImColor(0, 255, 0));
				}
				if (fovHack) {

					mem.writeMem<int>(val.localPlayer + csgo.defaultFov, fovValue);
				}

			}
			
		} 

		//rendering goes here
		ImGui::Render();
		constexpr float  color[4]{ 0.f,0.f,0.f,0.f };
		device_context->OMSetRenderTargets(1U, &render_target_view, nullptr);
		device_context->ClearRenderTargetView(render_target_view, color);

		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		Swap_chain->Present(1U,0U);


	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplDX11_Shutdown();

	if(Swap_chain)
	{
		Swap_chain->Release();
	}
	if(device_context)
	{
		device_context->Release();
	}
	if(device)
	{
		device->Release();
	}
	if(render_target_view)
	{
		render_target_view->Release();
	}
	ImGui::DestroyContext();


	DestroyWindow(window);
	UnregisterClassW(wc.lpszClassName, wc.hInstance);
	return 0;
}