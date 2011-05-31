/*
This file is part of YafXSI.
YafaRay Exporter for Autodesk(c) Softimage(c).

Copyright (C) 2010 2011  Pedro Alcaide, aka povmaniaco

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
#pragma warning (disable : 4244) 
#pragma warning (disable : 4996) 


#include "yafxsi_main.h"

using namespace XSI;
using namespace MATH;
using namespace yafaray;
using namespace std;



ofstream f;
Application app;
Model root( app.GetActiveSceneRoot() );

Null null;
CustomProperty prop ;
#define PI 3.14159265

//-----------------
//-- integrators --
//-----------------
int  vLighting = 0;
//-- SPPM
int vspphotons = 10000, vsppassNums = 500, vspbounces = 4, vspsearch = 50, vsptimes = 1;
bool vsppmIRE = false;
float vspdiffuseRadius = 1.0;

//-- photonmap
float vDiff_radius=0.5f, vCausticRadius=1.0f;
int vCaustic_depth = 10, vRaydepth = 3, vShadows_depth = 3;
int vDepth = 3, vPMDphotons = 100000, vSearch = 100, vCaustic_mix = 100, vFg_bounces = 3, vFg_samples = 16;
int vPMCphotons = 500000,  vIbl_samples = 1;

//-- pathtracing
int vPTCphotons = 500000, vPath_samples = 32, vPTCaustic_mix = 100, vPath_depth =5, vTracer_method = 0;
float vPTCaust_radius=1.0;
bool vUse_backg = false;

//-- directlighting
int vDLCphotons = 500000, vDLCaustic_mix = 100, vDLCaust_depth = 5;
float vCaust_radius = 0.5f;
float vAOc_red = 1.0f, vAOc_green = 1.0f, vAOc_blue = 1.0f, vAOc_alpha = 1.0f, vAO_distance = 1.0f;
bool vUse_AO = false;
int vAO_samples = 16;
//---------------------
//-- volume integrator 
//---------------------
int vAtt_grid=1, vIntegrator = 0;
float vStep_size = 1.0f;
bool  vAdaptive = false, vOptimize = false;

int vExtOut = 1;
//--------------------
//-- general settings
//--------------------
   
int  vThreads = 1, vtileorder = 0, vtilesize = 32;;
bool vUseCaust = false, vTransp_shad = false, vClay_render = false, vAuto_thread = true, vZ_buffer = false;
bool vClamp_rgb = false, vPremu_alpha = false, vshowpix = false;
bool vDraw_render = false, vFinal_gather = false, vShow_map = false, vNo_recursion = true, vUse_ibl = false;
float vContrast = 2.2f, vcontrast_in = 2.2f;

float vIbl_power = 1.0f;


//-------------------
//-- output settings
//-------------------
int vXRes = 640, vYRes = 480, vSavefile = 0;
//-- antialias 
int vAA_inc=1, vAA_passes=1, vAA_samples=5, vFilter_type = 0;
float vAA_threshold=0.05f, vAA_pixel=1.501f;

//-----------------
//-- worl settings
//-----------------
int vSetworl = 0;
//-- sunsky
float vXvect = 1.0f, vYvect = 1.0f, vZvect = 1.0f, vW_power = 1.0f, vTurbidity = 3.0f, vSun_power = 1.0f;
float vHorbright = 1.0f, vHorsprd = 1.0f, vSunbright = 1.0f, vSunsize = 1.0f, vBacklight = 1.0f;
bool vFrom_angle = false, vFrom_pos = false, vFrom_update = false;
bool vAdd_sun=false, vSkylight=false;

//-- gradient background 
float vBack_red = 0.0f, vBack_green = 0.0f, vBack_blue = 0.0f, vBack_alpha = 0.0f;
float vHor_red = 1.0f, vHor_green = 1.0f, vHor_blue = 1.0f, vHor_alpha = 1.0f;
float vZen_red = 1.0f, vZen_green = 1.0f, vZen_blue = 1.0f, vZen_alpha = 1.0f;
float vHorgro_red = 1.0f, vHorgro_green = 1.0f, vHorgro_blue = 1.0f, vHorgro_alpha = 1.0f;
float vZengro_red = 1.0f, vZengro_green = 1.0f, vZengro_blue = 1.0f, vZengro_alpha = 1.0f;

//-- texture background
bool vUse_interp = false;
int vW_samples = 8, vrotation = 0;
int vmapworld = 0;

//-- hiddend objects --->
bool vIsHiddenCam = true, vIsHiddenLight = true, vIsHiddenObj = true;
bool vIsHiddenSurfaces = true, vIsHiddenClouds = false;
//--
bool vIsLinux = true, vProg = true, vIsCaustic = false, vSmooth_mesh = false;
bool vAmbBack = false, vExportDone = false;

//---- matrices
//char aBool [2][6] = {"false", "true"};
char file_ext [6][4] = {"tif", "tga", "png", "jpg", "hdr", "exr" };
char AVolInte [2][24] = {"none", "SingleScatterIntegrator" };
char Aa_mode [4][9] = { "box", "gauss", "lanczos", "mitchell" };
///--------

CString vSun=L"", vHDRI=L"", vplugin_path=L"", vFileObjects=L"", vIES_file = L"";
CString vCam=L"", vCustom_string = L"YafaRay for Softimage, beta 0.1";
CString matName, texName;

CStringArray aMatList;
UIToolkit kit = app.GetUIToolkit();
ProgressBar pb = kit.GetProgressBar();
//--
//yafrayInterface_t *yi; 
//--
string out_type;

material_t *mat= NULL;
string mat_name;
string tex_name;
string map_name;
//-- 
map<string, material_t *> mMap;
map<string, material_t *>::const_iterator mi;
// pair<map<string, material_t *>::const_iterator, bool> ret;