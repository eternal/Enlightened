#pragma once
#include "SGLibResource.h"

class Renderer :
    public SGLib::SGRenderer
{
public:
    Renderer(std::vector<LPDIRECT3DTEXTURE9>* a_textureShadowMap, std::vector<LPDIRECT3DSURFACE9>* a_pSurfaceShadowDS, std::vector<LPDIRECT3DSURFACE9>* a_shadowMapSurface);
    ~Renderer(void);
    
    std::vector<LPDIRECT3DTEXTURE9>* m_textureShadowMap;
    std::vector<LPDIRECT3DSURFACE9>* m_pSurfaceShadowDS;
    std::vector<LPDIRECT3DSURFACE9>* m_shadowMapSurface;

    void Render(SGLib::Node* a_pNodeBase);
};

