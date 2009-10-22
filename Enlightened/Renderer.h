#pragma once
#include "SGLibResource.h"

class Renderer :
    public SGLib::SGRenderer
{
public:
    Renderer(LPDIRECT3DTEXTURE9 a_textureShadowMap, LPDIRECT3DSURFACE9 a_pSurfaceShadowDS);
    ~Renderer(void);
    
    LPDIRECT3DTEXTURE9 m_textureShadowMap;
    LPDIRECT3DSURFACE9 m_pSurfaceShadowDS;

    void Render(SGLib::Node* a_pNodeBase);
};

