/*
YafXSI - Softimage XSI Export addon for YafaRay.

Copyright (C) 2011  Pedro Alcaide aka povmaniaco

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

#include "stdafx.h"
//--
using namespace XSI;
using namespace MATH;
using namespace yafaray;
using namespace std;
//--

ofstream f;
Application app;
Model root( app.GetActiveSceneRoot() );

Null null;
CustomProperty prop ;
#define PI 3.14159265

int api_yaf();
//--
void yafaray_execute();
//--
CString readIni();
//--
void yafaray_update_values( CString paramName, Parameter changed, PPGEventContext ctxt );
//-- 
void yafaray_dynamic( Parameter changed, PPGEventContext ctxt );
//-- 
void yafaray_material(yafrayInterface_t *yi, CString texName);
//--
int vBounces=10,vSavefile=1,vXRes=640,vYRes=480;
 
//---/ integrator lightings /--->
int vCaustic_depth = 10, vAO_samples = 16, vRaydepth = 3, vShadows_depth = 3, vThreads = 1;
int vDepth = 3, vPMDphotons = 100000, vSearch = 100, vCaustic_mix = 100, vFg_bounces = 3, vFg_samples = 16;
int vPMCphotons = 500000, vPath_samples = 32, vPath_depth =5, vBackground = 0, vIllum = 0, vIbl_samples = 1;
int vDLCphotons = 500000, vPTCphotons = 500000, vPTCaustic_mix = 100, vDLCaustic_mix = 100, vDLCaust_depth = 5;

//-- antialias --->
int vAA_inc=1, vAA_passes=1, vAA_samples=5;
float vAA_threshold=0.05f, vAA_pixel=1.501f;

CString typeMat, matName, texName;
bool vText = false;
string out_type;

//-- volume integrator --->
int vAtt_grid=1, vIntegrator = 0;
//-- background ----->
int vW_samples = 8, vrotation = 0;
int vExtOut = 1, vFilter_type = 0;

bool vUseCaust = false, vTransp_shad=false, vClay_render=false, vAuto_thread=true, vZ_buffer=false;
bool  vClamp_rgb=false, vPremu_alpha=false, vshowpix = false;
bool vDraw_render=false,vFinal_gather=false,vShow_map=false, vNo_recursion=true, vUse_ibl=false;
bool vFrom_angle=false,vFrom_pos=false,vFrom_update=false, vUse_backg = false;
bool vAdd_sun=false, vSkylight=false, vAdaptive=false, vOptimize=false;
//-- ambient occlusion
float vAOc_red=1.0f, vAOc_green=1.0f, vAOc_blue=1.0f, vAOc_alpha=1.0f, vAO_distance=1.0f;
bool vUse_AO=false;

float vContrast=2.2f, vcontrast_in = 2.2f, vDiff_radius=0.5f,vCaust_radius=0.5f,vCausticRadius=1.0f, vPTCaust_radius;
float vIbl_power=1.0f,vHorbright=1.0f,vHorsprd=1.0f,vSunbright=1.0f,vSunsize=1.0f,vBacklight=1.0f,vXvect=1.0f;
float vYvect=1.0f,vZvect=1.0f,vW_power=1.0f,vTurbidity=3.0f,vSun_power=1.0f;

//--
int vtileorder = 0, vtilesize = 32;

float vStep_size=1.0f;
float vBack_red=0.0f,vBack_green=0.0f, vLighting = 0;
float vBack_blue=0.0f,vBack_alpha=0.0f,vHor_red=1.0f,vHor_green=1.0f;
//-- background --->
float vHor_blue=1.0f,vHor_alpha=1.0f,vZen_red=1.0f,vZen_green=1.0f,vZen_blue=1.0f,vZen_alpha=1.0f;
float vHorgro_red=1.0f,vHorgro_green=1.0f;
float vHorgro_blue=1.0f,vHorgro_alpha=1.0f,vZengro_red=1.0f,vZengro_green=1.0f,vZengro_blue=1.0f,vZengro_alpha=1.0f;

//-- hiddend objects --->
bool vIsHiddenCam = true, vIsHiddenLight = true, vIsHiddenObj = true;
bool vIsHiddenSurfaces = true, vIsHiddenClouds = false;
//--
bool vIsLinux = true, vProg = true, vIsCaustic = false, vSmooth_mesh = false;
bool vAmbBack = false, vExportDone = false;
//-- background-texture
bool vUse_interp = false;
int vmapworld = 0;
//-- background
int vSetworl = 0, vTracer_method = 0;

//--
//---- matrices
char aBool [2][6] = {"false", "true"};
char file_ext [6][4] = {"tif", "tga", "png", "jpg", "hdr", "exr" };
char AVolInte [3][24] = {"none", "SingleScatterIntegrator", "SkyIntegrator" };
char Asetworld [5][13] = {"constant", "gradientback", "image", "sunsky", "darksky" };
char Aa_mode [4][9] = { "box", "gauss", "lanczos", "mitchell" };
char Atracer [4][12] = {"none", "path", "photon", "path+photon" };
char Alight [4][15] = {"directlighting", "photonmapping", "pathtracing", "bidirectional" };
///--------

CString vSun=L"", vHDRI=L"", vYafXSIPath=L"", vFileObjects=L"", vIES_file = L"";
CString vCam=L"", vCustom_string = L"";

CRefArray aGroups;
CStringArray aMatList; 
CStringArray aInstanceList; // orig. only aMatList
UIToolkit kit = app.GetUIToolkit();
ProgressBar pb = kit.GetProgressBar();
//--
yafrayInterface_t *yi;

material_t *mat= NULL;
string mat_name;
string tex_name;
string map_name;
//-- 
map<string, material_t *> mMap;
map<string, material_t *>::const_iterator mi;
// pair<map<string, material_t *>::const_iterator, bool> ret;
//--
texture_t *tex = NULL;

map<string, texture_t *> texMap;
map<string, texture_t *>::const_iterator ti;
// pair<map<string, texture_t *>::const_iterator, bool> tet;

XSIPLUGINCALLBACK CStatus XSILoadPlugin( PluginRegistrar& in_reg )
{
	in_reg.PutAuthor(L"Pedro Alcaide");
	in_reg.PutName(L"YafaRay Renderer");
	in_reg.PutEmail(L"p.alcaide@hotmail.com");
	in_reg.PutURL(L"http://povmaniaco.blogspot.com");
	in_reg.PutVersion(0,1);
	in_reg.RegisterProperty(L"YafaRay Renderer");
	in_reg.RegisterMenu(siMenuTbRenderRenderID,L"YafaRay Renderer_Menu",false,false);
	//RegistrationInsertionPoint - do not remove this line

	return CStatus::OK;
}
//--
XSIPLUGINCALLBACK CStatus XSIUnloadPlugin( const PluginRegistrar& in_reg )
{
	CString strPluginName;
	strPluginName = in_reg.GetName();
	Application().LogMessage(strPluginName + L" has been unloaded.");
	return CStatus::OK;
}

//--
XSIPLUGINCALLBACK CStatus YafaRayRenderer_Define( CRef& in_ctxt )
{
	Context ctxt( in_ctxt ); 
	/*
	CStatus AddParameter(
		const CString& in_scriptname,
		CValue::DataType in_type,
		INT in_capabilities,
		const CString& in_name,
		const CString& in_description,
		const CValue& in_default,
		const CValue& in_min,
		const CValue& in_max,
		const CValue& in_suggestedmin,
		const CValue& in_suggestedmax,
		Parameter&	io_parameter );
	*/
	Parameter oParam;
	prop = ctxt.GetSource();
	int sps = siPersistable;
	int sanim = siAnimatable;
	CValue dft; 
	// values
//----/ objects hidden /---->
	prop.AddParameter( L"use_hidden_obj",   CValue::siBool,  sps, L"",L"", vIsHiddenObj,   dft,dft,dft,dft, oParam);
	prop.AddParameter( L"bhidden_light",    CValue::siBool,  sps, L"",L"", vIsHiddenLight, dft,dft,dft,dft, oParam);
	prop.AddParameter( L"use_hidden_cam",   CValue::siBool,  sps, L"",L"", vIsHiddenCam,   dft,dft,dft,dft, oParam);
	prop.AddParameter( L"smooth_mesh",      CValue::siBool,  sps, L"",L"", vSmooth_mesh,   dft,dft,dft,dft, oParam);

//----/ specials.. /-------------->
	prop.AddParameter( L"bIES_file", CValue::siString, sps, L"",L"", vIES_file, oParam);
	
//----/ image /-------------->
	prop.AddParameter( L"Width",    CValue::siInt4, sps, L"",L"", vXRes, 0l,2048l,0l,1024l, oParam);
	prop.AddParameter( L"Height",   CValue::siInt4, sps, L"",L"", vYRes, 0l,2048l,0l,768l,  oParam);
	prop.AddParameter( L"savefile", CValue::siInt4, sps, L"",L"", vSavefile,  oParam ) ;

//----/ directlighting settings /-------->
	prop.AddParameter( L"bcaustics",		CValue::siBool,  sps, L"",L"", vUseCaust, dft,dft,dft,dft, oParam);
	prop.AddParameter( L"bDLCphotons",		CValue::siInt4,  sps, L"",L"", vDLCphotons, 1,10000000,10000,500000, oParam);
	prop.AddParameter( L"bDLcaustic_mix",	CValue::siInt4,  sps, L"",L"", vDLCaustic_mix, 1,10000,1,1000, oParam);
	prop.AddParameter( L"bcaustic_depth",	CValue::siInt4,  sps, L"",L"", vDLCaust_depth, 0,50,0,50, oParam);
	prop.AddParameter( L"bDLcaust_radius",	CValue::siFloat, sps, L"",L"", vCaust_radius, 0,100,0,100, oParam);
	//----/ AO settings /------>
	prop.AddParameter( L"AOc_red",      CValue::siFloat, sps, L"",L"", vAOc_red,   0.0,1.0,0.0,1.0, oParam ) ;
	prop.AddParameter( L"AOc_green",    CValue::siFloat, sps, L"",L"", vAOc_green, 0.0,1.0,0.0,1.0, oParam ) ;
	prop.AddParameter( L"AOc_blue",     CValue::siFloat, sps, L"",L"", vAOc_blue,  0.0,1.0,0.0,1.0, oParam ) ;
	prop.AddParameter( L"AOc_alpha",    CValue::siFloat, sps, L"",L"", vAOc_alpha, 0.0,1.0,0.0,1.0, oParam ) ;
	prop.AddParameter( L"bao_samples",  CValue::siInt4,  sps, L"",L"", vAO_samples,  0,1000,0,1000, oParam);
	prop.AddParameter( L"bao_distance", CValue::siFloat, sps, L"",L"", vAO_distance, 0,1000,0.0,1000, oParam);
	
	prop.AddParameter( L"bUse_AO",  CValue::siBool, sps, L"",L"", vUse_AO, dft,dft,dft,dft, oParam);
	
//----/ photon mappings settings /------------>
	prop.AddParameter( L"bsearch",			CValue::siInt4, sps,  L"",L"", vSearch,			1,10000,1,10000,		oParam);
	prop.AddParameter( L"bdepth",			CValue::siInt4, sps,  L"",L"", vDepth,			0,50,0,50,				oParam);
	prop.AddParameter( L"bdiff_radius",		CValue::siFloat,sps,  L"",L"", vDiff_radius,	0,100,0,100,			oParam);
	prop.AddParameter( L"bdiff_photons",	CValue::siInt4, sps,  L"",L"", vPMDphotons,     1,10000000,1,10000000,	oParam);
	prop.AddParameter( L"bcaust_photons",	CValue::siInt4, sps,  L"",L"", vPMCphotons,     1,10000000,1,10000000,	oParam);
	prop.AddParameter( L"bcaust_radius",	CValue::siFloat,sps,  L"",L"", vCausticRadius,	0,100,0,100,			oParam);
	prop.AddParameter( L"bfinal_gather",	CValue::siBool, sps,  L"",L"", vFinal_gather,	dft,dft,dft,dft,		oParam);
	prop.AddParameter( L"bcaustic_mix",		CValue::siInt4, sps,  L"",L"", vCaustic_mix,	1,10000,1,10000,		oParam);
	prop.AddParameter( L"bfg_bounces",		CValue::siInt4, sps,  L"",L"", vFg_bounces,		1,20,1,20,				oParam);
	prop.AddParameter( L"bfg_samples",		CValue::siInt4, sps,  L"",L"", vFg_samples,		1,4096,1,4096,			oParam);
	prop.AddParameter( L"bshow_map",		CValue::siBool, sps,  L"",L"", vShow_map,		dft,dft,dft,dft,		oParam);

//----/ pathtracing settings /----------->
	prop.AddParameter( L"bpathcaus_depth",   CValue::siInt4,	sps,L"",L"", vCaustic_depth,	1,10000,1,10000,	oParam);
	prop.AddParameter( L"bpathcaus_radius",  CValue::siFloat,	sps,L"",L"", vPTCaust_radius,	0,100,0,100,		oParam);
	prop.AddParameter( L"bpathcaus_photons", CValue::siInt4,	sps,L"",L"", vPTCphotons,		1,10000000,1,10000000,oParam);
	prop.AddParameter( L"tracer_method",     CValue::siInt4,	sps,L"",L"", vTracer_method,	0,10,0,10,			oParam);
	prop.AddParameter( L"bpath_samples",     CValue::siInt4,	sps,L"",L"", vPath_samples,		0,5000,0,5000,		oParam);
	prop.AddParameter( L"bno_recursion",     CValue::siBool,	sps,L"",L"", vNo_recursion,		dft,dft,dft,dft,	oParam);
	prop.AddParameter( L"buseback",	         CValue::siBool,	sps,L"",L"", vUse_backg,		dft,dft,dft,dft,	oParam);
	prop.AddParameter( L"bpathcaus_mix",     CValue::siInt4,	sps,L"",L"", vPTCaustic_mix,	1,10000,1,10000,	oParam);
	prop.AddParameter( L"bpath_depth",       CValue::siInt4,	sps,L"",L"", vPath_depth,		0,50,0,50,			oParam);
		
//----/ general settings /-------------->
	prop.AddParameter( L"bclay",           CValue::siBool, sps, L"",L"", vClay_render,		dft,dft,dft,dft, oParam);
	prop.AddParameter( L"bthreads",        CValue::siInt4, sps, L"",L"", vThreads,			0,20,0,20,		 oParam);
	prop.AddParameter( L"bz_buffer",       CValue::siBool, sps, L"",L"", vZ_buffer,			dft,dft,dft,dft, oParam);
	prop.AddParameter( L"braydepth",       CValue::siInt4, sps, L"",L"", vRaydepth,			0,64,0,64,		 oParam);
	prop.AddParameter( L"bauto_thread",    CValue::siBool, sps, L"",L"", vAuto_thread,		dft,dft,dft,dft, oParam);
	prop.AddParameter( L"bshadows_depth",  CValue::siInt4, sps, L"",L"", vShadows_depth,	0,64,0,64,		 oParam);
	prop.AddParameter( L"btransp_shadows", CValue::siBool, sps, L"",L"", vTransp_shad,		dft,dft,dft,dft, oParam);
	prop.AddParameter( L"bshowpix",        CValue::siBool, sps, L"",L"", vshowpix,		    dft,dft,dft,dft, oParam);
    //--
    prop.AddParameter( L"btileorder",      CValue::siInt4, sps, L"",L"", vtileorder,			0,4,0,4,		 oParam);
    prop.AddParameter( L"btilesize",       CValue::siInt4, sps, L"",L"", vtilesize,			0,512,0,512,		 oParam);
	
