#include "GraphicsLayer.h"

namespace Graphics {

GraphicsLayer* GraphicsLayer::m_pGlobalGLayer = NULL;

GraphicsLayer::GraphicsLayer()
{

	//m_pMeshMaterials = NULL;
	//m_pMeshTextures = NULL;
	//m_pMesh = NULL;

}

GraphicsLayer::~GraphicsLayer()
{
	if (NULL != m_pGlobalGLayer) {
		delete m_pGlobalGLayer;
		m_pGlobalGLayer = NULL;
	}
}

int GraphicsLayer::SetCamera(const Camera& cam)
{
	m_camera = cam;

	Matrix tmpView = cam.getViewMatrix();
	Matrix tmpProj = cam.getProjectionMatrix();

	D3DXMATRIXA16 matView;
    D3DXMATRIXA16 matProj;

	//Copy the matrix for now
	for (int i = 0; i<16; i++) {
		matView[i] = tmpView[i];
		matProj[i] = tmpProj[i];
	}

    //D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI / 4, 1.0f, 1.0f, 100.0f );

	m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
	m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

	return 0;
}

int GraphicsLayer::SetCamera()
{
	//Simple camera for testing
    D3DXMATRIXA16 matWorld;
    D3DXMatrixRotationY( &matWorld, timeGetTime() / 1000.0f );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    D3DXVECTOR3 vEyePt( 0.0f, 3.0f,-10.0f );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXMATRIXA16 matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    D3DXMATRIXA16 matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI / 4, 1.0f, 1.0f, 100.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
	return 0;
}

int GraphicsLayer::Init( HWND hWnd ) 
{
    // Create the D3D object.
    if( NULL == ( m_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        return E_FAIL;

    // Set up the structure used to create the D3DDevice. Since we are now
    // using more complex geometry, we will create a device with a zbuffer.
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof( d3dpp ) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	// Create the D3DDevice
    if( FAILED( m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp, &m_pd3dDevice ) ) )
    {
        return E_FAIL;
    }

	// Turn on the zbuffer
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

    // Turn on ambient lighting 
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffffffff );
    return S_OK;
}

//This makes an assumption of 1 texture per model for now
HRESULT GraphicsLayer::LoadGeometryTest(LPD3DXMESH& pMesh, D3DMATERIAL9*& pMeshMaterials, 
										LPDIRECT3DTEXTURE9*& pMeshTextures, DWORD& dwNumMaterials, 
										const char* filePath, const char* textPath )
{
    LPD3DXBUFFER pD3DXMtrlBuffer;

	WCHAR wszFilePath[128]; //convert to wide char
	MultiByteToWideChar(CP_ACP, 0, filePath, -1, wszFilePath,128); 

    // Load the mesh from the specified file into device memory
    if( FAILED( D3DXLoadMeshFromX( filePath, D3DXMESH_SYSTEMMEM,
                                   m_pd3dDevice, NULL,
                                   &pD3DXMtrlBuffer, NULL, &dwNumMaterials,
                                   &pMesh ) ) )
    {
            //MessageBox( NULL, L"Could not find model", L"eRacer.exe", MB_OK );
            return E_FAIL;
    }

    // We need to extract the material properties and texture names from the 
    // pD3DXMtrlBuffer
    D3DXMATERIAL* d3dxMaterials = ( D3DXMATERIAL* )pD3DXMtrlBuffer->GetBufferPointer();
    pMeshMaterials = new D3DMATERIAL9[dwNumMaterials];
    if( pMeshMaterials == NULL )
        return E_OUTOFMEMORY;
    pMeshTextures = new LPDIRECT3DTEXTURE9[dwNumMaterials];
    if( pMeshTextures == NULL )
        return E_OUTOFMEMORY;

    for( DWORD i = 0; i < dwNumMaterials; i++ )
    {
        // Copy the material
        pMeshMaterials[i] = d3dxMaterials[i].MatD3D;

        // Set the ambient color for the material (D3DX does not do this)
        pMeshMaterials[i].Ambient = pMeshMaterials[i].Diffuse;

        pMeshTextures[i] = NULL;
        if( d3dxMaterials[i].pTextureFilename != NULL &&
            lstrlenA( d3dxMaterials[i].pTextureFilename ) > 0 )
        {
            // Create the texture
            if( FAILED( D3DXCreateTextureFromFileA( m_pd3dDevice,
													textPath,
                                                    &pMeshTextures[i] ) ) )
            {
				//MessageBox( NULL, L"Could not find texture", L"eRacer.exe", MB_OK );
            }
        }
    }

    // Done with the material buffer
    pD3DXMtrlBuffer->Release();

    return S_OK;
}

int GraphicsLayer::RenderFrame()
{
	// Clear the backbuffer and the zbuffer
    m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0f, 0 );

