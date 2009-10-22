#include "Renderer.h"

Renderer::Renderer( LPDIRECT3DTEXTURE9 a_textureShadowMap, LPDIRECT3DSURFACE9 a_pSurfaceShadowDS ) : SGLib::SGRenderer()
{
    this->m_textureShadowMap = a_textureShadowMap;
    this->m_pSurfaceShadowDS = a_pSurfaceShadowDS;
}
Renderer::~Renderer(void)
{
}

void Renderer::Render( SGLib::Node* a_pNodeBase )
{
    HRESULT hr;

    if (!a_pNodeBase)
        return;

    // clear back buffer
    V(a_pNodeBase->GetDevice()->Clear(0, NULL, m_dwOptions,
        m_colourClear, m_fZClear, m_dwStencil))
    
    LPDIRECT3DSURFACE9 pSurfaceOld = NULL;
    LPDIRECT3DSURFACE9 pSurfaceTexture = NULL;
    LPDIRECT3DSURFACE9 pSurfaceOldDS = NULL;
        
    V(a_pNodeBase->GetDevice()->GetRenderTarget(0, &pSurfaceOld))
    V(m_textureShadowMap->GetSurfaceLevel(0, &pSurfaceTexture))
    V(a_pNodeBase->GetDevice()->SetRenderTarget(0, pSurfaceTexture))
    V(a_pNodeBase->GetDevice()->GetDepthStencilSurface(&pSurfaceOldDS))
    V(a_pNodeBase->GetDevice()->SetDepthStencilSurface(m_pSurfaceShadowDS))

    V(a_pNodeBase->GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0, 0))

    V(a_pNodeBase->GetDevice()->SetRenderTarget(0, pSurfaceOld))
    V(a_pNodeBase->GetDevice()->SetDepthStencilSurface(pSurfaceOldDS))

    V(a_pNodeBase->GetDevice()->BeginScene())

        // call general render function for base node
        RenderNode(a_pNodeBase);

    V(a_pNodeBase->GetDevice()->EndScene())
}