//-------------------------------------------------------------------------------
// YCPU.cpp
// Yosbi Alves
// yosbito@gmail.com
// Copyright (c) 2011
//-----------------------------------------------------------------------
//Desc: Part of the static YEMath library, contains the methods to
//		sniff the caps of the CPU to know if the cpu supports SSE 
//		and othe instructions sets
//--------------------------------------------------------------------------

//-----------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------------
#include "YEMath.h"
#include <stdio.h>
#include <string.h>
#include <Windows.h> //for exeptions

//--------------------------------------------------------------------
// Global variable
//--------------------------------------------------------------------
bool g_bSSE = false;

//--------------------------------------------------------------------
// Fordward functions declarations
//--------------------------------------------------------------------
CPUINFO GetCPUInfo();
bool OSSupportsSSE();
void GetCPUName(char *, int, const char*);

//--------------------------------------------------------------------
// Name: YInitCPU()
// Desc: Sets global variable for availavility of SSE instructions
//--------------------------------------------------------------------
bool YInitCPU()
{
	// Check the cpu caps
	CPUINFO info = GetCPUInfo();

	// Check if the OS supports SSE
	bool bOS = OSSupportsSSE();

	if(info.bSSE && bOS) g_bSSE = true;
	else g_bSSE = false;

	return g_bSSE;
}

