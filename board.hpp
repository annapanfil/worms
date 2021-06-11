#ifndef BOARD_H_INCLUDED
#define BOARD_H_INCLUDED


//myCubeVertices -homogenous coordinates of vertices in model space
//myCubeNormals - homogenous normal vectors of faces (per vertex) in model space
//myCubeVertexNormals - homogenous vertex normals in model space
//myCubeTexCoords - texturing coordinates
//myCubeColors - vertex colors

int boardVertexCount=6;

float boardVertices[]={
				//Wall 3
				-1.0f,-1.0f,-1.0f,1.0f,
				1.0f,-1.0f, 1.0f,1.0f,
				1.0f,-1.0f,-1.0f,1.0f,

				-1.0f,-1.0f,-1.0f,1.0f,
				-1.0f,-1.0f, 1.0f,1.0f,
				1.0f,-1.0f, 1.0f,1.0f,
			};

float boardColors[]={
  				//Wall 3
				0.0f,0.0f,1.0f,1.0f,
				0.0f,0.0f,1.0f,1.0f,
				0.0f,0.0f,1.0f,1.0f,

				0.0f,0.0f,1.0f,1.0f,
				0.0f,0.0f,1.0f,1.0f,
				0.0f,0.0f,1.0f,1.0f
			};

float boardNormals[]={
	//Wall 3
	0.0f,-1.0f, 0.0f,0.0f,
	0.0f,-1.0f, 0.0f,0.0f,
	0.0f,-1.0f, 0.0f,0.0f,

	0.0f,-1.0f, 0.0f,0.0f,
	0.0f,-1.0f, 0.0f,0.0f,
	0.0f,-1.0f, 0.0f,0.0f
};

float boardVertexNormals[]={
	//Wall 3
	-1.0f,-1.0f,-1.0f,0.0f,
	1.0f,-1.0f, 1.0f,0.0f,
	1.0f,-1.0f,-1.0f,0.0f,

	-1.0f,-1.0f,-1.0f,0.0f,
	-1.0f,-1.0f, 1.0f,0.0f,
	1.0f,-1.0f, 1.0f,0.0f
};

float boardTexCoords[]={
				//Wall 3
				1.0f,1.0f, 0.0f,0.0f, 0.0f,1.0f,
				1.0f,1.0f, 1.0f,0.0f, 0.0f,0.0f
			};



#endif // BOARD_H_INCLUDED