    // Begin the scene
	//In the future this will be done inside a loop to handle each shader/effect
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
		vector<StaticGeometry*> visibleObjects;
		m_scene.GetVisibleNodes(m_camera, visibleObjects);
		for(vector<StaticGeometry*>::const_iterator object = visibleObjects.begin(); 
			object!=visibleObjects.end(); object++){
			//The camera can be set here, but does not need to be
			// Meshes are divided into subsets, one for each material. Render them in a loop
			for(unsigned int i = 0; i<(*object)->Materials().size(); i++){
				m_pd3dDevice->SetMaterial( (*object)->Materials()[i]);
				m_pd3dDevice->SetTexture(0, (*object)->Textures()[i]);
				(*object)->GetMesh()->DrawSubset(i);
			}
		}

        // End the scene
        m_pd3dDevice->EndScene();
    }

    // Present the backbuffer contents to the display
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	return S_OK;
}

//This function is a stop gap until caching by lists is completed
int GraphicsLayer::RenderFrame(const StaticGeometry& r)
{
	// Clear the backbuffer and the zbuffer
    m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0f, 0 );

    // Begin the scene
	//In the future this will be done inside a loop to handle each shader/effect
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
        //The camera can be set here, but does not need to be
        // Meshes are divided into subsets, one for each material. Render them in a loop
		for(unsigned int i = 0; i<r.Materials().size(); i++){
			m_pd3dDevice->SetMaterial( r.Materials()[i]);
			m_pd3dDevice->SetTexture(0, r.Textures()[i]);
			r.GetMesh()->DrawSubset(i);
		}
	/*
        for( DWORD i = 0; i < r->m_dwNumMaterials; i++ )
        {
            // Set the material and texture for this subset
            m_pd3dDevice->SetMaterial( &r->m_pMeshMaterials[i] );
            m_pd3dDevice->SetTexture( 0, r->m_pMeshTextures[i] );
            // Draw the mesh subset
            r->m_pMesh->DrawSubset( i );
        }
*/
        // End the scene
        m_pd3dDevice->EndScene();
    }

    // Present the backbuffer contents to the display
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	return S_OK;
}

int GraphicsLayer::Shutdown()
{
	//Free model and texture information
	if( NULL != m_pMeshMaterials )
        delete[] m_pMeshMaterials;
	m_pMeshMaterials = NULL;

    if( NULL != m_pMeshTextures )
    {
        for( DWORD i = 0; i < m_dwNumMaterials; i++ )
        {
            if( m_pMeshTextures[i] )
                m_pMeshTextures[i]->Release();
        }
        delete[] m_pMeshTextures;
		m_pMeshTextures = NULL;
    }

    if( NULL != m_pMesh)
        m_pMesh->Release();
	m_pMesh = NULL;

	//Release the Devce
    if( NULL != m_pd3dDevice )
        m_pd3dDevice->Release();
	m_pd3dDevice = NULL;

    if( NULL != m_pD3D)
        m_pD3D->Release();
	m_pD3D = NULL;

	return S_OK;
}

};