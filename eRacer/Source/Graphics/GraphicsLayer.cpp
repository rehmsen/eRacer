#include "GraphicsLayer.h"

#include "Core/Time.h"
#include "Math.h"

#include <iostream>
using namespace std;

namespace Graphics {

GraphicsLayer* GraphicsLayer::m_pGlobalGLayer = NULL;

GraphicsLayer::GraphicsLayer()
{
}

GraphicsLayer::~GraphicsLayer()
{
    if (NULL != m_pGlobalGLayer) {
        delete m_pGlobalGLayer;
        m_pGlobalGLayer = NULL;
    }
}

void GraphicsLayer::SetCamera(Camera& cam)
{
    camera = &cam;
    
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION,   &cam.GetProjectionMatrix() );
    m_pd3dDevice->SetTransform( D3DTS_VIEW,         &cam.GetViewMatrix() );

    // HACK!
    // In the future this will be done inside a loop to handle each shader/effect
	D3DXMATRIXA16 viewMat = cam.GetViewMatrix();
	D3DXMATRIXA16 projMat = cam.GetProjectionMatrix();
	HRESULT hr;
	hr = m_pEffect->SetMatrix( "g_ViewMatrix", &viewMat );
	hr = m_pEffect->SetMatrix( "g_ProjectionMatrix", &projMat );
	hr = m_pEffect->SetTechnique( "RenderSceneWithTextureDefault" );
}

int GraphicsLayer::Init( HWND hWnd ) 
{
    // Create the D3D object.
    if( NULL == ( m_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
    {
        assert(false);
        return E_FAIL;
    }

	resetPresentationParameters();
    
    
    // Create the D3DDevice
    if( FAILED( m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                      D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                      &m_presentationParameters, &m_pd3dDevice ) ) )
    {
        assert(false);
        return E_FAIL;
    }

    //Init the font manager
    m_fontManager.Init(m_pd3dDevice);

    // Turn on the zbuffer
    assert(SUCCEEDED(m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE )));

    // Turn on ambient lighting 
    assert(SUCCEEDED(m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffffffff )));

    //for testing, do not cull anything
    assert(SUCCEEDED(m_pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW)));

    assert(SUCCEEDED(m_pd3dDevice->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR)));
    assert(SUCCEEDED(m_pd3dDevice->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR)));
	assert(SUCCEEDED(m_pd3dDevice->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_WRAP)));
	assert(SUCCEEDED(m_pd3dDevice->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_WRAP)));
    
    // AA
    assert(SUCCEEDED(m_pd3dDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE)));
    
	//Shaders
	DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE | D3DXSHADER_DEBUG;
	// dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
    // dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT; //Force software shader since not everything is textured
	
	assert(SUCCEEDED(D3DXCreateEffectFromFile(
		m_pd3dDevice, 
		"Shaders/BasicHLSL.fx", 
		NULL, NULL, 
		dwShaderFlags, NULL, 
		&m_pEffect, NULL
	)));

    // Set effect variables as needed
    D3DXCOLOR colorMtrlDiffuse( 1.0f, 1.0f, 1.0f, 1.0f );
    D3DXCOLOR colorMtrlAmbient( 1.0f, 1.0f, 1.0f, 0 );
    m_pEffect->SetValue( "g_MaterialAmbientColor", &colorMtrlAmbient, sizeof( D3DXCOLOR ) );
    m_pEffect->SetValue( "g_MaterialDiffuseColor", &colorMtrlDiffuse, sizeof( D3DXCOLOR ) );

    // save the screen surface
    m_pd3dDevice->GetRenderTarget(0, &screen);
    
    
    D3DSURFACE_DESC desc;
    screen->GetDesc(&desc);
    
    // create a new surface
    // http://www.borgsoft.de/renderToSurface.html
    assert(SUCCEEDED(m_pd3dDevice->CreateRenderTarget(
      desc.Width, desc.Height,
      D3DFMT_A8R8G8B8,
      D3DMULTISAMPLE_2_SAMPLES, 0,
      false,
      &msaasurf,
      NULL
    )));
    
    // create a depth buffer to go with it
    assert(SUCCEEDED(m_pd3dDevice->CreateDepthStencilSurface(
        desc.Width, desc.Height,
        D3DFMT_D16,
        D3DMULTISAMPLE_2_SAMPLES, 0,
        TRUE,
        &depthsurf,
        NULL
    )));
    
    return S_OK;
}