//----/ output settings /------------->
	prop.AddParameter( L"gamma",		CValue::siFloat, sps,L"",L"", vContrast,0,10,0,3,oParam); // 3
	prop.AddParameter( L"bgamma_in",	CValue::siFloat, sps,L"",L"", vcontrast_in,0,10,0,3,oParam); // 3
	prop.AddParameter( L"bclamp_rgb",	CValue::siBool,  sps,L"",L"", vClamp_rgb,dft,dft,dft,dft,oParam);
	prop.AddParameter( L"bpremu_alpha",	CValue::siBool,  sps,L"",L"", vPremu_alpha,dft,dft,dft,dft,oParam);//

//----/ output method /----->
	prop.AddParameter( L"bdraw_render",   CValue::siBool,   sps, L"",L"", vDraw_render,dft,dft,dft,dft, oParam);
	prop.AddParameter( L"bcustom_string", CValue::siString, sps, L"",L"", vCustom_string, oParam ) ;

//----/ AA settings /----->
	prop.AddParameter( L"baa_modes",	CValue::siInt4, sps,L"", L"",   vFilter_type,0,10,0,10,oParam);
	prop.AddParameter( L"baa_passes",	CValue::siInt4, sps,L"", L"",   vAA_passes,1,100,1,100,oParam);
	prop.AddParameter( L"baa_samples",	CValue::siInt4, sps, L"",L"",	vAA_samples,1,256,1,256,oParam);
	prop.AddParameter( L"baa_threshold",CValue::siFloat, sps,L"",L"",	vAA_threshold,0,1,0,1,oParam); // 3
	prop.AddParameter( L"baa_inc",		CValue::siInt4, sps,L"",L"",	vAA_inc,1,64,1,64,oParam);
	prop.AddParameter( L"baa_pixel",	CValue::siFloat, sps,L"",L"",	vAA_pixel,1,20,1,20,oParam); // 3
	
