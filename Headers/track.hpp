#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <iostream>

#include <shader.hpp>
#include <rc_spline.h>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

// Struct containing a local coordinate system
struct Orientation {
	// Front
	glm::vec3 Front;
	// Up
	glm::vec3 Up;
	// Right
	glm::vec3 Right;
	// origin
	glm::vec3 origin;
};


class Track
{
public:

	// Control Points Loading Class for loading from File
	rc_Spline g_Track;

	// Vector of control points
	std::vector<glm::vec3> controlPoints;

	// Track data
	std::vector<Vertex> rightRailVertices;
	std::vector<Vertex> leftRailVertices;
	std::vector<Vertex> tieVertices;

	// Vector of Orientations
	std::vector<Orientation> orientations;

	// hmax for camera
	float hmax = 0.0f;


	// constructor, just use same VBO as before, 
	Track(const char* trackPath)
	{
		// load Track data
		load_track(trackPath);

		create_track();

		setup_track();
	}

	// render the mesh
	void Draw(Shader shader, unsigned int textureID1, unsigned int textureID2)
	{
		//draw right rail
		shader.use();
		glm::mat4 rail_model;
		shader.setMat4("model", rail_model);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID1);

		glBindVertexArray(rightRailVAO);
		glDrawArrays(GL_TRIANGLES, 0, rightRailVertices.size());

		//draw left rail
		shader.use();
		shader.setMat4("model", rail_model);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID1);

		glBindVertexArray(leftRailVAO);
		glDrawArrays(GL_TRIANGLES, 0, leftRailVertices.size());

		//draw ties
		shader.use();
		glm::mat4 tie_model;
		shader.setMat4("model", tie_model);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID2);

		glBindVertexArray(tieVAO);
		glDrawArrays(GL_TRIANGLES, 0, tieVertices.size());

		glBindVertexArray(0);
		glActiveTexture(GL_TEXTURE0);
	}

	// give a positive float s, find the point by interpolation
	// determine pA, pB, pC, pD based on the integer of s
	// determine u based on the decimal of s
	// E.g. s=1.5 is the at the halfway point between the 1st and 2nd control point,
	//		the 4 control points are:[0,1,2,3], with u=0.5
	glm::vec3 get_point(float s)
	{	
		//control points for interpolation
		int pA = floor(s) - 2;
		int pB = floor(s) - 1;
		int pC = floor(s);
		int pD = floor(s) + 1;
		float u = s - floor(s); //point along spline between the control points
		
		//these are for looping back to the beginning when the control points are reaching the end of the list
		if (pA == -1) {
			pA = controlPoints.size() - 1;
		}
		if (pD == controlPoints.size()) {
			pD = 0;
		}
		if (pC == controlPoints.size()) {
			pD = 1;
			pC = 0;
		}
		if (pB == controlPoints.size()) {
			pD = 2;
			pC = 1;
			pB = 0;
		}

		return interpolate(controlPoints[pA], controlPoints[pB], controlPoints[pC], controlPoints[pD], 0.5f, u);
	}

	// Perform cleanup 
	void delete_buffers()
	{
		glDeleteVertexArrays(1, &rightRailVAO);
		glDeleteBuffers(1, &rightRailVBO);
		glDeleteVertexArrays(1, &leftRailVAO);
		glDeleteBuffers(1, &leftRailVBO);
		glDeleteVertexArrays(1, &tieVAO);
		glDeleteBuffers(1, &tieVBO);
	}

