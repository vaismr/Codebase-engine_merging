#include "GameTechRenderer.h"
#include "../CSC8503Common/GameObject.h"
#include "../../Common/Camera.h"
#include "../../Common/Vector2.h"
#include "../../Common/Vector3.h"

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

//#define STB_IMAGE_IMPLEMENTATION
//#include "../../Common/stb/stb_image.h"
//#include "../../Common/Assets.h"

using namespace NCL;
using namespace Rendering;
using namespace CSC8503;



#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>

#define SHADOWSIZE 4096

Matrix4 biasMatrix = Matrix4::Translation(Vector3(0.5, 0.5, 0.5)) * Matrix4::Scale(Vector3(0.5, 0.5, 0.5));

GameTechRenderer::GameTechRenderer(GameWorld& world) : OGLRenderer(*Window::GetWindow()), gameWorld(world)	{
	glEnable(GL_DEPTH_TEST);

	shadowShader = new OGLShader("GameTechShadowVert.glsl", "GameTechShadowFrag.glsl");
	
	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			     SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	//Set up the light properties
	lightColour = Vector4(0.8f, 0.8f, 0.5f, 1.0f);
	lightRadius = 1000.0f;
	lightPosition = Vector3(-200.0f, 60.0f, -200.0f);
  
//post processing additions
	quad = Mesh::GenerateQuad();
	processDefaultShader = new OGLShader("PostVertex.glsl", "PostFrag.glsl");
	processGreyShader = new OGLShader("PostVertex.glsl", "PostFragGrey.glsl");
	processInvShader = new OGLShader("PostVertex.glsl", "PostFragInv.glsl");
	sceneShader = new OGLShader("GameTechVert.glsl", "GameTechFrag.glsl");

	glGenTextures(1, &processTexture);
	glBindTexture(GL_TEXTURE_2D, processTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, currentWidth, currentHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &processFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, processTexture, 0);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, currentWidth, currentHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GenerateSkybox();
	GenerateIce();
	
}

void GameTechRenderer::GenerateSkybox() {
	skyshader = new OGLShader("skyvertex.glsl", "skyfrag.glsl");
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
	
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	vector<std::string> faces
	{
		"hangingstone_rt.tga",
		"hangingstone_lf.tga",
		"hangingstone_up.tga",
		"hangingstone_dn.tga",
		"hangingstone_bk.tga",
		"hangingstone_ft.tga"
	};
	cubemapTexture = loadCubemap(faces);



}
void GameTechRenderer::GenerateIce() {
	iceshader = new OGLShader("icecubev.glsl","reflect.glsl");
}

GameTechRenderer::~GameTechRenderer()	{
	glDeleteTextures(1, &shadowTex);
	glDeleteFramebuffers(1, &shadowFBO);

	delete processDefaultShader;
	delete processGreyShader;
	delete processInvShader;
	delete sceneShader;
	delete quad;

	glDeleteTextures(1, &processTexture);
	glDeleteFramebuffers(1, &processFBO);
	glDeleteRenderbuffers(1, &rbo);
}

void GameTechRenderer::RenderFrame() {
	glEnable(GL_CULL_FACE);
	glClearColor(1, 1, 1, 1);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
  

	
	
	BuildObjectList();
	SortObjectList();	
	RenderShadowMap();

	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	RenderCamera();
	RenderSkybox();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	
	glDisable(GL_DEPTH_TEST);
	SelectPostType();
	quad->SetTexture(processTexture);
	quad->Draw();
	glDisable(GL_CULL_FACE); //Todo - text indices are going the wrong way...

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
unsigned int GameTechRenderer::loadCubemap(vector<std::string> faces) {

	
	glGenTextures(1, &skytextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skytextureID);
char* texData = nullptr;
	int width = 0;
	int height = 0;
	int channels = 0;
	int flags = 0;
	//int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{

		TextureLoader::LoadTexture(faces[i].c_str(), texData, width, height, channels, flags);

		OGLTexture::CubeTextureFromData(texData, width, height, channels, i,skytextureID);

		free(texData);
		/*
		int width, height, nrChannels;
		//const char* c = faces[i].c_str();
		std::string realPath = Assets::TEXTUREDIR + faces[i].c_str();
		unsigned char* data = stbi_load(realPath.c_str(), &width, &height, &nrChannels, 0);
		//unsigned char* data = TextureLoader::readskytex(c);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
		*/
		
	}

	
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	return skytextureID;

}

