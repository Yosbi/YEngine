//-------------------------------------------------------------------------------
// CD3DEnum.cpp
// Yosbi Alves
// yosbito@gmail.com
// Copyright (c) 2011
//-----------------------------------------------------------------------
//Desc: Direct3d enum and initialization
//--------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// General Diagram:
//
// Adapters (Array)
//    |
//    |---- Adapter Ordinal
//    |
//    |---- Adapter Identifier
//    |
//    |---- Display Modes (Array)
//    |           |
//    |           |------- Resolution
//    |           |
//    |           |------- Format / ColorDepth
//    |           |
//    |           |------- Refresh Rate
//    |           
//    |           
//    |---- Device Types (Array)
//                |
//                |------- Device Type - HAL / SW / REF
//                |
//                |------- Device Capabilities
//                |
//                |------- Device Options (Array)
//                               |
//                               |------- Adapter Format
//                               |
//                               |------- Back Buffer Format
//                               |
//                               |------- Is Windowed Mode
//                               |
//                               |------- Depth / Stencil Formats (Array)
//                               |
//                               |------- Multi-Sample Types (Array)   <--\
//                               |                                         > Linked
//                               |------- Multi-Sample Quality (Array) <--/
//                               |
//                               |------- Vertex Processing Types (Array)
//                               |
//                               |------- Presentation Intervals (Array)
//           
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "YD3D.h"

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
const ULONG					ValidAdapterFormatCount = 3;
const D3DFORMAT				ValidAdapterFormats[3]	= { D3DFMT_X8R8G8B8, D3DFMT_X1R5G5B5, D3DFMT_R5G6B5};

const ULONG					BackBufferFormatCount	= 11;
const D3DFORMAT				BackBufferFormats[11]	= { D3DFMT_R8G8B8, D3DFMT_A8R8G8B8, D3DFMT_X8R8G8B8,
													D3DFMT_R5G6B5, D3DFMT_A1R5G5B5, D3DFMT_X1R5G5B5,
													D3DFMT_R3G3B2, D3DFMT_A8R3G3B2, D3DFMT_X4R4G4B4,
													D3DFMT_A4R4G4B4,  D3DFMT_A2B10G10R10 };

const ULONG					DeviceTypeCount			= 3;
const D3DDEVTYPE			DeviceTypes[3]			= { D3DDEVTYPE_HAL, D3DDEVTYPE_SW, D3DDEVTYPE_REF };

const ULONG					MultiSampleTypeCount	= 17;
const D3DMULTISAMPLE_TYPE   MultiSampleTypes[17]    = { D3DMULTISAMPLE_NONE      , D3DMULTISAMPLE_NONMASKABLE,
                                                        D3DMULTISAMPLE_2_SAMPLES , D3DMULTISAMPLE_3_SAMPLES,
                                                        D3DMULTISAMPLE_4_SAMPLES , D3DMULTISAMPLE_5_SAMPLES,
                                                        D3DMULTISAMPLE_6_SAMPLES , D3DMULTISAMPLE_7_SAMPLES,
                                                        D3DMULTISAMPLE_8_SAMPLES , D3DMULTISAMPLE_9_SAMPLES,
                                                        D3DMULTISAMPLE_10_SAMPLES, D3DMULTISAMPLE_11_SAMPLES,
                                                        D3DMULTISAMPLE_12_SAMPLES, D3DMULTISAMPLE_13_SAMPLES,
                                                        D3DMULTISAMPLE_14_SAMPLES, D3DMULTISAMPLE_15_SAMPLES,
                                                        D3DMULTISAMPLE_16_SAMPLES };

const ULONG					DepthStencilFormatCount = 6;
const D3DFORMAT				DepthStencilFormats[6]  = { D3DFMT_D32, D3DFMT_D24X4S4, D3DFMT_D24X8,
														D3DFMT_D24S8, D3DFMT_D16, D3DFMT_D15S1 };

const ULONG					PresentIntervalCount	= 6;
const ULONG                 PresentIntervals[6]     = { D3DPRESENT_INTERVAL_IMMEDIATE, D3DPRESENT_INTERVAL_DEFAULT,
                                                        D3DPRESENT_INTERVAL_ONE, D3DPRESENT_INTERVAL_TWO,
                                                        D3DPRESENT_INTERVAL_THREE, D3DPRESENT_INTERVAL_FOUR };


//-----------------------------------------------------------------------------
// Name: ~CD3DEnumAdapter()
// Desc: CD3DEnumAdapter destructor
//-----------------------------------------------------------------------------
YD3DEnumAdapter::~YD3DEnumAdapter()
{
	for ( ULONG i = 0; i < Devices.size(); i++)
	{
		//release device
		if (Devices[i]) delete Devices [i];
	}

	//Clear vectors
	Modes.clear();
	Devices.clear();
	
}

//-----------------------------------------------------------------------------
// Name: ~CD3DEnumDevice()
// Desc: CD3DEnumDevice destructor
//-----------------------------------------------------------------------------
YD3DEnumDevice::~YD3DEnumDevice()
{
	for ( ULONG i = 0; i < Options.size(); i++)
	{
		//release device
		if (Options[i]) delete Options [i];
	}

	//Clear vector
	Options.clear();
	
}

