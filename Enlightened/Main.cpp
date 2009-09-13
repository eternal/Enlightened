//--------------------------------------------------------------------------------------
// File: Main.cpp
//
// Empty starting point for new Direct3D applications
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "dxstdafx.h"
#include "SGLibResource.h"
#include "ShaderSimple.h"
#include "ShaderComplex.h"
#include <sstream>

using namespace SGLib;

ShaderSimple*	g_pnodeShaderSimple = NULL;
ShaderComplex*	g_pnodeShaderComplex = NULL;
ShaderComplex*  g_pnodeShaderComplex2 = NULL;
Shader*			ref1 = NULL;

SGRenderer*		g_pRenderer = NULL;

Camera*			g_pnodeCamera = NULL;
Projection*		g_pnodeProj = NULL;
State*			g_pnodeState = NULL;

Transform*		g_pnodeTransPlane = NULL;
Transform*		g_pnodeTransPerson = NULL;
Transform*		g_pnodeTransHouse = NULL;
Transform*		g_pnodeTransHouse2 = NULL;

Geometry*		g_pnodePlane = NULL;
Geometry*		g_pnodeHouse = NULL;
Geometry*		g_pnodeHouse2 = NULL;

Articulated*	g_pnodeArt = NULL;

//Transform*		g_pnodeTransPerson = NULL;
Articulated*	g_pnodePerson = NULL;
Articulated*	g_pnodePelvis = NULL;
Articulated*	g_pnodeRUpperLeg = NULL;
Articulated*	g_pnodeLUpperLeg = NULL;
Articulated*	g_pnodeRLowerLeg = NULL;
Articulated*	g_pnodeLLowerLeg = NULL;
Articulated*	g_pnodeLowerBack = NULL;
Articulated*	g_pnodeUpperBack = NULL;
Articulated*	g_pnodeNeck = NULL;
Articulated*	g_pnodeHead = NULL;
Articulated*	g_pnodeShoulders = NULL;
Articulated*	g_pnodeRUpperArm = NULL;
Articulated*	g_pnodeLUpperArm = NULL;
Articulated*	g_pnodeRLowerArm = NULL;
Articulated*	g_pnodeLLowerArm = NULL;

D3DXVECTOR3		g_vecCamPos(32.5f, 68.0f, 5.0f);
D3DXVECTOR3		g_vecCamUp(0.0f, 0.0f, 1.0f);
D3DXVECTOR3		g_vecCamLook(0.0f, 20.0f, 5.0f);

LPD3DXFONT		g_pD3DXFont = NULL;

FLOAT			g_fAnimLength = 0.0f;
BOOL			g_bWalking = FALSE;

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
	D3DXCreateFont(	pd3dDevice, 14, 0, FW_BOLD, 0, FALSE, 
		            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 
		            DEFAULT_PITCH | FF_DONTCARE, TEXT("Verdana"), 
		            &g_pD3DXFont );

	if (g_pnodeCamera)
		g_pnodeCamera->OnCreateDevice(pd3dDevice);

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Create any D3DPOOL_DEFAULT resources here 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, 
                                const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	if (g_pD3DXFont)
		g_pD3DXFont->OnResetDevice();

	if (g_pnodeCamera)
		g_pnodeCamera->OnResetDevice(DXUTGetD3DDevice());

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Handle updates to the scene
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( IDirect3DDevice9* pd3dDevice, double dTime, float fElapsedTime, void* pUserContext )
{
	g_pnodeShaderComplex->SetCamPos(g_pnodeCamera->GetPos());
	g_pnodeShaderComplex2->SetCamPos(g_pnodeCamera->GetPos());
	g_pRenderer->Update(g_pnodeCamera, fElapsedTime);
}


//--------------------------------------------------------------------------------------
// Render the scene 
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double dTime, float fElapsedTime, void* pUserContext )
{
	g_pRenderer->Render(g_pnodeCamera);
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
	if (g_pD3DXFont)
		g_pD3DXFont->OnLostDevice();

	if (g_pnodeCamera)
		g_pnodeCamera->OnLostDevice();
}