void GameTechRenderer::RenderSkybox() {
	

	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	BindShader(skyshader);
	Matrix4 viewMatrix = Matrix4(Matrix3(gameWorld.GetMainCamera()->BuildViewMatrix())); // remove translation from the view matrix
	//skyboxShader.setMat4("view", view);
	//skyboxShader.setMat4("projection", projection);
	float screenAspect = (float)currentWidth / (float)currentHeight;
	//Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);
	
	int projLocation = glGetUniformLocation(skyshader->GetProgramID(), "projection");
	int viewLocation = glGetUniformLocation(skyshader->GetProgramID(), "view");
	glUniformMatrix4fv(projLocation, 1, false, (float*)&projMatrix);
	glUniformMatrix4fv(viewLocation, 1, false, (float*)&viewMatrix);

	int cubetexLocation = glGetUniformLocation(skyshader->GetProgramID(), "skybox");
	glUniform1i(cubetexLocation, 0);


	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
	
};

void GameTechRenderer::RenderLoadingFrame() {
	RenderCamera();
}

void GameTechRenderer::BuildObjectList() {
	std::vector<GameObject*>::const_iterator first;
	std::vector<GameObject*>::const_iterator last;

	gameWorld.GetObjectIterators(first, last);

	activeObjects.clear();

	const std::string filename = "map/" + name + "_map.txt";
	std::fstream fs;
	if (_save) {
		fs.open(filename, std::fstream::out);
	}
	if (_load) {
		fs.open(filename, std::fstream::in);
	}
	for (std::vector<GameObject*>::const_iterator i = first; i != last; ++i) {
		if ((*i)->IsActive()) {
			const RenderObject*g = (*i)->GetRenderObject();
			if (g) {
				activeObjects.emplace_back(g);
			}
		}
		if (fs.is_open()) {
			auto& pos = (*i)->GetTransform().GetWorldPosition();
			if (_save) {
				fs << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
			}
			else if (_load) {
				char buffer[4096];
				Vector3 pos;
				fs.getline(buffer, _countof(buffer));
				sscanf_s(buffer, "%f, %f, %f", &pos.x, &pos.y, &pos.z);
				(*i)->GetTransform().SetWorldPosition(pos);
			}
		}
	}

	if (fs.is_open()) {
		fs.close();
	}

	_save = false;
	_load = false;
}

void GameTechRenderer::SortObjectList() {

}

void GameTechRenderer::RenderShadowMap() {
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);

	glCullFace(GL_FRONT);

	BindShader(shadowShader);
	int mvpLocation = glGetUniformLocation(shadowShader->GetProgramID(), "mvpMatrix");

	Matrix4 shadowViewMatrix = Matrix4::BuildViewMatrix(lightPosition, Vector3(0, 0, 0), Vector3(0,1,0));
	Matrix4 shadowProjMatrix = Matrix4::Perspective(100.0f, 500.0f, 1, 45.0f);

	Matrix4 mvMatrix = shadowProjMatrix * shadowViewMatrix;

	shadowMatrix = biasMatrix * mvMatrix; //we'll use this one later on

	for (const auto&i : activeObjects) {
		Matrix4 modelMatrix = (*i).GetTransform()->GetWorldMatrix();
		Matrix4 mvpMatrix	= mvMatrix * modelMatrix;
		glUniformMatrix4fv(mvpLocation, 1, false, (float*)&mvpMatrix);
		BindMesh((*i).GetMesh());
		DrawBoundMesh();
	}

	glViewport(0, 0, currentWidth, currentHeight);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glCullFace(GL_BACK);
}

