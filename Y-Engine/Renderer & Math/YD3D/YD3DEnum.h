//-------------------------------------------------------------------------------
// CD3DEnum.h
// Yosbi Alves
// yosbito@gmail.com
// Copyright (c) 2011
//-----------------------------------------------------------------------
//Desc: Direct3d enum 
//--------------------------------------------------------------------------
#pragma once
#ifndef CD3DINITIALIZE_H
#define CD3DINITIALIZE_H


//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <d3dx9.h>
#include <vector>
#include "Y.h"
//
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "dxguid.lib")



//-----------------------------------------------------------------------------
// Name: vertexProsessigType
// Desc: Enumeration of all possible D3D vertex processing types
//-----------------------------------------------------------------------------
enum VERTEXPROCESSING_TYPE
{
	SOFTWARE_VP			= 0,		//Software vertex processing
	MIXED_VP			= 1,		//Mixed  vertex processing
	HARDWARE_VP			= 2,		//Hardware  vertex processing	
	PURE_HARDWARE_VP	= 3			//Pure hardware  vertex processing

};

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class YD3DEnumDeviceOptions;
class YD3DEnumDevice;
class YD3DEnumAdapter;
class YD3DSettings;

//-----------------------------------------------------------------------------
// Vector typedef for easy access
//-----------------------------------------------------------------------------
typedef std::vector<D3DMULTISAMPLE_TYPE>	VectorMSTtype;
typedef std::vector<D3DFORMAT>				VectorFormat;
typedef std::vector<ULONG>					VectorULONG;
typedef std::vector<VERTEXPROCESSING_TYPE>	VectorVPType;
typedef std::vector<D3DDISPLAYMODE>			VectorDisplayMode;
typedef std::vector<YD3DEnumDeviceOptions*>	VectorDeviceOptions;
typedef std::vector<YD3DEnumDevice*>		VectorDevice;
typedef std::vector<YD3DEnumAdapter*>		VectorAdapter;

//-----------------------------------------------------------------------------
// Name: CD3DEnumDeviceOptions (support)
// Desc: Stores the various device options aviable for any device
//-----------------------------------------------------------------------------
class YD3DEnumDeviceOptions
{
public:
	~YD3DEnumDeviceOptions();

	ULONG			AdapterOrdinal;
	D3DDEVTYPE		DeviceType;
	D3DCAPS9		Caps;
	D3DFORMAT		AdapterFormat;
	D3DFORMAT		BackBufferFormat;
	bool			Windowed;
	VectorMSTtype	MultiSampleTypes;
	VectorULONG		MultiSampleQuality;
	VectorFormat	DepthFormats;
	VectorVPType	VertexProcessingTypes;
	VectorULONG		PresentIntervals;
};

//-----------------------------------------------------------------------------
// Name: CD3DEnumDevice(support)
// Desc: Stores the various capabilities for an individual device type
//-----------------------------------------------------------------------------
class YD3DEnumDevice
{
public:
	~YD3DEnumDevice();

	D3DDEVTYPE			DeviceType;
	D3DCAPS9			Caps;
	VectorDeviceOptions Options;
};

//-----------------------------------------------------------------------------
// Name: CD3DEnumDevice(support)
// Desc: Stores the various capabilities for an individual device type
//-----------------------------------------------------------------------------
class YD3DEnumAdapter
{
public:
	~YD3DEnumAdapter();

	ULONG					Ordinal;
	D3DADAPTER_IDENTIFIER9  Identifier;
	VectorDisplayMode		Modes;
	VectorDevice			Devices;

};

//-----------------------------------------------------------------------------
// Name: CD3DSettings(support)
// Desc: allow to set up the various options that will be used; also used in CD3DSettingsDlg
//-----------------------------------------------------------------------------
class YD3DSettings
{
public:
	struct Settings
	{
		ULONG					AdapterOrdinal;
		D3DDISPLAYMODE			DisplayMode;
		D3DDEVTYPE              DeviceType;
        D3DFORMAT               BackBufferFormat;
        D3DFORMAT               DepthStencilFormat;
        D3DMULTISAMPLE_TYPE     MultisampleType;
        ULONG                   MultisampleQuality;
        VERTEXPROCESSING_TYPE   VertexProcessingType;
        ULONG                   PresentInterval;
	};
	bool	Windowed;
	Settings Windowed_Settings;
	Settings Fullscreen_Settings;

