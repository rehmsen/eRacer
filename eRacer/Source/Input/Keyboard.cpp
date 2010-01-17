/**
 * @file Keyboard.cpp
 * @brief Implementation of the Keyboard class
 *
 * @date 09.01.2010
 * @author: Don Ha
 */

#include "Keyboard.h"
#include <cstdio>

int Keyboard::Init(HWND hWnd, HINSTANCE hInstance)
{
	//if (!hInstance) hInstance = GetModuleHandle(NULL);
	if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_lpdi, NULL)))
		return -1;
	if (FAILED(m_lpdi->CreateDevice(GUID_SysKeyboard, &m_lpKeyboard, NULL)))
		return -1;

	if (FAILED(m_lpKeyboard->SetDataFormat(&c_dfDIKeyboard)))
		return -1;
	if (FAILED(m_lpKeyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
		return -1;
	if (FAILED(m_lpKeyboard->Acquire()))
		return -1;
	if (FAILED(m_lpKeyboard->GetDeviceState(sizeof(unsigned char[256]), (void*) m_KeyState)))
		return -1;

	return 0;
}

int Keyboard::Update(void)
{
	//Reacquire the keyboard on loss. Could potentially infinite loop
	HRESULT hr = m_lpKeyboard->Acquire();
	while( hr == DIERR_INPUTLOST )
		m_lpKeyboard->Acquire();

	//For efficiency, should swap between keystate buffer pointers instead of copying
	memcpy( m_OldKeyState, m_KeyState, 256);
	
	if (FAILED(m_lpKeyboard->GetDeviceState(sizeof(unsigned char[256]), (void*) m_KeyState)))
		return -1;

	//Events should be triggered here

	for (int i=0;i<256;i++)
	{
		if (KeyDown(m_OldKeyState, i) && !KeyDown(m_KeyState, i))
		{
			// a key was released
			printf("KeyReleasedEvent(%d)\n", i);
			//Event e = KeyReleasedEvent(i);
			//e.test();
		}
		if (!KeyDown(m_OldKeyState, i) && KeyDown(m_KeyState, i))
		{
			// a key was pressed
			printf("KeyPressedEvent(%d)\n", i);
			Event e = KeyPressedEvent(i);
			e.Send();
		}
	}

	return 0;
}

void Keyboard::Shutdown(void)
{
	if (NULL != m_lpKeyboard) {
		m_lpKeyboard->Unacquire();
		m_lpKeyboard = NULL;
	}
	if (NULL != m_lpdi) {
		m_lpdi->Release();
		m_lpKeyboard = NULL;
	}
}


bool Keyboard::isKeyPressed(int key)
{
	if (KeyDown(m_KeyState, key))
		return true;

	return false;
}
