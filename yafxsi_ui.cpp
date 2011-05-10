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

#include "stdafx.h"

using namespace XSI;
using namespace std;


XSIPLUGINCALLBACK CStatus YafaRayRenderer_DefineLayout( CRef& in_ctxt )
{
	//---------------
	PPGLayout lay = Context( in_ctxt ).GetSource();
	PPGItem item;
	lay.Clear();
			//----
	//------------------//
	lay.AddTab(L"Main");
	//------------------//
		lay.AddGroup(L"Image");
			lay.AddRow();
				lay.AddItem(L"Width").PutAttribute(siUINoSlider,true);
				lay.AddItem(L"Height").PutAttribute(siUINoSlider,true);
			lay.EndRow();
			CValueArray vItem3(12);
				vItem3[0] = L"Image file TIFF" ; vItem3[1] = 0;
				vItem3[2] = L"Image file TGA" ; vItem3[3] = 1;
				vItem3[4] = L"Image file PNG" ; vItem3[5] = 2;
				vItem3[6] = L"Image file JPG" ; vItem3[7] = 3;
				vItem3[8] = L"Image file HDR" ; vItem3[9] = 4;
				vItem3[10] = L"Image file EXR" ; vItem3[11] = 5;
		//	lay.AddEnumControl(L"savefile",vItem3,L"Save", siControlCombo );
		lay.EndGroup(); // image
	
	//----/ export.. /------->
		lay.AddGroup(L"Export hidden...");
			lay.AddItem(L"use_hidden_obj", L"Objects");
			lay.AddItem(L"use_hidden_cam", L"Cameras");
			lay.AddItem(L"bhidden_light", L"Lights");
			lay.AddItem(L"smooth_mesh", L"Export smooth meshes");
		lay.EndGroup();
	//----/ saves.. /---->
	//	lay.AddGroup(L"Save XML");
	//		lay.AddItem(L"fObjects",L"File export",siControlFilePath);
	//		item = lay.GetItem( L"fObjects" );
	//		item.PutAttribute( siUIFileFilter, L"YafXSI Scenes|*.xml" ) ;
	//	lay.EndGroup();

//			lay.AddItem(L"vYafPath",L"Path to YafaRay",siControlFilePath);
//		    PPGItem itm = lay.GetItem( L"vYafPath" );
//			itm.PutAttribute( siUIOpenFile, 1 ) ;
//			itm.PutAttribute( siUIFileMustExist, 1 ) ;
//			itm.PutAttribute( siControlFilePath , "test" ) ;
			
	//----/ Buttons.. /---->
		    lay.AddRow();
				lay.AddButton(L"export_xml",L"Export to XML");
				lay.AddButton(L"render_YafXSI",L"Render console");
				lay.AddButton(L"render_qtgui",L"Render YafaRay UI");
			lay.EndRow();
	//----------------------//
	lay.AddTab(L"Specials"); 
        //----------------------//
		lay.AddGroup(L"Lights..");
			lay.AddSpacer(10,2);
			lay.AddItem(L"bIES_file",L"IES file path",siControlFilePath);
				item = lay.GetItem( L"bIES_file" ); 
				item.PutAttribute( siUIOpenFile, 1 ) ;
				item.PutAttribute( siUIFileMustExist, 1 ) ;
				item.PutAttribute( siUIFileFilter, L"IES files|*.ies" ) ; // probas
				item.PutAttribute( siControlFilePath , "test" ) ;
		lay.EndGroup();
	
	//--------------------//
	lay.AddTab(L"World"); 
	//--------------------//
		lay.AddGroup(L"Background settings");
			CValueArray vWorld(10);
				vWorld[0] = L"Single color"	; vWorld[1] = 0;
				vWorld[2] = L"Gradient"		; vWorld[3] = 1;
				vWorld[4] = L"Texture"		; vWorld[5] = 2;
				vWorld[6] = L"Sunsky"		; vWorld[7] = 3;
				vWorld[8] = L"DarkTide's SunSky" ; vWorld[9] = 4;
			item = lay.AddEnumControl(L"setworld", vWorld, L"Background", siControlCombo);
		lay.AddSpacer(0,10); // for test		
		
		//----/ Gradient /----------->
		lay.AddGroup(L"Gradient");
			lay.AddColor( L"bhor_red", L"Horizont col." );
			lay.AddColor( L"bzen_red", L"Zenith col." );
			lay.AddColor( L"bhorgro_red", L"Hor. ground col." );
			lay.AddColor( L"bzengro_red", L"Zen. ground col." );
		lay.EndGroup();

		//----/ Texture /----------->
		lay.AddGroup(L"Texture");
			lay.AddItem(L"bHDRI",L"Image path",siControlFilePath);
				item = lay.GetItem( L"bHDRI" );
				item.PutAttribute( siUIOpenFile, 1 ) ;
				item.PutAttribute( siUIFileMustExist, 1 ) ;
				item.PutAttribute( siControlFilePath , "test" ) ;
			lay.AddItem( L"brotation", L"Rotation ");
			lay.AddItem( L"binterp", L"Use Interpolate ");
            CValueArray mppWorld(4);
				mppWorld[0] = L"Angular Map"; mppWorld[1] = 0;
				mppWorld[2] = L"Sphere"		; mppWorld[3] = 1;
            lay.AddEnumControl(L"bmapworld",mppWorld,L"Mapping type", siControlCombo );
		
		lay.EndGroup();
						
		//----/ sunsky /------->
		lay.AddGroup(L"Sunsky");	
			lay.AddItem(L"bturbidity", L"Turbidity");
			lay.AddItem(L"bA_horbright", L"HorBright");
			//-
			lay.AddItem(L"bB_horsprd", L"HorSprd");
			lay.AddItem(L"bC_sunbright", L"SunBright");
			//-
			lay.AddItem(L"bD_sunsize",L"SunSize");
			lay.AddItem(L"bE_backlight",L"Backlight");
		
			lay.AddItem(L"bfrom_pos",L"Get position");
			lay.AddItem(L"bfrom_angle",L"Get angle");
		
			//lay.AddItem(L"bx_vect",L"X: ");
			//lay.AddItem(L"by_vect",L"Y:");
			//lay.AddItem(L"bz_vect",L"Z:");

			lay.AddItem(L"bfrom_update",L"Update Sun");
			lay.AddItem(L"bskylight",L"Skylight");
			lay.AddItem(L"bw_samples",L"Samples");
				
			lay.AddItem(L"badd_sun",L"Add Sun");
			lay.AddItem(L"bsun_power",L"Sun power");
		lay.EndGroup(); //-- sunsky
		
		//----/ Single color /------>
		lay.AddGroup(L"Single color");
			lay.AddColor( L"bsc_red", L"Color" ) ;
		lay.EndGroup(); 

		lay.AddGroup(L"IBL"); // for single color, gradient  and texture
			lay.AddRow();
				lay.AddItem(L"buse_ibl",L"Use IBL");
			lay.EndRow();
			lay.AddRow();
				lay.AddItem(L"bibl_samples",L"IBL Samples");
				lay.AddItem(L"bibl_power",L"Power");
			lay.EndRow();
		lay.EndGroup(); //-- end ibl	

	//----/ volume integrator /-------->
		lay.AddGroup(L" Volume integrator ");

			CValueArray aV_intg(4);
				aV_intg[0] = L"None" ;			aV_intg[1] = 0;
				aV_intg[2] = L"Single Scatter" ;	aV_intg[3] = 1;
			lay.AddEnumControl( L"bVintegrator", aV_intg, L"Volume Integrator", siControlCombo ) ;
			//----/ scatter /---->
				lay.AddItem(L"bstep_size", L"Step size");	 // float
				lay.AddItem(L"batt_grid", L"Att. grid resolution"); // int
			lay.AddRow();
				lay.AddItem(L"badaptive", L"Adaptive"); // bool
				lay.AddItem(L"boptimize",L"Optimize");	//bool
			lay.EndRow();
		lay.EndGroup(); //----/ end volume int. /----->

	lay.EndGroup(); //----/ end background set /---------->
	//----/ Buttons row /----->
			lay.AddRow();
				lay.AddButton(L"export_xml",L"Export to XML");
				lay.AddButton(L"render_YafXSI",L"Render console");
				lay.AddButton(L"render_qtgui",L"Render YafaRay UI");
			lay.EndRow();
	//------------------------//
	lay.AddTab(L"Lighting "); 
	//------------------------//
	    lay.AddGroup(L"Lighting method"); //-- grupo lighting --->
			CValueArray iTlight_ps(8);
				iTlight_ps[0] = L"Direct Lighting"	   ; iTlight_ps[1] = 0;
				iTlight_ps[2] = L"Photon Map"		   ; iTlight_ps[3] = 1;
				iTlight_ps[4] = L"Path Tracing"		   ; iTlight_ps[5] = 2;
				iTlight_ps[6] = L"Bidirectional (EXP)" ; iTlight_ps[7] = 3;
			lay.AddEnumControl(L"blighting",iTlight_ps,L"Lighting Modes",siControlCombo ) ;
			
        lay.AddGroup(); // general, no-label
		
	        //-- directlighting
		lay.AddGroup(L"Caustics"); //(-- grupo caustic --->
				lay.AddItem(L"bcaustics", L"Use Caustics");
			lay.AddRow();
				lay.AddItem(L"bDLCphotons"   , L"C.Photons");
				lay.AddItem(L"bDLcaustic_mix", L"Caustics mix");
			lay.EndRow();
			lay.AddRow();
				lay.AddItem(L"bcaustic_depth",L"Caustics depth");
				lay.AddItem(L"bDLcaust_radius", L"Caustics radius");
			lay.EndRow();
		lay.EndGroup();//---/ end caustics /------)

		lay.AddGroup(L"Ambient Occlusion"); //)-- grupo AO --->
			lay.AddRow();
				lay.AddColor( L"AOc_red", L"Col.", true );
				lay.AddItem(L"bUse_AO",L"Use AO ");	
			lay.EndRow();
			lay.AddRow();
				lay.AddItem(L"bao_samples",L"AO Samples");
				lay.AddItem(L"bao_distance", L"AO Distance");
			lay.EndRow();
		lay.EndGroup();//-- end AO ---)
		
	//----/ photon mapping /------->
			    lay.AddItem(L"bdepth",L"Depth");
                lay.AddItem(L"bdiff_photons",L"Diff. Photons").PutLabelPercentage(60);
				lay.AddItem(L"bcaust_photons",L"Caust. Photons").PutLabelPercentage(60);
			lay.AddRow();
				lay.AddItem(L"bdiff_radius", L"Diff. radius");
				lay.AddItem(L"bcaust_radius", L"Caus. radius");
			lay.EndRow();
			lay.AddRow();
				lay.AddItem(L"bsearch", L"Search");
				lay.AddItem(L"bcaustic_mix", L"Caustic Mix ");
			lay.EndRow();
			lay.AddRow();
				lay.AddItem(L"bfg_samples", L"FG samples ");
				lay.AddItem(L"bfg_bounces", L"FG bounces ");
			lay.EndRow();
			lay.AddRow();
				lay.AddItem(L"bshow_map", L"Show map ");// bool
				lay.AddItem(L"bfinal_gather", L"Final gather ");// bool
			lay.EndRow();
		
	//----/ pathtracing /--------->
			CValueArray aTracer(8);
				aTracer[0] = L"None" ; aTracer[1] = 0;
				aTracer[2] = L"Path" ; aTracer[3] = 1;
				aTracer[4] = L"Photon" ; aTracer[5] = 2;
				aTracer[6] = L"Path + Photon" ; aTracer[7] = 3;
			lay.AddEnumControl(L"tracer_method", aTracer, L"Caustics method", siControlCombo ) ;
			lay.AddRow();
				lay.AddItem(L"bpathcaus_photons",L"Photons ");
				lay.AddItem(L"bpathcaus_mix",L"Caustic mix ");
			lay.EndRow();
			lay.AddRow();
				lay.AddItem(L"bpathcaus_depth",L"Caustic depth ");
				lay.AddItem(L"bpathcaus_radius",L"Caust. radius ");
			lay.EndRow();
			lay.AddRow();
				lay.AddItem(L"bpath_depth",L"Depth");
				lay.AddItem(L"bpath_samples",L"Path samples ");
			lay.EndRow();
			lay.AddRow();
				lay.AddItem(L"buseback",L"Use Background");
				lay.AddItem(L"bno_recursion",L"No Recursion");
			lay.EndRow();
            //--- SPPM
            lay.AddItem(L"bspphotons", L"Photons").PutLabelPercentage(60);
            lay.AddItem(L"bsppassNums", L"Passes").PutLabelPercentage(60);
            lay.AddItem(L"bspbounces", L"Bounces").PutLabelPercentage(60);
            lay.AddItem(L"bspdiffuseRadius", L"Search Radius").PutLabelPercentage(60);
            lay.AddItem(L"bspsearch", L"Search count").PutLabelPercentage(60);
            lay.AddItem(L"bsptimes", L"Radius Factor").PutLabelPercentage(60);      
            lay.AddItem(L"bsppmIRE", L"PM IRE").PutLabelPercentage(60);
            //---
        lay.EndGroup();
    lay.EndGroup(); // lighting method

        //-- buttons
			lay.AddRow();
				lay.AddButton(L"export_xml",L"Export to XML");
				lay.AddButton(L"render_YafXSI",L"Render console");
				lay.AddButton(L"render_qtgui",L"Render YafaRay UI");
			lay.EndRow();

	//-----------------------//
	lay.AddTab(L"General "); 
	//-----------------------//
		lay.AddGroup(L"General settings");
			lay.AddRow();
				lay.AddItem(L"braydepth",L"Raydepth ");// scalar
				lay.AddItem(L"btransp_shadows",L"Transparent Shadows"); // bool
			lay.EndRow();
			lay.AddSpacer(10,2);
			lay.AddRow();
				lay.AddItem(L"bclay",L"Clay render"); // bool
				lay.AddItem(L"bshadows_depth",L"Shadows depth"); // scalar
			lay.EndRow();
			lay.AddSpacer(10,2);
			lay.AddRow();
				lay.AddItem(L"bauto_thread",L"Auto-Threads"); // bool
				lay.AddItem(L"bz_buffer",L"Render Z_Buffer"); // bool
			lay.EndRow();
			lay.AddSpacer(10,2);
			lay.AddRow();
				lay.AddItem(L"bthreads",L"Threads"); // scalar
                lay.AddItem(L"bshowpix", L"Show pix samp");
			lay.EndRow();
            lay.AddRow();
                CValueArray aTile(4);
				    aTile[0] = L"Random" ; aTile[1] = 0;
				    aTile[2] = L"Linear" ; aTile[3] = 1;
                lay.AddEnumControl(L"btileorder",aTile,L"Tile order",siControlCombo ) ;
				//lay.AddItem(L"btileorder",L"Tile order"); // scalar
                lay.AddItem(L"btilesize", L"Tile size");
			lay.EndRow();
		lay.EndGroup(); // End general settings

	//----/ output settings /------>

		lay.AddGroup(L"Output settings");
		
			lay.AddRow();
				lay.AddItem(L"gamma",L"Gamma"); // scalar
				lay.AddItem(L"bgamma_in",L"Gamma In");  // scalar
			lay.EndRow();
			lay.AddSpacer(10,2);
			lay.AddRow();
				lay.AddItem(L"bclamp_rgb",L"Clamp RGB"); // bool
				lay.AddItem(L"bpremu_alpha",L"Premultiply Alpha"); // bool
			lay.EndRow();
		lay.EndGroup(); // End output settings
	//----// output method /----->
			lay.AddRow();
				lay.AddItem(L"bdraw_render",L"Draw render params");// bool
				lay.AddItem(L"bcustom_string",L"Custom string"); //string
			lay.EndRow();
	//----/ AA settings /----------->
		lay.AddGroup(L"AA settings");
			lay.AddRow();
				lay.AddItem(L"baa_passes",L"AA passes ");// scalar
				lay.AddItem(L"baa_samples",L"AA samples"); // bool
			lay.EndRow();
			lay.AddSpacer(10,2);
			lay.AddRow();
				lay.AddItem(L"baa_threshold",L"AA Threshold"); // float
				lay.AddItem(L"baa_inc",L"AA Inc samples"); // scalar
			lay.EndRow();
			lay.AddSpacer(10,2);
			lay.AddRow();
				CValueArray AAmodes(8);
				AAmodes[0] = L"box" ; AAmodes[1] = 0;
				AAmodes[2] = L"gauss" ;	AAmodes[3] = 1;
				AAmodes[4] = L"lanczos" ; AAmodes[5] = 2;
				AAmodes[6] = L"mitchell" ;	AAmodes[7] = 3;
			lay.AddEnumControl(L"baa_modes",AAmodes,L"AA Modes",siControlCombo ) ;
				lay.AddItem(L"baa_pixel",L"AA Pixelwidth"); //float
			lay.EndRow();
		lay.EndGroup(); // End AA settings

	//----/ Buttons row /----->
			lay.AddRow();
				lay.AddButton(L"export_xml",L"Export to XML");
				lay.AddButton(L"render_YafXSI",L"Render console");
				lay.AddButton(L"render_qtgui",L"Render YafaRay UI");
			lay.EndRow();
			
	
	return CStatus::OK;
}