//----/ world settings /-------->
	prop.AddParameter(L"setworld",		CValue::siInt4,sps,L"",L"",vBackground,0,5,0,5,oParam ) ;

	//----/ single color /-------->
	prop.AddParameter( L"bsc_red",   CValue::siFloat, sps, L"", L"",	vBack_red,  0.0,1.0,0.0,1.0, oParam ) ;
	prop.AddParameter( L"bsc_green", CValue::siFloat, sps, L"", L"",	vBack_green,0.0,1.0,0.0,1.0, oParam ) ;
	prop.AddParameter( L"bsc_blue",  CValue::siFloat, sps, L"", L"",	vBack_blue, 0.0,1.0,0.0,1.0, oParam ) ;
	prop.AddParameter( L"bsc_alpha", CValue::siFloat, sps, L"", L"",	vBack_alpha,0.0,1.0,0.0,1.0, oParam ) ;
	//----/ ibl and photons /---->
	prop.AddParameter( L"buse_ibl",     CValue::siBool,		sps,L"",L"",	vUse_ibl,	 dft,dft,dft,dft,oParam);
	prop.AddParameter( L"bibl_samples",	CValue::siInt4,		sps,L"",L"",	vIbl_samples,1,512,1,512,oParam ) ;
	//prop.AddParameter( L"bwdiff_photons", CValue::siInt4,  sps,L"",L"",  vWdiff_photons,1,100000000,1,100000000,oParam);
	//prop.AddParameter( L"bwcaus_photons", CValue::siInt4,  sps,L"",L"",  vWcaus_photons,1,100000000,1,100000000,oParam);
	prop.AddParameter( L"bibl_power",     CValue::siFloat, sps,L"",L"",  vIbl_power,0,10,0,10,oParam);
    //-- texture
    prop.AddParameter( L"binterp",      CValue::siBool,		sps,L"",L"",	vUse_interp, dft,dft,dft,dft,oParam);
	prop.AddParameter( L"bHdri",        CValue::siString,	sps,L"",L"",	vHDRI, oParam);
	prop.AddParameter( L"brotation",    CValue::siInt4,		sps,L"",L"",	vrotation,-360,360,-360,360,oParam );//bmapworld
    prop.AddParameter( L"bmapworld",	CValue::siInt4,		sps,L"",L"",	vmapworld, 0,5,0,5,     oParam ) ;
	
	//----/ gradient /----/ horizont /----/ color /---->
	prop.AddParameter(L"bhor_red",   CValue::siFloat, sps, L"", L"", vHor_red,   0.0,1.0,0.0,1.0,	oParam ) ;
	prop.AddParameter(L"bhor_green", CValue::siFloat, sps, L"", L"", vHor_green, 0.0,1.0,0.0,1.0,	oParam ) ;
	prop.AddParameter(L"bhor_blue",  CValue::siFloat, sps, L"", L"", vHor_blue,  0.0,1.0,0.0,1.0,	oParam ) ;
	prop.AddParameter(L"bhor_alpha", CValue::siFloat, sps, L"", L"", vHor_alpha, 0.0,1.0,0.0,1.0,	oParam ) ;

	//----/ gradient /----/ zenit /----/ color /------->
	prop.AddParameter( L"bzen_red",   CValue::siFloat, sps, L"", L"", vZen_red,0.0,1.0,0.0,1.0,		oParam ) ;
	prop.AddParameter( L"bzen_green", CValue::siFloat, sps, L"", L"", vZen_green,0.0,1.0,0.0,1.0,	oParam ) ;
	prop.AddParameter( L"bzen_blue",  CValue::siFloat, sps, L"", L"", vZen_blue,0.0,1.0,0.0,1.0,	oParam ) ;
	prop.AddParameter( L"bzen_alpha", CValue::siFloat, sps, L"", L"", vZen_alpha,0.0,1.0,0.0,1.0,	oParam ) ;

	//----/ gradient /----/ horizont ground /----/ color /---->
	prop.AddParameter( L"bhorgro_red", CValue::siFloat, sps, L"", L"", vHorgro_red,0.0,1.0,0.0,1.0,		oParam ) ;
	prop.AddParameter( L"bhorgro_green", CValue::siFloat, sps, L"", L"", vHorgro_green,0.0,1.0,0.0,1.0, oParam ) ;
	prop.AddParameter( L"bhorgro_blue", CValue::siFloat, sps, L"", L"", vHorgro_blue,0.0,1.0,0.0,1.0,	oParam ) ;
	prop.AddParameter( L"bhorgro_alpha", CValue::siFloat, sps, L"", L"", vHorgro_alpha,0.0,1.0,0.0,1.0,	oParam ) ;

	//----/gradient /----/ zenit ground /----/ color /---->
	prop.AddParameter( L"bzengro_red",	CValue::siFloat, sps, L"", L"", vZengro_red,  0.0,1.0,0.0,1.0, oParam ) ;
	prop.AddParameter( L"bzengro_green",CValue::siFloat, sps, L"", L"", vZengro_green,0.0,1.0,0.0,1.0, oParam ) ;
	prop.AddParameter( L"bzengro_blue",	CValue::siFloat, sps, L"", L"", vZengro_blue, 0.0,1.0,0.0,1.0, oParam ) ;
	prop.AddParameter( L"bzengro_alpha",CValue::siFloat, sps, L"", L"", vZengro_alpha,0.0,1.0,0.0,1.0, oParam ) ;
	
	//----/ sunsky /----------->
	prop.AddParameter( L"bturbidity",	CValue::siFloat, sps, L"",L"", vTurbidity,	1,20,1,20,			oParam);
	prop.AddParameter( L"bA_horbright",	CValue::siFloat, sps, L"",L"", vHorbright,	0,10,0,10,			oParam);
	prop.AddParameter( L"bB_horsprd",	CValue::siFloat, sps, L"",L"", vHorsprd,	0,10,0,10,			oParam);
	prop.AddParameter( L"bC_sunbright",	CValue::siFloat, sps, L"",L"", vSunbright,	0,10,0,10,			oParam);
	prop.AddParameter( L"bD_sunsize",	CValue::siFloat, sps, L"",L"", vSunsize,	0,10,0,10,			oParam);
	prop.AddParameter( L"bE_backlight",	CValue::siFloat, sps, L"",L"", vBacklight,	0,10,0,10,			oParam);
	prop.AddParameter( L"bx_vect",		CValue::siFloat, sps, L"",L"", vXvect,		0,1,0,1,			oParam);
	prop.AddParameter( L"by_vect",		CValue::siFloat, sps, L"",L"", vYvect,		0,1,0,1,			oParam);
	prop.AddParameter( L"bz_vect",		CValue::siFloat, sps, L"",L"", vZvect,		0,1,0,1,			oParam);
	prop.AddParameter( L"bsun_power",	CValue::siFloat, sps, L"",L"", vSun_power,	1,10,1,10,			oParam);
	prop.AddParameter( L"bfrom_angle",	CValue::siBool,  sps, L"",L"", vFrom_angle, dft,dft,dft,dft,	oParam);
	prop.AddParameter( L"bfrom_pos",	CValue::siBool,  sps, L"",L"", vFrom_pos,   dft,dft,dft,dft,	oParam);
	prop.AddParameter( L"bfrom_update",	CValue::siBool,  sps, L"",L"", vFrom_update,dft,dft,dft,dft,	oParam);
	prop.AddParameter( L"badd_sun",		CValue::siBool,  sps, L"",L"", vAdd_sun,	dft,dft,dft,dft,	oParam);
	prop.AddParameter( L"bskylight",	CValue::siBool,  sps, L"",L"", vSkylight,	dft,dft,dft,dft,	oParam);
	prop.AddParameter( L"bw_samples",	CValue::siInt4,  sps, L"",L"", vW_samples,	1,512,1,512,		oParam);
	
	//prop.AddParameter( L"setIMG",CValue::siFloat,sps,L"",L"",0.0,1,10000,1,10000,oParam);

	//----/ lighting  /------>
	prop.AddParameter(L"blighting",		CValue::siInt4, sps,L"",L"", vLighting,	0,5,0,5, oParam);
	prop.AddParameter(L"AmbBack",		CValue::siBool, sps,L"",L"", vAmbBack,	dft,dft,dft,dft,oParam);

	//----/ volume integrator /-------->
	prop.AddParameter( L"bVintegrator",	CValue::siInt4,	 sps,L"",L"",	vIntegrator,	0,10,0,10,		oParam);
	prop.AddParameter( L"bstep_size",	CValue::siFloat, sps,L"",L"",	vStep_size,		1,100,1,100,	oParam);
	prop.AddParameter( L"badaptive",	CValue::siBool,  sps,L"",L"",	vAdaptive,		dft,dft,dft,dft,oParam);
	prop.AddParameter( L"boptimize",	CValue::siBool,  sps,L"",L"",	vOptimize,		dft,dft,dft,dft,oParam);
	prop.AddParameter( L"batt_grid",		CValue::siInt4,  sps,L"",L"",	vAtt_grid,		1,50,1,50,		oParam);
	//--- new group of prop for  elements menu / test only
	
	//--
	vFileObjects = app.GetInstallationPath(siUserPath);
	#ifdef __unix__
		vFileObjects += L"/tmp.xml";
	#else
		vFileObjects += L"/tmp.xml";
	#endif

	vYafXSIPath=readIni(); // get yafaray path out of the ini

	prop.AddParameter( L"fObjects", CValue::siString, sps, L"", L"", vFileObjects, oParam ) ;

	prop.AddParameter( L"vYafPath", CValue::siString, sps, L"", L"", vYafXSIPath, oParam ) ;

	return CStatus::OK;
}
//--
XSIPLUGINCALLBACK CStatus YafaRayRenderer_PPGEvent( const CRef& in_ctxt )
{	
	//--
	PPGEventContext ctxt( in_ctxt ) ;
	PPGLayout lay = Context(in_ctxt).GetSource() ;
	
	PPGEventContext::PPGEvent eventID = ctxt.GetEventID() ;
	
	if ( eventID == PPGEventContext::siOnInit )	{

		CustomProperty prop = ctxt.GetSource() ;
		CRefArray params = prop.GetParameters(); // Update values on init
		for (int i=0;i<params.GetCount();i++){
			Parameter param(params[i]);
				yafaray_update_values(param.GetScriptName(), param, ctxt );
					}
	
		ctxt.PutAttribute(L"Refresh",true); 
	
	} else if ( eventID == PPGEventContext::siButtonClicked ) 
	{
		CValue buttonPressed = ctxt.GetAttribute( L"Button" ) ;

		if (buttonPressed.GetAsText() == L"export_xml")
		{
            out_type = "xml_file";
            api_yaf();
        }
		if (buttonPressed.GetAsText() == L"render_YafXSI")
		{
		//	out_type = "console";
        //    api_yaf();
		}
		if (buttonPressed.GetAsText() == L"render_qtgui")
		{
            out_type = "gui";
			api_yaf();
		}
		ctxt.PutAttribute(L"Refresh",true); 
		CRefArray params = prop.GetParameters();
		for (int i=0;i<params.GetCount();i++){
			Parameter param(params[i]);
			yafaray_update_values(param.GetScriptName(),param, ctxt );
        }
	} else if ( eventID == PPGEventContext::siTabChange )
	{
		CValue tabLabel = ctxt.GetAttribute( L"Tab" ) ;	
	} 
	else if ( eventID == PPGEventContext::siParameterChange )
	{
		Parameter changed = ctxt.GetSource();
		CustomProperty prop = changed.GetParent() ;
		CString paramName = changed.GetScriptName() ;
			yafaray_update_values(paramName, changed, ctxt);
		//--
	} 
	
	return CStatus::OK ;
}
//--
string replace(string input) {   // add \\ to path filenames ej. H:\\trabajo\\mio.png
	int len = input.length();
	 for (int i=0;i<len;i++){
		if (input[i]=='\\') {
			input.replace(i, 1, "\\\\");
			i++;
        }
	}
	return input;
}
//--
CVector3 convertMatrix(CVector3 v){
	CMatrix3 m2(1.0,0.0,0.0,  0.0,1.0,0.0,   0.0,0.0,1.0);
    return v.MulByMatrix3InPlace (m2);
}
//--
void yafaray_update_values(CString paramName, Parameter changed, PPGEventContext ctxt ){
	
	//----/ update variables /--------------------->
	if (paramName == L"savefile")  { vExtOut = changed.GetValue(); 
    //--
	} else if (paramName == L"blighting")		{ vLighting			= changed.GetValue(); 
	} else if (paramName == L"bDLCphotons")		{ vDLCphotons		= changed.GetValue();
	} else if (paramName == L"bDLcaustic_mix")	{ vDLCaustic_mix	= changed.GetValue();
	} else if (paramName == L"bcaustic_depth")	{ vDLCaust_depth	= changed.GetValue();
	} else if (paramName == L"bDLcaust_radius")	{ vCaust_radius		= changed.GetValue();
	} else if (paramName == L"bcaustics")		{ vUseCaust		= changed.GetValue();
	
	//----/ use AO /------->
	} else if (paramName == L"bUse_AO")		    { vUse_AO	= changed.GetValue();
	} else if (paramName == L"bao_samples")		{ vAO_samples	= changed.GetValue();
	} else if (paramName == L"bao_distance")	{ vAO_distance	= changed.GetValue();
	
	//----/ AO color /------->
	} else if (paramName == L"AOc_red")		    { vAOc_red      = changed.GetValue();
	} else if (paramName == L"AOc_green")		{ vAOc_green	= changed.GetValue();
	} else if (paramName == L"AOc_blue")		{ vAOc_blue	= changed.GetValue();
	} else if (paramName == L"AOc_alpha")		{ vAOc_alpha	= changed.GetValue();
	
    //----/ photon mapping settings /------>
	} else if (paramName == L"bdepth")		    { vDepth	= changed.GetValue();
	} else if (paramName == L"bdiff_photons")	{ vPMDphotons	= changed.GetValue();
	} else if (paramName == L"bcaust_photons")	{ vPMCphotons	= changed.GetValue();
	} else if (paramName == L"bdiff_radius")	{ vDiff_radius	= changed.GetValue();
	} else if (paramName == L"bcaust_radius")	{ vCausticRadius = changed.GetValue();
	} else if (paramName == L"bsearch")		    { vSearch       = changed.GetValue();
	} else if (paramName == L"bcaustic_mix")	{ vCaustic_mix	= changed.GetValue();
	} else if (paramName == L"bfinal_gather")	{ vFinal_gather = changed.GetValue();
	} else if (paramName == L"bfg_bounces")		{ vFg_bounces	= changed.GetValue();
	} else if (paramName == L"bfg_samples")		{ vFg_samples	= changed.GetValue();
	} else if (paramName == L"bshow_map")		{ vShow_map		= changed.GetValue();

    //----/ pathtracing /---------->
	} else if (paramName == L"bpathcaus_photons")	{ vPTCphotons     = changed.GetValue();
	} else if (paramName == L"bpathcaus_mix")	  { vPTCaustic_mix  = changed.GetValue();
	} else if (paramName == L"bpathcaus_depth")	  { vCaustic_depth  = changed.GetValue();
	} else if (paramName == L"bpathcaus_radius")  { vPTCaust_radius = changed.GetValue();
	} else if (paramName == L"bpath_depth")		  { vPath_depth     = changed.GetValue();
	} else if (paramName == L"bpath_samples")	  { vPath_samples   = changed.GetValue();
	} else if (paramName == L"bno_recursion")	  { vNo_recursion   = changed.GetValue();
	} else if (paramName == L"tracer_method")	  { vTracer_method  = changed.GetValue();
	} else if (paramName == L"buseback")		  { vUse_backg	  = changed.GetValue();

    //----/ general /---------->
	} else if (paramName == L"braydepth")		{ vRaydepth = changed.GetValue();
	} else if (paramName == L"btransp_shadows")	{ vTransp_shad = changed.GetValue();
	} else if (paramName == L"bclay")		    { vClay_render = changed.GetValue();
	} else if (paramName == L"bshadows_depth")	{ vShadows_depth = changed.GetValue();
	} else if (paramName == L"bauto_thread")	{ vAuto_thread = changed.GetValue();
	} else if (paramName == L"bz_buffer")		{ vZ_buffer = changed.GetValue();
	} else if (paramName == L"bthreads")		{ vThreads = changed.GetValue();
    //-- gui mode
    } else if (paramName == L"bshowpix")		{ vshowpix = changed.GetValue();
    } else if (paramName == L"btileorder")		{ vtileorder = changed.GetValue();
    } else if (paramName == L"btilesize")		{ vtilesize = changed.GetValue();

    //----/ output /------------>
	} else if (paramName == L"gamma")		    { vContrast = changed.GetValue();
    } else if (paramName == L"bgamma_in")		{ vcontrast_in = changed.GetValue();
	} else if (paramName == L"bclamp_rgb")		{ vClamp_rgb = changed.GetValue();
	} else if (paramName == L"bpremu_alpha")	{ vPremu_alpha = changed.GetValue();

//----/ output method /----->
	} else if (paramName == L"bdraw_render")	{ vDraw_render		= changed.GetValue();
	} else if (paramName == L"bcustom_string")	{ vCustom_string	= changed.GetValue();

//----/ AA settings /---->
	} else if (paramName == L"baa_passes")		{ vAA_passes	= changed.GetValue();
	} else if (paramName == L"baa_samples")		{ vAA_samples	= changed.GetValue();
	} else if (paramName == L"baa_threshold")	{ vAA_threshold	= changed.GetValue();
	} else if (paramName == L"baa_inc")		{ vAA_inc		= changed.GetValue();
	} else if (paramName == L"baa_pixel")		{ vAA_pixel		= changed.GetValue();
	} else if (paramName == L"baa_modes")		{ vFilter_type	= changed.GetValue();

    //----/ setworld /---->
	} else if (paramName == L"setworld")	{ vSetworl		= changed.GetValue();
	//----/ single color ------
	} else if (paramName == L"bsc_red")	{ vBack_red		= changed.GetValue();
	} else if (paramName == L"bsc_green")	{ vBack_green	= changed.GetValue();
	} else if (paramName == L"bsc_blue")	{ vBack_blue	= changed.GetValue();
	} else if (paramName == L"bsc_alpha")	{ vBack_alpha	= changed.GetValue();

	//----/ gradient /----/ horizont /---->
	} else if (paramName == L"bhor_red")	{ vHor_red	 = changed.GetValue();
	} else if (paramName == L"bhor_green")	{ vHor_green = changed.GetValue();
	} else if (paramName == L"bhor_blue")	{ vHor_blue	 = changed.GetValue();
	} else if (paramName == L"bhor_alpha")	{ vHor_alpha = changed.GetValue();
	
	//----/ gradient /----/ zenit /---->
	} else if (paramName == L"bzen_red")	 { vZen_red   = changed.GetValue();
	} else if (paramName == L"bzen_green")	 { vZen_green = changed.GetValue();
	} else if (paramName == L"bzen_blue")	 { vZen_blue  = changed.GetValue();
	} else if (paramName == L"bzen_alpha")	 { vZen_alpha = changed.GetValue();
	
	//----/ gradient /----/ horizont ground /---->
	} else if (paramName == L"bhorgro_red")	  { vHorgro_red   = changed.GetValue();
	} else if (paramName == L"bhorgro_green") { vHorgro_green = changed.GetValue();
	} else if (paramName == L"bhorgro_blue")  { vHorgro_blue  = changed.GetValue();
	} else if (paramName == L"bhorgro_alpha") { vHorgro_alpha = changed.GetValue();
	
	//----/ gradient /----/ zenit ground /----> 
	} else if (paramName == L"bzengro_red")	  { vZengro_red	  = changed.GetValue();
	} else if (paramName == L"bzengro_green") { vZengro_green = changed.GetValue();
	} else if (paramName == L"bzengro_blue")  { vZengro_blue  = changed.GetValue();
	} else if (paramName == L"bzengro_alpha") { vZengro_alpha = changed.GetValue();

	//----/ texture /--->
	} else if (paramName == L"bHdri")         { vHDRI       = changed.GetValue();
	} else if (paramName == L"binterp")		  { vUse_interp = changed.GetValue();
	} else if (paramName == L"brotation")     { vrotation   = changed.GetValue();
	} else if (paramName == L"bmapworld")     { vmapworld   = changed.GetValue();

	//----/ sunsky /--------->
	} else if (paramName == L"bturbidity")	 { vTurbidity	= changed.GetValue();
	} else if (paramName == L"bA_horbright") { vHorbright	= changed.GetValue();
	} else if (paramName == L"bB_horsprd")	 { vHorsprd		= changed.GetValue();
	} else if (paramName == L"bC_sunbright") { vSunbright	= changed.GetValue();
	} else if (paramName == L"bD_sunsize")	 { vSunsize		= changed.GetValue();
	} else if (paramName == L"bE_backlight") { vBacklight	= changed.GetValue();
	} else if (paramName == L"bfrom_angle")	 { vFrom_angle	= changed.GetValue();
	} else if (paramName == L"bx_vect")		 { vXvect		= changed.GetValue();
	} else if (paramName == L"bfrom_pos")	 { vFrom_pos	= changed.GetValue();
	} else if (paramName == L"by_vect")		 { vYvect		= changed.GetValue();
	} else if (paramName == L"bfrom_update") { vFrom_update	= changed.GetValue();
	} else if (paramName == L"bz_vect")		 { vZvect		= changed.GetValue();
	} else if (paramName == L"badd_sun")	 { vAdd_sun		= changed.GetValue();
	} else if (paramName == L"bsun_power")	 { vSun_power	= changed.GetValue();
	} else if (paramName == L"bskylight")	 { vSkylight	= changed.GetValue();
	} else if (paramName == L"bw_samples")	 { vW_samples	= changed.GetValue();
	
	//--- IBL
	} else if (paramName == L"bibl_power")   { vIbl_power	= changed.GetValue();
	} else if (paramName == L"bibl_samples") { vIbl_samples	= changed.GetValue();
	} else if (paramName == L"buse_ibl")     { vUse_ibl		= changed.GetValue();
	
	//----/ vol. integrator /------>
	} else if (paramName == L"bVintegrator") { vIntegrator  = changed.GetValue();	
	} else if (paramName == L"bstep_size")	 { vStep_size	= changed.GetValue();
	} else if (paramName == L"badaptive")	 { vAdaptive	= changed.GetValue();
	} else if (paramName == L"boptimize")	 { vOptimize	= changed.GetValue();
	} else if (paramName == L"batt_grid")	 { vAtt_grid	= changed.GetValue();

	//----/ hidden objects /--------->
	} else if (paramName == L"use_hidden_obj")	{ vIsHiddenObj		= changed.GetValue();
	} else if (paramName == L"use_hidden_surf")	{ vIsHiddenSurfaces	= changed.GetValue();
	} else if (paramName == L"bhiddenclouds")   { vIsHiddenClouds	= changed.GetValue();
	} else if (paramName == L"use_hidden_cam")	{ vIsHiddenCam		= changed.GetValue();
	} else if (paramName == L"bhidden_light")   { vIsHiddenLight	= changed.GetValue();
	} else if (paramName == L"smooth_mesh")		{ vSmooth_mesh		= changed.GetValue();
	//----/    /------>
	} else if (paramName == L"fObjects")	{ vFileObjects	= changed.GetValue();
	} else if (paramName == L"AmbBack")		{ vAmbBack		= changed.GetValue();
	} else if (paramName == L"vYafPath")	{ vYafXSIPath	= changed.GetValue();	

	//----/ Image /------->
	} else if (paramName == L"Width")	{ vXRes	= changed.GetValue();
	} else if (paramName == L"Height")  { vYRes	= changed.GetValue();

//----/ specials... /------->
	} else if (paramName == L"bIES_file") { vIES_file	=  changed.GetValue();
	
    //--
	} else {
        app.LogMessage(L"End update values...");
	}
	//-- link logic bloq
	yafaray_dynamic( changed, ctxt);
} 
//--
void yafaray_dynamic( Parameter changed, PPGEventContext ctxt){
	
//	Parameter changed = ctxt.GetSource();
		CustomProperty prop = changed.GetParent() ;
		CString paramName = changed.GetScriptName() ;

	//-- GROUP LIGHTING ----

		if ((changed.GetName() == L"blighting" ) ||
            ( changed.GetName() == L"bcaustics" ) ||
		    ( changed.GetName() == L"bUse_AO" ) ||
		    ( changed.GetName() == L"bfinal_gather" ) ||
		    ( changed.GetName() == L"tracer_method" ))
        {
            vLighting = prop.GetParameterValue( L"blighting" );
            vUseCaust = prop.GetParameterValue( L"bcaustics" );
            vUse_AO = prop.GetParameterValue( L"bUse_AO" );
            vFinal_gather = prop.GetParameterValue( L"bfinal_gather" );
            vTracer_method = prop.GetParameterValue( L"tracer_method" );

        //-- all params with group  lighting
            Parameter lphotons		    = prop.GetParameters().GetItem( L"bDLCphotons" ); //--
	        Parameter lcaustic_mix	    = prop.GetParameters().GetItem( L"bDLcaustic_mix" ); //--
            Parameter lcaustic_depth	= prop.GetParameters().GetItem( L"bcaustic_depth" ) ; //---
            Parameter lcaustic_radius	= prop.GetParameters().GetItem( L"bDLcaust_radius" ) ;
            Parameter lao_samples	    = prop.GetParameters().GetItem( L"bao_samples" ) ;
	        Parameter lao_distance	    = prop.GetParameters().GetItem( L"bao_distance" ) ;
	        Parameter lAOc_red		    = prop.GetParameters().GetItem( L"AOc_red" ) ;
            //-- event controls
            Parameter lbcaustics	= prop.GetParameters().GetItem( L"bcaustics" );
            Parameter lbUse_AO		= prop.GetParameters().GetItem( L"bUse_AO" );

            //-- PhotonMap
            Parameter lbdepth		    = prop.GetParameters().GetItem( L"bdepth");			
            Parameter lbdiff_photons	= prop.GetParameters().GetItem( L"bdiff_photons");	
            Parameter lbcaust_photons	= prop.GetParameters().GetItem( L"bcaust_photons");	
            Parameter lbdiff_radius  	= prop.GetParameters().GetItem( L"bdiff_radius");	
            Parameter lbcaust_radius    = prop.GetParameters().GetItem( L"bcaust_radius");	
            Parameter lbsearch		    = prop.GetParameters().GetItem( L"bsearch");
            Parameter lbcaustic_mix	    = prop.GetParameters().GetItem( L"bcaustic_mix");	
            Parameter lbfinal_gather	= prop.GetParameters().GetItem( L"bfinal_gather"); 
            Parameter lbfg_bounces	    = prop.GetParameters().GetItem( L"bfg_bounces");
            Parameter lbfg_samples	    = prop.GetParameters().GetItem( L"bfg_samples");	 
            Parameter lbshow_map	    = prop.GetParameters().GetItem( L"bshow_map");		
		
            //-- path tracing
            Parameter ltracer_method	= prop.GetParameters().GetItem( L"tracer_method");
            Parameter lbpathcaus_photons = prop.GetParameters().GetItem( L"bpathcaus_photons");
            Parameter lbpathcaus_mix	= prop.GetParameters().GetItem( L"bpathcaus_mix");
            Parameter lbpathcaus_depth	= prop.GetParameters().GetItem( L"bpathcaus_depth")	;
            Parameter lbpathcaus_radius = prop.GetParameters().GetItem( L"bpathcaus_radius");
            Parameter lbpath_depth	    = prop.GetParameters().GetItem( L"bpath_depth");
            Parameter lbpath_samples	= prop.GetParameters().GetItem( L"bpath_samples");
            Parameter lbno_recursion	= prop.GetParameters().GetItem( L"bno_recursion");
            Parameter lbuseback     	= prop.GetParameters().GetItem( L"buseback");
		
        //-- direct lighting
            lbcaustics.PutCapabilityFlag( siNotInspectable, true ) ;
            lbUse_AO.PutCapabilityFlag( siNotInspectable, true ) ;
            lphotons.PutCapabilityFlag( siNotInspectable, true ) ;
            lcaustic_depth.PutCapabilityFlag( siNotInspectable, true ) ;
            lcaustic_radius.PutCapabilityFlag( siNotInspectable, true ) ;
            lcaustic_mix.PutCapabilityFlag( siNotInspectable, true ) ;
            lao_samples.PutCapabilityFlag( siNotInspectable, true ) ;
		    lao_distance.PutCapabilityFlag( siNotInspectable, true ) ;
		    lAOc_red.PutCapabilityFlag( siNotInspectable, true ) ;

            //-- photonmap
		    lbdepth.PutCapabilityFlag( siNotInspectable, true ) ;			
		    lbdiff_photons.PutCapabilityFlag( siNotInspectable, true ) ;	
		    lbcaust_photons.PutCapabilityFlag( siNotInspectable, true ) ;
		    lbdiff_radius.PutCapabilityFlag( siNotInspectable, true ) ;		
		    lbcaust_radius.PutCapabilityFlag( siNotInspectable, true ) ;	
		    lbsearch.PutCapabilityFlag( siNotInspectable, true ) ;
		    lbcaustic_mix.PutCapabilityFlag( siNotInspectable, true ) ;
		    lbfinal_gather.PutCapabilityFlag( siNotInspectable, true ) ; //-->
		    lbfg_bounces.PutCapabilityFlag( siNotInspectable, true );	
		    lbfg_samples.PutCapabilityFlag( siNotInspectable, true );	
		    lbshow_map.PutCapabilityFlag( siNotInspectable, true );
				
            //-- pathtracing
		    ltracer_method.PutCapabilityFlag( siNotInspectable, true ) ;
		    lbpath_depth.PutCapabilityFlag( siNotInspectable, true ) ;
		    lbpath_samples.PutCapabilityFlag( siNotInspectable, true ) ;
		    lbno_recursion.PutCapabilityFlag( siNotInspectable, true ) ;
		    lbuseback.PutCapabilityFlag( siNotInspectable, true ) ;
		    lbpathcaus_photons.PutCapabilityFlag( siNotInspectable, true );
		    lbpathcaus_mix.PutCapabilityFlag( siNotInspectable, true );
		    lbpathcaus_depth.PutCapabilityFlag( siNotInspectable, true );
		    lbpathcaus_radius.PutCapabilityFlag( siNotInspectable, true );
		 //---
	
		if ( vLighting == 0 ) // directlighting
        { 
            lbcaustics.PutCapabilityFlag( siNotInspectable, false ) ;
		    lbUse_AO.PutCapabilityFlag( siNotInspectable, false ) ;
            if ( vUse_AO )
            {
                lao_samples.PutCapabilityFlag( siNotInspectable, false ) ;
			    lao_distance.PutCapabilityFlag( siNotInspectable, false ) ;
			    lAOc_red.PutCapabilityFlag( siNotInspectable, false ) ;
            }
            if ( vUseCaust )
            {
                lphotons.PutCapabilityFlag( siNotInspectable, false ) ;
			    lcaustic_depth.PutCapabilityFlag( siNotInspectable, false ) ;
			    lcaustic_radius.PutCapabilityFlag( siNotInspectable, false ) ;
			    lcaustic_mix.PutCapabilityFlag( siNotInspectable, false ) ;
            }
            ctxt.PutAttribute(L"Refresh", true );
        }
        else if ( vLighting == 1 ) // photonmap
        {
		    lbdepth.PutCapabilityFlag( siNotInspectable, false ) ;			
		    lbdiff_photons.PutCapabilityFlag( siNotInspectable, false ) ;	
		    lbcaust_photons.PutCapabilityFlag( siNotInspectable, false ) ;
		    lbdiff_radius.PutCapabilityFlag( siNotInspectable, false ) ;		
		    lbcaust_radius.PutCapabilityFlag( siNotInspectable, false ) ;	
		    lbsearch.PutCapabilityFlag( siNotInspectable, false ) ;
		    lbcaustic_mix.PutCapabilityFlag( siNotInspectable, false ) ;
		    lbfinal_gather.PutCapabilityFlag( siNotInspectable, false ) ;
            //--
            if ( vFinal_gather )
            {
                lbfg_bounces.PutCapabilityFlag( siNotInspectable, false );	
		        lbfg_samples.PutCapabilityFlag( siNotInspectable, false );	
				lbshow_map.PutCapabilityFlag( siNotInspectable, false );
            }
            ctxt.PutAttribute(L"Refresh", true );
		}
		else if ( vLighting == 2 ) //-- pathtracing
        {
            ltracer_method.PutCapabilityFlag( siNotInspectable, false ) ;
		    lbpath_depth.PutCapabilityFlag( siNotInspectable, false ) ;
		    lbpath_samples.PutCapabilityFlag( siNotInspectable, false ) ;
		    lbno_recursion.PutCapabilityFlag( siNotInspectable, false ) ;
		    lbuseback.PutCapabilityFlag( siNotInspectable, false ) ;
		    //--
			if ( vTracer_method > 1 ) //-- path + photon
			{
				lbpathcaus_photons.PutCapabilityFlag( siNotInspectable, false );
				lbpathcaus_mix.PutCapabilityFlag( siNotInspectable,  false );
				lbpathcaus_depth.PutCapabilityFlag( siNotInspectable,  false );
				lbpathcaus_radius.PutCapabilityFlag( siNotInspectable,  false );
			}
			ctxt.PutAttribute(L"Refresh", true );
		}
		else //-- bidirecctional
		{
            ctxt.PutAttribute(L"Refresh", true );
        }
        }

	//----------------------------logic for world ------------------------------------------------
	  
        if (( changed.GetName() == L"setworld" )||
             ( changed.GetName() == L"buse_ibl" )||
             ( changed.GetName() == L"badd_sun" ))
        {
			vSetworl = prop.GetParameterValue(L"setworld");
			vUse_ibl = prop.GetParameterValue(L"buse_ibl");
            vAdd_sun = prop.GetParameterValue(L"badd_sun");
		
        //-- single color 
		Parameter lbsc_red		= prop.GetParameters().GetItem(L"bsc_red");
		//-- gradient
		Parameter lbhor_red		= prop.GetParameters().GetItem(L"bhor_red");
		Parameter lbzen_red		= prop.GetParameters().GetItem(L"bzen_red");
		Parameter lbhorgro_red	= prop.GetParameters().GetItem(L"bhorgro_red");
		Parameter lbzengro_red	= prop.GetParameters().GetItem(L"bzengro_red");
		//-- texture
		Parameter lbHdri		= prop.GetParameters().GetItem(L"bHdri");
		Parameter lbinterpolate	= prop.GetParameters().GetItem(L"binterp");
		Parameter lbrotation	= prop.GetParameters().GetItem(L"brotation");
        Parameter lbmapworld	= prop.GetParameters().GetItem(L"bmapworld");
		//-- sunsky
		Parameter lbturbidity	= prop.GetParameters().GetItem(L"bturbidity");
		Parameter lbA_horbright = prop.GetParameters().GetItem(L"bA_horbright");
		Parameter lbB_horsprd	= prop.GetParameters().GetItem(L"bB_horsprd");
		Parameter lbC_sunbright = prop.GetParameters().GetItem(L"bC_sunbright");
		Parameter lbD_sunsize	= prop.GetParameters().GetItem(L"bD_sunsize");
		Parameter lbE_backlight = prop.GetParameters().GetItem(L"bE_backlight");
	    Parameter lbfrom_angle	= prop.GetParameters().GetItem(L"bfrom_angle");
		Parameter lbx_vect		= prop.GetParameters().GetItem(L"bx_vect");
		Parameter lbfrom_pos	= prop.GetParameters().GetItem(L"bfrom_pos");
		Parameter lby_vect		= prop.GetParameters().GetItem(L"by_vect");
		Parameter lbfrom_update	= prop.GetParameters().GetItem(L"bfrom_update");	
		Parameter lbz_vect		= prop.GetParameters().GetItem(L"bz_vect");
		Parameter lbadd_sun		= prop.GetParameters().GetItem(L"badd_sun"); 
		Parameter lbskylight	= prop.GetParameters().GetItem(L"bskylight");
		Parameter lbw_samples	= prop.GetParameters().GetItem(L"bw_samples");
		Parameter lbsun_power	= prop.GetParameters().GetItem(L"bsun_power");
		//-- ibl
		Parameter lbuse_ibl		= prop.GetParameters().GetItem(L"buse_ibl"); 
		Parameter lbibl_power	= prop.GetParameters().GetItem(L"bibl_power" );
		Parameter lbibl_samples = prop.GetParameters().GetItem(L"bibl_samples" );
    
	//-- color
		lbsc_red.PutCapabilityFlag( siNotInspectable, true ); 
	//-- IBL
		lbuse_ibl.PutCapabilityFlag( siNotInspectable, true );
		lbibl_power.PutCapabilityFlag( siNotInspectable, true );
		lbibl_samples.PutCapabilityFlag( siNotInspectable, true );
	//-- gradient
		lbhor_red.PutCapabilityFlag( siNotInspectable, true ); 
		lbzen_red.PutCapabilityFlag( siNotInspectable, true );
		lbhorgro_red.PutCapabilityFlag( siNotInspectable, true );
		lbzengro_red.PutCapabilityFlag( siNotInspectable, true );
	//-- texture
		lbHdri.PutCapabilityFlag( siNotInspectable, true ); // zenit color
		lbinterpolate.PutCapabilityFlag( siNotInspectable, true );
		lbrotation.PutCapabilityFlag( siNotInspectable, true );
        lbmapworld.PutCapabilityFlag( siNotInspectable, true );	
	//-- sunsky
		lbturbidity.PutCapabilityFlag( siNotInspectable, true );
		lbA_horbright.PutCapabilityFlag( siNotInspectable, true );
		lbB_horsprd.PutCapabilityFlag( siNotInspectable, true );
		lbC_sunbright.PutCapabilityFlag( siNotInspectable, true );
		lbD_sunsize.PutCapabilityFlag( siNotInspectable, true );
		lbE_backlight.PutCapabilityFlag( siNotInspectable, true );
		lbfrom_angle.PutCapabilityFlag(	siNotInspectable, true );	
		lbx_vect.PutCapabilityFlag(	siNotInspectable, true );
		lbfrom_pos.PutCapabilityFlag( siNotInspectable, true );
		lby_vect.PutCapabilityFlag(	siNotInspectable, true );
		lbfrom_update.PutCapabilityFlag(siNotInspectable, true );
		lbz_vect.PutCapabilityFlag(	siNotInspectable, true );
		lbadd_sun.PutCapabilityFlag( siNotInspectable, true );
		lbskylight.PutCapabilityFlag( siNotInspectable, true );
		lbw_samples.PutCapabilityFlag( siNotInspectable, true );
		lbsun_power.PutCapabilityFlag( siNotInspectable, true ); 
	//---------------->

		if ( vSetworl == 1 )
            { //-- gradient
                lbhor_red.PutCapabilityFlag( siNotInspectable, false );
				lbzen_red.PutCapabilityFlag( siNotInspectable, false ); 
				lbhorgro_red.PutCapabilityFlag( siNotInspectable, false );
				lbzengro_red.PutCapabilityFlag( siNotInspectable, false );
				lbuse_ibl.PutCapabilityFlag( siNotInspectable, false );
                //--
				if ( vUse_ibl )
                {
					lbibl_power.PutCapabilityFlag( siNotInspectable, false );
					lbibl_samples.PutCapabilityFlag( siNotInspectable, false );
				}
                ctxt.PutAttribute(L"Refresh", true);
			}
			else if ( vSetworl == 2 ) 
			{ // texture
				lbHdri.PutCapabilityFlag( siNotInspectable, false ); // zenit color
				lbinterpolate.PutCapabilityFlag( siNotInspectable, false );
				lbrotation.PutCapabilityFlag( siNotInspectable, false );
				lbuse_ibl.PutCapabilityFlag( siNotInspectable, false );
                lbmapworld.PutCapabilityFlag( siNotInspectable, false );	
                //--
				if ( vUse_ibl )
				{
					lbibl_power.PutCapabilityFlag( siNotInspectable, false );
					lbibl_samples.PutCapabilityFlag( siNotInspectable, false );
				}
                ctxt.PutAttribute(L"Refresh", true);
			}
			else if ( vSetworl == 3 ) 
			{ //-- Sunsky
				lbturbidity.PutCapabilityFlag( siNotInspectable, false );
				lbA_horbright.PutCapabilityFlag( siNotInspectable, false );
				lbB_horsprd.PutCapabilityFlag( siNotInspectable, false );
				lbC_sunbright.PutCapabilityFlag( siNotInspectable, false );
				lbD_sunsize.PutCapabilityFlag( siNotInspectable, false );
				lbE_backlight.PutCapabilityFlag( siNotInspectable, false );
				lbfrom_angle.PutCapabilityFlag( siNotInspectable, false );	
				lbx_vect.PutCapabilityFlag( siNotInspectable, false );
				lby_vect.PutCapabilityFlag( siNotInspectable, false );
				lbz_vect.PutCapabilityFlag( siNotInspectable, false );
				lbfrom_update.PutCapabilityFlag( siNotInspectable, false );
				lbfrom_pos.PutCapabilityFlag( siNotInspectable, false );
				lbadd_sun.PutCapabilityFlag( siNotInspectable, false ); //--
				lbskylight.PutCapabilityFlag( siNotInspectable, false );
				lbw_samples.PutCapabilityFlag( siNotInspectable, false );
                //--
                if ( vAdd_sun )
                {
                    lbsun_power.PutCapabilityFlag( siNotInspectable, false );
                }
                ctxt.PutAttribute(L"Refresh", true);	
            } 
			else if ( vSetworl == 4 )
			{ // darktide
				lbibl_power.PutCapabilityFlag( siNotInspectable, true );
				lbibl_samples.PutCapabilityFlag( siNotInspectable, true );
			//	lbuse_ibl.PutCapabilityFlag( siNotInspectable, true );
					ctxt.PutAttribute(L"Refresh", true);
			}
			else //-- color
			{
				lbsc_red.PutCapabilityFlag( siNotInspectable, false );
				lbuse_ibl.PutCapabilityFlag( siNotInspectable, false );
				if ( vUse_ibl )
				{
					lbibl_power.PutCapabilityFlag( siNotInspectable, false );
					lbibl_samples.PutCapabilityFlag( siNotInspectable, false );
				}
                ctxt.PutAttribute(L"Refresh", true);
			}
		} //-- end setworld

	//-- logic for volume integrator 
	if ( changed.GetName() == L"bVintegrator" ) 
	{
		vIntegrator = prop.GetParameterValue( L"bVintegrator" );
		//-- params
		Parameter lbstep_size = prop.GetParameters().GetItem(L"bstep_size");					
		Parameter lbadaptive = prop.GetParameters().GetItem(L"badaptive");						
		Parameter lboptimize = prop.GetParameters().GetItem(L"boptimize");					
		Parameter lbatt_grid = prop.GetParameters().GetItem(L"batt_grid");
		//-- flags
		lbstep_size.PutCapabilityFlag( siNotInspectable, true );			
		lbadaptive.PutCapabilityFlag( siNotInspectable, true );				
		lboptimize.PutCapabilityFlag( siNotInspectable, true );					
		lbatt_grid.PutCapabilityFlag( siNotInspectable, true );

		if ( vIntegrator == 1 ) //-- logic for dynamic UI
		{
			lbstep_size.PutCapabilityFlag( siNotInspectable, false );
			lbadaptive.PutCapabilityFlag( siNotInspectable, false );
			lboptimize.PutCapabilityFlag( siNotInspectable, false );
			lbatt_grid.PutCapabilityFlag( siNotInspectable, false );
			ctxt.PutAttribute(L"Refresh", true);
		} 
		else 
		{ //-- only for resize layout
			ctxt.PutAttribute(L"Refresh", true);
		}
    }

}