private:
	/*  Render data  */
	unsigned int rightRailVAO, rightRailVBO, leftRailVAO, leftRailVBO, tieVAO, tieVBO;

	void load_track(const char* trackPath)
	{
		// Set folder path for our projects (easier than repeatedly defining it)
		g_Track.folder = "../Project_2/Media/";

		// Load the control points
		g_Track.loadSplineFrom(trackPath);
	}

	// Implement the Catmull-Rom Spline here
	//	Given 4 points, a tau and the u value 
	//	u in range of [0,1]  
	//	Since you can just use linear algebra from glm, just make the vectors and matrices and multiply them.  
	//	This should not be a very complicated function
	glm::vec3 interpolate(glm::vec3 pointA, glm::vec3 pointB, glm::vec3 pointC, glm::vec3 pointD, float tau, float u)
	{
		glm::vec3 point;

		glm::vec4 uVec = glm::vec4(1.0f, u, pow(u, 2), pow(u, 3));
		glm::mat4 mMat = glm::mat4(
			0, 1, 0, 0,
			-tau, 0, tau, 0,
			2 * tau, tau - 3, 3 - 2 * tau, -tau,
			-tau, 2 - tau, tau - 2, tau);
		glm::mat4x3 pMat = glm::mat4x3(
			pointA.x, pointA.y, pointA.z,
			pointB.x, pointB.y, pointB.z,
			pointC.x, pointC.y, pointC.z,
			pointD.x, pointD.y, pointD.z);

		point = pMat * mMat * uVec;    //matrix multiplication for catmull-rom spline

		return point;
	}

	// Here is the class where you will make the Vertices or positions of the necessary objects of the track (calling subfunctions)
	//  For example, to make a basic roller coster:
	//    First, make the Vertices for each rail here (and indices for the EBO if you do it that way).  
	//        You need the XYZ world coordinates, the Normal Coordinates, and the texture coordinates.
	//        The normal coordinates are necessary for the lighting to work.  
	//    Second, make vector of transformations for the planks across the rails
	void create_track()
	{
		glm::vec3 currentpos = glm::vec3(0.0f, 0.0f, 0.0f);
		//iterate throught the points g_Track.points() returns the vector containing all the control points
		for (pointVectorIter ptsiter = g_Track.points().begin(); ptsiter != g_Track.points().end(); ptsiter++)
		{
			//get the next point from the iterator 
			glm::vec3 pt(*ptsiter);
			currentpos += pt;

			controlPoints.push_back(currentpos);
		}

		Orientation ori_prev;
		//set initial orientation
		ori_prev.origin = get_point(1.9);
		ori_prev.Front = glm::vec3(1.0f, 0.0f, 0.0f);
		ori_prev.Right = glm::vec3(0.0f, 0.0f, 1.0f);
		ori_prev.Up = glm::vec3(0.0f, 1.0f, 0.0f);

		Orientation	ori_cur;

		//set offsets multipliers to reduce the size of the objects
		glm::vec3 railOffset = glm::vec3(0.02f, 0.02f, 0.02f);
		glm::vec3 tieOffset = glm::vec3(0.15f, 0.15f, 0.15f);

		bool placeTie = false; //bool to tell make triangle to place a tie

		int counter = 0;
		for (float s = 2; s < controlPoints.size() + 2; s += 0.1) {

			//calculate the orientations and origins of each point along the curve
			ori_cur.origin = get_point(s);
			ori_cur.Front = glm::normalize((ori_cur.origin - ori_prev.origin) / glm::length(ori_cur.origin - ori_prev.origin));
			ori_cur.Right = glm::normalize(glm::cross(ori_cur.Front, ori_prev.Up));
			ori_cur.Up = glm::normalize(glm::cross(ori_cur.Right, ori_cur.Front));

			makeRailPart(ori_prev, ori_cur, railOffset, tieOffset, placeTie);

			//place a tie every two points along the curve
			if (counter%2 == 0) {
				placeTie = true;}
			else {
				placeTie = false;}
			orientations.push_back(ori_cur);
			ori_prev = ori_cur;
			counter++;
		}
	}

	// Given 3 Points, create a triangle and push it into Vertices (and EBO if you are using one)
	// Optional boolean to flip the normal if you need to
	// Note that if you're using an EBO, you may want to use a different approach than making
	// new Vertices every time, you may need to access existing Vertices
	//object tells which vertex array to add to
	//placeTie tells when to add to tie vertices
	void make_triangle(glm::vec3 posA, glm::vec3 posB, glm::vec3 posC, glm::vec2 textureA, glm::vec2 textureB, glm::vec2 textureC, bool flipNormal, std::string object, bool placeTie)
	{
		Vertex vA;
		Vertex vB;
		Vertex vC;

		vA.Position = posA;
		vB.Position = posB;
		vC.Position = posC;

		set_normals(vA, vB, vC, flipNormal);

		vA.TexCoords = textureA;
		vB.TexCoords = textureB;
		vC.TexCoords = textureC;

		if (object == "right rail") {
			rightRailVertices.push_back(vA);
			rightRailVertices.push_back(vB);
			rightRailVertices.push_back(vC);
		}
		if (object == "left rail") {
			leftRailVertices.push_back(vA);
			leftRailVertices.push_back(vB);
			leftRailVertices.push_back(vC);
		}
		if (object == "tie") {
			tieVertices.push_back(vA);
			tieVertices.push_back(vB);
			tieVertices.push_back(vC);
		}
	}

	// Given two orientations, create the rail between them. Offset can be useful if you want to call this for multiple rails
	void makeRailPart(Orientation ori_prev, Orientation ori_cur, glm::vec3 railOffset, glm::vec3 tieOffset, bool placeTie)
	{
		glm::vec3 posA;
		glm::vec3 posB;
		glm::vec3 posC;
		glm::vec2 textureA;
		glm::vec2 textureB;
		glm::vec2 textureC;

		std::string railType = "right rail"; //string to tell make triangle to add to left or right rail verticies
		glm::vec3 leftRightOffset = glm::vec3(0.1f, 0.1f, 0.1f); //offset multiplier to reduce distance of rails from the origin

		//first loop adds right rail vertices and second loop adds left rail vertices
		for (int railNum = 1; railNum <= 2; railNum++) {

			if (railType == "left rail") {
				leftRightOffset = glm::vec3(-0.1f, -0.1f, -0.1f);}

			//calculate triangle vertices and texture coordinates for the rails
			//rail top
				//trianlge1
				posA = ori_prev.origin + ori_prev.Up * railOffset - ori_prev.Right * railOffset + ori_prev.Right * leftRightOffset;
				posB = ori_prev.origin + ori_prev.Up * railOffset + ori_prev.Right * railOffset + ori_prev.Right * leftRightOffset;
				posC = ori_cur.origin + ori_cur.Up * railOffset - ori_cur.Right * railOffset + ori_cur.Right * leftRightOffset;
				textureA = glm::vec2(0.0f, 0.0f);
				textureB = glm::vec2(1.0f, 0.0f);
				textureC = glm::vec2(0.0f, 1.0f);
				make_triangle(posA, posB, posC, textureA, textureB, textureC, false, railType, placeTie);
				//triangle2
				posA = ori_prev.origin + ori_prev.Up * railOffset + ori_prev.Right * railOffset + ori_prev.Right * leftRightOffset;
				posB = ori_cur.origin + ori_cur.Up * railOffset + ori_cur.Right * railOffset + ori_cur.Right * leftRightOffset;
				posC = ori_cur.origin + ori_cur.Up * railOffset - ori_cur.Right * railOffset + ori_cur.Right * leftRightOffset;
				textureA = glm::vec2(1.0f, 0.0f);
				textureB = glm::vec2(1.0f, 1.0f);
				textureC = glm::vec2(0.0f, 1.0f);
				make_triangle(posA, posB, posC, textureA, textureB, textureC, false, railType, placeTie);

			//rail bottom
				//trianlge1
				posA = ori_prev.origin - ori_prev.Up * railOffset - ori_prev.Right * railOffset + ori_prev.Right * leftRightOffset;
				posB = ori_prev.origin - ori_prev.Up * railOffset + ori_prev.Right * railOffset + ori_prev.Right * leftRightOffset;
				posC = ori_cur.origin - ori_cur.Up * railOffset - ori_cur.Right * railOffset + ori_cur.Right * leftRightOffset;
				textureA = glm::vec2(0.0f, 0.0f);
				textureB = glm::vec2(1.0f, 0.0f);
				textureC = glm::vec2(0.0f, 1.0f);
				make_triangle(posA, posB, posC, textureA, textureB, textureC, true, railType, placeTie);
				//triangle2
				posA = ori_prev.origin - ori_prev.Up * railOffset + ori_prev.Right * railOffset + ori_prev.Right * leftRightOffset;
				posB = ori_cur.origin - ori_cur.Up * railOffset + ori_cur.Right * railOffset + ori_cur.Right * leftRightOffset;
				posC = ori_cur.origin - ori_cur.Up * railOffset - ori_cur.Right * railOffset + ori_cur.Right * leftRightOffset;
				textureA = glm::vec2(1.0f, 0.0f);
				textureB = glm::vec2(1.0f, 1.0f);
				textureC = glm::vec2(0.0f, 1.0f);
				make_triangle(posA, posB, posC, textureA, textureB, textureC, true, railType, placeTie);

			//rail left
				//trianlge1
				posA = ori_prev.origin + ori_prev.Up * railOffset - ori_prev.Right * railOffset + ori_prev.Right * leftRightOffset;
				posB = ori_prev.origin - ori_prev.Up * railOffset - ori_prev.Right * railOffset + ori_prev.Right * leftRightOffset;
				posC = ori_cur.origin + ori_cur.Up * railOffset - ori_cur.Right * railOffset + ori_cur.Right * leftRightOffset;
				textureA = glm::vec2(0.0f, 0.0f);
				textureB = glm::vec2(1.0f, 0.0f);
				textureC = glm::vec2(0.0f, 1.0f);
				make_triangle(posA, posB, posC, textureA, textureB, textureC, true, railType, placeTie);
				//triangle2
				posA = ori_prev.origin - ori_prev.Up * railOffset - ori_prev.Right * railOffset + ori_prev.Right * leftRightOffset;
				posB = ori_cur.origin - ori_cur.Up * railOffset - ori_cur.Right * railOffset + ori_cur.Right * leftRightOffset;
				posC = ori_cur.origin + ori_cur.Up * railOffset - ori_cur.Right * railOffset + ori_cur.Right * leftRightOffset;
				textureA = glm::vec2(1.0f, 0.0f);
				textureB = glm::vec2(1.0f, 1.0f);
				textureC = glm::vec2(0.0f, 1.0f);
				make_triangle(posA, posB, posC, textureA, textureB, textureC, true, railType, placeTie);

			//rail right
				//trianlge1
				posA = ori_prev.origin + ori_prev.Up * railOffset + ori_prev.Right * railOffset + ori_prev.Right * leftRightOffset;
				posB = ori_prev.origin - ori_prev.Up * railOffset + ori_prev.Right * railOffset + ori_prev.Right * leftRightOffset;
				posC = ori_cur.origin + ori_cur.Up * railOffset + ori_cur.Right * railOffset + ori_cur.Right * leftRightOffset;
				textureA = glm::vec2(0.0f, 0.0f);
				textureB = glm::vec2(1.0f, 0.0f);
				textureC = glm::vec2(0.0f, 1.0f);
				make_triangle(posA, posB, posC, textureA, textureB, textureC, false, railType, placeTie);
				//triangle2
				posA = ori_prev.origin - ori_prev.Up * railOffset + ori_prev.Right * railOffset + ori_prev.Right * leftRightOffset;
				posB = ori_cur.origin - ori_cur.Up * railOffset + ori_cur.Right * railOffset + ori_cur.Right * leftRightOffset;
				posC = ori_cur.origin + ori_cur.Up * railOffset + ori_cur.Right * railOffset + ori_cur.Right * leftRightOffset;
				textureA = glm::vec2(1.0f, 0.0f);
				textureB = glm::vec2(1.0f, 1.0f);
				textureC = glm::vec2(0.0f, 1.0f);
				make_triangle(posA, posB, posC, textureA, textureB, textureC, false, railType, placeTie);
			
			railType = "left rail";
		}

		if (placeTie == true) {
			//calculate triangles vertices and texture coordinates for the ties
			//tie top
				//triangle1
				posA = ori_cur.origin - ori_cur.Up * railOffset - ori_cur.Right * tieOffset;
				posB = ori_cur.origin - ori_cur.Up * railOffset - ori_cur.Right * tieOffset + ori_cur.Front * railOffset;
				posC = ori_cur.origin - ori_cur.Up * railOffset + ori_cur.Right * tieOffset;
				textureA = glm::vec2(0.0f, 0.0f);
				textureB = glm::vec2(0.0f, 1.0f);
				textureC = glm::vec2(1.0f, 0.0f);
				make_triangle(posA, posB, posC, textureA, textureB, textureC, true, "tie", placeTie);

				//triangle2
				posA = ori_cur.origin - ori_cur.Up * railOffset - ori_cur.Right * tieOffset + ori_cur.Front * railOffset;
				posB = ori_cur.origin - ori_cur.Up * railOffset + ori_cur.Right * tieOffset + ori_cur.Front * railOffset;
				posC = ori_cur.origin - ori_cur.Up * railOffset + ori_cur.Right * tieOffset;
				textureA = glm::vec2(0.0f, 1.0f);
				textureB = glm::vec2(1.0f, 1.0f);
				textureC = glm::vec2(1.0f, 0.0f);
				make_triangle(posA, posB, posC, textureA, textureB, textureC, true, "tie", placeTie);

			//tie bottom
				//triangle1
				posA = ori_cur.origin - ori_cur.Up * 0.05f - ori_cur.Right * tieOffset;
				posB = ori_cur.origin - ori_cur.Up * 0.05f - ori_cur.Right * tieOffset + ori_cur.Front * railOffset;
				posC = ori_cur.origin - ori_cur.Up * 0.05f + ori_cur.Right * tieOffset;
				textureA = glm::vec2(0.0f, 0.0f);
				textureB = glm::vec2(0.0f, 1.0f);
				textureC = glm::vec2(1.0f, 0.0f);
				make_triangle(posA, posB, posC, textureA, textureB, textureC, false, "tie", placeTie);

				//triangle2
				posA = ori_cur.origin - ori_cur.Up * 0.05f - ori_cur.Right * tieOffset + ori_cur.Front * railOffset;
				posB = ori_cur.origin - ori_cur.Up * 0.05f + ori_cur.Right * tieOffset + ori_cur.Front * railOffset;
				posC = ori_cur.origin - ori_cur.Up * 0.05f + ori_cur.Right * tieOffset;
				textureA = glm::vec2(0.0f, 1.0f);
				textureB = glm::vec2(1.0f, 1.0f);
				textureC = glm::vec2(1.0f, 0.0f);
				make_triangle(posA, posB, posC, textureA, textureB, textureC, false, "tie", placeTie);

			//tie right
				//triangle1
				posA = ori_cur.origin - ori_cur.Up * railOffset + ori_cur.Right * tieOffset;
				posB = ori_cur.origin - ori_cur.Up * 0.05f + ori_cur.Right * tieOffset;
				posC = ori_cur.origin - ori_cur.Up * 0.05f + ori_cur.Right * tieOffset + ori_cur.Front * railOffset;;
				textureA = glm::vec2(0.0f, 1.0f);
				textureB = glm::vec2(0.0f, 0.0f);
				textureC = glm::vec2(1.0f, 0.0f);
				make_triangle(posA, posB, posC, textureA, textureB, textureC, false, "tie", placeTie);

				//triangle2
				posA = ori_cur.origin - ori_cur.Up * railOffset + ori_cur.Right * tieOffset;
				posB = ori_cur.origin - ori_cur.Up * railOffset + ori_cur.Right * tieOffset + ori_cur.Front * railOffset;
				posC = ori_cur.origin - ori_cur.Up * 0.05f + ori_cur.Right * tieOffset + ori_cur.Front * railOffset;
				textureA = glm::vec2(0.0f, 1.0f);
				textureB = glm::vec2(1.0f, 1.0f);
				textureC = glm::vec2(1.0f, 0.0f);
				make_triangle(posA, posB, posC, textureA, textureB, textureC, true, "tie", placeTie);

			//tie left
				//triangle1
				posA = ori_cur.origin - ori_cur.Up * railOffset - ori_cur.Right * tieOffset;
				posB = ori_cur.origin - ori_cur.Up * 0.05f - ori_cur.Right * tieOffset;
				posC = ori_cur.origin - ori_cur.Up * 0.05f - ori_cur.Right * tieOffset + ori_cur.Front * railOffset;;
				textureA = glm::vec2(0.0f, 1.0f);
				textureB = glm::vec2(0.0f, 0.0f);
				textureC = glm::vec2(1.0f, 0.0f);
				make_triangle(posA, posB, posC, textureA, textureB, textureC, true, "tie", placeTie);

				//triangle2
				posA = ori_cur.origin - ori_cur.Up * railOffset - ori_cur.Right * tieOffset;
				posB = ori_cur.origin - ori_cur.Up * railOffset - ori_cur.Right * tieOffset + ori_cur.Front * railOffset;
				posC = ori_cur.origin - ori_cur.Up * 0.05f - ori_cur.Right * tieOffset + ori_cur.Front * railOffset;
				textureA = glm::vec2(0.0f, 1.0f);
				textureB = glm::vec2(1.0f, 1.0f);
				textureC = glm::vec2(1.0f, 0.0f);
				make_triangle(posA, posB, posC, textureA, textureB, textureC, false, "tie", placeTie);

			//tie front
				//triangle1
				posA = ori_cur.origin - ori_cur.Up * railOffset - ori_cur.Right * tieOffset + ori_cur.Front * railOffset;
				posB = ori_cur.origin - ori_cur.Up * railOffset + ori_cur.Right * tieOffset + ori_cur.Front * railOffset;
				posC = ori_cur.origin - ori_cur.Up * 0.05f + ori_cur.Right * tieOffset + ori_cur.Front * railOffset;
				textureA = glm::vec2(0.0f, 1.0f);
				textureB = glm::vec2(1.0f, 1.0f);
				textureC = glm::vec2(1.0f, 0.0f);
				make_triangle(posA, posB, posC, textureA, textureB, textureC, false, "tie", placeTie);

				//triangle2
				posA = ori_cur.origin - ori_cur.Up * 0.05f - ori_cur.Right * tieOffset + ori_cur.Front * railOffset;
				posB = ori_cur.origin - ori_cur.Up * railOffset - ori_cur.Right * tieOffset + ori_cur.Front * railOffset;
				posC = ori_cur.origin - ori_cur.Up * 0.05f + ori_cur.Right * tieOffset + ori_cur.Front * railOffset;
				textureA = glm::vec2(0.0f, 0.0f);
				textureB = glm::vec2(0.0f, 1.0f);
				textureC = glm::vec2(1.0f, 0.0f);
				make_triangle(posA, posB, posC, textureA, textureB, textureC, false, "tie", placeTie);

			//tie back
				//triangle1
				posA = ori_cur.origin - ori_cur.Up * railOffset - ori_cur.Right * tieOffset;
				posB = ori_cur.origin - ori_cur.Up * railOffset + ori_cur.Right * tieOffset;
				posC = ori_cur.origin - ori_cur.Up * 0.05f + ori_cur.Right * tieOffset;
				textureA = glm::vec2(0.0f, 1.0f);
				textureB = glm::vec2(1.0f, 1.0f);
				textureC = glm::vec2(1.0f, 0.0f);
				make_triangle(posA, posB, posC, textureA, textureB, textureC, true, "tie", placeTie);

				//triangle2
				posA = ori_cur.origin - ori_cur.Up * 0.05f - ori_cur.Right * tieOffset;
				posB = ori_cur.origin - ori_cur.Up * railOffset - ori_cur.Right * tieOffset;
				posC = ori_cur.origin - ori_cur.Up * 0.05f + ori_cur.Right * tieOffset;
				textureA = glm::vec2(0.0f, 0.0f);
				textureB = glm::vec2(0.0f, 1.0f);
				textureC = glm::vec2(1.0f, 0.0f);
				make_triangle(posA, posB, posC, textureA, textureB, textureC, true, "tie", placeTie);
		}
	}

	// Find the normal for each triangle uisng the cross product and then add it to all three Vertices of the triangle.  
	//   The normalization of all the triangles happens in the shader which averages all norms of adjacent triangles.   
	//   Order of the triangles matters here since you want to normal facing out of the object.  
	void set_normals(Vertex& p1, Vertex& p2, Vertex& p3, bool flipNormal)
	{
		glm::vec3 normal = glm::cross(p2.Position - p1.Position, p3.Position - p1.Position);
		if (flipNormal == true) {
			normal = -normal;
		}
		p1.Normal += normal;
		p2.Normal += normal;
		p3.Normal += normal;
	}

	void setup_track()
	{
		//generate and bind VAO and VBO for right rail
		glGenVertexArrays(1, &rightRailVAO);
		glGenBuffers(1, &rightRailVBO);

		glBindVertexArray(rightRailVAO);
		glBindBuffer(GL_ARRAY_BUFFER, rightRailVBO);
		glBufferData(GL_ARRAY_BUFFER, rightRailVertices.size() * sizeof(Vertex), &rightRailVertices[0], GL_STATIC_DRAW);

		//positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

		//normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

		//textures
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));


		//generate and bind VAO and VBO for left rail
		glGenVertexArrays(1, &leftRailVAO);
		glGenBuffers(1, &leftRailVBO);

		glBindVertexArray(leftRailVAO);
		glBindBuffer(GL_ARRAY_BUFFER, leftRailVBO);
		glBufferData(GL_ARRAY_BUFFER, leftRailVertices.size() * sizeof(Vertex), &leftRailVertices[0], GL_STATIC_DRAW);

		//positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

		//normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

		//textures
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));


		//generate and bind VAO and VBO for ties
		glGenVertexArrays(1, &tieVAO);
		glGenBuffers(1, &tieVBO);

		glBindVertexArray(tieVAO);
		glBindBuffer(GL_ARRAY_BUFFER, tieVBO);
		glBufferData(GL_ARRAY_BUFFER, tieVertices.size() * sizeof(Vertex), &tieVertices[0], GL_STATIC_DRAW);

		//positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

		//normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

		//textures
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

		glBindVertexArray(0);
	}

};
