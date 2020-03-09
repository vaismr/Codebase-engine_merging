#pragma once
#include "../../Plugins/OpenGLRendering/OGLRenderer.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"

#include "../CSC8503Common/GameWorld.h"


#include "Post Processing/Mesh.h"

#include <queue>

#define POST_PASSES 10


#include "../../Common/TextureLoader.h"

namespace NCL {
	class Maths::Vector3;
	class Maths::Vector4;
	namespace CSC8503 {
		class RenderObject;

		class GameTechRenderer : public OGLRenderer {
		public:
			GameTechRenderer(GameWorld& world);
			~GameTechRenderer();

		protected:
			void RenderFrame()	override;
			void RenderLoadingFrame() override;

			OGLShader* defaultShader;

			GameWorld& gameWorld;

			void BuildObjectList();
			void SortObjectList();
			void RenderShadowMap();
			void RenderCamera();

			//skybox
			unsigned int skytextureID;
			unsigned int cubemapTexture;
			OGLShader* skyshader;
			unsigned int loadCubemap(vector<std::string> faces);
			void RenderSkybox();
			void GenerateSkybox();
			unsigned int skyboxVAO, skyboxVBO;

			//icecube
			OGLShader* iceshader;
			void GenerateIce();

			void SetupDebugMatrix(OGLShader* s) override;

			void DrawWithShader(OGLShader* shader);

			vector<const RenderObject*> activeObjects;

			void SetupFBO(GLuint* FBO, GLuint* RBO, GLuint* tex);

			//post process things
			OGLShader* processDefaultShader;
			OGLShader* processGreyShader;
			OGLShader* processInvShader;
			OGLShader* sceneShader;
			Mesh* quad;

			std::queue<GLuint> screenFBOs;
			GLuint screenFBO0;
			GLuint screenFBO1;
			GLuint screenFBO2;
			GLuint screenFBO3;
			GLuint screenFBO4;
			GLuint screenRBO0;
			GLuint screenRBO1;
			GLuint screenRBO2;
			GLuint screenRBO3;
			GLuint screenRBO4;
			GLuint screenTex0;
			GLuint screenTex1;
			GLuint screenTex2;
			GLuint screenTex3;
			GLuint screenTex4;

			GLuint currentFBO;
			bool firstRender = true;

			GLuint processFBO;
			GLuint processTexture;
			GLuint rbo;

			bool greyPost = false;
			bool invPost = false;

			void SelectPostType();

			//shadow mapping things
			OGLShader* shadowShader;
			GLuint		shadowTex;
			GLuint		shadowFBO;
			Matrix4     shadowMatrix;

			Vector4		lightColour;
			float		lightRadius;
			Vector3		lightPosition;
		};
	}
}