//--
XSIPLUGINCALLBACK CStatus YafaRayRenderer_Menu_Init( CRef& in_ctxt )
{
	Context ctxt( in_ctxt );
	Menu oMenu;
	oMenu = ctxt.GetSource();
	MenuItem oNewItem;
	oMenu.AddCallbackItem(L"YafaRay Renderer",L"OnYafaRayRenderer_MenuClicked",oNewItem);
	return CStatus::OK;
}
//--
XSIPLUGINCALLBACK CStatus OnYafaRayRenderer_MenuClicked( XSI::CRef& )
//Implementado; abrir solo una interfaz (from of the last version)
{	Application app;
	CValueArray addpropArgs(5) ;
	addpropArgs[0] = L"YafaRay Renderer"; // Type of Property
	addpropArgs[3] = L"YafaRay Renderer"; // Name for the Property
	addpropArgs[1] = L"Scene_Root";
	bool vAlreadyThere=false;
	CValue retVal ;
	CStatus st;

	// search for existing interface
	CRefArray vItems = app.GetActiveSceneRoot().GetProperties();
	for (int i=0;i<vItems.GetCount();i++){
		if (SIObject(vItems[i]).GetType()==L"YafaRay Renderer") {
			vAlreadyThere=true;
			st = true;
			break;
		}
	}

	if (!vAlreadyThere){
		st = app.ExecuteCommand( L"SIAddProp", addpropArgs, retVal ) ;
	}

	if ( st.Succeeded() )
	{
		CValueArray args(5) ;

		args[0] = prop ;
		args[2] = "YafaRay Renderer";
		args[3] = siLock;
		args[4] = false;

		app.ExecuteCommand( L"InspectObj", args, retVal ) ;
	}

	return st ;
}
//--
bool find(CStringArray a, CString s){ // Returns true if String s is in Array a

	for (int i=0;i<a.GetCount();i++){
		if (a[i]==s) {
			return true;
		}
	}
	return false;
}

