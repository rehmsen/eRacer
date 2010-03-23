/**
 * @file GraphicsLayer.h
 * @brief Definition of the GraphicsLayer class
 *
 * @date 09.01.2010
 * @author Don Ha
 */

#pragma once
#ifndef GRAPHICSLAYER_H
#define GRAPHICSLAYER_H

#include <d3d9.h>
#include <d3dx9.h>
#include <mmsystem.h>
#include "Renderable.h"
#include "StringSprite.h"
#include "Camera.h"

#include "DebugRenderable.h"

namespace Graphics {


/**
 * @brief wraps DirectX calls for flexble rendering
 */
class GraphicsLayer
{

private:
	static GraphicsLayer *m_pGlobalGLayer;

	LPDIRECT3D9						m_pD3D;				//Used to create the D3DDevice
	IDirect3DDevice9*   	m_pd3dDevice;	//Our rendering device
	D3DPRESENT_PARAMETERS m_presentationParameters;
	StringSprite* stringSprite;

	void resetPresentationParameters();
	void resetDevice();
protected:
	GraphicsLayer();	//Constructor, Singleton 
	Camera *camera;
	
	IDirect3DSurface9* screen;
	IDirect3DSurface9* msaasurf;
	IDirect3DSurface9* depthsurf;
	std::map<std::string, ID3DXEffect*> effects;

public:
	int width, height;
	
	DebugRenderable* debugRenderable;
	ID3DXEffect* m_pEffect;       // Temporary Variable Only!! Please do not use!
	D3DMATERIAL9 DefaultMaterial(); // also a hack
	
	ID3DXEffect* GraphicsLayer::GetEffect(char* file);
	
	void 		SetCamera(Camera& camera);
	Camera* GetCamera() { return camera; }

	ID3DXSprite* CreateSprite(int x, int y, int w);	
	
	void 		SetViewport(int x, int y, int w, int h);
	void ResetViewport();

	IDirect3DDevice9* GetDevice() const { return m_pd3dDevice; }
	
	~GraphicsLayer();	//Destructor
	int Init( HWND hWnd );

	
	void PreRender();
	void PostRender();
	
	void WriteString(const char* text, const char* family, int size, const Vector3 &pos, const Vector3 &color = WHITE);
	
	void Shutdown();
	
	
	void SetTexture(UINT id, IDirect3DTexture9* tex)
	{
		static IDirect3DTexture9* oldtex[16] =  {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
		if (tex != oldtex[id])
			m_pd3dDevice->SetTexture(0, oldtex[id]=tex);
	}

	static GraphicsLayer *GetInstance()
	{
		if (m_pGlobalGLayer == NULL)	m_pGlobalGLayer = new GraphicsLayer;
		return m_pGlobalGLayer;
	}

};

}

#endif