# pragma once
# include "../../Plugins/OpenGLRendering/OGLRenderer.h"
#include "../../Common/MeshGeometry.h"
#include "../../Plugins/OpenGLRendering/glad/glad.h"

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
	Mesh(void);
	~Mesh(void); 
	virtual void Draw();
	static Mesh * GenerateTriangle();
	static Mesh* GenerateQuad();
	static Mesh* GenerateLand();

	void SetTexture(GLuint tex) { texture = tex; }
	GLuint GetTexture() { return texture; }

	void SetDisTexture(GLuint tex) { displacementTexture = tex; }
	GLuint GetDisTexture() { return displacementTexture; }

	void SetBumpMap(GLuint tex) { bumpTexture = tex; }
	GLuint GetBumpMap() { return bumpTexture; }

	void SetTexture1(GLuint tex) { texture1 = tex; }
	GLuint GetTexture1() { return texture1; }

	void SetTexture2(GLuint tex) { texture2 = tex; }
	GLuint GetTexture2() { return texture2; }

	void SetTexture3(GLuint tex) { texture3 = tex; }
	GLuint GetTexture3() { return texture3; }

	void SetTexture4(GLuint tex) { texture4 = tex; }
	GLuint GetTexture4() { return texture4; }

	void SetTexture5(GLuint tex) { texture5 = tex; }
	GLuint GetTexture5() { return texture5; }

	void SetNormalTexture(GLuint tex) { normalTex = tex; }
	GLuint GetNormalTexture() { return normalTex; }

	void BufferData();
	void GenerateNormals();

 protected:
	

	void GenerateTangents();
	Vector3 GenerateTangent(const Vector3& a, const Vector3& b, const Vector3& c,
						const Vector2& ta, const Vector2& tb, const Vector2& tc);

	GLuint arrayObject;
	GLuint bufferObject[MAX_BUFFER];
	GLuint numVertices;
	GLuint type;
	
	Vector3 * vertices;
	Vector4 * colours;

	GLuint texture;
	GLuint displacementTexture;
	Vector2* textureCoords;

	GLuint numIndices;
	unsigned int* indices;

	Vector3* normals;

	Vector3* tangents;
	GLuint bumpTexture;

	GLuint texture1;
	GLuint texture2;
	GLuint texture3;
	GLuint texture4;
	GLuint texture5;

	GLuint normalTex;
};