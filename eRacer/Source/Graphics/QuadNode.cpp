/**
 * @file QuadNode.cpp
 * @brief Implementation of the QuadNode class
 *
 * @date 21.02.2010
 * @author: Ole Rehmsen
 */

#include "QuadNode.h"

#include "GraphicsLayer.h"

namespace Graphics {

QuadNode::QuadNode(const string& name, const Matrix& transform)
: RenderableNode(name,transform),
  initialized(false)
{
	assert(SUCCEEDED(
		GraphicsLayer::GetInstance().GetDevice()->CreateVertexBuffer(
			4 * sizeof(Vertex),   
			0,
			D3DFVF_XYZ | D3DFVF_TEX1,
			D3DPOOL_MANAGED,
			&vertexBuffer_,
			NULL)
		));
	
	Vertex* vertices;
	vertexBuffer_->Lock(0, 0, (void**)&vertices, 0);

	//top left
	vertices[0].position = Point3(-.5f,.5f,0);
	//vertices[0].color = D3DCOLOR_COLORVALUE( 1.0f, 1.0f, 1.0f, 1.0f );
	vertices[0].u = 0.0f;
	vertices[0].v = 0.0f;

	//top right
	vertices[1].position = Point3(.5f,.5f,0);
	//vertices[1].color = D3DCOLOR_COLORVALUE( 1.0f, 1.0f, 1.0f, 1.0f );
	vertices[1].u = 1.0f;
	vertices[1].v = 0.0f;

	//bottom right
	vertices[2].position = Point3(.5f,-.5f,0);
	//vertices[2].color = D3DCOLOR_COLORVALUE( 1.0f, 1.0f, 1.0f, 1.0f );
	vertices[2].u = 1.0f;
	vertices[2].v = 1.0f;

	//bottom left
	vertices[3].position = Point3(-.5f,-.5f,0);
	//vertices[3].color = D3DCOLOR_COLORVALUE( 1.0f, 1.0f, 1.0f, 1.0f );
	vertices[3].u = 0.0f;
	vertices[3].v = 1.0f;

	vertexBuffer_->Unlock();
}

void QuadNode::SetTextureCoordinates(float tl_u, float tl_v, 
									 float tr_u, float tr_v, 
									 float br_u, float br_v, 
									 float bl_u, float bl_v){
	
	Vertex* vertices;
	vertexBuffer_->Lock(0, 0, (void**)&vertices, 0);

	//top left
	vertices[0].u = tl_u;
	vertices[0].v = tl_v;

	//top right
	vertices[1].u = tr_u;
	vertices[1].v = tr_v;

	//bottom right
	vertices[2].u = br_u;
	vertices[2].v = br_v;

	//bottom left
	vertices[3].u = bl_u;
	vertices[3].v = bl_v;

	vertexBuffer_->Unlock();
}


void QuadNode::Init(IDirect3DTexture9* texture){
	assert(NULL != texture);
	texture_ = texture;
	initialized = true;
	UpdateWorldBounds();
}

void QuadNode::Draw(IDirect3DDevice9* device) const{
	if(!initialized)
		return;

	assert(NULL != device);

  // set the transform
	device->SetTransform(D3DTS_WORLDMATRIX(0), &transform_);
	device->SetStreamSource(0, vertexBuffer_, 0, sizeof(Vertex));
	device->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1);

	//FIXME Should not be accessed directly
	ID3DXEffect* effect = GraphicsLayer::GetInstance().m_pEffect;
  effect->SetMatrix( "g_WorldMatrix", &transform_);
  Vector2 z;
  z.u = 0;
  z.v = 0;
  effect->SetValue( "g_TexOffset", &z,     sizeof( Vector2 ) );

	assert(SUCCEEDED(effect->SetTechnique( "RenderSceneWithTextureFixedLight" )));
	UINT cPasses = 1;
	assert(SUCCEEDED(effect->Begin( &cPasses, 0 )));
	for(UINT iPass = 0; iPass < cPasses; iPass++ ){
		effect->BeginPass( iPass ) ;
		device->SetTexture (0, texture_);

		device->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
		assert(SUCCEEDED(effect->EndPass()));
	}
	assert(SUCCEEDED(effect->End()));

}

void QuadNode::UpdateWorldBounds(){
	if(!initialized)	
		return;
	
	Point3 p[4];
	p[0] = mul1(transform_,Point3(-.5f,.5f,0));
	p[1] = mul1(transform_,Point3(.5f,.5f,0));
	p[2] = mul1(transform_,Point3(.5f,-.5f,0));
	p[3] = mul1(transform_,Point3(-.5f,-.5f,0));
	
	float max = 0.0f;
	
	for(unsigned int i=0; i<4; i++)
		for(unsigned int j=i+1; j<4; j++){
			float distance = length(p[i]-p[j]);
			if(distance > max){
				max = distance;
				worldBounds_.center = (p[i]+p[j])/2;
			}
		}
	worldBounds_.radius = max /2;
}


}