//--------------------------------------------------------------------
// Name: GetCPUInfo()
// Desc: Function to detect SSE availability in CPU
//--------------------------------------------------------------------
CPUINFO GetCPUInfo()
{
	CPUINFO info;
	char *pStr = info.vendor;
	int  n	   = 1;
	int *pn	   = &n;

	// Set all values to 0 (false)
	ZeroMemory(&info, sizeof(CPUINFO));

	// 1: See if can fer CPUID and vendors name then check for SSE and 
	// MMX Support (vendor independent)
	__try
	{
		__asm
		{
			mov eax, 0				// eax = 0 ---> CPUID returns vendor name
			CPUID					// Performs CPUID function

			mov esi,	   pStr
			mov [esi],	   ebx		// First 4 chars
			mov [esi + 4], edx		// Next 4 chars
			mov [esi + 8], ecx		// Last 4 chars

			mov eax, 1				// EAX = 1 ---> CPUID returns feature bits
			CPUID

			test edx, 04000000h		// test bit 26 for SSE2
			jz	 _NOSSE2			// If test failed jump
			mov	[info.bSSE], 1		// Set to true

_NOSSE2: test edx, 02000000h  // test bit 25 for SSE
         jz   _NOSSE          // if test failed jump
         mov  [info.bSSE], 1  // set to true

_NOSSE:  test edx, 00800000h  // test bit 23 for MMX
         jz   _EXIT1          // if test failed jump
         mov  [info.bMMX], 1  // set to true
_EXIT1:  // nothing to do anymore
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER) {
      if (_exception_code() == STATUS_ILLEGAL_INSTRUCTION)
         return info;         // cpu inactive
      return info;            // unexpected exception occurred
      }

	// 2: See if can get extended info (vendor independant)
   _asm {
      mov  eax, 80000000h     // EAX=0x8 => CPUID returns extended features
      CPUID
      cmp  eax, 80000000h     // must be greater than 0x80
      jbe  _EXIT2             // below or equal 0x80 then jump away
      mov [info.bEXT], 1      // set to true

      mov  eax, 80000001h     // => CPUID will copy ext bits to EDX
      CPUID
      test edx, 80000000h     // 0x8 indicates 3DNow!support
      jz   _EXIT2             // if failed jump away
      mov  [info.b3DNOW], 1   // set to true
_EXIT2:
      }

   // 3: Get vendor specific stuff
   //    INTEL: CPU id
   //    AMD:   CPU id, 3dnow_ex, mmx_ex
   if ( (strncmp(info.vendor, "GenuineIntel", 12) == 0) && info.bEXT) {       // INTEL
      _asm {

         mov  eax, 1             // => CPUID will copy ext. feat. info
         CPUID                   //    to EDX and brand id to EBX
         mov  esi,   pn          // get brand id which is only supported
         mov  [esi], ebx         // by processors > PIII/Celeron
         }
      int m=0;
      memcpy(&m, pn, sizeof(char)); // id only needs lowest 8 bits
      n = m;
      }
   else if ( (strncmp(info.vendor, "AuthenticAMD", 12) == 0) && info.bEXT) {  // AMD

      _asm {
         mov  eax, 1             // => CPUID will copy ext. feat. info
         CPUID                   //    to EDX and brand id to EAX
         mov  esi,   pn          // get cpu type
         mov  [esi], eax

         mov  eax, 0x80000001    // => CPUID will copy ext. feat. bits
         CPUID                   //    to EDX and cpu type to EAX

         test edx, 0x40000000    // 0x4 indicates AMD extended 3DNow!
         jz   _AMD1              // if failed jump away
         mov  [info.b3DNOWEX], 1 // set to true
_AMD1:   test edx, 0x00400000    // 0x004 indicates AMD extended MMX
         jz   _AMD2              // if fail jump away
         mov  [info.bMMXEX], 1   // set to true
_AMD2:
         }
      }

   else {
      if (info.bEXT)
         ; /* UNKNOWN VENDOR */
      else
         ; /* lack of extended features */
      }

   info.vendor[13] = '\0';                // end of string
   GetCPUName(info.name, n, info.vendor); // select cpu name

   return info;

}

//--------------------------------------------------------------------
// Name: OSSupportsSSE()
// Desc: Function to detect SSE availabiliti in operating system
//--------------------------------------------------------------------
bool OSSupportsSSE() 
{
   // try SSE instruction and look for crash
   __try
      {
      _asm xorps xmm0, xmm0
      }
   __except(EXCEPTION_EXECUTE_HANDLER) 
   {
      if (_exception_code() == STATUS_ILLEGAL_INSTRUCTION)
         return false;  // sse not supported by os
      return false;     // unknown exception occured
   }
      
   return true;
}

//--------------------------------------------------------------------
// Name: GetCPUName()
// Desc: Get name to found processor id, this method is not accurated
//		and not cover all procesors
//--------------------------------------------------------------------
void GetCPUName(char *chName, int n, const char *vendor) 
{
   // Intel processors
   if (strncmp(vendor, "GenuineIntel", 12) == 0) 
   {
      switch (n) {
         case 0: {
            sprintf(chName, "< Pentium III/Celeron");
            } break;
         case 1: {
            sprintf(chName, "Pentium Celeron (1)");
            } break;
         case 2: {
            sprintf(chName, "Pentium III (2)");
            } break;
         case 3: {
            sprintf(chName, "Pentium III Xeon/Celeron");
            } break;
         case 4: {
            sprintf(chName, "Pentium III (4)");
            } break;
         case 6: {
            sprintf(chName, "Pentium III-M");
            } break;
         case 7: {
            sprintf(chName, "Pentium Celeron (7)");
            } break;
         case 8: {
            sprintf(chName, "Pentium IV (Genuine)");
            } break;
         case 9: {
            sprintf(chName, "Pentium IV");
            } break;
         case 10: {
            sprintf(chName, "Pentium Celeron (10)");
            } break;
         case 11: {
            sprintf(chName, "Pentium Xeon / Xeon-MP");
            } break;
         case 12: {
            sprintf(chName, "Pentium Xeon-MP");
            } break;
         case 14: {
            sprintf(chName, "Pentium IV-M / Xeon");
            } break;
         case 15: {
            sprintf(chName, "Pentium Celeron (15)");
            } break;

         default: 
			 sprintf(chName, "Older intel CPU");
			 break;
         }
      }
   // AMD processors
   else if (strncmp(vendor, "AuthenticAMD", 12) == 0) 
   {
      switch (n) {
         case 1660: {
            sprintf(chName, "Athlon / Duron (Model-7)");
            } break;
         case 1644: {
            sprintf(chName, "Athlon / Duron (Model-6)");
            } break;
         case 1596: {
            sprintf(chName, "Athlon / Duron (Model-3)");
            } break;
         case 1612: {
            sprintf(chName, "Athlon (Model-4)");
            } break;
         case 1580: {
            sprintf(chName, "Athlon (Model-2)");
            } break;
         case 1564: {
            sprintf(chName, "Athlon (Model-1)");
            } break;
         case 1463: {
            sprintf(chName, "K6-III (Model-9)");
            } break;
         case 1420: {
            sprintf(chName, "K6-2 (Model-8)");
            } break;
         case 1404: {
            sprintf(chName, "K6 (Model-7)");
            } break;
         case 1388: {
            sprintf(chName, "K6 (Model-6)");
            } break;
         case 1340: {
            sprintf(chName, "K5 (Model-3)");
            } break;
         case 1324: {
            sprintf(chName, "K5 (Model-2)");
            } break;
         case 1308: {
            sprintf(chName, "K5 (Model-1)");
            } break;
         case 1292: {
            sprintf(chName, "K5 (Model-0)");
            } break;

         default: { sprintf(chName, "older than K5"); break; }
         }
      }
   return;
}



