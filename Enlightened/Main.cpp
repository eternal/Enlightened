//====================================================================
// Main.cpp
// Main implementatioon of scene graph
// Version 0.9
// Authors: Microsoft Corporation, Douglas Cook & Nick Kinsey
// Date 15/09/09
//====================================================================
#include "dxstdafx.h"
#include "SGLibResource.h"
#include "MasterShader.h"
#include "Mouse.h"
#include "Camera.h"
#include "Renderer.h"
#include <sstream>
#include <map>
#include <vector>
#include <string>

using namespace SGLib;

#define PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679f

MasterShader*	g_masterShader = NULL;

Renderer*		g_renderer = NULL;

std::vector<LPDIRECT3DTEXTURE9>* g_textureShadowMaps;
std::vector<LPDIRECT3DSURFACE9>* g_pSurfaceShadowDS;
std::vector<LPDIRECT3DSURFACE9>* g_shadowMapSurface;

Feisty::Camera* g_camera = NULL;
Feisty::Mouse*  g_mouse = NULL;
Projection*		g_projection = NULL;
State*			g_state = NULL;

Transform*		g_dwarfTransform = NULL;
Transform*		g_treeTransform = NULL;
Transform*		g_monsterTransform = NULL;
Transform*      g_gasStationTransform = NULL;
Transform*		g_characterTransform = NULL;

Geometry*		g_dwarfGeometry = NULL;
Geometry*		g_treeGeometry = NULL;
Geometry*		g_monsterGeometry = NULL;
Geometry*       g_gasStationGeometry = NULL;

Articulated*	g_characterNode = NULL;
Articulated*	g_characterPelvis = NULL;
Articulated*	g_characterRUpperLeg = NULL;
Articulated*	g_characterLUpperLeg = NULL;
Articulated*	g_characterRLowerLeg = NULL;
Articulated*	g_characterLLowerLeg = NULL;
Articulated*	g_characterLowerBack = NULL;
Articulated*	g_characterUpperBack = NULL;
Articulated*	g_characterNeck = NULL;
Articulated*	g_characterHead = NULL;
Articulated*	g_characterShoulders = NULL;
Articulated*	g_characterRUpperArm = NULL;
Articulated*	g_characterLUpperArm = NULL;
Articulated*	g_characterRLowerArm = NULL;
Articulated*	g_characterLLowerArm = NULL;

bool            g_animating          = false;
//--------------------------------------------------------------------------------------
// Rejects any devices that aren't acceptable by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, 
                                  D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
    // Typically want to skip backbuffer formats that don't support alpha blending
    IDirect3D9* pD3D = DXUTGetD3DObject(); 
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                    AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, 
                    D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
        return false;

    return true;
}

//--------------------------------------------------------------------------------------
// Before a device is created, modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, void* pUserContext )
{
	return true;
}


//--------------------------------------------------------------------------------------
// Create any D3DPOOL_MANAGED resources here 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* a_device, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	if (g_camera)
	{
		g_camera->OnCreateDevice(a_device);
	}

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Create any D3DPOOL_DEFAULT resources here 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, 
                                const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	if (g_camera)
	{
		g_camera->OnResetDevice(DXUTGetD3DDevice());
	}
    return S_OK;
}


//--------------------------------------------------------------------------------------
// Handle updates to the scene
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( IDirect3DDevice9* pd3dDevice, double a_time, float a_elapsedTime, void* pUserContext )
{
	g_camera->Update(a_elapsedTime);
	g_masterShader->SetCameraPosition(g_camera->GetPosition());
	g_renderer->Update(g_camera, a_elapsedTime);
}


//--------------------------------------------------------------------------------------
// Render the scene 
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double dTime, float fElapsedTime, void* pUserContext )
{
	g_renderer->Render(g_camera);
}