//--
CString findInGroup(CString s){

	CRefArray grps = root.GetGroups();

	for (int i=0;i<grps.GetCount();i++){
		CRefArray a=Group(grps[i]).GetMembers();
		 for (int j=0;j<a.GetCount();j++){
			if (X3DObject(a[j]).GetName()==s) {
				return Group(grps[i]).GetName();
			}
		}
	}
	return L"";
}


//--
void yafaray_integrator(yafrayInterface_t *yi){

	//----/ integrator /------>
	yi->paramsClearAll();

	if (vLighting==0)
    {
        yi->paramsSetString("type", "directlighting");

        if (vUse_AO)
        {
			yi->paramsSetBool("do_AO", vUse_AO);
            yi->paramsSetInt("AO_samples", vAO_samples);
    		yi->paramsSetFloat("AO_distance", vAO_distance);
     		yi->paramsSetColor("AO_color", vAOc_red, vAOc_green, vAOc_blue, vAOc_alpha);
        }
		if (vUseCaust)
        {
			yi->paramsSetBool("caustics", vUseCaust);
			yi->paramsSetInt("photons", vDLCphotons);
			yi->paramsSetInt("caustic_mix", vDLCaustic_mix);
			yi->paramsSetInt("caustic_depth", vCaustic_depth);
			yi->paramsSetFloat("caustic_radius", vCaust_radius);
        }
    }
	else if (vLighting == 1) //-- photonmap
    {
		yi->paramsSetString("type", "photonmapping");
       	yi->paramsSetInt("fg_samples", vFg_samples);
		yi->paramsSetInt("photons", vPMDphotons);
       	yi->paramsSetInt("cPhotons", vPMCphotons);
        yi->paramsSetFloat("diffuseRadius", vDiff_radius);
       	yi->paramsSetFloat("causticRadius", vCausticRadius);
		yi->paramsSetInt("search", vSearch);
		yi->paramsSetBool("show_map", vShow_map);
        yi->paramsSetInt("fg_bounces", vFg_bounces);
		yi->paramsSetInt("caustic_mix", vCaustic_mix);
        yi->paramsSetBool("finalGather", vFinal_gather);
		yi->paramsSetInt("bounces", vBounces);
    }
	else if (vLighting == 2) //-- path
    {
        //--
        char A_method [4][7] = {"none", "path", "photon", "both" }; //-- iter vTracer_method
        bool photons = false;
        if ( vTracer_method > 1 ) photons = true;
        string metod;
        metod = char(A_method[vTracer_method]);
        //--
		yi->paramsSetInt("bounces", vPath_depth);
		yi->paramsSetBool("no_recursive", vNo_recursion);
		yi->paramsSetInt("path_samples", vPath_samples);
				
	//	if (vTracer_method == 0 )  //-- none// string(A_method[vTracer_method]).c_str()
    //    {
        yi->paramsSetString("caustic_type", metod.c_str());
    
        if (photons) 
        {
            yi->paramsSetInt("photons", vPTCphotons);
			yi->paramsSetInt("caustic_mix", vPTCaustic_mix);
			yi->paramsSetInt("caustic_depth", vCaustic_depth);
			yi->paramsSetFloat("caustic_radius", vCaust_radius);
        }
        yi->paramsSetString("type", "pathtracing");
    } 
    else  //-- vLighting == 3)
    {
        yi->paramsSetString("type", "bidirectional");
    }
    //-- commons values
		yi->paramsSetInt("raydepth", vRaydepth);
		yi->paramsSetInt("shadowDepth", vShadows_depth);
		yi->paramsSetBool("transpShad", vTransp_shad);
        //--
	yi->createIntegrator("default");
}
//--
void yafaray_volume_integrator(yafrayInterface_t *yi)	//----/ volume integrator /----->
{	
	yi->paramsClearAll();
    //--
	if (vIntegrator == 1)
	{
		yi->paramsSetString("type", "SingleScatterIntegrator");
        yi->paramsSetFloat("stepSize", vStep_size);
        yi->paramsSetBool("adaptive", vAdaptive);
        yi->paramsSetBool("optimize", vOptimize);
	} 
	else 
	{
		yi->paramsSetString("type", "none");
	}
	yi->createIntegrator("volintegr");
}

