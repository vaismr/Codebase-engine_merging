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

		void SetTexture0(GLuint tex) { texture0 = tex; }
		GLuint GetTexture0() { return texture0; }
		
		void SetTexture1(GLuint tex) { texture1 = tex; }
		GLuint GetTexture1() { return texture1; }

		void SetTexture2(GLuint tex) { texture2 = tex; }
		GLuint GetTexture2() { return texture2; }

		void SetTexture3(GLuint tex) { texture3 = tex; }
		GLuint GetTexture3() { return texture3; }

		void SetTexture4(GLuint tex) { texture4 = tex; }
		GLuint GetTexture4() { return texture4; }

	protected:
		GLuint texture0;
		GLuint texture1;
		GLuint texture2;
		GLuint texture3;
		GLuint texture4;
		GLuint arrayObject;
		GLuint bufferObject[MAX_BUFFER];
		GLuint numVertices;
		GLuint type;

		Vector2* texCoords;
		Vector3* vertices;
		Vector4* colours;
	};
}