//--------------------------------------------------------------------------------------
// Handle mouse events
//--------------------------------------------------------------------------------------
void CALLBACK OnMouse(bool a_leftButton, bool a_rightButton, bool a_middleButtonDown, bool a_sideButton1Down, bool a_sideButton2Down, int a_mouseWheelDelta, int a_xPosition, int a_yPosition, void* a_userContext )
{
	g_mouse->Track(a_leftButton, a_rightButton, a_mouseWheelDelta, a_xPosition, a_yPosition);
}


//--------------------------------------------------------------------------------------
// Handle messages to the application 
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, 
                          bool* pbNoFurtherProcessing, void* pUserContext )
{
	switch(uMsg)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
			break;

		case WM_KEYUP:
		{
			switch(wParam)
			{
				case VK_ESCAPE:
					DestroyWindow(hWnd);
					return 0;
					break;

				case VK_F1:
					break;

				case VK_F2:
				    DXUTToggleFullScreen();
					break;
			}
			break;
		}
	}
	return 0;
}


//--------------------------------------------------------------------------------------
// Release resources created in the OnResetDevice callback here 
//--------------------------------------------------------------------------------------
void CALLBACK OnLostDevice( void* pUserContext )
{
	if (g_camera)
		g_camera->OnLostDevice();
}


//--------------------------------------------------------------------------------------
// Release resources created in the OnCreateDevice callback here
//--------------------------------------------------------------------------------------
void CALLBACK OnDestroyDevice( void* pUserContext )
{
	if (g_camera)
		g_camera->OnDestroyDevice();
}

void InitalizeGraph()
{
	LPDIRECT3DDEVICE9 device = DXUTGetD3DDevice();
	
	g_textureShadowMaps = new std::vector<LPDIRECT3DTEXTURE9>();
	g_shadowMapSurface = new std::vector<LPDIRECT3DSURFACE9>();
	g_pSurfaceShadowDS = new std::vector<LPDIRECT3DSURFACE9>();
 	for (int i = 0; i < 6; i++)
	{
        // create texture used to render the shadow map to
        LPDIRECT3DTEXTURE9 tex = NULL;
        D3DXCreateTexture(device, 1024, 1024, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &tex);
        g_textureShadowMaps->push_back(tex);
        
        LPDIRECT3DSURFACE9 shadowMapSurface;
        g_textureShadowMaps->at(i)->GetSurfaceLevel(0, &shadowMapSurface);
        g_shadowMapSurface->push_back(shadowMapSurface);

        // create depth surface used when rendering the shadow map
        LPDIRECT3DSURFACE9 surf = NULL;
        device->CreateDepthStencilSurface(1024, 1024, D3DFMT_D24X8, D3DMULTISAMPLE_NONE, 0, TRUE, &surf, NULL); 
        g_pSurfaceShadowDS->push_back(surf);
	}
	
	g_renderer = new Renderer(g_textureShadowMaps, g_pSurfaceShadowDS, g_shadowMapSurface);
    
    std::vector<std::string>* meshNames = new std::vector<std::string>();
    meshNames->push_back("dwarf");
    meshNames->push_back("tree");
    meshNames->push_back("PetrolStation");

	// Shaders
    g_masterShader = new MasterShader(device, L"shader.fx.c", meshNames, g_textureShadowMaps, g_pSurfaceShadowDS, g_shadowMapSurface);

	// Camera
	
	g_camera = new Feisty::Camera(device);
	g_camera->AdoptShader(g_masterShader);

	// Mouse
	g_mouse = new Feisty::Mouse();
	g_mouse->SetCamera(g_camera);

	// init matrices
	D3DXMATRIX scalingMatrix, rotationMatrix, translationMatrix, worldMatrix, identityMatrix;
	D3DXMatrixIdentity(&identityMatrix);

// dwarf
	D3DXMatrixScaling(&scalingMatrix, 1.0f, 1.0f, 1.0f);
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, -PI/4, 0.0f, 0.0f);
	D3DXMatrixTranslation(&translationMatrix, -300.0f, 12.0f, 100.0f);
	D3DXMatrixMultiply(&worldMatrix, &scalingMatrix, &rotationMatrix);
	D3DXMatrixMultiply(&worldMatrix, &worldMatrix, &translationMatrix);

	g_dwarfTransform = new Transform(device, worldMatrix);
	g_dwarfGeometry = new Geometry(device, L"dwarf.X");
	g_dwarfGeometry->SetDescription((LPCTSTR)"dwarf");

	g_masterShader->SetChild(g_dwarfTransform);
	g_dwarfTransform->SetChild(g_dwarfGeometry);