//-----------------------------------------------------------------------------
// Name: ~CD3DEnumDeviceOptions()
// Desc: CD3DEnumDeviceOptions destructor
//-----------------------------------------------------------------------------
YD3DEnumDeviceOptions::~YD3DEnumDeviceOptions()
{
	//Clear vectors
	DepthFormats.clear();
	MultiSampleTypes.clear();
	MultiSampleQuality.clear();
	PresentIntervals.clear();
	VertexProcessingTypes.clear();	
}

//-----------------------------------------------------------------------------
// Name: CD3DInitialize()
// Desc: CD3DInitialize constructor
//-----------------------------------------------------------------------------
YD3DInitialize::YD3DInitialize()
{
	//clear all required values
	m_pD3D			= NULL;
	m_pD3DDevice	= NULL;
}

//-----------------------------------------------------------------------------
// Name: ~CD3DInitialize()
// Desc: CD3DInitialize destructor
//-----------------------------------------------------------------------------
YD3DInitialize::~YD3DInitialize()
{
	//clean up D3D objects
	if (m_pD3D) m_pD3D->Release();
	m_pD3D = NULL;

	if (m_pD3DDevice) m_pD3DDevice->Release();
	m_pD3DDevice = NULL;

	//release all enumerated adapters
	for (ULONG i = 0; i < m_vpAdapters.size(); i++)
	{//release adapter
		if (m_vpAdapters[i]) delete m_vpAdapters[i];
	}

	//Clear vectors
	m_vpAdapters.clear();

}

//-----------------------------------------------------------------------------
// Name: Enumerate
// Desc: called first to enumerate all available devices, adapters, modes and formats
//		prior to initialization
//-----------------------------------------------------------------------------
HRESULT	YD3DInitialize::Enumerate(LPDIRECT3D9 pD3D)
{
	HRESULT	hRet;

	//store the d3d object
	m_pD3D = pD3D;
	if (!m_pD3D) return E_FAIL;

	//AddRef on the D3D9 Object to be able to autoclean
	m_pD3D->AddRef();

	//enumetate the adapters
	if(FAILED(hRet = EnumerateAdapters())) return hRet;

	//success!
	return S_OK;

}

