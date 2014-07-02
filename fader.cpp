/*
 Uses the properties of the RGB Colour Cube
 The RGB colour space can be viewed as a cube of colour. If we assume a cube of dimension 1, then the 
 coordinates of the vertices for the cubve will range from (0,0,0) to (1,1,1) (all black to all white).
 The transitions between each vertex will be a smooth colour flow and we can exploit this by using the 
 path coordinates as the LED transition effect. 
*/

#include <iostream>
#include <unistd.h>
#include "fader.h"
using namespace std;

Fader::Fader(int r_pin, int g_pin, int b_pin) {
	this->R_PIN = r_pin;
	this->G_PIN = g_pin;
	this->B_PIN = b_pin;
}

// Move along the colour line from where we are to the next vertex of the cube.
// The transition is achieved by applying the 'delta' value to the coordinate.
// By definition all the coordinates will complete the transition at the same 
// time as we only have one loop index.
void Fader::traverse(int delay)
{	
	int vertexA, vertexB = 0, i, dx, dy, dz;    // the new vertex and the previous one

	// initialise the place we start from as the first vertex in the array
	currentRGB.x = (vertex[vertexB].x ? MAX_RGB_VALUE : MIN_RGB_VALUE);
	currentRGB.y = (vertex[vertexB].y ? MAX_RGB_VALUE : MIN_RGB_VALUE);
	currentRGB.z = (vertex[vertexB].z ? MAX_RGB_VALUE : MIN_RGB_VALUE);

	// Now just loop through the path, traversing from one point to the next
	for (i = 0; i < 2 * PATH_SIZE; i++)
	{
		// !! loop index is double what the path index is as it is a nybble index !! 
		vertexA = vertexB;
		if (i & 1) {  // odd number is the second vertex and ...
			vertexB = path[i >> 1] & 0xf;  // ... the bottom nybble (index /2) or ...
		}
		else {    // ... even number is the first vertex and ...
			vertexB = path[i >> 1] >> 4;  // ... the top nybble
		}
		
		//printf("from point %d to point %d\n", v1, v2); 
		dx = vertex[vertexB].x -vertex[vertexA].x;
		dy = vertex[vertexB].y -vertex[vertexA].y;
		dz = vertex[vertexB].z -vertex[vertexA].z;

		if ((dx == 0) && (dy == 0) && (dz == 0)) {  // no point looping if we are staying in the same spot!
			return;
		}
			
		for (i = 0; i < MAX_RGB_VALUE - MIN_RGB_VALUE; i++, currentRGB.x += dx, currentRGB.y += dy, currentRGB.z += dz)
		{
			// set the colour in the LED
			this->writeValues(currentRGB);

			// wait fot the transition usleep
			usleep(delay); 
		}
	}
}

void Fader::writeValues(coord rgb) 
{
	cout << rgb.x << " " << rgb.y << " " << rgb.z << "\n";
}

/*
	This function's purpose is to fetch a color, each time it is called, such that calling it 
	multiple times would simulate the effect of Fader::traverse
*/
void Fader::setNextColor()
{
	int dx, dy, dz;

	// get the delta values
	dx = vertex[currentVertexB].x - vertex[currentVertexA].x;
	dy = vertex[currentVertexB].y - vertex[currentVertexA].y;
	dz = vertex[currentVertexB].z - vertex[currentVertexA].z;

	// update current RGB color
	seqCurrentRGB.x += dx;
	seqCurrentRGB.y += dy;
	seqCurrentRGB.z += dz;

	if(currentColorPos == MAX_RGB_VALUE - MIN_RGB_VALUE) {

		// if we reached the end of our path, we reset currentPosInPath an start from the beggining
		if(currentPosInPath + 1 == 2 * PATH_SIZE) {
			currentPosInPath = 0;
		}

		currentPosInPath++;
		currentVertexA = currentVertexB;

		if (currentPosInPath & 1) {  // if currentPosInPath is odd, take second half of current item in path
			currentVertexB = path[currentPosInPath >> 1] & 0xf;  // ... the bottom nybble (index /2) or ...
		} else {    // if currentPosInPath is even, take first half of current item in path
			currentVertexB = path[currentPosInPath >> 1] >> 4;  // ... the top nybble
		}

		currentColorPos = -1;
	}

	currentColorPos++;
	this->writeValues(seqCurrentRGB);
}