void GameTechRenderer::RenderCamera() 
{
	glClear((GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

	float screenAspect = (float)currentWidth / (float)currentHeight;
	Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);

	OGLShader* activeShader = nullptr;
	int projLocation = 0;
	int viewLocation = 0;
	int modelLocation = 0;
	int colourLocation = 0;
	int hasVColLocation = 0;
	int hasTexLocation = 0;
	int shadowLocation = 0;

	int lightPosLocation = 0;
	int lightColourLocation = 0;
	int lightRadiusLocation = 0;

	int cameraLocation = 0;

	float pTime = 0.0f;

	//TODO - PUT IN FUNCTION
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, shadowTex);


	for (const auto& i : activeObjects) {
		if (i->GetRenderName() != "icecube") {
			OGLShader* shader = (OGLShader*)(*i).GetShader();
			BindShader(shader);

			BindTextureToShader((OGLTexture*)(*i).GetDefaultTexture(), "mainTex", 0);

			if (activeShader != shader) {
				projLocation = glGetUniformLocation(shader->GetProgramID(), "projMatrix");
				viewLocation = glGetUniformLocation(shader->GetProgramID(), "viewMatrix");
				modelLocation = glGetUniformLocation(shader->GetProgramID(), "modelMatrix");
				shadowLocation = glGetUniformLocation(shader->GetProgramID(), "shadowMatrix");
				colourLocation = glGetUniformLocation(shader->GetProgramID(), "objectColour");
				hasVColLocation = glGetUniformLocation(shader->GetProgramID(), "hasVertexColours");
				hasTexLocation = glGetUniformLocation(shader->GetProgramID(), "hasTexture");

				lightPosLocation = glGetUniformLocation(shader->GetProgramID(), "lightPos");
				lightColourLocation = glGetUniformLocation(shader->GetProgramID(), "lightColour");
				lightRadiusLocation = glGetUniformLocation(shader->GetProgramID(), "lightRadius");



				cameraLocation = glGetUniformLocation(shader->GetProgramID(), "cameraPos");
				glUniform3fv(cameraLocation, 1, (float*)&gameWorld.GetMainCamera()->GetPosition());

				glUniformMatrix4fv(projLocation, 1, false, (float*)&projMatrix);
				glUniformMatrix4fv(viewLocation, 1, false, (float*)&viewMatrix);

				glUniform3fv(lightPosLocation, 1, (float*)&lightPosition);
				glUniform4fv(lightColourLocation, 1, (float*)&lightColour);
				glUniform1f(lightRadiusLocation, lightRadius);

				int shadowTexLocation = glGetUniformLocation(shader->GetProgramID(), "shadowTex");
				glUniform1i(shadowTexLocation, 1);

				activeShader = shader;
			}

			pTime = glGetUniformLocation(shader->GetProgramID(), "time");
			glUniform1f(pTime, particleTime);

			Matrix4 modelMatrix = (*i).GetTransform()->GetWorldMatrix();
			glUniformMatrix4fv(modelLocation, 1, false, (float*)&modelMatrix);

			Matrix4 fullShadowMat = shadowMatrix * modelMatrix;
			glUniformMatrix4fv(shadowLocation, 1, false, (float*)&fullShadowMat);

			glUniform4fv(colourLocation, 1, (float*)&i->GetColour());

			glUniform1i(hasVColLocation, !(*i).GetMesh()->GetColourData().empty());

			glUniform1i(hasTexLocation, (OGLTexture*)(*i).GetDefaultTexture() ? 1 : 0);

			BindMesh((*i).GetMesh());
			DrawBoundMesh();
		}
		//render icecube	
		else {
			BindShader(iceshader);
			//Bind Texture of skybox
			int cubetexLocation = glGetUniformLocation(iceshader->GetProgramID(), "skybox");
			glUniform1i(cubetexLocation, 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

			projLocation = glGetUniformLocation(iceshader->GetProgramID(), "projection");
			viewLocation = glGetUniformLocation(iceshader->GetProgramID(), "view");
			modelLocation = glGetUniformLocation(iceshader->GetProgramID(), "model");
			cameraLocation = glGetUniformLocation(iceshader->GetProgramID(), "cameraPos");

			glUniform3fv(cameraLocation, 1, (float*)&gameWorld.GetMainCamera()->GetPosition());

			glUniformMatrix4fv(projLocation, 1, false, (float*)&projMatrix);
			glUniformMatrix4fv(viewLocation, 1, false, (float*)&viewMatrix);
			Matrix4 modelMatrix = (*i).GetTransform()->GetWorldMatrix();
			glUniformMatrix4fv(modelLocation, 1, false, (float*)&modelMatrix);
			BindMesh((*i).GetMesh());
			DrawBoundMesh();
		}

	};

}


void GameTechRenderer::SetupDebugMatrix(OGLShader*s) {
	float screenAspect = (float)currentWidth / (float)currentHeight;
	Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);

	Matrix4 vp = projMatrix * viewMatrix;

	int matLocation = glGetUniformLocation(s->GetProgramID(), "viewProjMatrix");

	glUniformMatrix4fv(matLocation, 1, false, (float*)&vp);
}

void GameTechRenderer::DrawWithShader(OGLShader* shader)
{
	glDisable(GL_DEPTH_TEST);
	BindShader(shader);
	glUniform1i(glGetUniformLocation(shader->GetProgramID(), "texture1"), 0);
	Matrix4 modelMatrix = Matrix4::Rotation(180.0f, Vector3(0, 0, 1)) * Matrix4::Rotation(180.0f, Vector3(0, 1, 0));
	glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "model"), 1, false, (float*)&modelMatrix);
}

void GameTechRenderer::SelectPostType()
{
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUMPAD1))
	{
		greyPost = true;
		invPost = false;
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUMPAD2))
	{
		greyPost = false;
		invPost = true;
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUMPAD0))
	{
		greyPost = false;
		invPost = false;
	}

	if (greyPost)
	{
		DrawWithShader(processGreyShader);
	}
	else if (invPost)
	{
		DrawWithShader(processInvShader);
	}
	else
	{
		DrawWithShader(processDefaultShader);
	}
}