//--
void yafaray_cam(yafrayInterface_t *yi, X3DObject obj_cam){

	yi->paramsClearAll();

	CString vProj = L"";
 	X3DObject obj_target;
	Camera cam;
	if (obj_cam.GetType()==L"camera") {
		obj_target = obj_cam;
			cam = obj_cam;
	} else {
		obj_target = obj_cam.GetChildren()[0];
			cam = obj_cam.GetChildren()[0];
	}

	//---/ state and transforms /--------->
	KinematicState  gs = obj_target.GetKinematics().GetGlobal();
	CTransformation gt = gs.GetTransform();
	CVector3 tranlation(0,1,1);
	CTransformation target=obj_target.GetKinematics().GetGlobal().GetTransform().AddLocalTranslation(tranlation);
	CVector3 up(target.GetTranslation());
	
	//changed ci for 'cam_int'
	X3DObject ci(obj_cam.GetChildren()[1]);
	CValue vCType=L"pinhole";
	CValue vFdist=0.0, vLensr=0.0, vFocal=0;

	CRefArray cShaders = cam.GetShaders();
	for (int i=0; i < cShaders.GetCount(); i++){
		CString vCSID((Shader(cShaders[i]).GetProgID()).Split(L".")[1]);
		if (vCSID==L"sib_dof") {// Depth_of_field shader found
			vLensr=Shader(cShaders[i]).GetParameterValue(L"strenght");
			vFdist=Shader(cShaders[i]).GetParameterValue(L"auto_focal_distance");
		}
	}
	KinematicState  ci_gs = ci.GetKinematics().GetGlobal();
	CTransformation ci_gt = ci_gs.GetTransform();
	CVector3 vCam_from = gt.GetTranslation();
	CVector3 vCam_to = ci_gt.GetTranslation();
    //--
	double cfrom_X, cfrom_Y, cfrom_Z, cto_X, cto_Y, cto_Z, cup_X, cup_Y, cup_Z;
	//---
    cfrom_X = vCam_from.GetX();	    cto_X = vCam_to.GetX();     cup_X = up.GetX();
	cfrom_Y = -vCam_from.GetZ();	cto_Y = -vCam_to.GetZ();    cup_Y = -up.GetZ();
	cfrom_Z = vCam_from.GetY(); 	cto_Z = vCam_to.GetY();     cup_Z = up.GetY();
	//---
	float vfov;
	if ((int)cam.GetParameterValue(CString(L"fovtype"))==1) {
		// calculate the proper FOV (horizontal -> vertical)
		float hfov = (float)cam.GetParameterValue(CString(L"fov"));
		vfov =(float)(2* atan(1/(float)cam.GetParameterValue(CString(L"aspect")) * tan(hfov/2*PI/180))*180/PI);
	} else {
		// keep vertical FOV
		vfov=(float)cam.GetParameterValue(CString(L"fov"));
	}
	//----/ vCam, create variable for render bloq /------>
	vCam = cam.GetName().GetAsciiString();
	
   // yi->paramsClearAll();
		yi->paramsSetPoint("from", cfrom_X, cfrom_Y, cfrom_Z );
        yi->paramsSetPoint("to", cto_X, cto_Y, cto_Z );
        yi->paramsSetPoint("up", cup_X, cup_Y, cup_Z );
		yi->paramsSetInt("resx", vXRes);
		yi->paramsSetInt("resy", vYRes);
		yi->paramsSetFloat("focal", 1.09);
        yi->paramsSetString("type", "perspective");
        yi->createCamera("camera");
    //--	
}
//--
int yafaray_light(yafrayInterface_t *yi, X3DObject o)
{
	//--- values ---->
 //   X3DObject o;
     
	yi->paramsClearAll();
	//--
	float a=0, b=0, c=0, alpha=0;
	CRefArray shad(Light(o).GetShaders());
	Shader s((Light(o).GetShaders())[0]);
	OGLLight myOGLLight(Light(o).GetOGLLight());
	s.GetColorParameterValue(L"color",a,b,c,alpha );
	//CString lName = findInGroup(o.GetName());
	//----/ transforms /---->
	KinematicState  gs = o.GetKinematics().GetGlobal();
	CTransformation gt = gs.GetTransform();
	//----
	X3DObject li; 
	li= X3DObject(o.GetParent()).GetChildren()[1];
	KinematicState  gs2 = li.GetKinematics().GetGlobal();
	CTransformation gt2 = gs2.GetTransform();
	//---
	CVector3 vLightFrom = gt.GetTranslation(); // from (x, -z, y)
	CVector3 vSpotTo = gt2.GetTranslation(); // to (x, -z, y)
	
    //---
	double from_X, from_Y, from_Z, to_X, to_Y, to_Z;

	from_X = vLightFrom.GetX();		to_X = vSpotTo.GetX();
	from_Y = -vLightFrom.GetZ();	to_Y = -vSpotTo.GetZ();
	from_Z = vLightFrom.GetY();		to_Z = vSpotTo.GetY();
	//---
    float vLightCone = o.GetParameterValue(L"LightCone");
	float vIntensity = s.GetParameterValue(L"intensity");
    string light_name = o.GetName().GetAsciiString();
	
	//-- commons params for all lights; name,  from, target(to) 
	yi->paramsSetColor("color", a, b, c );
	yi->paramsSetPoint("from", from_X, from_Y, from_Z );
	yi->paramsSetFloat("power", vIntensity );
	yi->paramsSetInt("samples", 16); //lamp  samples)

	//--
	CString light_type; 
	string::size_type loc = string(CString(o.GetName()).GetAsciiString()).find( "IES", 0 );
	if (loc != string::npos)
    {
	    light_type = L"IES";
    }
	//--
	if ( light_type == L"IES" ) 
    {
	    //--- use for IES light 
        yi->paramsSetString("type", "ieslight");
		yi->paramsSetPoint("to", to_X, to_Y, to_Z);
	    yi->paramsSetString("file", string(vIES_file.GetAsciiString()).c_str());
		yi->paramsSetBool("soft_shadows", false); 
		yi->paramsSetFloat("cone_angle", vLightCone);
	}
	//--- lights case 
	else if (myOGLLight.GetType() == siLightSpot ){
		//--- Spotlight
		yi->paramsSetFloat("blend", 0.25 );
		yi->paramsSetFloat("cone_angle", vLightCone );
	    yi->paramsSetBool("photon_only", true ); 
		yi->paramsSetFloat("shadowFuzzyness", 1 );
		yi->paramsSetBool("soft_shadows", false );
		yi->paramsSetPoint("to", to_X, to_Y, to_Z );
		yi->paramsSetString("type", "spotlight" );
	
	} else if ( myOGLLight.GetType() == siLightInfinite ) {
		//-- sun light
		CMatrix4 sunTransMat = o.GetKinematics().GetLocal().GetTransform().GetMatrix4();
		double sun_X = sunTransMat.GetValue(2,0); 
		double sun_Y = -sunTransMat.GetValue(2,2); 
		double sun_Z = sunTransMat.GetValue(2,1);
		//--
		yi->paramsSetFloat("angle", 0.5);
		yi->paramsSetPoint("direction", sun_X, sun_Y, sun_Z);
		yi->paramsSetString("type", "sunlight");
	
	}  //-- point light
    else
    {
        bool vSiArealight = o.GetParameterValue(L"LightArea");
	    bool vSiArea_vis = o.GetParameterValue(L"LightAreaVisible");
	    int vlight_geo = o.GetParameterValue(L"LightAreaGeom");
	    //--
	    if (vSiArealight == true) 
        {
            if (vlight_geo == 1)
            {
                // rectangle
			//yi->paramsSetPoint("corner", 1, 2, 3);
			//yi->paramsSetPoint("point1", 1, 2, 3);
			//yi->paramsSetPoint("point2", 1, 2, 3);
			//yi->paramsSetString("type", "arealight");
            }
            if (vlight_geo == 3)
            {
			    // sphere
			    yi->paramsSetFloat("radius", 1);
			    yi->paramsSetString("type", "spherelight");
            }
		}
        else 
        { 
		    yi->paramsSetString("type", "pointlight");
        }
   
	//--
	yi->createLight(light_name.c_str());	
    }
   
	return 0;
}
//--
void yafaray_texture(yafrayInterface_t *yi)
{
    yi->paramsClearAll();
	// cleaning params --------------------->

	Scene scene = app.GetActiveProject().GetActiveScene(); 
	Library matlib = scene.GetActiveMaterialLibrary();
	CRefArray materials = matlib.GetItems();
    	
    for ( LONG i=0; i < materials.GetCount(); i++ ) 
    {   
        Texture vTexture;       //....
		CString vFileBump=L"";	//----/ file  for bump chanel only /----------->
		CString vFile_tex_name; // local variable
		CString vTexType;
        
        ImageClip2 vBumpFile;
		Shader vBumpTex;
		bool vIsSet=false,vIsNorm=false,vFileText=false;
		bool vIsSubD=true,vNorm=false;
		
        Material m( materials[i] );
        if ( (int)m.GetUsedBy().GetCount()==0) 
        {
			continue;
		}
       
		CRefArray shad(m.GetShaders());	// Array of all shaders attached to the material [e.g. phong]
		Shader s(shad[0]);
		CString vMatID((s.GetProgID()).Split(L".")[1]);
		char sname[256];
		strcpy(sname,m.GetName().GetAsciiString());
        vText = false;
        //--
        char A_proc [4] [7]={"none", "clouds", "marble", "wood"};
        int vproced = 0;
        //-- texture values 
		CRefArray mat_shaders = m.GetShaders();
		for (int i=0;i<mat_shaders.GetCount();i++) 
        { //-- shaders connect to material
			CRefArray mat_textures = Shader(mat_shaders[i]).GetShaders();
			for (int j=0;j<mat_textures.GetCount();j++)
            { //-- textures connect to shaders          

				CString vWhat((Shader(mat_textures[j]).GetProgID()).Split(L".")[1]);
				if (vWhat==L"txt2d-image-explicit" || vWhat==L"Softimage.txt2d-image-explicit.1")
                {
                    vTexType = L"image";
                    vTexture=mat_textures[j]; 
				    ImageClip2 vImgClip(vTexture.GetImageClip() );
                 	Source vImgClipSrc(vImgClip.GetSource());
					vFile_tex_name = vImgClipSrc.GetParameterValue( L"path");
					vNorm = (bool)vTexture.GetParameterValue(L"bump_inuse");   
                }
                //--
                vproced = (int)s.GetParameterValue(L"procedural");
                if ( vproced > 0 ) vTexType = A_proc[vproced];
                     
		        //--
		        char A_project [5][9] = {"extend","clip","clipcube","repeat","checker"};
		        int vClipp = s.GetParameterValue(L"projection");
            
		        if (vTexType == L"image")
                {
			        yi->printInfo("Exporter: Creating Texture: \"" + tex_name + "\" type IMAGE");
			        yi->paramsSetString("filename", string(vFile_tex_name.GetAsciiString()).c_str());
			        yi->paramsSetFloat("gamma", vContrast);
			        yi->paramsSetBool("use_alpha", bool(s.GetParameterValue(L"use_alpha")));
			        yi->paramsSetBool("calc_alpha", bool(s.GetParameterValue(L"calc_alpha")));
			        yi->paramsSetFloat("normalmap", bool(s.GetParameterValue(L"normalmap")));
			        yi->paramsSetString("clipping", A_project[vClipp]);
			
			        if (vClipp == 3) //-- repeat
                    {
			    	    yi->paramsSetInt("xrepeat", int(s.GetParameterValue(L"repeat_x")));
			    	    yi->paramsSetInt("yrepeat", int(s.GetParameterValue(L"repeat_y")));
                    }
			        if (vClipp == 4) //-- checker
                    {
				        yi->paramsSetBool("even_tiles", bool( s.GetParameterValue(L"even")));	
				        yi->paramsSetBool("odd_tiles", bool( s.GetParameterValue(L"odd")));
                    }
			        yi->paramsSetFloat("cropmax_x", float(s.GetParameterValue(L"maxx")));	
			        yi->paramsSetFloat("cropmax_y", float(s.GetParameterValue(L"maxy")));
			        yi->paramsSetFloat("cropmin_x", float(s.GetParameterValue(L"minx")));
			        yi->paramsSetFloat("cropmin_y", float(s.GetParameterValue(L"miny")));
			        yi->paramsSetBool("rot90", bool(s.GetParameterValue(L"rot")));
                    //--
			        yi->paramsSetString("type", "image");
				}
                if (vTexType == L"clouds")
                {
                    yi->printInfo("Exporter: Creating Texture: \"" + tex_name + "\" type CLOUDS");
                    yi->paramsSetString("type", "clouds");
                    // noise_size = tex.noise_scale
                    // if  noise_size > 0: noise_size = 1.0/noise_size
                    yi->paramsSetFloat("size", 4);
                    // if tex.noise_type == 'HARD_NOISE' :
                    yi->paramsSetBool("hard", false);                                                   
                    yi->paramsSetInt("depth", 2);

                    // textureConfigured = True
                }
                if (vTexType == L"marble")
                {
                    /*
                    yi->printInfo("Exporter: Creating Texture: \"" + tex_name + "\" type MARBLE")
                    yi->paramsSetString("type", "marble")

                    yi->paramsSetInt("depth", tex.noise_depth)
                    yi->paramsSetFloat("turbulence", tex.turbulence)

                    noise_size = tex.noise_scale
                    if  noise_size > 0:
                    noise_size = 1.0/noise_size

                   if tex.noise_type == 'HARD_NOISE' :
                       hard = True
                   else:
                       hard = False

                   yi->paramsSetFloat("size", noise_size)
                   yi->paramsSetBool("hard", hard )

                   sharp = 4.0
                   if tex.marble_type == 'SOFT':
                   sharp = 2.0
                   elif tex.marble_type == 'SHARP':
                   sharp = 4.0
                   elif tex.marble_type == 'SHARPER':
                   sharp = 8.0

                   yi->paramsSetFloat("sharpness", sharp)
                   yi->paramsSetString("noise_type", noise2string(tex.noise_basis) )

                   if tex.noisebasis_2 == 'SAW'  :
                   ts="saw"
                   elif tex.noisebasis_2 == 'TRI':
                   ts="tri"
                   else:
                   ts = "sin"

                   yi->paramsSetString("shape", ts)

                   textureConfigured = True
                */
                }
                if (vTexType == L"wood")
                {
                    /*
                    elif tex.type == 'WOOD':

                    yi->printInfo("Exporter: Creating Texture: \"" + name + "\" type WOOD")
                    yi->paramsSetString("type", "wood")

                    yi->paramsSetInt("depth", 0)

                    turb       = 0.0
                    noise_size = 0.25
                    hard       = True

                    if tex.wood_type == 'BANDNOISE' or tex.wood_type == 'RINGNOISE':

                turb = tex.turbulence
                noise_size = tex.noise_scale

                if  noise_size > 0:
                    noise_size = 1.0/noise_size
                if tex.noise_type == 'SOFT_NOISE' :
                    hard = False

            yi->paramsSetFloat("turbulence", turb)
            yi->paramsSetFloat("size", noise_size)
            yi->paramsSetBool("hard", hard )

            ts = "bands"

            if tex.wood_type == 'RINGS' or tex.wood_type == 'RINGNOISE':
                ts = "rings"

            yi->paramsSetString("wood_type", ts )
            yi->paramsSetString("noise_type", noise2string(tex.noise_basis) )

            # shape parameter

            if tex.noise_basis == 'SAW'  :
                ts="saw"
            elif tex.noise_basis == 'TRI':
                ts="tri"
            else:
                ts = "sin"

            yi->paramsSetString("shape", ts )

            textureConfigured = True
            */
                }
                //--
                matName = m.GetName().GetAsciiString();
				texName = matName + L"_tex";
                tex_name = string(texName.GetAsciiString()).c_str();

                tex = yi->createTexture(tex_name.c_str());
                texMap[tex_name.c_str()]= tex;
            }
        }
    }

	/*
        elif tex.type == 'VORONOI':

            yi->printInfo("Exporter: Creating Texture: \"" + name + "\" type VORONOI")
            yi->paramsSetString("type", "voronoi")

            if tex.color_mode == 'POSITION':
                ts = "col1"
            elif tex.color_mode  == 'POSITION_OUTLINE':
                ts = "col2"
            elif tex.color_mode  == 'POSITION_OUTLINE_INTENSITY':
                ts = "col3"
            else:
                ts = "int"

            yi->paramsSetString("color_type", ts)

            yi->paramsSetFloat("weight1", tex.weight_1)
            yi->paramsSetFloat("weight2", tex.weight_2)
            yi->paramsSetFloat("weight3", tex.weight_3)
            yi->paramsSetFloat("weight4", tex.weight_4)

            yi->paramsSetFloat("mk_exponent", tex.minkovsky_exponent)
            yi->paramsSetFloat("intensity", tex.noise_intensity)

            noise_size = tex.noise_scale
            if  noise_size > 0:
                noise_size = 1.0/noise_size
            yi->paramsSetFloat("size", noise_size)

            ts = "actual"
            if tex.distance_metric == 'DISTANCE_SQUARED':
                ts = "squared"
            elif tex.distance_metric == 'MANHATTAN':
                ts = "manhattan"
            elif tex.distance_metric == 'CHEBYCHEV':
                ts = "chebychev"
            elif tex.distance_metric == 'MINKOVSKY_HALF':
                ts = "minkovsky_half"
            elif tex.distance_metric == 'MINKOVSKY_FOUR':
                ts = "minkovsky_four"
            elif tex.distance_metric == 'MINKOVSKY':
                ts = "minkovsky"

            yi->paramsSetString("distance_metric", ts)

            textureConfigured = True

        elif tex.type == 'MUSGRAVE':

            yi->printInfo("Exporter: Creating Texture: \"" + name + "\" type MUSGRAVE")
            yi->paramsSetString("type", "musgrave")

            ts = "fBm"
            if tex.musgrave_type == 'MULTIFRACTAL'  :
                ts = "multifractal"
            elif tex.musgrave_type == 'RIDGED_MULTIFRACTAL':
                ts = "ridgedmf"
            elif tex.musgrave_type == 'HYBRID_MULTIFRACTAL':
                ts = "hybridmf"
            elif tex.musgrave_type == 'HETERO_TERRAIN':
                ts = "heteroterrain"

            yi->paramsSetString("musgrave_type", ts)
            yi->paramsSetString("noise_type", noise2string(tex.noise_basis))
            yi->paramsSetFloat("H", tex.dimension_max)
            yi->paramsSetFloat("lacunarity", tex.lacunarity)
            yi->paramsSetFloat("octaves", tex.octaves)

            noise_size = tex.noise_scale
            if  noise_size > 0:
                noise_size = 1.0/noise_size
            yi->paramsSetFloat("size", noise_size)

            yi->paramsSetFloat("intensity", tex.offset)

            textureConfigured = True

        elif tex.type == 'DISTORTED_NOISE':

            yi->printInfo("Exporter: Creating Texture: \"" + name + "\" type DISTORTED NOISE")
            yi->paramsSetString("type", "distorted_noise")

            yi->paramsSetFloat("distort", tex.distortion)

            noise_size = tex.noise_scale
            if  noise_size > 0:
                noise_size = 1.0/noise_size
            yi->paramsSetFloat("size", noise_size)

            yi->paramsSetString("noise_type1", noise2string(tex.noise_basis))
            yi->paramsSetString("noise_type2", noise2string(tex.noise_distortion))

            textureConfigured = True
			*/

}
//--
void yafaray_material(yafrayInterface_t *yi)
{
    //--
	Scene scene = app.GetActiveProject().GetActiveScene();
	Library matlib = scene.GetActiveMaterialLibrary();
    CRefArray materials = matlib.GetItems();
	
    //--
    float red=0.0f,green=0.0f,blue=0.0f,alpha=0.0f;
	bool vIsSet=false,vIsNorm=false,vFileText=false;
	bool vIsSubD=true,vNorm=false;
    //--
   
	for ( LONG i=0; i < materials.GetCount(); i++ ) 
    {
        Material m( materials[i] );
       
		if ( (int)m.GetUsedBy().GetCount()==0) 
        {
			continue; // exit to for cicle
		}
		CRefArray shad(m.GetShaders());	// Array of all shaders attached to the material [e.g. phong]
     //   app.LogMessage(L"name shaders "+ CString(shad.GetAsText()));
     //   app.LogMessage(L"num shaders  "+ CString(shad.GetCount()));
        Shader s(shad[0]); 
		CString vMatID((s.GetProgID()).Split(L".")[1]);
       
		char sname[256];
		strcpy(sname,m.GetName().GetAsciiString()); 
      
		if ( find(aMatList, m.GetName() ) ) 
        {
			continue; // exit, if name of material exist into list materials
		} 
        else 
        {
			aMatList.Add(m.GetName());
		}
        //--
        mat_name = string ( m.GetName().GetAsciiString());
        map_name = mat_name + "_map";
        string t_name = mat_name + "_tex";
        tex_name = texMap.find(t_name.c_str())->first;
        app.LogMessage( tex_name.c_str());
        // mat = mMap.find(mat_name.c_str())->second;

        string layer_name ; 
        int n_lay = 0;
        string diffRoot, bumpRoot, mircolRoot, mirrRoot, transluRoot, transpRoot, glossRoot, glossrefRoot, filtcolRoot;
		//--
        yi->paramsClearAll();
        
        //--
        if ( vMatID==L"yaf_glass" || vMatID==L"yaf_roughglass" ) 
        { //-- glass or rough glass 
			yi->paramsSetFloat("IOR", float(s.GetParameterValue(L"ior")));
				s.GetColorParameterValue(L"absorption",red, green, blue, alpha );
			yi->paramsSetColor("absorption", red, green, blue, alpha);
			yi->paramsSetFloat("absorption_dist", float(s.GetParameterValue(L"absorption_dist")));
		    yi->paramsSetFloat("dispersion_power", float(s.GetParameterValue(L"dispersion")));
		    yi->paramsSetFloat("exponent", float(s.GetParameterValue(L"exponent")));
		    yi->paramsSetBool("fake_shadows", bool(s.GetParameterValue(L"fake_shadows")));
				s.GetColorParameterValue(L"filter_color",red, green, blue, alpha );
			yi->paramsSetColor("filter_color", red, green, blue);
				s.GetColorParameterValue(L"mirror_color",red, green, blue, alpha );
			yi->paramsSetColor("mirror_color", red, green, blue, alpha);
			yi->paramsSetFloat("transmit_filter", float(s.GetParameterValue(L"transmit_filter"))); 
			if ((float)s.GetParameterValue(L"exponent")> 0)
            { 
				yi->paramsSetString("type", "rough_glass");
            } 
            else 
            {
                yi->paramsSetString("type", "glass");
            }
            //-- layers
            if ((bool)s.GetParameterValue(L"diff_layer")== true)
            {
                layer_name = "diff_layer";
                yi->paramsSetString("diffuse_shader", "diff_layer");
                n_lay++;
                diffRoot = layer_name;
            }
		    if ((bool)s.GetParameterValue(L"bump_layer") == true) 
            {
	    	    layer_name = "bump_layer";
                yi->paramsSetString("bump_shader", "bump_layer");
                n_lay++ ;
                bumpRoot = layer_name;
            }  
		    if ((bool)s.GetParameterValue(L"ch_cmir")==true) //----/ mirror color layer /---->
            {
                layer_name = "mircol_layer";
                yi->paramsSetString("mirror_color_shader", "mircol_layer");
                n_lay++;
                mircolRoot = layer_name;
		    }
		}
        else if ( vMatID==L"yaf_glossy" || vMatID==L"yaf_coated_glossy" )
        {
            yi->paramsSetFloat("IOR", float(s.GetParameterValue(L"ior")));
		    yi->paramsSetBool("anisotropic", bool(s.GetParameterValue(L"anisotropic")));
		    yi->paramsSetBool("as_diffuse", bool(s.GetParameterValue(L"as_diffuse"))); 
			    s.GetColorParameterValue(L"diffuse",red,green,blue,alpha );
		    yi->paramsSetColor("diffuse_color", red, green, blue, alpha);
			    s.GetColorParameterValue(L"glossy",red,green,blue,alpha );
		    yi->paramsSetColor("color", red, green, blue, alpha);
		    yi->paramsSetFloat("diffuse_reflect", float(s.GetParameterValue(L"diffuse_reflec")));
		    yi->paramsSetFloat("exp_u",	float(s.GetParameterValue(L"exponent_hor")));
		    yi->paramsSetFloat("exp_v",	float(s.GetParameterValue(L"exponent_ver")));
		    yi->paramsSetFloat("exponent", float(s.GetParameterValue(L"exponent")));
		    yi->paramsSetFloat("glossy_reflect", float(s.GetParameterValue(L"reflect_glossy")));
		    yi->paramsSetFloat("sigma", float(s.GetParameterValue(L"sigma")));
		    if ( (float)s.GetParameterValue(L"ior") > 0 )
            {
			    yi->paramsSetString("type", "coated_glossy"); 
            }
            else 
            {
                yi->paramsSetString("type", "glossy");
            }
	        //-- shaders
	        if ((bool)s.GetParameterValue(L"diff_layer")== true)
            {
                layer_name = "diff_layer";
                yi->paramsSetString("diffuse_shader", "diff_layer");
                n_lay++;
                diffRoot = layer_name;
            }  
		    if ((bool)s.GetParameterValue(L"ch_spec")==true) 
            {
			    layer_name = "glossref_layer";
                yi->paramsSetString("glossy_reflect_shader", "glossref_layer");
                n_lay++;
                glossrefRoot = layer_name;
		    }
		    if ((bool)s.GetParameterValue(L"ch_cspec")==true) 
            {
                layer_name = "gloss_layer";
                yi->paramsSetString("glossy_shader", "gloss_layer");
                n_lay++;
                glossRoot = layer_name;
                //-
		    } 
	        if ((bool)s.GetParameterValue(L"bump_layer") == true) 
            {
	    	    layer_name = "bump_layer";
                yi->paramsSetString("bump_shader", "bump_layer");
                n_lay++ ;
                bumpRoot = layer_name;
            }  	
        }
        else if (vMatID==L"yaf_shinydiffusemat")
        {   
            yi->paramsSetFloat("IOR", float(s.GetParameterValue(L"ior")));
			    s.GetColorParameterValue(L"diffuse_color",red,green,blue,alpha );
            if ((int)s.GetParameterValue(L"brdf")== 0)
            {
	    	    yi->paramsSetString("diffuse_brdf", "oren_nayar");
	    	    yi->paramsSetFloat("sigma", float(s.GetParameterValue(L"sigma")));
            }
            else 
            {
                yi->paramsSetString("diffuse_brdf", "Normal (Lambert)");
            }
		    yi->paramsSetColor("color", red, green, blue, alpha);
		    yi->paramsSetFloat("diffuse_reflect", float(s.GetParameterValue(L"diffuse_reflec")));
		    yi->paramsSetFloat("emit", float(s.GetParameterValue(L"emit")));
		    yi->paramsSetBool("fresnel_effect", bool(s.GetParameterValue(L"fresnel")));
			    s.GetColorParameterValue(L"mirror_color",red,green,blue,alpha );
		    yi->paramsSetColor("mirror_color", red, green, blue, alpha);
    		yi->paramsSetFloat("specular_reflect", float(s.GetParameterValue(L"mirror_strength")));
	    	yi->paramsSetFloat("translucency", float(s.GetParameterValue(L"translucency"))); // proba
		    yi->paramsSetFloat("transmit_filter", float(s.GetParameterValue(L"transmit_filter")));
		    yi->paramsSetFloat("transparency", float(s.GetParameterValue(L"transparency")));
		
            //-- layers
		    if ((bool)s.GetParameterValue(L"diff_layer")== true)
            {
                layer_name = "diff_layer";
                yi->paramsSetString("diffuse_shader", "diff_layer");
                diffRoot = layer_name;
                n_lay++;
            } 
		    if ((bool)s.GetParameterValue(L"mircol_layer")==true)
            {
                layer_name = "mircol_layer";
                yi->paramsSetString("mirror_color_shader", "mircol_layer"); 
                mircolRoot = "mircol_layer";
                n_lay++;
            }
    	    if ((bool)s.GetParameterValue(L"mirr_layer")==true) 
            {
                layer_name = "mirr_layer";
                yi->paramsSetString("mirror_shader", "mirr_layer");
                mirrRoot = layer_name;
                n_lay++;
            } 
		    if ((bool)s.GetParameterValue(L"ch_translu")==true) 
            {
                layer_name = "translu_layer";
                yi->paramsSetString("translucency_shader", "translu_layer");
                transluRoot = layer_name;
                n_lay++;
		    } 
		    if ((bool)s.GetParameterValue(L"bump_layer") == true) 
            {
	    	    layer_name = "bump_layer";
                yi->paramsSetString("bump_shader", "bump_layer");
                bumpRoot = layer_name;
                n_lay++ ;
            } 
		    if ((bool)s.GetParameterValue(L"ch_alpha")==true) 
            {
			    layer_name = "transp_layer";
                yi->paramsSetString("transparency_shader", "transp_layer");
                transpRoot = layer_name;
                n_lay++;
		    } 

		    yi->paramsSetString("type", "shinydiffusemat"); 
		    

        //	} else if (vMatID==L"yaf_blendMaterial"){ //TODO; create blend mat
        }
        else 
        {   // fall back shader
		    yi->paramsSetColor("color", 0.9f, 0.9f, 0.9f, 1.0f);
		    yi->paramsSetString("type", "shinydiffusemat"); 
        }
    
        //--
        bool d_color = false, d_scalar = false, is_image = false;
        if ( m.GetAllImageClips().GetCount() > 0 ) is_image = true; //-- lack voronoi option
        //--
        if ( n_lay > 0 ) //-- list_element == layers ?
        {
            for ( int k = 0; k< n_lay; k++ )
            {
                //--
                if (diffRoot != "") {
                    layer_name = diffRoot;    d_color = true;    d_scalar = false;
                    diffRoot = "";
                } 
                else if (bumpRoot != "") {
                    layer_name = bumpRoot;    d_color = false;   d_scalar = true;
                    vNorm = true;    bumpRoot = "";
                }
                else if (mircolRoot   != "") { 
                    layer_name = mircolRoot;    d_color = true;    d_scalar = false;   
                    mircolRoot   = "";
                } 
                else if (mirrRoot     != "") { layer_name = mirrRoot;     mirrRoot     = "";
                } 
                else if (transluRoot  != "") { layer_name = transluRoot;  transluRoot  = "";
                } 
                else if (transpRoot   != "") { layer_name = transpRoot;   transpRoot   = "";
                } 
                else if (glossRoot    != "") { layer_name = glossRoot;    glossRoot    = "";
                } 
                else if (glossrefRoot != "") { layer_name = glossrefRoot; glossrefRoot = "";
                } 
                else if (filtcolRoot  != "") { layer_name = filtcolRoot;  filtcolRoot  = "";
                } 
                else 
                { 
                    app.LogMessage( L" end layers\n");
                }
            
                
                //string map_name = mat_name;
                //+= string(CString(k).GetAsciiString());
                
                yi->paramsPushList();
                if ( d_color == true ){
                    yi->paramsSetFloat("colfac", float(s.GetParameterValue(L"infl_color")));// chcolor 
                }
	            yi->paramsSetBool("color_input", is_image); // TODO; is Image or voronoi
			    s.GetColorParameterValue(L"def_col",red,green,blue,alpha );
		        yi->paramsSetColor("def_col", red, green, blue, alpha);
		        yi->paramsSetFloat("def_val", float(s.GetParameterValue(L"dvar")));
		        yi->paramsSetBool("do_color", d_color);
		        yi->paramsSetBool("do_scalar", d_scalar); //----/>
		        yi->paramsSetString("element", "shader_node");
                yi->paramsSetString("type", "layer");
                yi->paramsSetString("name", layer_name.c_str());  //----/ A_layers nodes /------>
                yi->paramsSetString("input", map_name.c_str()); //"map0");
		        yi->paramsSetInt("mode", int(s.GetParameterValue(L"bmode")));

		        yi->paramsSetBool("negative", bool(s.GetParameterValue(L"negative")));
		        yi->paramsSetBool("noRGB", bool(s.GetParameterValue(L"nrgb")));
		        yi->paramsSetBool("stencil", bool(s.GetParameterValue(L"stencil")));  
    			
			    s.GetColorParameterValue(L"diffuse_color",red,green,blue,alpha );
		        yi->paramsSetColor("upper_color", red, green, blue, 1.0);
		        yi->paramsSetFloat("upper_value", 1.0f); // TODO;
                if (d_scalar){
                    yi->paramsSetFloat("valfac", float(s.GetParameterValue(L"dvar"))); // other var
                }
		        yi->paramsSetBool("use_alpha", bool(s.GetParameterValue(L"use_alpha")));
                yi->paramsEndList();
  			
	            //-- Map input 
			
			    char aMap [10][12]={"global", "transformed", "uv", "orco", "stick",
			    					"window", "normal", "reflect", "stress", "tangent"};
       		    int fMap =	s.GetParameterValue(L"from_map"); //---/ for texco value /---->

			    char aType [4][7]={"plain","tube","cube","sphere"};
			    int tMap =	s.GetParameterValue(L"map_type"); //----/ for mapping value /---->
                
			    //-- Transform, translate 
                float vOff_x = s.GetParameterValue(L"ofset_x"); // in SPDL file
			    float vOff_y = s.GetParameterValue(L"ofset_y");
			    float vOff_z = s.GetParameterValue(L"ofset_z");
			    //-- scale 
			    float vSize_x =	s.GetParameterValue(L"size_x"); // in SPDL file
			    float vSize_y = s.GetParameterValue(L"size_y");
			    float vSize_z = s.GetParameterValue(L"size_z");
			    //----/ projection map /---->
			    int vPmap_x = s.GetParameterValue(L"combo_x"); // in SPDL file
			    int vPmap_y = s.GetParameterValue(L"combo_y");
			    int vPmap_z = s.GetParameterValue(L"combo_z");
                float bump_factor = s.GetParameterValue(L"bump_fac");
                //--
                yi->paramsPushList(); // Map input -------->
                if ( vNorm )   
                {
				    yi->paramsSetFloat("bump_strength", bump_factor);
                }
			    yi->paramsSetString("element", "shader_node");
			    yi->paramsSetString("type", "texture_mapper");
			    yi->paramsSetString("name", map_name.c_str()); // map0
                yi->paramsSetString("texture", tex_name.c_str()); // Tex_name
			    yi->paramsSetString("mapping", aType[tMap]);
			    yi->paramsSetPoint("offset", vOff_x, vOff_y, vOff_z);
			    yi->paramsSetInt("proj_x", vPmap_x);
			    yi->paramsSetInt("proj_y", vPmap_y);
			    yi->paramsSetInt("proj_z", vPmap_z);
			    yi->paramsSetPoint("scale", vSize_x, vSize_y, vSize_z);
			    yi->paramsSetString("texco", aMap[fMap]);
                yi->paramsEndList();
            }
        }
        
        mat_name = string(m.GetName().GetAsciiString()).c_str();
        mat =  yi->createMaterial(mat_name.c_str()); 
        mMap[mat_name.c_str()]= mat; 
     }
}     	
//--
int yafaray_object(yafrayInterface_t *yi, X3DObject o ){
		// Writes objects
	CScriptErrorDescriptor status ;
	CValueArray fooArgs(1) ;
	fooArgs[0] = L"" ;
	CValue retVal=false ;
	bool vIsMeshLight=false, vIsSet=false, vText=false, vIsSubD=false, vIsMod=false;
	Geometry g(o.GetActivePrimitive().GetGeometry()) ;
	
	CRefArray mats(o.GetMaterials()); // Array of all materials of the object
	Material m = mats[0];
	CRefArray shad(m.GetShaders());	// Array of all shaders attached to the material [e.g. phong]
	Shader s(shad[0]);
    CString oMat = o.GetMaterial().GetName().GetAsciiString();
    mat_name = string(oMat.GetAsciiString()).c_str();
   
	//----/ search for texture image map ( UV data ) /---->
	CRefArray mat_shaders=m.GetShaders();
	for (int i=0;i<mat_shaders.GetCount();i++)
    {
		CRefArray mat_textures=Shader(mat_shaders[i]).GetShaders();
		for (int j=0;j<mat_textures.GetCount();j++)
        {
			CString vWhat((Shader(mat_textures[j]).GetProgID()).Split(L".")[1]);
			if (vWhat==L"txt2d-image-explicit" || vWhat==L"Softimage.txt2d-image-explicit.1")
            {
				vText=true;
            }
        }
    }
    //----/ end search for UV data /---->
	
	CGeometryAccessor in_ga;
	CString wFaceIdx, wVertex, wNorVect, wNorIdx, wUVIdx, wUvVect;
	
    //----//----------->
	int subdLevel=0;
	Property geopr=o.GetProperties().GetItem(L"Geometry Approximation");
	if ((int)geopr.GetParameterValue(L"gapproxmordrsl")>0 || (int)geopr.GetParameterValue(L"gapproxmosl")>0)
    {
		vIsSubD=true;
		subdLevel=(int)geopr.GetParameterValue(L"gapproxmordrsl");
    }
    else 
    {
        vIsSubD=false;
    }
//--- transforms
	KinematicState  gs = o.GetKinematics().GetGlobal();
	CTransformation gt = gs. GetTransform();
	
//----/ XSISDK, import-export, 06/06/10 /---------------->
	siConstructionMode constMode = (siConstructionModeModeling);
		siSubdivisionRuleType subdType = (siCatmullClark);
			in_ga = PolygonMesh(g).GetGeometryAccessor( constMode, subdType, subdLevel ); 
	//--
    yi->paramsClearAll(); 
	yi->startGeometry();
	    
    //--
	int vertexCount(in_ga.GetVertexCount());
	int facesCount(in_ga.GetNodeCount()/2);
	unsigned int ID = yi->getNextFreeID();
    //--
    mat = mMap.find(mat_name.c_str())->second;
   
	CVector3 vTrans(gt.GetTranslation()); // change by multbymatriz
	CFloatArray vNvector;
		in_ga.GetNodeNormals(vNvector);
	CDoubleArray vtxPos;
		in_ga.GetVertexPositions(vtxPos);
	//--
	yi->startTriMesh(ID, vertexCount, facesCount, false, vText, 0);
	//--
	LONG valCount = vtxPos.GetCount();
		for ( LONG i=0; i < valCount; )
		{
			size_t x = i++, z = i++, y = i++;
			yi->addVertex(vtxPos[x], -vtxPos[y], vtxPos[z]);
			//if(vSmooth_mesh == true || subdLevel > 0) // yet not work in my compilation of Yafaray
			//	yi->addNormal(vNvector[x], vNvector[y], vNvector[z]);
		}
    //----// uv_vectors -getnodecount//---------->
	CRefArray uvs = in_ga.GetUVs();
	LONG nUVs = uvs.GetCount();
	 // iterate over the uv data
    for ( LONG i=0; i<nUVs; i++ )
	{
        ClusterProperty uv(uvs[i]);
	    // get the values
        CFloatArray uvValues;
        uv.GetValues( uvValues );

        // log the values
        LONG nValues = uvValues.GetCount();
        for ( LONG idxElem=0; idxElem<nValues; idxElem += 3)
        {
            yi->addUV(uvValues[idxElem], uvValues[idxElem + 1]); 
        }
    }
    //--
	CLongArray triVtxIdx;
	in_ga.GetTriangleVertexIndices(triVtxIdx);
    //--
	CLongArray triSizeArray(in_ga.GetTriangleCount());
	CLongArray numVert;
	in_ga.GetPolygonVerticesCount(numVert);
    //--
	CLongArray triNodeIdx;
	in_ga.GetTriangleNodeIndices(triNodeIdx);
    //--
    for (LONG i=0, offset=0; i<triSizeArray.GetCount(); i++) // mio
    {
        yi->addTriangle(triVtxIdx[offset], triVtxIdx[offset + 1], triVtxIdx[offset + 2], 
						triNodeIdx[offset], triNodeIdx[offset + 1], triNodeIdx[offset + 2], mat);
        offset += 3; 
    }
//--
    int vAngle=0;
    if ((vSmooth_mesh==true)&&(vIsSubD==false)) { vAngle=90;} // if false, vAngle=0, not smooth
	if ((vSmooth_mesh==true)&&(vIsSubD==true)) { vAngle=181;} // maxim smooth
			
	    yi->endTriMesh();
	    yi->smoothMesh(0, vAngle);
        yi->endGeometry();
	return 0;
}
//--
void yafaray_world(yafrayInterface_t *yi) 
{
    yi->paramsClearAll();

	// Look if there is an background image for Image Based Lighting (e.g. HDRI)
	CRefArray aEnv = app.GetActiveProject().GetActiveScene().GetActivePass().GetNestedObjects();
	for (int i=0;i<aEnv.GetCount();i++)
    {
        if (SIObject(aEnv[i]).GetName()==L"Environment Shader Stack") 
        {
            CRefArray aImages = SIObject(aEnv[i]).GetNestedObjects();
            for (int j=0;j<aImages.GetCount();j++)
            {
                if (SIObject(aImages[j]).GetType()==L"Shader")
                {
                    Shader s(aImages[j]);
                    CRefArray aEnvImg=s.GetImageClips();
                    for (int k=0;k<aEnvImg.GetCount();k++)
                    {
                        ImageClip2 vImgClip(aEnvImg[k]);
                        Source vImgClipSrc(vImgClip.GetSource());
                        CString vFileName = vImgClipSrc.GetParameterValue( L"path");
				        if (vFileName!=L"")
                        {
                            vHDRI=vFileName.GetAsciiString();
					        break;
                        }
                    }
                }
            }
			break;
        }
    }
	//-------------
	if (vSetworl==4) // darktide
    {
		//f = world.bg_from
        // yi->paramsSetPoint("from", f[0], f[1], f[2])
        // yi->paramsSetFloat("altitude", world.bg_dsaltitude)
	    yi->paramsSetFloat("turbidity", vTurbidity);
        yi->paramsSetFloat("a_var", vHorbright);
		yi->paramsSetBool("add_sun", vAdd_sun);
		yi->paramsSetFloat("b_var", vHorsprd);
		yi->paramsSetBool("background_light", vSkylight);
		yi->paramsSetFloat("c_var", vSunbright);
		yi->paramsSetFloat("d_var", vSunsize);
		yi->paramsSetFloat("e_var", vBacklight);
		yi->paramsSetBool("with_caustic", true);
        yi->paramsSetBool("with_diffuse", true);
        yi->paramsSetFloat("sun_power", vSun_power);
        yi->paramsSetInt("light_samples", vW_samples);
	    yi->paramsSetFloat("power", vW_power);       
        //        yi->paramsSetFloat("bright", bg_dsbright)
        //        yi->paramsSetBool("night", false); // TODO: create menu
        //        yi->paramsSetFloat("exposure", bg_exposure)
        yi->paramsSetBool("clamp_rgb", false);
        yi->paramsSetBool("gamma_enc", false);
        yi->paramsSetString("type", "darksky");
    }
    else if (vSetworl==3)  // sunsky
    {
	    yi->paramsSetFloat("a_var", vHorbright);
		yi->paramsSetBool("add_sun", vAdd_sun);
		yi->paramsSetFloat("b_var", vHorsprd);
		yi->paramsSetBool("background_light", vSkylight);
		yi->paramsSetFloat("c_var", vSunbright);
		yi->paramsSetFloat("d_var", vSunsize);
		yi->paramsSetFloat("e_var", vBacklight);
		//yi->paramsSetPoint("from", f[0], f[1], f[2]); // not defined /------>
		yi->paramsSetInt("light_samples", vW_samples);
		yi->paramsSetFloat("power", vW_power);
		yi->paramsSetFloat("sun_power", vSun_power);
		yi->paramsSetFloat("turbidity", vTurbidity);
		yi->paramsSetString("type", "sunsky");
    }
	else if (vSetworl == 2) //-- texture
    {
        yi->paramsSetFloat("exposure_adjust", 0.5); // TODO
		yi->paramsSetString("filename", string(vHDRI.GetAsciiString()).c_str());
		if (vUse_interp) 
        { 
			yi->paramsSetString("interpolate", "bilinear");
		} 
        else
        {
			yi->paramsSetString("interpolate", "none");
        }
        if ( vmapworld == 0 )
        {
            yi->paramsSetString("mapping", "probe");
        }
        else if ( vmapworld == 1 )
        {
            yi->paramsSetString("mapping", "sphere");
        }
        else
        {
            yi->printWarning("World texture mapping neither Sphere or AngMap!");
        }
        yi->paramsSetString("type", "image");
		yi->createTexture("world_texture");
			
	//----/ IBL options /------------>
		yi->paramsSetString("type", "textureback");
		yi->paramsSetString("texture", "world_texture");
		yi->paramsSetBool("ibl", vUse_ibl);
        yi->paramsSetBool("with_caustic", true); //
        yi->paramsSetBool("with_diffuse", true);
		yi->paramsSetInt("ibl_samples", vIbl_samples);
        yi->paramsSetFloat("power", vW_power);
        yi->paramsSetFloat("rotation", vrotation);
    }
    else if (vSetworl == 1) // gradient
    {
		yi->paramsSetColor("horizon_color", vHor_red, vHor_green, vHor_blue, vHor_alpha);
		yi->paramsSetColor("horizon_ground_color", vHorgro_red, vHorgro_green, vHorgro_blue, vHorgro_alpha);
		yi->paramsSetBool("ibl", vUse_ibl);
		yi->paramsSetFloat("power", vW_power);
		yi->paramsSetString("type", "gradientback");
		yi->paramsSetColor("zenith_color", vZen_red, vZen_green, vZen_blue, vZen_alpha);
		yi->paramsSetColor("zenith_ground_color", vZengro_red, vZengro_green, vZengro_blue, vZengro_alpha);

	} 
    else  //----/ constant /---->
    {
		yi->paramsSetColor("color", vBack_red, vBack_green, vBack_blue, vBack_alpha);
		yi->paramsSetBool("ibl", vUse_ibl);
		yi->paramsSetFloat("power", vW_power);
		yi->paramsSetString("type", "constant");

	}
	if (vUse_ibl == true)
    {
		yi->paramsSetInt("ibl_samples", vIbl_samples);
    }
	yi->createBackground("world_background");

}
//--
void yafaray_render(yafrayInterface_t *yi){
    // create render -------------------->
    char A_tile_order [2] [7] = {"random", "linear"};
		yi->paramsClearAll();   
		//---
		yi->paramsSetInt("AA_inc_samples", vAA_inc );
		yi->paramsSetInt("AA_minsamples", vAA_samples );
		yi->paramsSetInt("AA_passes", vAA_passes );
		yi->paramsSetFloat("AA_pixelwidth", vAA_pixel );
		yi->paramsSetFloat("AA_threshold", vAA_threshold );
		yi->paramsSetString("filter_type", Aa_mode[vFilter_type] );
		//--
		yi->paramsSetString("background_name", "world_background");
		yi->paramsSetString("camera_name", "camera");
		yi->paramsSetString("integrator_name", "default");
		yi->paramsSetString("volintegrator_name", "volintegr");
	    yi->paramsSetBool("clamp_rgb", vClamp_rgb);
		yi->paramsSetString("customString",  string(vCustom_string.GetAsciiString()).c_str());
		yi->paramsSetBool("drawParams", vDraw_render);
		yi->paramsSetFloat("gamma", vContrast);
		yi->paramsSetBool("premult", vPremu_alpha);
		yi->paramsSetBool("show_sam_pix", false); // TODO; create option menu
		if (vAuto_thread==true){
	       yi->paramsSetInt("threads", -1 ); 
	   } else {
	       yi->paramsSetInt("threads", vThreads ); 
		   }
        yi->paramsSetInt("tile_size", vtilesize); // TODO;
        yi->paramsSetString("tiles_order", A_tile_order[vtileorder]);
	    yi->paramsSetInt("width", vXRes);
        yi->paramsSetInt("height", vYRes);
        yi->paramsSetInt("xstart", 0);
	    yi->paramsSetInt("ystart", 0);
			

}
//--
 CString readIni(){

	char x;
	CString data,iniPath;
	ifstream load;

	//CString iniPath;
		iniPath = app.GetInstallationPath(siUserPath);
//		app.LogMessage(L"userdir:"+ iniPath);
	#ifdef __unix__
		iniPath += L"/YafXSI.ini";
	#else
		iniPath += L"\\YafXSI.ini";
	#endif
//		app.LogMessage(L""+iniPath);
		load.open( iniPath.GetAsciiString() );

	while(load.get(x)) {
      data+=x;
   }
 //  app.LogMessage(L"" + CString(data));
   load.close();
   return data;
}
//--
int api_yaf()