//--------------------------------------------------------------------------------------
// Release resources created in the OnCreateDevice callback here
//--------------------------------------------------------------------------------------
void CALLBACK OnDestroyDevice( void* pUserContext )
{
	SAFE_RELEASE(g_pD3DXFont);

	if (g_pnodeCamera)
		g_pnodeCamera->OnDestroyDevice();
}

void InitalizeGraph()
{
	LPDIRECT3DDEVICE9 pD3DDevice = DXUTGetD3DDevice();
	g_pRenderer = new SGRenderer();

	//Here are the shader nodes to be created later to be inserted.

	g_pnodeShaderSimple = new ShaderSimple(pD3DDevice, L"SimpleEffect.fx");
	g_pnodeShaderComplex = new ShaderComplex(pD3DDevice, L"PhongSpotEffect.fx");
	g_pnodeShaderComplex2 = new ShaderComplex(pD3DDevice, L"PhongSpotEffect.fx");

	g_pnodeCamera = new Camera(pD3DDevice, g_vecCamPos, g_vecCamUp, g_vecCamLook);
	g_pnodeCamera->SetSimpleMovement(true);
    g_pnodeShaderComplex->SetCamPos(g_pnodeCamera->GetPos());
    
	D3DXMATRIX oMatrixProj;
	D3DXMatrixPerspectiveFovLH(&oMatrixProj, D3DX_PI * 0.25f, 1.5f, 1.0f, 1000.0f);
	g_pnodeProj = new Projection(pD3DDevice, oMatrixProj);

	D3DXMATRIX matScale, matRotX, matRotY, matRotZ, matTrans, matWorld;
	D3DXMatrixScaling(&matScale, 0.001f, 0.001f, 0.001f);
	D3DXMatrixRotationY(&matRotY, 0.5f);
	D3DXMatrixRotationZ(&matRotZ, -D3DX_PI/4);
	D3DXMatrixMultiply(&matWorld, &matScale, &matRotY);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matRotZ);
	g_pnodeTransPlane = new Transform(pD3DDevice, matWorld);

	g_pnodePlane = new Geometry(pD3DDevice, L"plane.x");

	D3DXMatrixScaling(&matScale, 5.0f, 5.0f, 5.0f);
	D3DXMatrixRotationZ(&matRotZ, -D3DX_PI * 0.85f);
	D3DXMatrixTranslation(&matTrans, -45.0f, 0.0f, -10.0f);
	D3DXMatrixMultiply(&matWorld, &matScale, &matRotZ);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans);
	g_pnodeTransHouse = new Transform(pD3DDevice, matWorld);

	D3DXMatrixTranslation(&matTrans, 2.0f, 50.0f, 0.0f);
	D3DXMatrixScaling(&matScale, 0.2f, 0.2f, 0.2f);
	g_pnodeTransHouse2 = new Transform(pD3DDevice, matTrans);
	
	g_pnodeHouse = new Geometry(pD3DDevice, L"house.x");

	g_pnodeCamera->SetChild(g_pnodeProj);

	// Shader is now a child of the projection matrix, and any subchildren will be using this shader

	g_pnodeProj->SetChild(g_pnodeShaderComplex2);

	D3DXMatrixRotationY(&matRotY, -D3DX_PI/2);
	D3DXMatrixRotationZ(&matRotZ, D3DX_PI);
	D3DXMatrixTranslation(&matTrans, 15.0f, 0.0f, 0.0f);

	D3DXMatrixMultiply(&matWorld, &matRotY, &matRotZ);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans);

	g_pnodeTransPerson = new Transform(pD3DDevice, matWorld);

	//												Leng, Disp,		Rot Default Min Max,					Twist Default Min Max,			Filename
	g_pnodePerson =		new Articulated(pD3DDevice, 0.0f, 0.0f,		0.0f, 0.0f, 0.0f,						0.0f, 0.0f, 0.0f,				NULL);
	g_pnodePelvis =		new Articulated(pD3DDevice, 0.0f, 0.0f,		D3DX_PI, D3DX_PI, D3DX_PI,				0.0f, 0.0f, 0.0f,				NULL);
	
	g_pnodeRUpperLeg =	new Articulated(pD3DDevice, 5.0f, -1.5f,	D3DX_PI/15, -D3DX_PI/4, D3DX_PI/3,		0.0f, 0.0f, 0.0f,				L"cyc1-5.x"); 
	g_pnodeLUpperLeg =	new Articulated(pD3DDevice, 5.0f, 1.5f,		D3DX_PI/15, -D3DX_PI/4, D3DX_PI/3,		0.0f, 0.0f, 0.0f,				L"cyc1-5.x");
	g_pnodeRLowerLeg =	new Articulated(pD3DDevice, 5.0f, 0.0f,		-D3DX_PI/13, -D3DX_PI/2, 0.0f,			0.0f, 0.0f, 0.0f,				L"cyc1-5.x");
	g_pnodeLLowerLeg =	new Articulated(pD3DDevice, 5.0f, 0.0f,		-D3DX_PI/13, -D3DX_PI/2, 0.0f,			0.0f, 0.0f, 0.0f,				L"cyc1-5.x");
	
	g_pnodeLowerBack =	new Articulated(pD3DDevice, 5.0f, 0.0f,		D3DX_PI/25, -D3DX_PI/10, D3DX_PI/10,	0.0f, -D3DX_PI/8, D3DX_PI/8,	L"cyc2-5.x");
	g_pnodeUpperBack =	new Articulated(pD3DDevice, 5.0f, 0.0f,		-D3DX_PI/24, -D3DX_PI/2, 0.0f,			0.0f, -D3DX_PI/8, D3DX_PI/8,	L"cyc2-5.x");
	
	g_pnodeNeck =		new Articulated(pD3DDevice, 1.0f, 0.0f,		0.0f, 0.0f, 0.0f,						0.0f, -D3DX_PI/3, D3DX_PI/3,	NULL);
	g_pnodeHead =		new Articulated(pD3DDevice, 1.5f, 0.0f,		0.0f, -D3DX_PI/3, D3DX_PI/3,			0.0f, 0.0f, 0.0f,				L"head.x");
	g_pnodeShoulders =	new Articulated(pD3DDevice, 0.5f, 0.0f,		D3DX_PI, D3DX_PI, D3DX_PI,				0.0f, 0.0f, 0.0f,				NULL);
	
	g_pnodeRUpperArm =	new Articulated(pD3DDevice, 3.0f, -2.5f,	D3DX_PI/20, -D3DX_PI/2, D3DX_PI/1.1f,	0.0f, -D3DX_PI/12, D3DX_PI/12,	L"cyc1-3.x");
	g_pnodeLUpperArm =	new Articulated(pD3DDevice, 3.0f, 2.5f,		D3DX_PI/20, -D3DX_PI/2, D3DX_PI/1.1f,	0.0f, -D3DX_PI/12, D3DX_PI/12,	L"cyc1-3.x");
	g_pnodeRLowerArm =	new Articulated(pD3DDevice, 3.0f, 0.0f,		D3DX_PI/10, 0.0f, D3DX_PI/1.3f,			0.0f, 0.0f, 0.0f,				L"cyc1-3.x");
	g_pnodeLLowerArm =	new Articulated(pD3DDevice, 3.0f, 0.0f,		D3DX_PI/10, 0.0f, D3DX_PI/1.3f,			0.0f, 0.0f, 0.0f,				L"cyc1-3.x");

	g_pnodePerson->SetDescription(L"Base");
	g_pnodeNeck->SetDescription(L"Neck");
	g_pnodeHead->SetDescription(L"Head");

	g_pnodeShaderComplex2->SetChild(g_pnodeTransPerson);
	
	g_pnodeTransPerson->SetSibling(g_pnodeTransPlane);
	g_pnodeTransPlane->SetChild(g_pnodePlane);

	g_pnodeTransPlane->SetSibling(g_pnodeTransHouse);
	g_pnodeTransHouse->SetChild(g_pnodeHouse);
	g_pnodeHouse->SetChild(g_pnodeTransHouse2);

	// Now the house has a shader added to it to perform lighting.

	g_pnodeTransHouse->InsertChild(g_pnodeShaderComplex);
	
	g_pnodeTransPerson->SetChild(g_pnodePerson);
	g_pnodePerson->SetChild(g_pnodeLowerBack);
	g_pnodeLowerBack->SetSibling(g_pnodePelvis);
	g_pnodePelvis->SetChild(g_pnodeLUpperLeg);
	g_pnodeLUpperLeg->SetSibling(g_pnodeRUpperLeg);
	g_pnodeRUpperLeg->SetChild(g_pnodeRLowerLeg);
	g_pnodeLUpperLeg->SetChild(g_pnodeLLowerLeg);
	g_pnodeLowerBack->SetChild(g_pnodeUpperBack);
	g_pnodeUpperBack->SetChild(g_pnodeNeck);
	g_pnodeNeck->SetChild(g_pnodeHead);
	g_pnodeNeck->SetSibling(g_pnodeShoulders);
	g_pnodeShoulders->SetChild(g_pnodeRUpperArm);
	g_pnodeRUpperArm->SetSibling(g_pnodeLUpperArm);
	g_pnodeRUpperArm->SetChild(g_pnodeRLowerArm);
	g_pnodeLUpperArm->SetChild(g_pnodeLLowerArm);
    
    
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

	g_pnodeRUpperArm->AddAnimation(L"Walk", RUAWalkAnim);
	g_pnodeLUpperArm->AddAnimation(L"Walk", LUAWalkAnim);
	g_pnodeRUpperLeg->AddAnimation(L"Walk", RULWalkAnim);
	g_pnodeLUpperLeg->AddAnimation(L"Walk", LULWalkAnim);
	g_pnodeRLowerLeg->AddAnimation(L"Walk", RLLWalkAnim);
	g_pnodeLLowerLeg->AddAnimation(L"Walk", LLLWalkAnim);
	g_pnodeUpperBack->AddAnimation(L"Walk", UBWalkAnim);

}

