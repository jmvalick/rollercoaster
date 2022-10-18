## Summary
This folder contains the Project 2 starter code Intermediate Submision 2. 
The program draws a continuous track with 2 rails and planks

Contents:
	Headers
		camera.hpp
		heightmap.hpp
		mesh.hpp
		model.hpp
		Project2.hpp
		rc_spline.h
		shader.hpp
		track.hpp
	Media
		car
		heightmaps
		nanosuit
		skybox
		skybox_old
		spine
		spline_parts
		textures
	Shaders
		lightingShader_basic.frag
		lightingShader_basic.vert
		lightingShader_nMap.frag
		lightingShader_nMap.vert
		lightingShader_specular.frag
		lightingShader_specular.vert
		normal.frag
		normal.geom
		normal.vert
		reflectionShader.frag
		reflectionShader.vert
		skyboxShader.frag
		skyboxShader.vert
	Sources
		Project1.cpp
		rc_spline.cpp
	Project_2.exe

How to run: navigate  Project1_Intermediate2_Valick_Jamin->Project_1->Project_1.exe

Controls:

	T: toggle between riding the coaster and free movement
	
	W, A, S, D to move camera

	H: toggle heightmap
	N: toggle normals
	B: toggle boxes

			    No Modifier							Shift							Ctrl
	U: Increase rotation rate in x-axis | Increase the scale in x-axis | Positive translation in the x-Axis
	J: Decrease rotation rate in x-axis | Decrease the scale in x-axis | Negative translation in the x-Axis
	I: Increase rotation rate in y-axis | Increase the scale in y-axis | Positive translation in the y-Axis
	K: Decrease rotation rate in y-axis | Decrease the scale in y-axis | Negative translation in the y-Axis
	O: Increase rotation rate in z-axis | Increase the scale in z-axis | Positive translation in the z-Axis
	L: Decrease rotation rate in z-axis | Decrease the scale in z-axis | Negative translation in the z-Axis
	
	
Moddifications:
	track.hpp
		lines 40-42:
			Vertex arrays for the right rail, left rail, and ties
		lines 65-98:
			draw function is implemented
			right rail, left rail, and ties are drawn
		lines 108-132:
			Get_point function implemented
		lines 136-141:
			Free data from vertex arrays
		line 148:
			VAOs and VBOs for the right rail, left rail, and ties
		lines 166-182:
			interpolate function implemented
			Catmull-Tom Spline interpolation
		lines 193-237:
			create_track function implemented
		lines 249-276:
			make_triangle function implemented
		lines 283-489:
			makeRailPart function implemented
		lines 509-571:
			setup_track function implemented

	camera.hpp
		lines 40-41:
			car variables
		lines 52-54:
			T button toggle variables
		lines 102-165:
			ProcessTrackMovement function implemented
	
	Project2.cpp
		line 177:
			rail texture loaded
		lines 194-196:
			car model loaded
		lines 235-238:
			ProcessTrackMovement called
		lines 284-287:
			call to draw track
		lines 382-392:
			draw car model
		lines 449-458:
			free look toggle
		lines 461:
			disable movement controls when on track
		line 632:
			disable camera rotation control when on track

	Project2.hpp
		line 71:
			drawTrack bool added

Extra Credit:
	car model is added and follows the rail when onTrack is true
	the model follows the position and rotation of the camera