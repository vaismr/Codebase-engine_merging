#include "Quad.h"

using namespace NCL;

Quad::Quad()
{
	for (int i = 0; i < MAX_BUFFER; ++i)
	{
		bufferObject[i] = 0;
	}

	glGenVertexArrays(1, &arrayObject);

	vertices = NULL;
	colours = NULL;
	texture = 0;
	numVertices = 0;
	type = GL_TRIANGLE_STRIP;
	texCoords = NULL;
}

Quad::~Quad()
{
	glDeleteVertexArrays(1, &arrayObject);
	glDeleteBuffers(MAX_BUFFER, bufferObject);
	delete[] colours;
	delete[] vertices;
	delete[] texCoords;
	glDeleteTextures(1, &texture);
}