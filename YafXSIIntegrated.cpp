/*
This file is part of YafXSI.
YafaRay Exporter for Autodesk(c) Softimage(c).

Copyright (C) 2011  Jeremiah Darais

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "yafxsi_main.h"

using namespace XSI;
using namespace MATH;

static bool abortFlag;
HANDLE abortHandle;
CRITICAL_SECTION abortCriticalSection;

void setAbort( bool in_abort )
{
   ::EnterCriticalSection( &abortCriticalSection );
   abortFlag = in_abort;
   if( in_abort )
       ::SetEvent( abortHandle );
   else
       ::ResetEvent( abortHandle );
   ::LeaveCriticalSection( &abortCriticalSection );
}

bool isAborted( )
{
   bool   bAbort;
   ::EnterCriticalSection( &abortCriticalSection );
   bAbort= abortFlag;
   ::LeaveCriticalSection( &abortCriticalSection );

   return( bAbort );
}


XSIPLUGINCALLBACK CStatus YafaRayIntegratedRenderer_Init( CRef& in_ctxt )
{
	Context ctxt( in_ctxt );
	Renderer renderer = ctxt.GetSource();

	CLongArray processTypes;
	processTypes.Add( siRenderProcessRender );
	processTypes.Add( siRenderProcessExportArchive );
	renderer.PutProcessTypes( processTypes );

	renderer.AddProperty( siRenderPropertyOptions, L"YafaRay Renderer.YafaRay Renderer" );

	// Add the Softimage PIC format as an output format.
   renderer.AddOutputImageFormat( L"Softimage PIC", L"pic" );
   renderer.AddOutputImageFormatSubType( siRenderChannelColorType, L"RGBA", siImageBitDepthInteger8 );

   // And some arbitrary image format.
   renderer.AddOutputImageFormat( L"Foo Format", L"foo" );
   renderer.AddOutputImageFormatSubType( siRenderChannelColorType, L"RGBA", siImageBitDepthInteger8 );
   renderer.AddOutputImageFormatSubType( siRenderChannelColorType, L"RGBA", siImageBitDepthInteger16 );
   renderer.AddOutputImageFormatSubType( siRenderChannelColorType, L"RGB", siImageBitDepthInteger8 );
   renderer.AddOutputImageFormatSubType( siRenderChannelGrayscaleType, L"Gray", siImageBitDepthInteger16 );

   // Create the handles for a thread-safe abort
   abortFlag = false;
   ::InitializeCriticalSection( &abortCriticalSection );
   abortHandle = ::CreateEvent( NULL, FALSE, FALSE, NULL );

	return CStatus::OK;
}

XSIPLUGINCALLBACK CStatus YafaRayIntegratedRenderer_Process( CRef& in_ctxt )
{

	return CStatus::OK;
}

XSIPLUGINCALLBACK CStatus YafaRayIntegratedRenderer_Abort( CRef& in_ctxt )
{
	setAbort( true );
	return CStatus::OK;
}