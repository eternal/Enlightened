//--------------------------------------------------------------------------------------
// File: Main.cpp
//
// Empty starting point for new Direct3D applications
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "dxstdafx.h"
#include "SGLibResource.h"
#include "MasterShader.h"
#include <sstream>
#include <map>
#include <vector>
#include <string>
using namespace SGLib;

MasterShader*	g_masterShader = NULL;

SGRenderer*		g_renderer = NULL;

Camera*			g_camera = NULL;
Projection*		g_projection = NULL;
State*			g_state = NULL;

Transform*		g_dwarfTransform = NULL;
Transform*		g_treeTransform = NULL;
Transform*		g_monsterTransform = NULL;

Geometry*		g_dwarfGeometry = NULL;
Geometry*		g_treeGeometry = NULL;
Geometry*		g_monsterGeometry = NULL;

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
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	if (g_camera)
	{
		g_camera->OnCreateDevice(pd3dDevice);
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
		g_camera->OnResetDevice(DXUTGetD3DDevice());

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Handle updates to the scene
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( IDirect3DDevice9* pd3dDevice, double a_time, float a_elapsedTime, void* pUserContext )
{
	g_masterShader->SetCameraPosition(g_camera->GetPos());
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
	g_renderer = new SGRenderer();
    
    std::vector<std::string>* meshNames = new std::vector<std::string>();
    meshNames->push_back("dwarf");

	// Shaders
    g_masterShader = new MasterShader(device, L"shader.fx.c", meshNames);

	// Camera
	D3DXVECTOR3 cameraPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3	cameraUp       = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	D3DXVECTOR3	cameraLook     = D3DXVECTOR3(0.0f, 0.0f, 10.0f);
	g_camera = new Camera(device, cameraPosition, cameraUp, cameraLook);
	g_camera->SetSimpleMovement(true);
    g_masterShader->SetCameraPosition(g_camera->GetPos());
    
	// Projection
	D3DXMATRIX oMatrixProj;
	D3DXMatrixPerspectiveFovLH(&oMatrixProj, D3DX_PI * 0.25f, 1.5f, 1.0f, 1000.0f);
	g_projection = new Projection(device, oMatrixProj);

	// Start building scene graph
	g_camera->SetChild(g_projection);
	g_projection->SetChild(g_masterShader);

	// init matrices
	D3DXMATRIX scalingMatrix, rotationMatrix, translationMatrix, worldMatrix, identityMatrix;
	D3DXMatrixIdentity(&identityMatrix);

// dwarf
	D3DXMatrixScaling(&scalingMatrix, 1.0f, 1.0f, 1.0f);
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, 0.0f, 0.0f, 0.0f);
	D3DXMatrixTranslation(&translationMatrix, 0.0f, 0.0f, 0.0f);
	D3DXMatrixMultiply(&worldMatrix, &scalingMatrix, &rotationMatrix);
	D3DXMatrixMultiply(&worldMatrix, &worldMatrix, &translationMatrix);

	g_dwarfTransform = new Transform(device, worldMatrix);
	g_dwarfGeometry = new Geometry(device, L"dwarf.X");
	g_dwarfGeometry->SetDescription((LPCTSTR)"dwarf");
	

	g_masterShader->SetChild(g_dwarfTransform);
	g_dwarfTransform->SetChild(g_dwarfGeometry);
}

void CleanUp()
{
	SAFE_DELETE(g_renderer);

	SAFE_DELETE(g_camera);
	SAFE_DELETE(g_projection);
	SAFE_DELETE(g_state);

	SAFE_DELETE(g_masterShader);
}

//--------------------------------------------------------------------------------------
// Initialize everything and go into a render loop
//--------------------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
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

    // Initialize DXUT and create the desired Win32 window and Direct3D device for the application
    DXUTInit( true, true, true ); // Parse the command line, handle the default hotkeys, and show msgboxes
	DXUTSetCursorSettings( true, true ); // Show the cursor and clip it when in full screen
    DXUTCreateWindow( TEXT("Enlightened") );
    DXUTCreateDevice( D3DADAPTER_DEFAULT, true, 640, 480, IsDeviceAcceptable, ModifyDeviceSettings );

	InitalizeGraph();

    // Start the render loop
    DXUTMainLoop();

	CleanUp();

    return DXUTGetExitCode();
}