void GraphicsLayer::resetPresentationParameters(){
    // Set up the structure used to create the D3DDevice. Since we are now
    // using more complex geometry, we will create a device with a zbuffer.
    ZeroMemory( &m_presentationParameters, sizeof( m_presentationParameters ) );
    m_presentationParameters.Windowed = TRUE;
    m_presentationParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
    m_presentationParameters.MultiSampleType = D3DMULTISAMPLE_8_SAMPLES;
    m_presentationParameters.MultiSampleQuality = 0;
    m_presentationParameters.BackBufferFormat = D3DFMT_UNKNOWN;
    m_presentationParameters.EnableAutoDepthStencil = TRUE;
    m_presentationParameters.AutoDepthStencilFormat = D3DFMT_D16;
    m_presentationParameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
}


void GraphicsLayer::PreRender(){
    
    // render to offscreen surface
    assert(SUCCEEDED(m_pd3dDevice->SetRenderTarget(0, msaasurf)));
    assert(SUCCEEDED(m_pd3dDevice->SetDepthStencilSurface(depthsurf)));
    
    // Clear the backbuffer and the zbuffer
    assert(SUCCEEDED(m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0f, 0 )));

    // Begin the scene
    //In the future this will be done inside a loop to handle each shader/effect
    assert(SUCCEEDED( m_pd3dDevice->BeginScene()));
    assert(SUCCEEDED(m_pd3dDevice->SetTransform(D3DTS_WORLDMATRIX(0), &IDENTITY)));
}

void GraphicsLayer::PostRender(){

     // draw overlay
    m_fontManager.Draw();
    
    

    // assert(SUCCEEDED(m_pd3dDevice->Present( NULL, NULL, NULL, NULL )));
    
    // End the scene
    assert(SUCCEEDED(m_pd3dDevice->EndScene()));
    
    assert(SUCCEEDED(m_pd3dDevice->SetRenderTarget(0, screen)));
    IDirect3DSurface9* backBuffer = NULL;
    assert(SUCCEEDED(m_pd3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer)));
    assert(SUCCEEDED(m_pd3dDevice->StretchRect(msaasurf, NULL, backBuffer, NULL, D3DTEXF_LINEAR)));
    backBuffer->Release();
    

    // Present the backbuffer contents to the display
	HRESULT r = m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

	switch(r){
	case D3DERR_DRIVERINTERNALERROR:
		printf("driver internal error - trying to reset presentation parameters\n");
		resetDevice();
		break;
	case D3DERR_DEVICEREMOVED:
		throw runtime_error("Fatal error: The Direct3D Device has been removed");
		break;
	default:
		if(FAILED(r))
			printf("Encountered logic error 0x%x\n",(int)r);

		assert(SUCCEEDED(r));
	}
}

void GraphicsLayer::resetDevice(){
	resetPresentationParameters();
	HRESULT r = m_pd3dDevice->Reset(&m_presentationParameters);

	switch(r){
	case D3DERR_DEVICELOST:
		printf("Fatal error: The Direct3D Device has been lost");
		throw runtime_error("Fatal error: The Direct3D Device has been lost");
	case D3DERR_DEVICEREMOVED:
		printf("Fatal error: The Direct3D Device has been lost");
		throw runtime_error("Fatal error: The Direct3D Device has been removed");
	case D3DERR_DRIVERINTERNALERROR:
		printf("again driver internal error\n");
		throw runtime_error("Fatal error: Repeated driver internal error.");
	case D3DERR_OUTOFVIDEOMEMORY:
		printf("Fatal error: Out of video memory\n");
		throw runtime_error("Fatal error: Out of video memory.");
	}
}



void GraphicsLayer::WriteString(const char* msg, const char* fontName, int size, const Vector3 &pos, const RGB &color)
{
    m_fontManager.WriteString(msg, fontName, size, pos, color);
}

void GraphicsLayer::Shutdown()
{
    if( NULL != m_pEffect)
        m_pEffect->Release();
    m_pEffect = NULL;

    //Release the Devce
    if( NULL != m_pd3dDevice )
        m_pd3dDevice->Release();
    m_pd3dDevice = NULL;

    if( NULL != m_pD3D)
        m_pD3D->Release();
    m_pD3D = NULL;

    m_fontManager.Shutdown();
}

}