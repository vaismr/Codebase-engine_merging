#pragma once
#include "../../Plugins/OpenGLRendering/OGLRenderer.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"

#include "../CSC8503Common/GameWorld.h"


#include "Post Processing/Mesh.h"

#define POST_PASSES 10


#include "../../Common/TextureLoader.h"
#include <string>

namespace NCL {
	class Maths::Vector3;
	class Maths::Vector4;
	namespace CSC8503 {
		class RenderObject;

		class GameTechRenderer : public OGLRenderer {
		public:
			GameTechRenderer(GameWorld& world);
			~GameTechRenderer();

			void DoSaveMap() {
				_save = true;
			}
			void DoLoadMap() {
				_load = true;
			}
			void setLevelName(const char* name) {
				this->name = name;
			}

		protected:
			std::string name;
			bool _save = false;
			bool _load = false;
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

