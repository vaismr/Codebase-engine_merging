#pragma once
#include "../../Plugins/OpenGLRendering/OGLRenderer.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"

#include "../CSC8503Common/GameWorld.h"

<<<<<<< HEAD
#include "../CSC8503Common/"

#define POST_PASSES 10

=======
>>>>>>> parent of aa21b98... Added to header as per pp tutorial
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

			void UpdateParticleTime(float dt)
			{

			}

			vector<const RenderObject*> activeObjects;

<<<<<<< HEAD

			//post process things
			void PresentScene();
			void DrawPostProcess();
			void DrawScene();

			OGLShader* processShader;
			Mesh* quad;

			GLuint bufferFBO;
			GLuint processFBO;
			GLuint bufferColourTex[2];
			GLuint bufferDepthTex;

=======
>>>>>>> parent of aa21b98... Added to header as per pp tutorial
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