	Settings* GetSettings() { return (Windowed) ? &Windowed_Settings : &Fullscreen_Settings; };

};

//-----------------------------------------------------------------------------
// Name: CD3DInitialize
// Desc: Direct3D initialization class. Detects supported formats, modes n caps, 
//		n initializes the devices based on the choosen details
//-----------------------------------------------------------------------------
class YD3DInitialize
{
public:
			YD3DInitialize();
	virtual ~YD3DInitialize();

	//public functions 
	HRESULT					Enumerate (LPDIRECT3D9 pD3D);

	HRESULT					CreateDisplay (YD3DSettings &D3DSettings, ULONG Flags = 0, HWND hWnd = NULL, WNDPROC pWndProc = NULL, LPCTSTR Title = NULL,
										ULONG Width = CW_USEDEFAULT, ULONG Height = CW_USEDEFAULT, LPVOID lParam = NULL);

	HRESULT					ResetDisplay (LPDIRECT3DDEVICE9 pD3DDevice, YD3DSettings &D3DSettings, HWND = NULL);

	bool					FindBestWindowedMode (YD3DSettings &D3DSettings, bool bRequireHAL = false, bool bRequireREF = false);
	bool					FindBestFullscreenMode (YD3DSettings &D3DSettings,D3DDISPLAYMODE *pMatchMode = NULL, bool bRequireHAL = false, bool bRequireREF = false);

	D3DPRESENT_PARAMETERS	BuildPresentParameters (YD3DSettings &D3DSettings,ULONG Flags = 0);

	ULONG					GetAdapterCount () const { return m_vpAdapters.size();};
	const YD3DEnumAdapter	*GetAdapter (ULONG Index) { return (Index > GetAdapterCount()) ? NULL : m_vpAdapters[Index]; };
	HWND					GetHWND () { return m_hWnd; };
	const LPDIRECT3D9		GetDirect3D () { return m_pD3D; };
	LPDIRECT3DDEVICE9		GetDirect3DDevice();
private:
	//private functions
	HRESULT		EnumerateAdapters ();
	HRESULT		EnumerateDisplayModes (YD3DEnumAdapter *pAdapter);
	HRESULT		EnumerateDevices (YD3DEnumAdapter *pAdapter);
	HRESULT		EnumerateDeviceOptions (YD3DEnumDevice *pDevice, YD3DEnumAdapter *pAdapter);
	HRESULT		EnumerateDepthStencilFormats (YD3DEnumDeviceOptions *pDevicesOptions);
	HRESULT		EnumerateMultiSampleTypes  (YD3DEnumDeviceOptions *pDevicesOptions);
	HRESULT		EnumerateVertexProcessingTypes (YD3DEnumDeviceOptions *pDevicesOptions);
	HRESULT		EnumeratePresentIntervals (YD3DEnumDeviceOptions *pDevicesOptions);

	//Private virtual functions
	virtual bool ValidateDisplayMode (const D3DDISPLAYMODE &Mode) {return true;};
	virtual bool ValidateDevice	(const D3DDEVTYPE &Type, const D3DCAPS9 &Caps) {return true;};
	virtual bool ValidateDeviceOptions (const D3DFORMAT &BackBufferFormat, bool IsWindowed) {return true;};
	virtual bool ValidateDepthStencilFormat (const D3DFORMAT &DepthStencilFotmat) {return true;};
	virtual bool ValidateMultiSampleType (const D3DMULTISAMPLE_TYPE &Type) {return true;};
	virtual bool ValidateVertexProcessingType (const VERTEXPROCESSING_TYPE &Type) {return true;};
	virtual bool ValidatePresentInterval (const ULONG &Interval) {return true;};

	//private variables
	LPDIRECT3D9			m_pD3D;			//Primary Direct3D Object
	LPDIRECT3DDEVICE9	m_pD3DDevice;	//Created Direct3D Device
	HWND				m_hWnd;			//Created window handle
	VectorAdapter		m_vpAdapters;	//Enumetated Adapters
};

#endif //CD3DINITIALIZE_H

