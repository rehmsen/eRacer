/**
 * @file Mouse.cpp
 * @brief Implementation of the Mouse class
 *
 * @date 01.02.2010
 * @author: Ole Rehmsen
 */

#include "Mouse.h"

#include "Core/Event.h"

namespace Input {

Mouse::Mouse()	
: Device()
{ 

}

Mouse::~Mouse() { 
	Shutdown(); 
}



void Mouse::Init(HWND hWnd, IDirectInput8* directInput)
{
	Device::Init(hWnd, directInput);

	handleCreateDeviceReturnCode(directInput->CreateDevice(GUID_SysMouse, &m_pDevice, NULL));



	assert(SUCCEEDED(m_pDevice->SetDataFormat(&c_dfDIMouse2)));

	assert(SUCCEEDED(m_pDevice->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)));
	
	//if this fails, it will be acquired in the keyboard update function
	HRESULT hr = m_pDevice->Acquire();
	assert(DIERR_INVALIDPARAM != hr);
	assert(DIERR_NOTINITIALIZED != hr);
	if(SUCCEEDED(hr))
		m_pDevice->GetDeviceState(sizeof(DIMOUSESTATE2), (LPVOID)&currentState());
	else
		memset(&currentState(),0,sizeof(DIMOUSESTATE2));

	DIDEVCAPS mouseCapabilities; 
	mouseCapabilities.dwSize = sizeof(mouseCapabilities);
	assert(SUCCEEDED(m_pDevice->GetCapabilities(&mouseCapabilities)));

	if(!(mouseCapabilities.dwFlags & DIDC_ATTACHED))
		throw runtime_error("Mouse is not attached!");

	//m_dwNAxes = mouseCapabilities.dwAxes;
	//m_dwNButtons = mouseCapabilities.dwButtons;

	flipBuffers();
	initialized_=true;

}


void Mouse::Update(void)
{
	Device::Update();

	HRESULT hr = m_pDevice->GetDeviceState(sizeof(DIMOUSESTATE2), (LPVOID)&currentState());
	
	switch(hr){
		case DI_OK:
			break; //everything is fine
		case DIERR_INPUTLOST:  
		case DIERR_NOTACQUIRED:
			m_pDevice->Acquire(); //get the device back 
			return; //and try next time again
		case E_PENDING: //not ready yet, maybe next frame
			return;
		default:
			assert(hr != DIERR_NOTINITIALIZED);
			assert(hr != DIERR_INVALIDPARAM);
	}

	/* emit events */
	
	if(currentState().lX || currentState().lY)
		EVENT(MouseMovedEvent(currentState().lX,currentState().lY));

	if(currentState().lZ)
		EVENT(MouseWheelEvent(currentState().lY));

	for(int i=0; i<N_MOUSE_BUTTONS; i++)
	{
		if (Up(currentState().rgbButtons, i) && Down(oldState().rgbButtons, i))
			EVENT(MouseButtonReleasedEvent(i));
		else if (Down(currentState().rgbButtons, i) && Up(oldState().rgbButtons, i))
			EVENT(MouseButtonPressedEvent(i));
	}

	flipBuffers();
}


bool Mouse::isButtonDown(int button)
{
	//old states because buffers have been swapped already
	return Down(oldState().rgbButtons,button);
} 

}