#pragma once
#include "../../Plugins/OpenGLRendering/OGLRenderer.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"

#include "../CSC8503Common/GameWorld.h"

#include "Post Processing/Mesh.h"

#define POST_PASSES 10

namespace NCL {
	class Maths::Vector3;
	class Maths::Vector4;
	namespace CSC8503 {
		class RenderObject;

		class GameTechRenderer : public OGLRenderer	{
		public:
			GameTechRenderer(GameWorld& world);
			~GameTechRenderer();

		protected:
			void RenderFrame()	override;
			void RenderLoadingFrame() override;

			OGLShader*		defaultShader;

			GameWorld&	gameWorld;

			void BuildObjectList();
			void SortObjectList();
			void RenderShadowMap();
			void RenderCamera(); 

			//skybox
			OGLShader* skyshader;
			unsigned int loadCubemap(vector<std::string> faces);
			void RenderSkybox();

			void SetupDebugMatrix(OGLShader*s) override;

			void DrawWithShader(OGLShader* shader);

			vector<const RenderObject*> activeObjects;



			//post process things
			OGLShader* processDefaultShader;
			OGLShader* processGreyShader;
			OGLShader* processInvShader;
			OGLShader* sceneShader;
			Mesh* quad;

			GLuint processFBO;
			GLuint processTexture;
			GLuint rbo;

			bool greyPost = false;
			bool invPost = false;

			void SelectPostType();

			//shadow mapping things
			OGLShader*	shadowShader;
			GLuint		shadowTex;
			GLuint		shadowFBO;
			Matrix4     shadowMatrix;

			Vector4		lightColour;
			float		lightRadius;
			Vector3		lightPosition;
		};
	}
}

