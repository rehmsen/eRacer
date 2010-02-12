#pragma once
#ifndef FONTMANAGER_H
#define FONTMANAGER_H

#include <d3d9.h>
#include <d3dx9.h>
#include <map>
#include <string>
#include <vector>
#include <algorithm>

#include "../Core/Math.h"

using namespace std;

namespace Graphics {

typedef pair<string, int> FontDescription;

class StringRenderable
{
public:
	ID3DXFont* m_pFont;
	string m_strTextBuffer;
	RECT m_renderArea; //Position and area to render in
	D3DXCOLOR m_color;

	StringRenderable();
	~StringRenderable();
	/**
	 * @brief Comparison operator for sorting - sorts by the address of the font used
	 */
	bool operator<(const StringRenderable& s);
};



class FontManager
{
public:
	//static const unsigned int FONT_SIZE;
	map<FontDescription, ID3DXFont*> m_fontCacheSimple;
	vector<StringRenderable> m_strList;	//List of strings to render

	LPDIRECT3DDEVICE9   m_pd3dDevice;
	ID3DXSprite*        m_pTextSprite;

	FontManager();
	~FontManager();

	void Init(LPDIRECT3DDEVICE9 device);

	void WriteString(const char* msg, const char* fontName, int size, const Vector3 &pos, const Vector3 &color);
	void WriteString(const char* msg, ID3DXSprite* font, Vector3 color);
	void CreateFont(const char* fontName, int size, bool bold, bool italic); 
	void Draw();
	void Shutdown();
};

};
#endif