void CleanUp()
{
	SAFE_DELETE(g_pRenderer);

	SAFE_DELETE(g_pnodeCamera);
	SAFE_DELETE(g_pnodeProj);
	SAFE_DELETE(g_pnodeState);

	SAFE_DELETE(g_pnodeTransPlane);
	SAFE_DELETE(g_pnodeTransPerson);
	SAFE_DELETE(g_pnodeTransHouse);
	SAFE_DELETE(g_pnodeTransHouse2);

	SAFE_DELETE(g_pnodeArt);

	SAFE_DELETE(g_pnodePlane);
	SAFE_DELETE(g_pnodeHouse);
	SAFE_DELETE(g_pnodeHouse2);

	SAFE_DELETE(g_pnodeShaderSimple);
	SAFE_DELETE(g_pnodeShaderComplex);
	SAFE_DELETE(g_pnodeShaderComplex2);

	SAFE_DELETE(ref1);

	SAFE_DELETE(g_pnodeTransPerson);

	SAFE_DELETE(g_pnodePerson);
	SAFE_DELETE(g_pnodePelvis);
	SAFE_DELETE(g_pnodeRUpperLeg);
	SAFE_DELETE(g_pnodeLUpperLeg);
	SAFE_DELETE(g_pnodeRLowerLeg);
	SAFE_DELETE(g_pnodeLLowerLeg);
	SAFE_DELETE(g_pnodeLowerBack);
	SAFE_DELETE(g_pnodeUpperBack);
	SAFE_DELETE(g_pnodeNeck);
	SAFE_DELETE(g_pnodeHead);
	SAFE_DELETE(g_pnodeShoulders);
	SAFE_DELETE(g_pnodeRUpperArm);
	SAFE_DELETE(g_pnodeLUpperArm);
	SAFE_DELETE(g_pnodeRLowerArm);
	SAFE_DELETE(g_pnodeLLowerArm);
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
    DXUTCreateWindow( TEXT("Simple Scene Graph Implementation") );
    DXUTCreateDevice( D3DADAPTER_DEFAULT, true, 640, 480, IsDeviceAcceptable, ModifyDeviceSettings );

	InitalizeGraph();

    // Start the render loop
    DXUTMainLoop();

	CleanUp();

    return DXUTGetExitCode();
}