//tree
	D3DXMatrixScaling(&scalingMatrix, 50.0f, 50.0f, 50.0f);
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, 0.0f, 0.0f, 0.0f);
	D3DXMatrixTranslation(&translationMatrix, 60.0f, 0.0f, -500.0f);
	D3DXMatrixMultiply(&worldMatrix, &scalingMatrix, &rotationMatrix);
	D3DXMatrixMultiply(&worldMatrix, &worldMatrix, &translationMatrix);

	g_treeTransform = new Transform(device, worldMatrix);
	g_treeGeometry = new Geometry(device, L"tree.X");
	g_treeGeometry->SetDescription((LPCTSTR)"tree");	

    g_dwarfTransform->SetSibling(g_treeTransform);
    g_treeTransform->SetChild(g_treeGeometry);
    
//gas station
	D3DXMatrixScaling(&scalingMatrix, 50.0f, 50.0f, 50.0f);
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, 0.0f, 0.0f, 0.0f);
	D3DXMatrixTranslation(&translationMatrix, 60.0f, 0.0f, 100.0f);
	D3DXMatrixMultiply(&worldMatrix, &scalingMatrix, &rotationMatrix);
	D3DXMatrixMultiply(&worldMatrix, &worldMatrix, &translationMatrix);
	
	g_gasStationTransform = new Transform(device, worldMatrix);
	g_gasStationGeometry = new Geometry(device, L"GasStation.X");
	g_gasStationGeometry->SetDescription((LPCTSTR)"PetrolStation");

    g_treeTransform->SetSibling(g_gasStationTransform);
    g_gasStationTransform->SetChild(g_gasStationGeometry);

