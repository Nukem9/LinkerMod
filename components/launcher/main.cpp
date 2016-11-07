#include <imgui.h>
#include <imgui_impl_dx9.h>
#include <d3d9.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tchar.h>

#pragma comment(lib, "d3d9.lib")

// Data
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp;

extern LRESULT ImGui_ImplDX9_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplDX9_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();
			g_d3dpp.BackBufferWidth = LOWORD(lParam);
			g_d3dpp.BackBufferHeight = HIWORD(lParam);
			HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
			if (hr == D3DERR_INVALIDCALL)
				IM_ASSERT(0);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// Create application window
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, LoadCursor(NULL, IDC_ARROW), NULL, NULL, _T("ImGui Example"), NULL };
	RegisterClassEx(&wc);
	HWND hwnd = CreateWindow(_T("ImGui Example"), _T("ImGui DirectX9 Example"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

	// Initialize Direct3D
	LPDIRECT3D9 pD3D;
	if ((pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
	{
		UnregisterClass(_T("ImGui Example"), wc.hInstance);
		return 0;
	}
	ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
	g_d3dpp.Windowed = TRUE;
	g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	g_d3dpp.EnableAutoDepthStencil = TRUE;
	g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	// Create the D3DDevice
	if (pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
	{
		pD3D->Release();
		UnregisterClass(_T("ImGui Example"), wc.hInstance);
		return 0;
	}

	// Show the window
	ShowWindow(hwnd, SW_SHOWDEFAULT);
	UpdateWindow(hwnd);

	// Setup ImGui binding
	ImGui_ImplDX9_Init(hwnd, g_pd3dDevice);

	bool show_another_window = true;
	ImVec4 clear_col = ImColor(114, 144, 154);

	int moveOffsetX = 0;
	int moveOffsetY = 0;

	ImGui::GetStyle().WindowRounding = 0.0f;

	// Main loop
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}

		ImGui_ImplDX9_NewFrame();

		if (ImGui::IsMouseClicked(0))
		{
			POINT point;
			RECT rect;

			GetCursorPos(&point);
			GetWindowRect(hwnd, &rect);

			// Calculate the difference between the cursor pos and window pos
			moveOffsetX = point.x - rect.left;
			moveOffsetY = point.y - rect.top;
		}

		float windowWidth = 1920.0f;
		float windowHeight = 1080.0f;

		float titlebarHeight = 30.0f;

		ImGui::GetStyle().Colors[ImGuiCol_Button] = ImVec4(.92f, .92f, .92f, 1.0f);
		ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered] = ImVec4(.90f, .945f, .988f, 1.0f);


		ImGui::GetStyle().WindowMinSize = ImVec2(10, 10);
		ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg] = ImVec4(.96f, .96f, .96f, 1.0f);
		ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
		ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

		// 2. Show another simple window, this time using an explicit Begin/End pair
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(.16f, .55f, .84f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
		ImGui::Begin("Another Window", &show_another_window,
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoInputs |
			ImGuiWindowFlags_ShowBorders);
		{
			ImGui::SetWindowPos(ImVec2(0, 0), ImGuiSetCond_Once);
			ImGui::SetWindowSize(ImVec2(windowWidth, windowHeight), ImGuiSetCond_Once);

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));	// White
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));	// Black
			ImGui::Button("Test Program", ImVec2(windowWidth, titlebarHeight));
			ImGui::PopStyleColor(2);

			if ((moveOffsetY >= 0 && moveOffsetY <= (int)titlebarHeight) &&	// Cursor must be *on* the titlebar
				ImGui::IsMouseDragging())					// User must drag mouse (hold LMB)
			{
				POINT point;
				GetCursorPos(&point);

				SetWindowPos(hwnd, nullptr, point.x - moveOffsetX, point.y - moveOffsetY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			}
		}
		ImGui::End();
		ImGui::PopStyleColor(2);
		ImGui::PopStyleVar();

		ImGui::Begin("keking", &show_another_window, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_ShowBorders);
		{
			ImGui::SetWindowPos(ImVec2(1, titlebarHeight - 1), ImGuiSetCond_Once);
			ImGui::SetWindowSize(ImVec2(windowWidth - 2, windowHeight - titlebarHeight), ImGuiSetCond_Once);

			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					ImGui::MenuItem("View Game Directory", "F5");
					ImGui::MenuItem("Exit", "Alt+F4");
					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Docs"))
				{
					ImGui::MenuItem("Maya CoDTools");
					ImGui::MenuItem("Exporter Tutorial");
					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Tools"))
				{
					ImGui::MenuItem("New Mod", "F8");
					ImGui::MenuItem("Maya Plugin Setup");
					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Help"))
				{
					ImGui::MenuItem("Wiki");
					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}

			ImGui::PushClipRect(ImVec2(1, titlebarHeight - 1), ImVec2(windowWidth - 1, windowHeight - titlebarHeight), false);
			ImGui::SetCursorPos(ImVec2(0, 20));
			ImGui::Separator();
			ImGui::PopClipRect();

			ImGui::BeginGroup();
			{
				ImGui::Text("<Black Ops Image>");
				ImGui::NewLine();

				ImGui::Text("Game");
				ImGui::BeginChild("launch pane", ImVec2(150, 104), true, ImGuiWindowFlags_AlwaysAutoResize);
				{
					bool dothing;
					ImGui::Checkbox("Developer", &dothing);
					bool dothing2;
					ImGui::Checkbox("Logfile", &dothing2);

					ImGui::Button("Singleplayer", ImVec2(ImGui::GetContentRegionAvailWidth(), 0));
					ImGui::Button("Multiplayer", ImVec2(ImGui::GetContentRegionAvailWidth(), 0));
				}
				ImGui::EndChild();

				ImGui::NewLine();

				ImGui::Text("Tools");
				ImGui::BeginChild("tools pane", ImVec2(150, 127), true);
				{
					ImGui::Button("Effects Editor", ImVec2(ImGui::GetContentRegionAvailWidth(), 0));
					ImGui::Button("Asset Manager", ImVec2(ImGui::GetContentRegionAvailWidth(), 0));
					ImGui::Button("Asset Viewer", ImVec2(ImGui::GetContentRegionAvailWidth(), 0));
					ImGui::Button("Converter", ImVec2(ImGui::GetContentRegionAvailWidth(), 0));
					ImGui::Button("Radiant", ImVec2(ImGui::GetContentRegionAvailWidth(), 0));
				}
				ImGui::EndChild();

				ImGui::NewLine();

				ImGui::Text("Processes");
				ImGui::BeginChild("processes pane", ImVec2(150, 500), true);
				{
				}
				ImGui::EndChild();
			}
			ImGui::EndGroup(); ImGui::SameLine();

			ImGui::BeginGroup();
			{
				static const char* tabNames[] = { "Mod Builder", "Level", "Explore" };
				static int tabOrder[] = { 0, 1, 2 };
				static int tabSelected = 0;
				const bool tabChanged = false; // ImGui::TabLabels(tabNames, ARRAYSIZE(tabNames), tabSelected, tabOrder);

				if (tabSelected == 0)
				{
					ImGui::BeginGroup();
					ImGui::BeginChild("##unnamed", ImVec2(1000, 500), true);
					{
						ImGui::BeginGroup();
						ImGui::Text("Mod");
						ImGui::BeginChild("##unnamed3", ImVec2(500, 0), true);
						{
							ImGui::Text("Fastfile Zone Source");
							ImGui::BeginChild("##unnamed2", ImVec2(0, 58), true);
							{
								ImGui::Button("Edit Zone Source", ImVec2(ImGui::GetContentRegionAvailWidth(), 0));
								ImGui::Button("Missing Assets", ImVec2(ImGui::GetContentRegionAvailWidth() / 2, 0)); ImGui::SameLine();
								ImGui::Button("Zone Source", ImVec2(ImGui::GetContentRegionAvailWidth(), 0));
							}
							ImGui::EndChild();

							ImGui::Text("Build Mod");
							ImGui::BeginChild("##unnamed4", ImVec2(0, 138), true);
							{
								bool linkFF;
								bool buildIWD;
								bool verbose;

								ImGui::Checkbox("Link Fastfile", &linkFF); ImGui::SameLine();
								ImGui::Checkbox("Build IWD", &buildIWD);
								ImGui::Checkbox("Verbose (More detailed information)", &verbose);

								ImGui::NewLine();

								char buf[256];
								ImGui::Text("Custom Linker Options");
								ImGui::InputText("##option_input", buf, 256);

								ImGui::Button("Build Mod", ImVec2(ImGui::GetContentRegionAvailWidth(), 0));
							}
							ImGui::EndChild();

							ImGui::Text("Mod Folder");
							ImGui::BeginChild("##unnamed5", ImVec2(0, 35), true);
							{
								ImGui::Button("View Mod", ImVec2(ImGui::GetContentRegionAvailWidth(), 0));
							}
							ImGui::EndChild();
						}
						ImGui::EndChild();
						ImGui::EndGroup();

						ImGui::SameLine();

						ImGui::BeginGroup();
						ImGui::Text("IWD File List");
						ImGui::BeginChild("##unnamed6", ImVec2(0, 0), true);
						{
							bool v = false;
							ImGui::Checkbox("##test1", &v); ImGui::SameLine();
							if (ImGui::TreeNodeEx("images", ImGuiTreeNodeFlags_DefaultOpen))
							{
								ImGui::Checkbox("##test2", &v); ImGui::SameLine();
								ImGui::TreeNodeEx("$identitynormalmap.iwi", ImGuiTreeNodeFlags_Leaf); ImGui::TreePop();
								ImGui::Checkbox("##test2", &v); ImGui::SameLine();
								ImGui::TreeNodeEx("$identitynormalmap.iwi", ImGuiTreeNodeFlags_Leaf); ImGui::TreePop();
								ImGui::Checkbox("##test2", &v); ImGui::SameLine();
								ImGui::TreeNodeEx("$identitynormalmap.iwi", ImGuiTreeNodeFlags_Leaf); ImGui::TreePop();
								ImGui::Checkbox("##test2", &v); ImGui::SameLine();
								ImGui::TreeNodeEx("$identitynormalmap.iwi", ImGuiTreeNodeFlags_Leaf); ImGui::TreePop();
								ImGui::Checkbox("##test2", &v); ImGui::SameLine();
								ImGui::TreeNodeEx("$identitynormalmap.iwi", ImGuiTreeNodeFlags_Leaf); ImGui::TreePop();
								ImGui::Checkbox("##test2", &v); ImGui::SameLine();
								ImGui::TreeNodeEx("$identitynormalmap.iwi", ImGuiTreeNodeFlags_Leaf); ImGui::TreePop();
								ImGui::Checkbox("##test2", &v); ImGui::SameLine();
								ImGui::TreeNodeEx("$identitynormalmap.iwi", ImGuiTreeNodeFlags_Leaf); ImGui::TreePop();
								ImGui::Checkbox("##test2", &v); ImGui::SameLine();
								ImGui::TreeNodeEx("$identitynormalmap.iwi", ImGuiTreeNodeFlags_Leaf); ImGui::TreePop();
								ImGui::TreePop();
							}
						}
						ImGui::EndChild();
						ImGui::EndGroup();
					}
					ImGui::EndChild();
					ImGui::EndGroup();
				}
			}
			ImGui::EndGroup();
		}
		ImGui::End();

		// Rendering
		g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, false);
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
		D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_col.x*255.0f), (int)(clear_col.y*255.0f), (int)(clear_col.z*255.0f), (int)(clear_col.w*255.0f));
		g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
		if (g_pd3dDevice->BeginScene() >= 0)
		{
			ImGui::Render();
			g_pd3dDevice->EndScene();
		}
		g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
	}

	ImGui_ImplDX9_Shutdown();
	if (g_pd3dDevice) g_pd3dDevice->Release();
	if (pD3D) pD3D->Release();
	UnregisterClass(_T("ImGui Example"), wc.hInstance);

	return 0;
}