//-----------------------------------------------------------------------------
// Name: EnumerateAdapters
// Desc: enumerates the individual adapters contained in the usr machine
//-----------------------------------------------------------------------------
HRESULT	YD3DInitialize::EnumerateAdapters()
{
	HRESULT	hRet;

	//STORE THE NUMBER OF AVAILABLE ADAPTERS
	ULONG nAdapterCount = m_pD3D->GetAdapterCount();

	//loop through each adapter
	for(ULONG i = 0; i < nAdapterCount; i++)
	{
		YD3DEnumAdapter *pAdapter = new YD3DEnumAdapter;
		if (!pAdapter) return E_OUTOFMEMORY;

		//Store adapter identifier
		pAdapter->Ordinal = i;

		//retrieve adapter identifier
		m_pD3D->GetAdapterIdentifier(i, 0, &pAdapter->Identifier);

		//enumerate all display modes for this adapter
		if(FAILED( hRet = EnumerateDisplayModes( pAdapter )) ||	FAILED(hRet = EnumerateDevices(pAdapter)))
		{
			delete pAdapter;
            if ( hRet == E_ABORT ) continue; else return hRet;
		}
		
		//add this adapter to the list
		try {m_vpAdapters.push_back( pAdapter ); } catch (...)
		{
			delete pAdapter;
			return E_OUTOFMEMORY;
		}
	}//next adapter

	//Success!
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: EnumerateDisplayModes () private
// Desc: enumerates all display modes for the adapter specified
//-----------------------------------------------------------------------------
HRESULT YD3DInitialize::EnumerateDisplayModes( YD3DEnumAdapter *pAdapter)
{
	HRESULT			hRet;
	ULONG			i, j;
	D3DDISPLAYMODE  Mode;

	//loop through each valid adapter format 
	for (i = 0; i < ValidAdapterFormatCount; i++)
	{
		//retrieve the number of valid modes for this format e.g 800x600
		ULONG nModeCount =m_pD3D->GetAdapterModeCount( pAdapter->Ordinal, ValidAdapterFormats[i] );
		if(nModeCount == 0) continue;

		//loop throught each display mode for this pixel format
		for (j = 0; j < nModeCount; j++){
			//Retrieve the display mode
			hRet = m_pD3D->EnumAdapterModes(pAdapter->Ordinal, ValidAdapterFormats[i], j, &Mode);
			if(FAILED(hRet)) return hRet;

			//Is supported by the user??
			if (!ValidateDisplayMode( Mode )) continue;

			//add this mode to the adapter
			try {pAdapter->Modes.push_back(Mode);} catch(...)
			{
				return E_OUTOFMEMORY;
			}

		}// next adapter mode
	}//Next adapter format

	//success?
	return (pAdapter->Modes.size() == 0) ? E_ABORT : S_OK;
}

//-----------------------------------------------------------------------------
// Name: EnumerateDevices () private
// Desc: enumerates all the valid device types for the especified adapter
//-----------------------------------------------------------------------------
HRESULT YD3DInitialize::EnumerateDevices (YD3DEnumAdapter *pAdapter)
{
	ULONG	 i;
	HRESULT  hRet;
	D3DCAPS9 Caps;

	//loop throught each device type (HAL, SW, REF)
	for (i = 0; i < DeviceTypeCount; i++)
	{
		//retrieve device caps (on failure, device not generally available)
		if (FAILED(m_pD3D->GetDeviceCaps( pAdapter->Ordinal, DeviceTypes[i], &Caps ) ) ) continue;

		//supported by the user?
		if (!ValidateDevice (DeviceTypes[i], Caps) ) continue;

		//allocate new device
		YD3DEnumDevice *pDevice = new YD3DEnumDevice;
		if (!pDevice ) return E_OUTOFMEMORY;

		//store device information
		pDevice->DeviceType = DeviceTypes[i];
		pDevice->Caps		= Caps;

		//retrieve various init options for this device
		if (FAILED( hRet = EnumerateDeviceOptions (pDevice, pAdapter) ) )
		{
			delete pDevice;
			if (hRet == E_ABORT) continue; else return hRet;
		}

		//add it to adapter list
		try { pAdapter->Devices.push_back(pDevice); } catch( ... )
		{
			delete pDevice;
			return E_OUTOFMEMORY;
		}
	}//next device type

	//SUCCESS?
	return (pAdapter->Devices.size() == 0) ? E_ABORT : S_OK;
}

//-----------------------------------------------------------------------------
// Name: EnumerateDeviceOptions () private
// Desc: enumerates the various initialization options available for the device
//		especified, such as backbuffer formats, etc
//-----------------------------------------------------------------------------
HRESULT YD3DInitialize::EnumerateDeviceOptions(YD3DEnumDevice *pDevice, YD3DEnumAdapter *pAdapter)
{
	HRESULT		hRet;
	ULONG		i, j, k;
	bool		Windowed;
	D3DFORMAT	AdapterFormats[ ValidAdapterFormatCount ];
	ULONG		AdapterFormatCount = 0;
	D3DFORMAT	AdapterFormat, BackBufferFormat;

	//build a list of all the formats used by the adapter
	for(i = 0; i < pAdapter->Modes.size(); i++)
	{
		//already added to the list?
		for(j = 0; j < AdapterFormatCount; j++)
			if(pAdapter->Modes[i].Format == AdapterFormats[j] ) break;

		//addit to the list if not exist
		if(j == AdapterFormatCount)
			AdapterFormats[ AdapterFormatCount++ ] = pAdapter->Modes[i].Format;
	}//next adapter format

	//loop through each adapter format available
	for( i = 0; i < AdapterFormatCount; i++)
	{
		//store adapter format
		AdapterFormat = AdapterFormats[i];

		//loop through all valid backbuffer formats
		for(j = 0; j < BackBufferFormatCount; j++)
		{
			//store backbuffer format
			BackBufferFormat = BackBufferFormats[j];

			//test windowed/full screen modes
			for( k = 0; k < 2; k++)
			{
				//select windowed/full screen
				Windowed = (k == 0) ? true : false;

				//skip if this is not a valid device (check with this function)
				if(FAILED(m_pD3D->CheckDeviceType(pAdapter->Ordinal, pDevice->DeviceType, AdapterFormat, BackBufferFormat, Windowed))) continue;

				//allocate a new device option set
				YD3DEnumDeviceOptions *pDeviceOptions = new YD3DEnumDeviceOptions;
				if(!pDeviceOptions) return E_OUTOFMEMORY;

				//store device option details
				pDeviceOptions->AdapterOrdinal		= pAdapter->Ordinal;
				pDeviceOptions->DeviceType			= pDevice->DeviceType;
				pDeviceOptions->AdapterFormat		= AdapterFormat;
				pDeviceOptions->BackBufferFormat	= BackBufferFormat;
				pDeviceOptions->Caps				= pDevice->Caps;
				pDeviceOptions->Windowed			= Windowed;

				// Is this option supported by the user?
				if ( !ValidateDeviceOptions(BackBufferFormat, Windowed) )
                {
                    delete pDeviceOptions;
                    continue;
                
                }

				//Enumerate the various options components
				if(FAILED(hRet = EnumerateDepthStencilFormats( pDeviceOptions )) || 
					FAILED(hRet = EnumerateMultiSampleTypes( pDeviceOptions )) || 
					FAILED(hRet = EnumerateVertexProcessingTypes( pDeviceOptions )) || 
					FAILED(hRet = EnumeratePresentIntervals ( pDeviceOptions )) )
				{
					// Release our invalid options
                    delete pDeviceOptions;

                    // If returned anything other than abort, this is fatal
                    if ( hRet == E_ABORT ) continue; else return hRet;
				}
					 
				 // Add this to our device
                try { pDevice->Options.push_back( pDeviceOptions ); } catch ( ... )
                {
                    delete pDeviceOptions;
                    return E_OUTOFMEMORY;

                }

			}//next windowed/full screen

		}//next backbuffer format

	}//next adapter format
	//succes?
	return (pDevice->Options.size() == 0) ? E_ABORT : S_OK;
}

//-----------------------------------------------------------------------------
// Name: EnumerateDepthStencilFormats () Private
// Desc: Enumerates all the valid depth / stencil formats for this device set.
//-----------------------------------------------------------------------------
HRESULT YD3DInitialize::EnumerateDepthStencilFormats( YD3DEnumDeviceOptions * pDeviceOptions )
{
    ULONG i;

    try
    {
        //loop through each depth stencil format
        for ( i = 0; i < DepthStencilFormatCount; i++ )
        {
            //test to see if this is a valid depth surface format
            if ( SUCCEEDED( m_pD3D->CheckDeviceFormat( pDeviceOptions->AdapterOrdinal, pDeviceOptions->DeviceType, 
                                                       pDeviceOptions->AdapterFormat, D3DUSAGE_DEPTHSTENCIL,
                                                       D3DRTYPE_SURFACE, DepthStencilFormats[ i ] ) ) )
            {
                //test to see if this is a valid depth / stencil format for this mode
                if ( SUCCEEDED( m_pD3D->CheckDepthStencilMatch( pDeviceOptions->AdapterOrdinal, pDeviceOptions->DeviceType, 
                                                                pDeviceOptions->AdapterFormat, pDeviceOptions->BackBufferFormat,
                                                                DepthStencilFormats[ i ] ) ) )
                {

                    // is this supported by the user ?
                    if ( ValidateDepthStencilFormat( DepthStencilFormats[ i ] ) )
                    {
                        //add this as a valid depthstencil format
                        pDeviceOptions->DepthFormats.push_back( DepthStencilFormats[ i ] );

                    } 
                } //end if valid for this mode

            } //end if valid DepthStencil format

        } //next DepthStencil Format

    } //end Try Block

    catch ( ... ) { return E_OUTOFMEMORY; }

    // Success ?
    return ( pDeviceOptions->DepthFormats.size() == 0 ) ? E_ABORT : S_OK;
}

//-----------------------------------------------------------------------------
// Name: EnumerateMultiSampleTypes () Private
// Desc: Enumerates multi-sample types available for this device set.
//-----------------------------------------------------------------------------
HRESULT YD3DInitialize::EnumerateMultiSampleTypes( YD3DEnumDeviceOptions * pDeviceOptions )
{
    ULONG i, Quality;

    try
    {
        //loop through each multi-sample type
        for ( i = 0; i < MultiSampleTypeCount; i++ )
        {
            //check if this multi-sample type is supported
            if ( SUCCEEDED( m_pD3D->CheckDeviceMultiSampleType( pDeviceOptions->AdapterOrdinal, pDeviceOptions->DeviceType,
                                                                pDeviceOptions->BackBufferFormat, pDeviceOptions->Windowed,
                                                                MultiSampleTypes[ i ], &Quality ) ) )
            {
                //is this supported by the user ?
                if ( ValidateMultiSampleType( MultiSampleTypes[ i ] ) )
                {
                    //supported, add these to our list
                    pDeviceOptions->MultiSampleTypes.push_back( MultiSampleTypes[i] );
                    pDeviceOptions->MultiSampleQuality.push_back( Quality );

                } 

            } //end if valid for this mode

        } //next Sample Type

    } //end try Block

    catch ( ... ) { return E_OUTOFMEMORY; }

    // Success?
    return ( pDeviceOptions->MultiSampleTypes.size() == 0 ) ? E_ABORT : S_OK;
}

//-----------------------------------------------------------------------------
// Name: EnumerateVertexProcessingTypes () Private
// Desc: Enumerates all the types of vertex processing available.
//-----------------------------------------------------------------------------
HRESULT YD3DInitialize::EnumerateVertexProcessingTypes( YD3DEnumDeviceOptions * pDeviceOptions )
{
    try
    {
        //if the device supports Hardware T&L
        if ( pDeviceOptions->Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
        {
            //if the device can be created as 'Pure'
            if ( pDeviceOptions->Caps.DevCaps & D3DDEVCAPS_PUREDEVICE )
            {
                //supports Pure hardware device ?
                if ( ValidateVertexProcessingType( PURE_HARDWARE_VP ) )
                    pDeviceOptions->VertexProcessingTypes.push_back( PURE_HARDWARE_VP );

            } 

            // supports hardware T&L and Mixed by definitiion ?
            if ( ValidateVertexProcessingType( HARDWARE_VP ) )
                pDeviceOptions->VertexProcessingTypes.push_back( HARDWARE_VP );

            if ( ValidateVertexProcessingType( MIXED_VP ) )
                pDeviceOptions->VertexProcessingTypes.push_back( MIXED_VP );

        } //end if HW T&L

        //always supports software
        if ( ValidateVertexProcessingType( SOFTWARE_VP ) )
            pDeviceOptions->VertexProcessingTypes.push_back( SOFTWARE_VP );

    } //end try Block

    catch ( ... ) { return E_OUTOFMEMORY; }

    //Success?
    return ( pDeviceOptions->VertexProcessingTypes.size() == 0 ) ? E_ABORT : S_OK;
}

//-----------------------------------------------------------------------------
// Name: EnumeratePresentIntervals () Private
// Desc: Enumerates all the valid present intervals available for this set.
//-----------------------------------------------------------------------------
HRESULT YD3DInitialize::EnumeratePresentIntervals( YD3DEnumDeviceOptions * pDeviceOptions )
{
    ULONG i, Interval;

    try
    {
        //loop through each presentation interval
        for ( i = 0; i < PresentIntervalCount; i++ )
        {
            //store for easy access
            Interval = PresentIntervals[i];

            //if device is windowed, skip anything above ONE
            if ( pDeviceOptions->Windowed )
            {
                if ( Interval == D3DPRESENT_INTERVAL_TWO   ||
                     Interval == D3DPRESENT_INTERVAL_THREE ||
                     Interval == D3DPRESENT_INTERVAL_FOUR ) continue;

            }

            // DEFAULT is always available, others must be tested
            if ( Interval == D3DPRESENT_INTERVAL_DEFAULT )
            {
                pDeviceOptions->PresentIntervals.push_back( Interval );
                continue;

            } 

            //Supported by the device?
            if ( pDeviceOptions->Caps.PresentationIntervals & Interval )
            {
                if ( ValidatePresentInterval( Interval ) )
                    pDeviceOptions->PresentIntervals.push_back( Interval );

            } 

        } //next Interval Type

    } //end try Block

    catch ( ... ) { return E_OUTOFMEMORY; }

    // Success?
    return ( pDeviceOptions->PresentIntervals.size() == 0 ) ? E_ABORT : S_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateDisplay ()
// Desc: Creates the display devices ready for rendering.
//-----------------------------------------------------------------------------
HRESULT YD3DInitialize::CreateDisplay( YD3DSettings& D3DSettings, ULONG Flags, HWND hWnd, WNDPROC pWndProc, LPCTSTR Title, ULONG Width, ULONG Height, LPVOID lParam )
{
    ULONG CreateFlags = 0;
    YD3DSettings::Settings *pSettings = D3DSettings.GetSettings();
    
    if ( !hWnd )
    {
        //register the new windows window class.
        WNDCLASS			wc;	
	    wc.style			= CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW;
	    wc.lpfnWndProc		= pWndProc;
	    wc.cbClsExtra		= 0;
	    wc.cbWndExtra		= 0;
	    wc.hInstance		= (HINSTANCE)GetModuleHandle(NULL);
        wc.hIcon			= NULL;
	    wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	    wc.hbrBackground	= (HBRUSH )GetStockObject(WHITE_BRUSH);
	    wc.lpszMenuName		= NULL;
	    wc.lpszClassName	= Title;
	    RegisterClass(&wc);

        ULONG Left  = CW_USEDEFAULT, Top = CW_USEDEFAULT;
        ULONG Style = WS_OVERLAPPEDWINDOW;

        //create the rendering window
        if ( !D3DSettings.Windowed )
        {
            Left   = 0; Top = 0;
            Width  = pSettings->DisplayMode.Width;
            Height = pSettings->DisplayMode.Height;
            Style  = WS_VISIBLE | WS_POPUP;
	        
        } 

        //create the window
        m_hWnd = CreateWindow( Title, Title, Style, Left, Top, Width, Height, NULL, NULL, wc.hInstance, lParam );

        //on error
        if (!m_hWnd) return E_FAIL;

    } //end if no Window Passed
    else
    {
        //store HWND
        m_hWnd = hWnd;
        
        //setup styles based on windowed / fullscreen mode
        if ( !D3DSettings.Windowed )
        {
			//set null menu
            SetMenu( m_hWnd, NULL );
            SetWindowLong( m_hWnd, GWL_STYLE, WS_VISIBLE | WS_POPUP );
            SetWindowPos( m_hWnd,NULL, 0, 0, pSettings->DisplayMode.Width, pSettings->DisplayMode.Height, SWP_NOZORDER );
        
        } //end if Fullscreen
        else
        {
            RECT rc;

            //get the windows client rectangle
            GetWindowRect( hWnd, &rc );

            //setup the window properties
            SetWindowLong( m_hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW );
            SetWindowPos( hWnd, HWND_NOTOPMOST, rc.left, rc.top, (rc.right - rc.left), (rc.bottom - rc.top), SWP_NOACTIVATE | SWP_SHOWWINDOW );

        } //end if Windowed
    
    } //end if window passed

    //build our present parameters
    D3DPRESENT_PARAMETERS d3dpp = BuildPresentParameters( D3DSettings );
    
    //build our creation flags
    if ( pSettings->VertexProcessingType == PURE_HARDWARE_VP )
        CreateFlags = D3DCREATE_PUREDEVICE | D3DCREATE_HARDWARE_VERTEXPROCESSING;
    else if ( pSettings->VertexProcessingType == HARDWARE_VP )
        CreateFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
    else if ( pSettings->VertexProcessingType == MIXED_VP )
        CreateFlags = D3DCREATE_MIXED_VERTEXPROCESSING;
    else if ( pSettings->VertexProcessingType == SOFTWARE_VP )
        CreateFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

    //create the device
    m_pD3DDevice = NULL;
    HRESULT hRet = m_pD3D->CreateDevice( pSettings->AdapterOrdinal, pSettings->DeviceType,
                                         m_hWnd, CreateFlags, &d3dpp, &m_pD3DDevice );
    //did the creation fail ?
    if ( FAILED( hRet ) ) 
    {
        if ( m_pD3DDevice ) m_pD3DDevice->Release();
        m_pD3DDevice = NULL;
        return hRet;
    } //end if failed

    //success
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: BuildPresentParameters ()
// Desc: Builds a set of present parameters from the Settings passed.
//-----------------------------------------------------------------------------
D3DPRESENT_PARAMETERS YD3DInitialize::BuildPresentParameters( YD3DSettings& D3DSettings, ULONG Flags )
{
    D3DPRESENT_PARAMETERS   d3dpp;
    YD3DSettings::Settings *pSettings = D3DSettings.GetSettings();
    
    ZeroMemory ( &d3dpp, sizeof(D3DPRESENT_PARAMETERS) );

    //fill out our common present parameters

	//MUST DO A FUNCTION TO GET THE MAX BACKBUFFER COUNT !!!!IS IMPORTANT YOSBI!!!!
    d3dpp.BackBufferCount           = 3;
    d3dpp.BackBufferFormat          = pSettings->BackBufferFormat;
    d3dpp.Windowed                  = D3DSettings.Windowed;
    d3dpp.MultiSampleType           = pSettings->MultisampleType;
    d3dpp.MultiSampleQuality        = pSettings->MultisampleQuality;
    d3dpp.EnableAutoDepthStencil    = TRUE;
    d3dpp.AutoDepthStencilFormat    = pSettings->DepthStencilFormat;
    d3dpp.PresentationInterval      = pSettings->PresentInterval;
    d3dpp.Flags                     = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
    d3dpp.SwapEffect                = D3DSWAPEFFECT_DISCARD;

    // Is this fullscreen ?
    if ( !d3dpp.Windowed )
    {
        d3dpp.FullScreen_RefreshRateInHz = pSettings->DisplayMode.RefreshRate;
        d3dpp.BackBufferWidth            = pSettings->DisplayMode.Width;
        d3dpp.BackBufferHeight           = pSettings->DisplayMode.Height;

    } // End if fullscreen
    
    // Success
    return d3dpp;
}

//-----------------------------------------------------------------------------
// Name: ResetDisplay ()
// Desc: Reset the display device, and optionally the window etc.
//-----------------------------------------------------------------------------
HRESULT YD3DInitialize::ResetDisplay( LPDIRECT3DDEVICE9 pD3DDevice, YD3DSettings& D3DSettings, HWND hWnd )
{   
    D3DPRESENT_PARAMETERS d3dpp = BuildPresentParameters( D3DSettings );
    YD3DSettings::Settings *pSettings = D3DSettings.GetSettings();

    if ( hWnd )
    {
        //Setup styles based on windowed / fullscreen mode
        if ( !D3DSettings.Windowed )
        {
            SetMenu( hWnd, NULL );
            SetWindowLong( hWnd, GWL_STYLE, WS_VISIBLE | WS_POPUP );
            SetWindowPos( hWnd, NULL, 0, 0, pSettings->DisplayMode.Width, pSettings->DisplayMode.Height, SWP_NOZORDER );
        
        } //end if Fullscreen
        else
        {
            SetWindowLong( hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW );
            SetWindowPos( hWnd, HWND_NOTOPMOST, 50, 50, 640, 480, SWP_NOACTIVATE | SWP_SHOWWINDOW );

        } //end if Windowed

    } //end if

    //reset the device
    HRESULT hRet = pD3DDevice->Reset( &d3dpp );
    if ( FAILED( hRet ) ) return hRet;

    // Success
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: GetDirect3DDevice ()
// Desc: Return a copy of the Direct3DDevice pointer (adds ref on the interface)
//-----------------------------------------------------------------------------
LPDIRECT3DDEVICE9 YD3DInitialize::GetDirect3DDevice( )
{ 
    //if not created
    if ( !m_pD3DDevice ) return NULL;

    //AddRef on the device
    m_pD3DDevice->AddRef();

    //duplicated pointer
    return m_pD3DDevice; 
}

//-----------------------------------------------------------------------------
// Name: FindBestWindowedMode ()
// Desc: Find the best windowed mode, and fill out our D3DSettings structure.
//-----------------------------------------------------------------------------
bool YD3DInitialize::FindBestWindowedMode( YD3DSettings & D3DSettings, bool bRequireHAL, bool bRequireREF )
{
    ULONG                    i, j, k;
    D3DDISPLAYMODE           DisplayMode;
    YD3DEnumAdapter         *pBestAdapter = NULL;
    YD3DEnumDevice          *pBestDevice  = NULL;
    YD3DEnumDeviceOptions   *pBestOptions = NULL;
    YD3DSettings::Settings  *pSettings    = NULL;

    //retrieve the primary adapters display mode.
    m_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &DisplayMode);

    //loop through each adapter
    for( i = 0; i < GetAdapterCount(); i++ )
    {
        YD3DEnumAdapter * pAdapter = m_vpAdapters[ i ];
        
        //loop through each device
        for( j = 0; j < pAdapter->Devices.size(); j++ )
        {
            YD3DEnumDevice * pDevice = pAdapter->Devices[ j ];

            //skip if this is not of the required type
            if ( bRequireHAL && pDevice->DeviceType != D3DDEVTYPE_HAL ) continue;
            if ( bRequireREF && pDevice->DeviceType != D3DDEVTYPE_REF ) continue;
            
            //loop through each option set
            for ( k = 0; k < pDevice->Options.size(); k++ )
            {
                YD3DEnumDeviceOptions * pOptions = pDevice->Options[ k ];

                //determine if back buffer format matches adapter 
                bool MatchedBB = false;
				if (pOptions->BackBufferFormat == D3DFMT_A8R8G8B8)
					MatchedBB = true;
				else if (pOptions->BackBufferFormat == pOptions->AdapterFormat )
					MatchedBB = true;

                // skip if this is not windowed, and formats dont match
                if (!pOptions->Windowed) continue;
                if ( pOptions->AdapterFormat != DisplayMode.Format) continue;

                //if havent found a compatible option set yet, or if this set
                // is better (because its HAL / formats match better) then save it.
                if( pBestOptions == NULL || (pOptions->DeviceType == D3DDEVTYPE_HAL && MatchedBB ) ||
                    (pBestOptions->DeviceType != D3DDEVTYPE_HAL && pOptions->DeviceType == D3DDEVTYPE_HAL) )
                {
                    //store best
                    pBestAdapter = pAdapter;
                    pBestDevice  = pDevice;
                    pBestOptions = pOptions;
                    
                    if ( pOptions->DeviceType == D3DDEVTYPE_HAL && MatchedBB )
                    {
                        //this windowed device option looks great, take it
                        goto EndWindowedDeviceOptionSearch;
                    }
                    
                } 
            
            } //next Option Set
        
        } //next Device Type
    
    } //next Adapter

EndWindowedDeviceOptionSearch:
    
    if ( pBestOptions == NULL ) return false;

	bool found = false;
	// Depth stencil format
	for (i = 0; i < pBestOptions->DepthFormats.size(); i++)
	{
		if (pBestOptions->DepthFormats[i] == D3DFMT_D24S8)
		{
			found = true;
			break;
		}
	}

	int ind = 0;
	if (found)
		ind = i;


    //fill out passed settings details
    D3DSettings.Windowed               = true;
    pSettings                          = D3DSettings.GetSettings();
    pSettings->AdapterOrdinal          = pBestOptions->AdapterOrdinal;
    pSettings->DisplayMode             = DisplayMode;
    pSettings->DeviceType              = pBestOptions->DeviceType;
    pSettings->BackBufferFormat        = pBestOptions->BackBufferFormat;
    pSettings->DepthStencilFormat      = pBestOptions->DepthFormats[ ind ];
    pSettings->MultisampleType         = pBestOptions->MultiSampleTypes[ 0 ];
    pSettings->MultisampleQuality      = 0;
    pSettings->VertexProcessingType    = pBestOptions->VertexProcessingTypes[ 0 ];
    pSettings->PresentInterval         = pBestOptions->PresentIntervals[ 0 ];

    //found a mode
    return true;
}

//-----------------------------------------------------------------------------
// Name: FindBestFullscreenMode ()
// Desc: Find the best fullscreen mode, and fill out D3DSettings structure.
//-----------------------------------------------------------------------------
bool YD3DInitialize::FindBestFullscreenMode( YD3DSettings & D3DSettings, D3DDISPLAYMODE * pMatchMode, bool bRequireHAL, bool bRequireREF )
{
    //For fullscreen, default to first HAL option that supports the current desktop 
    //display mode, or any display mode if HAL is not compatible with the desktop mode, or 
    //non-HAL if no HAL is available
    
    ULONG                    i, j, k;
    D3DDISPLAYMODE           AdapterDisplayMode;
    D3DDISPLAYMODE           BestAdapterDisplayMode;
    D3DDISPLAYMODE           BestDisplayMode;
    YD3DEnumAdapter         *pBestAdapter = NULL;
    YD3DEnumDevice          *pBestDevice  = NULL;
    YD3DEnumDeviceOptions   *pBestOptions = NULL;
    YD3DSettings::Settings  *pSettings    = NULL;
    
    BestAdapterDisplayMode.Width		= 0;
    BestAdapterDisplayMode.Height		= 0;
    BestAdapterDisplayMode.Format		= D3DFMT_UNKNOWN;
    BestAdapterDisplayMode.RefreshRate	= 0;

    //loop through each adapter
    for( i = 0; i < GetAdapterCount(); i++ )
    {
        YD3DEnumAdapter * pAdapter = m_vpAdapters[ i ];
        
        //retrieve the desktop display mode
        m_pD3D->GetAdapterDisplayMode( pAdapter->Ordinal, &AdapterDisplayMode );

        //if any settings were passed, overwrite to test for matches
        if ( pMatchMode ) 
        {
            if ( pMatchMode->Width  != 0 ) AdapterDisplayMode.Width  = pMatchMode->Width;
            if ( pMatchMode->Height != 0 ) AdapterDisplayMode.Height = pMatchMode->Height;
            if ( pMatchMode->Format != D3DFMT_UNKNOWN ) AdapterDisplayMode.Format = pMatchMode->Format;
            if ( pMatchMode->RefreshRate != 0 ) AdapterDisplayMode.RefreshRate = pMatchMode->RefreshRate;

        } 

        //loop through each device
        for( j = 0; j < pAdapter->Devices.size(); j++ )
        {
            YD3DEnumDevice * pDevice = pAdapter->Devices[ j ];
            
            //skip if this is not of the required type
            if ( bRequireHAL && pDevice->DeviceType != D3DDEVTYPE_HAL ) continue;
            if ( bRequireREF && pDevice->DeviceType != D3DDEVTYPE_REF ) continue;
            
            //loop through each option set
            for ( k = 0; k < pDevice->Options.size(); k++ )
            {
                YD3DEnumDeviceOptions * pOptions = pDevice->Options[ k ];

                //determine if back buffer format matches adapter 
                bool MatchedBB = false;
				if (pOptions->BackBufferFormat == D3DFMT_A8R8G8B8)
					MatchedBB = true;
				else if (pOptions->BackBufferFormat == pOptions->AdapterFormat )
					MatchedBB = true;
                bool MatchedDesktop = (pOptions->AdapterFormat == AdapterDisplayMode.Format);
                
                //skip if this is not fullscreen
                if ( pOptions->Windowed ) continue;

                //if  havent found a compatible option set yet, or if this set
                //is better (because it's HAL / formats match better) then save it.
                if ( pBestOptions == NULL ||
                    (pBestOptions->DeviceType != D3DDEVTYPE_HAL && pDevice->DeviceType == D3DDEVTYPE_HAL ) ||
                    (pOptions->DeviceType == D3DDEVTYPE_HAL && pBestOptions->AdapterFormat != AdapterDisplayMode.Format && MatchedDesktop ) ||
                    (pOptions->DeviceType == D3DDEVTYPE_HAL && MatchedDesktop && MatchedBB) )
                {
                    //store best so far
                    BestAdapterDisplayMode = AdapterDisplayMode;
                    pBestAdapter = pAdapter;
                    pBestDevice  = pDevice;
                    pBestOptions = pOptions;
                    
                    if ( pOptions->DeviceType == D3DDEVTYPE_HAL && MatchedDesktop && MatchedBB )
                    {
                        //this fullscreen device option looks great -- take it
                        goto EndFullscreenDeviceOptionSearch;
                    }

                } //end if not a better match
            
            } //next Option Set
        
        } //next Device Type
    
    } //next Adapter

EndFullscreenDeviceOptionSearch:
    
    if ( pBestOptions == NULL) return false;

    //need to find a display mode on the best adapter that uses pBestOptions->AdapterFormat
    //and is as close to BestAdapterDisplayModes res as possible
    BestDisplayMode.Width       = 0;
    BestDisplayMode.Height      = 0;
    BestDisplayMode.Format      = D3DFMT_UNKNOWN;
    BestDisplayMode.RefreshRate = 0;

    //loop through valid display modes
    for( i = 0; i < pBestAdapter->Modes.size(); i++ )
    {
        D3DDISPLAYMODE Mode = pBestAdapter->Modes[ i ];
        
        //skip if it doesnt match our best format
        if( Mode.Format != pBestOptions->AdapterFormat ) continue;

        //determine how good a match this is
        if( Mode.Width == BestAdapterDisplayMode.Width &&
            Mode.Height == BestAdapterDisplayMode.Height && 
            Mode.RefreshRate == BestAdapterDisplayMode.RefreshRate )
        {
            //found a perfect match, so stop
            BestDisplayMode = Mode;
            break;

        } 
        else if( Mode.Width == BestAdapterDisplayMode.Width &&
                 Mode.Height == BestAdapterDisplayMode.Height && 
                 Mode.RefreshRate > BestDisplayMode.RefreshRate )
        {
            // refresh rate doesn't match, but width/height match, so keep this
            // and keep looking
            BestDisplayMode = Mode;
        }
        else if( Mode.Width == BestAdapterDisplayMode.Width )
        {
            //width matches, so keep this and keep looking
            BestDisplayMode = Mode;
        }
        else if( BestDisplayMode.Width == 0 )
        {
            //dont have anything better yet, so keep this and keep looking
            BestDisplayMode = Mode;
        
        }
    
    } //next Mode

	bool found = false;
	// Depth stencil format
	for (i = 0; i < pBestOptions->DepthFormats.size(); i++)
	{
		if (pBestOptions->DepthFormats[i] == D3DFMT_D24S8)
		{
			found = true;
			break;
		}
	}

	int ind = 0;
	if (found)
		ind = i;

    //fill out passed settings details
    D3DSettings.Windowed               = false;
    pSettings                          = D3DSettings.GetSettings();
    pSettings->AdapterOrdinal          = pBestOptions->AdapterOrdinal;
    pSettings->DisplayMode             = BestDisplayMode;
    pSettings->DeviceType              = pBestOptions->DeviceType;
    pSettings->BackBufferFormat        = pBestOptions->BackBufferFormat;
    pSettings->DepthStencilFormat      = pBestOptions->DepthFormats[ ind ];
    pSettings->MultisampleType         = pBestOptions->MultiSampleTypes[ 0 ];
    pSettings->MultisampleQuality      = 0;
    pSettings->VertexProcessingType    = pBestOptions->VertexProcessingTypes[ 0 ];
    pSettings->PresentInterval         = pBestOptions->PresentIntervals[ 0 ];

    // Success
    return true;
}