//monster
	D3DXMatrixScaling(&scalingMatrix, 5.0f, 5.0f, 5.0f);
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, PI/2.5, 0.0f, 0.0f);
	D3DXMatrixTranslation(&translationMatrix, 60.0f, 0.0f, 0.0f);
	D3DXMatrixMultiply(&worldMatrix, &scalingMatrix, &rotationMatrix);
	D3DXMatrixMultiply(&worldMatrix, &worldMatrix, &translationMatrix);
	
	g_monsterTransform = new Transform(device, worldMatrix);
	g_monsterGeometry = new Geometry(device, L"monster.X");
	g_monsterGeometry->SetDescription(L"monster");
	
	g_gasStationTransform->SetSibling(g_monsterTransform);
	g_monsterTransform->SetChild(g_monsterGeometry);
	
	// person setup
	D3DXMatrixScaling(&scalingMatrix, 3.0f, 3.0f, 3.0f);
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, 0.0f, 0.0f, PI/2);
	D3DXMatrixTranslation(&translationMatrix, 15.0f, 25.0f, -80.0f);
	D3DXMatrixMultiply(&worldMatrix, &scalingMatrix, &rotationMatrix);
	D3DXMatrixMultiply(&worldMatrix, &worldMatrix, &translationMatrix);
	
	g_characterTransform = new Transform(device, worldMatrix);
	g_camera->SetTargetNode(g_characterTransform);
	g_monsterTransform->SetSibling(g_characterTransform);

	// setup each articulated link
	//										    Leng, Disp,     OrigTheta,Min,    Max,					OrigAlpha,Min,  Max,			Filename
	g_characterNode =		new Articulated(device, 0.0f,  0.0f,    0.0f,     0.0f,   0.0f,					0.0f,     0.0f, 0.0f,			NULL);
	g_characterPelvis =		new Articulated(device, -0.8f, 0.0f,	D3DX_PI,  D3DX_PI, D3DX_PI,				0.0f,     0.0f, 0.0f,			NULL);
	
	g_characterRUpperLeg =	new Articulated(device, 3.3f, -0.8f,	D3DX_PI/15, -D3DX_PI/4, D3DX_PI/3,		0.0f, 0.0f, 0.0f,				L"barney_rightupperleg.x"); 
	g_characterLUpperLeg =	new Articulated(device, 3.3f, 0.8f,		D3DX_PI/15, -D3DX_PI/4, D3DX_PI/3,		0.0f, 0.0f, 0.0f,				L"barney_leftupperleg.x");
	g_characterRLowerLeg =	new Articulated(device, 5.0f, -0.3f,	-D3DX_PI/13, -D3DX_PI/2, 0.0f,			0.0f, 0.0f, 0.0f,				L"barney_rightlowerleg.x");
	g_characterLLowerLeg =	new Articulated(device, 5.0f, 0.3f,		-D3DX_PI/13, -D3DX_PI/2, 0.0f,			0.0f, 0.0f, 0.0f,				L"barney_leftlowerleg.x");
	
	g_characterLowerBack =	new Articulated(device, 3.0f, 0.0f,		D3DX_PI/25, -D3DX_PI/10, D3DX_PI/10,	0.0f, -D3DX_PI/8, D3DX_PI/8,	L"barney_lowerbody.x");
	g_characterUpperBack =	new Articulated(device, 3.0f, 0.0f,		-D3DX_PI/24, -D3DX_PI/2, 0.0f,			0.0f, -D3DX_PI/8, D3DX_PI/8,	L"barney_upperbody.x");
	
	g_characterNeck =		new Articulated(device, 0.0f, 0.0f,		0.0f, 0.0f, 0.0f,						0.0f, -D3DX_PI/3, D3DX_PI/3,	NULL);
	g_characterHead =		new Articulated(device, 1.5f, 0.0f,		0.0f, -D3DX_PI/3, D3DX_PI/3,			0.0f, 0.0f, 0.0f,				L"barney_head.x");
	g_characterShoulders =	new Articulated(device, 1.0f, 0.0f,		D3DX_PI, D3DX_PI, D3DX_PI,				0.0f, 0.0f, 0.0f,				NULL);
	
	g_characterRUpperArm =	new Articulated(device, 3.0f, -2.5f,	D3DX_PI/20, -D3DX_PI/2, D3DX_PI/1.1f,	0.0f, -D3DX_PI/12, D3DX_PI/12,	L"barney_rightupperarm.x");
	g_characterLUpperArm =	new Articulated(device, 3.0f, 2.5f,		D3DX_PI/20, -D3DX_PI/2, D3DX_PI/1.1f,	0.0f, -D3DX_PI/12, D3DX_PI/12,	L"barney_leftupperarm.x");
	g_characterRLowerArm =	new Articulated(device, 3.0f, 0.0f,		D3DX_PI/10, 0.0f, D3DX_PI/1.3f,			0.0f, 0.0f, 0.0f,				L"barney_rightlowerarm.x");
	g_characterLLowerArm =	new Articulated(device, 3.0f, 0.0f,		D3DX_PI/10, 0.0f, D3DX_PI/1.3f,			0.0f, 0.0f, 0.0f,				L"barney_leftlowerarm.x");

	g_camera->SetAnimationNode(g_characterNode);
	
    g_characterTransform->SetChild(g_characterNode);
	g_characterNode->SetChild(g_characterLowerBack);
	g_characterLowerBack->SetSibling(g_characterPelvis);
	g_characterPelvis->SetChild(g_characterLUpperLeg);
	g_characterLUpperLeg->SetSibling(g_characterRUpperLeg);
	g_characterRUpperLeg->SetChild(g_characterRLowerLeg);
	g_characterLUpperLeg->SetChild(g_characterLLowerLeg);
	g_characterLowerBack->SetChild(g_characterUpperBack);
	g_characterUpperBack->SetChild(g_characterNeck);
	g_characterNeck->SetChild(g_characterHead);
	g_characterNeck->SetSibling(g_characterShoulders);
	g_characterShoulders->SetChild(g_characterRUpperArm);
	g_characterRUpperArm->SetSibling(g_characterLUpperArm);
	g_characterRUpperArm->SetChild(g_characterRLowerArm);
	g_characterLUpperArm->SetChild(g_characterLLowerArm);
    
    std::vector<TimeStep> vecRotRUA, vecTwistRUA, vecRotLUA, vecTwistLUA;
    vecRotLUA.push_back(TimeStep(0.0f, D3DX_PI/20));
	vecRotLUA.push_back(TimeStep(0.5f, D3DX_PI/6));
	vecRotLUA.push_back(TimeStep(1.0f, D3DX_PI/20));
	vecRotLUA.push_back(TimeStep(1.5f, -D3DX_PI/25));
	vecRotLUA.push_back(TimeStep(2.0f, D3DX_PI/20));
	
	vecRotRUA.push_back(TimeStep(0.0f, D3DX_PI/20));
	vecRotRUA.push_back(TimeStep(0.5f, -D3DX_PI/25));
	vecRotRUA.push_back(TimeStep(1.0f, D3DX_PI/20));
	vecRotRUA.push_back(TimeStep(1.5f, D3DX_PI/6));
	vecRotRUA.push_back(TimeStep(2.0f, D3DX_PI/20));

	vecTwistRUA.push_back(TimeStep(0.0f, -D3DX_PI/15));
	vecTwistLUA.push_back(TimeStep(0.0f, D3DX_PI/15));

	std::vector<TimeStep> vecRotRUL, vecRotLUL, vecRotRLL, vecRotLLL, vecRotUB, vecTwist;
	vecTwist.push_back(TimeStep(0.0f, 0.0f));

	vecRotRUL.push_back(TimeStep(0.0f, D3DX_PI/15));
	vecRotRUL.push_back(TimeStep(0.5f, D3DX_PI/6));
	vecRotRUL.push_back(TimeStep(1.0f, D3DX_PI/15));
	vecRotRUL.push_back(TimeStep(1.5f, -D3DX_PI/10));
	vecRotRUL.push_back(TimeStep(2.0f, D3DX_PI/15));

	vecRotLUL.push_back(TimeStep(0.0f, D3DX_PI/15));
	vecRotLUL.push_back(TimeStep(0.5f, -D3DX_PI/10));
	vecRotLUL.push_back(TimeStep(1.0f, D3DX_PI/15));
	vecRotLUL.push_back(TimeStep(1.5f, D3DX_PI/6));
	vecRotLUL.push_back(TimeStep(2.0f, D3DX_PI/15));

	vecRotRLL.push_back(TimeStep(0.0f, -D3DX_PI/13));
	vecRotRLL.push_back(TimeStep(0.5f, -D3DX_PI/50));
	vecRotRLL.push_back(TimeStep(1.0f, -D3DX_PI/13));
	vecRotRLL.push_back(TimeStep(1.5f, -D3DX_PI/6));
	vecRotRLL.push_back(TimeStep(2.0f, -D3DX_PI/13));

	vecRotLLL.push_back(TimeStep(0.0f, -D3DX_PI/13));
	vecRotLLL.push_back(TimeStep(0.5f, -D3DX_PI/6));
	vecRotLLL.push_back(TimeStep(1.0f, -D3DX_PI/13));
	vecRotLLL.push_back(TimeStep(1.5f, -D3DX_PI/50));
	vecRotLLL.push_back(TimeStep(2.0f, -D3DX_PI/13));

	vecRotUB.push_back(TimeStep(0.0f, -D3DX_PI/24));
	vecRotUB.push_back(TimeStep(0.5f, -D3DX_PI/20));
	vecRotUB.push_back(TimeStep(1.0f, -D3DX_PI/24));
	vecRotUB.push_back(TimeStep(1.5f, -D3DX_PI/20));
	vecRotUB.push_back(TimeStep(2.0f, -D3DX_PI/24));

	AnimContainer RUAWalkAnim(vecRotRUA, vecTwistRUA);
	AnimContainer LUAWalkAnim(vecRotLUA, vecTwistLUA);
	AnimContainer RULWalkAnim(vecRotRUL, vecTwist);
	AnimContainer LULWalkAnim(vecRotLUL, vecTwist);
	AnimContainer RLLWalkAnim(vecRotRLL, vecTwist);
	AnimContainer LLLWalkAnim(vecRotLLL, vecTwist);
	AnimContainer UBWalkAnim(vecRotUB, vecTwist);

	g_characterRUpperArm->AddAnimation(L"Walk", RUAWalkAnim);
	g_characterLUpperArm->AddAnimation(L"Walk", LUAWalkAnim);
	g_characterRUpperLeg->AddAnimation(L"Walk", RULWalkAnim);
	g_characterLUpperLeg->AddAnimation(L"Walk", LULWalkAnim);
	g_characterRLowerLeg->AddAnimation(L"Walk", RLLWalkAnim);
	g_characterLLowerLeg->AddAnimation(L"Walk", LLLWalkAnim);
	g_characterUpperBack->AddAnimation(L"Walk", UBWalkAnim);

}

