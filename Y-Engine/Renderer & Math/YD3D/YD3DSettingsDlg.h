//-----------------------------------------------------------------------
// YD3DSettingsDlg.h
// Yosbi Alves
// yosbito@gmail.com
// Copyright (c) 2011
//-----------------------------------------------------------------------
// Desc: Direct3D device settings dialog. Allows a user to select the various
//       device initialization options.


#ifndef YD3DSETTINGSDLG_H
#define YD3DSETTINGSDLG_H

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "YD3DEnum.h"		
#include <tchar.h>


//-----------------------------------------------------------------------------
// Name: CD3DSettingsDlg Class
// Desc: Creates and controls a settings dialog for the d3d devices.
//-----------------------------------------------------------------------------
class YD3DSettingsDlg
{
public:
    //-------------------------------------------------------------------------
	//constructors & Destructors
	//-------------------------------------------------------------------------
	         YD3DSettingsDlg();
	virtual ~YD3DSettingsDlg();

	//-------------------------------------------------------------------------
	//public Functions
	//-------------------------------------------------------------------------
	int				ShowDialog ( YD3DInitialize * pInitialize, YD3DSettings *pSettings = NULL, HWND hWndParent = NULL, HINSTANCE hInstParent = NULL );
    YD3DSettings    GetD3DSettings ( ) const { return m_D3DSettings; }
    
private:
    //-------------------------------------------------------------------------
	// Private Functions
	//-------------------------------------------------------------------------
	BOOL CALLBACK	SettingsDlgProc ( HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
    void            AdapterChanged (  );
    void            DeviceChanged (  );
    void            WindowedFullscreenChanged (  );
    void            AdapterFormatChanged (  );
    void            ResolutionChanged (  );
    void            RefreshRateChanged (  );
    void            BackBufferFormatChanged (  );
    void            DepthStencilFormatChanged (  );
    void            MultisampleTypeChanged (  );
    void            MultisampleQualityChanged (  );
    void            VertexProcessingChanged (  );
    void            PresentIntervalChanged (  );

    void            Combo_ItemAdd ( ULONG ComboID, LPVOID pData, LPCTSTR pStrDesc );
    LPVOID          Combo_GetSelectedItemData ( ULONG ComboID );
    ULONG           Combo_GetItemCount ( ULONG ComboID );
    void            Combo_SelectItem ( ULONG ComboID, ULONG Index );
    void            Combo_SelectItemData ( ULONG ComboID, LPVOID pData );
    void            Combo_Clear ( ULONG ComboID );
    bool            Combo_FindText ( ULONG ComboID, LPCTSTR pStrText );

    YD3DSettings::Settings * GetSettings ( )  { return m_D3DSettings.GetSettings(); }

	//-------------------------------------------------------------------------
	// Private Static Function
	//-------------------------------------------------------------------------
    static BOOL CALLBACK StaticDlgProc (HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

    //-------------------------------------------------------------------------
	// Private Variables 
	//-------------------------------------------------------------------------
	YD3DInitialize *m_pInitialize;			// The class which stores enum objects
    HWND            m_hWndDlg;              // The main dialog window handle 
    YD3DSettings    m_D3DSettings;          // The settings we are currently using

};


#endif // CD3DSETTINGSDLG_H