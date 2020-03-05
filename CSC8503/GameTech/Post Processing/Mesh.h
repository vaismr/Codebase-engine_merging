#pragma once
#include "../../../Plugins/OpenGLRendering/OGLRenderer.h"
#include "../../../Plugins/OpenGLRendering/OGLShader.h"

namespace NCL {

	enum MeshBuffer
	{
		VERTEX_BUFFER, COLOUR_BUFFER,
		TEXTURE_BUFFER, NORMAL_BUFFER,
		TANGENT_BUFFER, INDEX_BUFFER,
		MAX_BUFFER
	};

	class Mesh
	{
	public:
		Mesh();
		~Mesh();

		void Draw();
		static Mesh* GenerateQuad();
		void BufferData();
		void SetTexture(GLuint tex) { texture = tex; }
		GLuint GetTexture() { return texture; }

	protected:
		GLuint texture;
		GLuint arrayObject;
		GLuint bufferObject[MAX_BUFFER];
		GLuint numVertices;
		GLuint type;

		Vector2* texCoords;
		Vector3* vertices;
		Vector4* colours;
	};
}