#pragma once
#include "../../Plugins/OpenGLRendering/OGLRenderer.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"

#include "../CSC8503Common/GameWorld.h"
#include "../../Common/TextureLoader.h"
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

			OGLShader*		defaultShader;

			GameWorld&	gameWorld;

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

			void SetupDebugMatrix(OGLShader*s) override;

			vector<const RenderObject*> activeObjects;

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