{
    if ( out_type == "xml_file" )
    {
	    yi = new xmlInterface_t();
    }
    else
    {
        yi = new yafrayInterface_t();
	    yi->loadPlugins(string("C:/Yafaray/msvc/plugins").c_str()); //you may call loadPlugins with additional paths
    }
    
	
    //first of all, start scene
	yi->startScene();
    

	// create arrays of objects --------->
	CRefArray array, aYaflight, aMesh, aCam; // create a ref array object.
	CStringArray emptyArray;
    //--- cleaning arrays
	emptyArray.Clear();
	array.Clear();
	aYaflight.Clear();
	aMesh.Clear();
    aCam.Clear();
	aMatList.Clear();
		
	// detecting visibility of elements to scene -->
	array += root.FindChildren( L"", L"", emptyArray, true );
	for ( int i=0; i < array.GetCount();i++ )
    {
	    X3DObject o(array[i]);
        
		//--
		Property visi=o.GetProperties().GetItem(L"Visibility");
		if (o.GetType()==L"light")
        {
			if (vIsHiddenLight || (vIsHiddenLight == false && ((bool)visi.GetParameterValue(L"viewvis")==true && (bool)visi.GetParameterValue(L"rendvis")==true)))
            {
                aYaflight.Add(o);	// visibilty check
            }
        }
		if (o.GetType()==L"polymsh")
        {
			if (vIsHiddenObj || (vIsHiddenObj == false && ((bool)visi.GetParameterValue(L"viewvis")==true && (bool)visi.GetParameterValue(L"rendvis")==true))) 
            {
                aMesh.Add(o);	// visibilty check
            }
        }
		if (o.GetType()==L"CameraRoot")
        {
		    if (vIsHiddenCam || (vIsHiddenCam==false && ((bool)visi.GetParameterValue(L"viewvis")==true && (bool)visi.GetParameterValue(L"rendvis")==true)))
            {
                aCam.Add(o);	// visibilty check
            }
        }
		if (o.GetType()==L"camera" && X3DObject(o.GetParent()).GetType()!=L"CameraRoot")
        {
            if (vIsHiddenCam || (vIsHiddenCam==false && ((bool)visi.GetParameterValue(L"viewvis")==true && (bool)visi.GetParameterValue(L"rendvis")==true)))
            {
                aCam.Add(o);	// visibilty check
            }
        } 
    } 

    //-- call progresss bar
    pb.PutValue(0);
	pb.PutMaximum( 100 ); //-------->
	pb.PutStep(10);
	pb.PutVisible( true );
	pb.PutCaption( L"Processing scene data..." );
	pb.PutCancelEnabled(true);
                 
	//-- create textures 
	yi->printInfo("Exporter: Creating Textures.....");
       	
	yafaray_texture(yi);
			
	//-- create material ------------------
	yi->printInfo("Exporter: Processing Materials.....");
    //--
    yafaray_material(yi); 
        
	//-- create lights --------------------		
	yi->printInfo("Exporter: Creating Lights.....");
	for (int i=0; i < aYaflight.GetCount(); i++)
    {
		yafaray_light(yi, aYaflight[i]);
  //      pb.Increment(2); //-- test
    }

	// create geometry ------------------> funciona 2011 / 7.5
    yi->printInfo("Exporter: Creating Geometry.....");
       
 		for (int i=0; i < aMesh.GetCount(); i++)
        {
           yafaray_object(yi, aMesh[i]);
            pb.Increment(2); //-- test
        }
	
	// create camera -------------------> funciona 2011 / 7.5
	yi->printInfo("Exporter: Creating Camera.....");
	for (int i=0; i < aCam.GetCount(); i++)
    {
        yafaray_cam(yi, aCam[i]);
        pb.Increment(2); //-- test
    }

	// create background ----------------> funciona 2011 / 7.5
	yi->printInfo("Exporter: Creating Background.....");
			yafaray_world(yi);
	
	// create integrator ----------------> funciona  2011 / 7.5
	yi->printInfo("Exporter: Creating Integrator....."); 
	
    yafaray_integrator(yi);
	
	// create volume integrator ------->
    yi->printInfo("Exporter: Creating Volume Integrator.....");// 2011
	yafaray_volume_integrator(yi);

	// create output -------------------->
    // colorOutput_t *output = NULL;
    if ( out_type == "xml_file" )
    {
	   yi->paramsClearAll();
	   string out_name("H:/wip/test.tga");
	   yi->paramsSetString("type", file_ext[vSavefile]);
       yi->paramsSetInt("width", vXRes);
       yi->paramsSetInt("height", vYRes);
       yi->paramsSetBool("z_channel", vZ_buffer);

       imageHandler_t* ih = yi->createImageHandler(out_name.c_str());
             
   //    if(!ih) return 1;
   //    if(!(output = new imageOutput_t(ih, out_name, 0, 0))) return 1; 
   //   output = new imageOutput_t(ih, out_name, 0, 0);
    }
	// create render -------------------->
    yi->printInfo("Exporter: Creating render .....");// 2011
			yafaray_render(yi);
	
    //--
	pb.PutVisible( false );
    
    //--
    if ( out_type == "gui" )
    {   
        // settings for Qt gui
        Settings mysettings; 
		mysettings.autoSave = false;
		mysettings.autoSaveAlpha = false;
		mysettings.closeAfterFinish = false;
		mysettings.fileName = "";
		//--
		initGui();
		createRenderWidget(yi, vXRes, vYRes, 0, 0, mysettings);
		//---
    }
    if ( out_type == "xml_file" )
    {   //-- save image
	//	yi->render(*output);
    }
	yi->clearAll();
	delete yi;
    //--
    //save image file:
	//output->flush();
    //--
   
   
	return 0;
  }
//--  
#if defined(_WIN32) || defined(_WIN64)
	void loader(const char szArgs[])
	{
		//HANDLE hFile ;
		PROCESS_INFORMATION  pi;
		// start a program in windows
		STARTUPINFO  si = { sizeof(si) };
		CreateProcess(NULL, (LPSTR)szArgs, 0, 0, FALSE, 0, 0, 0, LPSTARTUPINFOA(&si), &pi);
	}
#endif

void yafaray_execute(){
	
	if (vYafXSIPath!=L""){
        if (vExportDone) 
        {
            #ifdef __unix__
				pid_t pid = fork();
				if( 0 == pid ) 
                {
					system ( (vYafXSIPath +L" " + vFileObjects).GetAsciiString());
					exit(0);
				}
			#else
				// win
				//----/ start YafaRay from XML file /--------->
				CString exec = vYafXSIPath +" \""+ vFileObjects + "\"";
				//app.LogMessage(exec);
				loader(exec.GetAsciiString());
				
			#endif
			
		}
	} else {
		app.LogMessage(L"Select path to YafaRay",siErrorMsg );
	}
}
