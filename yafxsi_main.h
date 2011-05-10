/*
This file is part of YafXSI.
YafaRay Exporter addon for Autodesk(c) Softimage(c).

Copyright (C) 2010 2011  Pedro Alcaide aka povmaniaco

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

#pragma once

#include <xsi_application.h>
#include <xsi_camera.h>
#include <xsi_context.h>
#include <xsi_controlpoint.h>
#include <xsi_group.h>
#include <xsi_segment.h>
#include <xsi_materiallibrary.h>
#include <xsi_particlecloud.h>
#include <xsi_particlecloudprimitive.h>
#include <xsi_pluginregistrar.h>
#include <xsi_geometryaccessor.h>
#include <xsi_iceattribute.h>
#include <xsi_iceattributedataarray.h>
#include <xsi_menu.h>
#include <xsi_model.h>
#include <xsi_source.h>
#include <xsi_triangle.h>
#include <xsi_material.h>
#include <xsi_kinematics.h>
#include <xsi_project.h>
#include <xsi_null.h>
#include <xsi_ppglayout.h>
#include <xsi_ppgeventcontext.h>
#include <xsi_light.h>
#include <xsi_polygonmesh.h>
#include <xsi_texture.h>
#include <xsi_imageclip2.h>
#include <xsi_progressbar.h>
#include <xsi_uitoolkit.h>
#include <xsi_pass.h>
#include <xsi_scene.h>
#include <xsi_nurbssurfacemesh.h>
#include <xsi_nurbssurface.h>
#include <xsi_ogllight.h>
#include <xsi_trianglevertex.h>
#include <xsi_polygonface.h>
#include <xsi_vertex.h>

#include <xsi_viewcontext.h>
#include <xsi_viewnotification.h>
#include <xsi_polygonnode.h>
#include <xsi_utils.h>

#include <xsi_scene.h>
#include <xsi_library.h>
#include <xsi_source.h>
//---/ for integration 2011 version /--->
#include <xsi_renderer.h>
#include <xsi_renderercontext.h>
#include <xsi_math.h> // problems ?
#include <xsi_projectitem.h>
#include <xsi_siobject.h>
#include <xsi_base.h>
//---/ for renderer /------>
#include <xsi_decl.h>
#include <xsi_longarray.h>
#include <xsi_customproperty.h>
#include <xsi_passcontainer.h>
#include <xsi_primitive.h>
#include <xsi_x3dobject.h>


// yafaray
#include <yafray_config.h>
#include <interface/yafrayinterface.h>
#include <interface/xmlinterface.h>
#include <yafraycore/imageOutput.h>
#include <include/gui/yafqtapi.h>

// system
#include <math.h>
#include <iostream>
#include <sstream>
#include <cstdlib> // for yafrayinterface ?
#include <fstream>
#include <string.h>

//
#if defined(_WIN32) || defined(_WIN64)
	#include <windows.h>
	#include <winioctl.h>
#endif


// Excluir material rara vez utilizado de encabezados de Windows
#define WIN32_LEAN_AND_MEAN 