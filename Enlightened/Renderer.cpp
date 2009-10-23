#include "Renderer.h"

Renderer::Renderer(std::vector<LPDIRECT3DTEXTURE9>* a_textureShadowMap, std::vector<LPDIRECT3DSURFACE9>* a_pSurfaceShadowDS, std::vector<LPDIRECT3DSURFACE9>* a_shadowMapSurface) : SGLib::SGRenderer()
{
    this->m_textureShadowMap = a_textureShadowMap;
    this->m_pSurfaceShadowDS = a_pSurfaceShadowDS;
    this->m_shadowMapSurface = a_shadowMapSurface;
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
    LPDIRECT3DSURFACE9 pSurfaceOldDS = NULL;
            
    V(a_pNodeBase->GetDevice()->GetRenderTarget(0, &pSurfaceOld))
    V(a_pNodeBase->GetDevice()->GetDepthStencilSurface(&pSurfaceOldDS))
    for (int i = 0; i < 6; i++)
    {
        V(m_textureShadowMap->at(i)->GetSurfaceLevel(0, &(m_shadowMapSurface->at(i))))
        V(a_pNodeBase->GetDevice()->SetRenderTarget(0, m_shadowMapSurface->at(i)))
        V(a_pNodeBase->GetDevice()->SetDepthStencilSurface(m_pSurfaceShadowDS->at(i)))
        V(a_pNodeBase->GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0, 0))    
    }
    
    V(a_pNodeBase->GetDevice()->SetRenderTarget(0, pSurfaceOld))
    V(a_pNodeBase->GetDevice()->SetDepthStencilSurface(pSurfaceOldDS))

    V(a_pNodeBase->GetDevice()->BeginScene())

        // call general render function for base node
        RenderNode(a_pNodeBase);

    V(a_pNodeBase->GetDevice()->EndScene())
}