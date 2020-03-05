#include "Mesh.h"

using namespace NCL;

Mesh::Mesh()
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

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &arrayObject);
	glDeleteBuffers(MAX_BUFFER, bufferObject);
	delete[] colours;
	delete[] vertices;
	delete[] texCoords;
	glDeleteTextures(1, &texture);
}

Mesh* Mesh::GenerateQuad()
{
	Mesh* q = new Mesh();
	q->numVertices = 4;

	q->vertices = new Vector3[q->numVertices];
	q->texCoords = new Vector2[q->numVertices];
	q->colours = new Vector4[q->numVertices];

	q->vertices[0] = Vector3(-1.0f, -1.0f, 0.0f);
	q->vertices[1] = Vector3(-1.0f, 1.0f, 0.0f);
	q->vertices[2] = Vector3(1.0f, -1.0f, 0.0f);
	q->vertices[3] = Vector3(1.0f, 1.0f, 0.0f);

	q->texCoords[0] = Vector2(0.0f, 1.0f);
	q->texCoords[1] = Vector2(0.0f, 0.0f);
	q->texCoords[2] = Vector2(1.0f, 1.0f);
	q->texCoords[3] = Vector2(1.0f, 0.0f);

	for (int i = 0; i < 4; ++i)
	{
		q->colours[i] = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	q->BufferData();
	return q;
}

void Mesh::BufferData()
{
	glBindVertexArray(arrayObject);
	glGenBuffers(1, &bufferObject[VERTEX_BUFFER]);
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[VERTEX_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector3), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(VERTEX_BUFFER, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(VERTEX_BUFFER);

	if (texCoords) // This bit is new !
	{
		glGenBuffers(1, &bufferObject[TEXTURE_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[TEXTURE_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector2), texCoords, GL_STATIC_DRAW);
		glVertexAttribPointer(TEXTURE_BUFFER, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(TEXTURE_BUFFER);
	}

	if (colours) // Just in case the data has no colour attribute ...
	{
		glGenBuffers(1, &bufferObject[COLOUR_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[COLOUR_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector4), colours, GL_STATIC_DRAW);
		glVertexAttribPointer(COLOUR_BUFFER, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(COLOUR_BUFFER);
	}
	glBindVertexArray(0);
}

void Mesh::Draw()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);


	glBindVertexArray(arrayObject);
	glDrawArrays(type, 0, numVertices);
	glBindVertexArray(0);
}