void CleanUp()
{
	SAFE_DELETE(g_renderer);

	//SAFE_DELETE(g_camera->GetNode());
	SAFE_DELETE(g_projection);
	SAFE_DELETE(g_state);

	SAFE_DELETE(g_masterShader);
	
	SAFE_DELETE(g_dwarfTransform);
	SAFE_DELETE(g_monsterTransform);
	SAFE_DELETE(g_treeTransform);
	
	SAFE_DELETE(g_dwarfGeometry);
	SAFE_DELETE(g_monsterGeometry);
	SAFE_DELETE(g_treeGeometry);
}

//--------------------------------------------------------------------------------------
// Initialize everything and go into a render loop
//--------------------------------------------------------------------------------------
//INT WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
//{
int main()
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    // Set the callback functions
    DXUTSetCallbackDeviceCreated( OnCreateDevice );
    DXUTSetCallbackDeviceReset( OnResetDevice );
    DXUTSetCallbackDeviceLost( OnLostDevice );
    DXUTSetCallbackDeviceDestroyed( OnDestroyDevice );
    DXUTSetCallbackMsgProc( MsgProc );
    DXUTSetCallbackFrameRender( OnFrameRender );
    DXUTSetCallbackFrameMove( OnFrameMove );
	DXUTSetCallbackMouse( OnMouse, true );

    // Initialize DXUT and create the desired Win32 window and Direct3D device for the application
    DXUTInit( true, true, true ); // Parse the command line, handle the default hotkeys, and show msgboxes
	DXUTSetCursorSettings( true, true ); // Show the cursor and clip it when in full screen
    DXUTCreateWindow( TEXT("Enlightened") );
    //DXUTCreateDevice( D3DADAPTER_DEFAULT, true, 640, 480, IsDeviceAcceptable, ModifyDeviceSettings );
    
	DXUTDeviceSettings Settings;
    D3DPRESENT_PARAMETERS d3dpp; 
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.Windowed = true;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.BackBufferWidth = 800;
	d3dpp.BackBufferHeight = 600;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_2_SAMPLES;
	
    Settings.AdapterFormat = D3DFMT_A8R8G8B8;
	Settings.AdapterOrdinal = D3DADAPTER_DEFAULT;
	Settings.BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE;
	Settings.DeviceType = D3DDEVTYPE_HAL;
	Settings.pp = d3dpp;
	DXUTFindValidDeviceSettings(&Settings,&Settings);
	DXUTCreateDeviceFromSettings( &Settings );
    //DXUTCreateDeviceFromSettings( D3DADAPTER_DEFAULT, true, 640, 480, IsDeviceAcceptable, ModifyDeviceSettings );

	InitalizeGraph();

    // Start the render loop
    DXUTMainLoop();

	CleanUp();

    return DXUTGetExitCode();
}