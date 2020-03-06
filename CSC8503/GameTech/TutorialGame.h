#pragma once
#include "GameTechRenderer.h"
#include "../CSC8503Common/PhysicsSystem.h"
#include "../CSC8503Common/EnemyAI.h"

#include <iostream>
#include "../GameTech/Sound/AudioEngine.h"
#include <imgui/imgui.h>
#include "../CSC8503Common/LoadingScreen.h"
#include "ball.h"
#include <ctime>
#include "psapi.h"
#include "icecube.h"

class LevelBase;

namespace NCL {
	namespace CSC8503 {

		enum class GameState {
			MAIN_MENU,
			LOADING,
			IN_GAME,
			PAUSED,
			END_GAME_WIN,
			END_GAME,
		};

		class TutorialGame {
			friend LevelBase;

		public:
			TutorialGame();
			~TutorialGame();

			virtual void UpdateGame(float dt);

			GameObject* AddFloorToWorld(const Vector3& position);
			GameObject* AddSphereToWorld(const Vector3& position, float radius, float inverseMass = 10.0f);
			Ball* AddPlayerToWorld(const Vector3& position, float radius, float inverseMass = 10.0f);
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);
			GameObject* AddIceToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);
			GameObject* AddWaterToWorld(const Vector3& position);
			GameObject* AddFirePowerUpToWorld(const Vector3& position);
			GameObject* AddIcePowerUpToWorld(const Vector3& position);
			GameObject* AddPortalToWorld(const Vector3& position);
			//IT'S HAPPENING
			GameObject* AddGooseToWorld(const Vector3& position);
			GameObject* AddParkKeeperToWorld(const Vector3& position);
			GameObject* AddCharacterToWorld(const Vector3& position);
			GameObject* AddAppleToWorld(const Vector3& position);
			GameObject* AddParticleToWorld(const Vector3& position, OGLTexture* texture, const float alpha);


			void InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius);
			void InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing);
			void InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims);

			bool closed;

			bool IsClosed()
			{
				return closed;
			}

			void SetWorld(GameWorld* a) {
				if (world) {
					delete world;
				}

				world = a;
			}

		protected:
			void InitialiseAssets();

			void InitCamera();
			void UpdateKeys();
			void UpdateAI(float dt);

			void InitWorld();
			void Initball();

			void TogglePauseMenu() {
				if (state == GameState::PAUSED) {
					state = GameState::IN_GAME;
					Window::GetWindow()->ShowOSPointer(false);
					Window::GetWindow()->LockMouseToWindow(true);
				}
				else if (state == GameState::IN_GAME) {
					state = GameState::PAUSED;
					Window::GetWindow()->ShowOSPointer(true);
					Window::GetWindow()->LockMouseToWindow(false);
				}
			}
			void UpdatePauseMenu();

			void ToggleEndgameMenu() {
				if (state == GameState::END_GAME) {
					state = GameState::IN_GAME;
					Window::GetWindow()->ShowOSPointer(false);
					Window::GetWindow()->LockMouseToWindow(true);
				}
				else if (state == GameState::IN_GAME) {
					state = GameState::END_GAME;
					Window::GetWindow()->ShowOSPointer(true);
					Window::GetWindow()->LockMouseToWindow(false);
				}
				Window::GetWindow()->ShowOSPointer(state == GameState::END_GAME);
			}
			void UpdateEndgameMenu();

			/*
			These are some of the world/object creation functions I created when testing the functionality
			in the module. Feel free to mess around with them to see different objects being created in different
			test scenarios (constraints, collision types, and so on).
			*/
			void BridgeConstraintTest();
			void SimpleGJKTest();

			bool SelectObject();
			void MoveSelectedObject();
			void DebugObjectMovement();
			void LockedObjectMovement();
			void LockedCameraMovement();
			
			float timeLeft = 0;
			//arrow
			float dir;
			void UpdateArrow();
			float Impulsesize = 0;
			Vector3 Impulsedir;
			Vector3 totalImpulse;
			Vector3 Arrowlength;

			void RenderInGameHud(float dt);
			void RenderMainGameMenu(float dt);
			void RenderPauseMenu(float dt);
			void TutorialGame::RenderEndgameMenu(float dt);
			void RenderDebugUi(float dt);

#pragma region Bridge
			Vector3 velocity1 = Vector3(300, 0, 0);
			Vector3 MoveBridgePosition;
			Transform MoveBridgeTransform;

#pragma endregion
			int backgroundMusic = -1;

			GameTechRenderer* renderer;
			PhysicsSystem* physics;
			GameWorld* world;

			//ball
			Ball* ball;
            //icecube
			Icecube* icecube;
			GameObject* AddIcecubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);
			
			//void Updateballco();

			bool useGravity;
			bool inSelectionMode;
			bool inDebugMode;

			float		forceMagnitude;
			float temppitch;

			ImFont* fontMainDlg = nullptr;
			ImFont* fontbutton = nullptr;
			ImFont* fontHeader = nullptr;

			GameObject* selectionObject = nullptr;

			OGLMesh* cubeMesh = nullptr;
			OGLMesh* sphereMesh = nullptr;
			OGLTexture* basicTex = nullptr;
			OGLTexture* backgroundTex = nullptr;
			OGLTexture* Itemicon = nullptr;

			OGLShader* basicShader = nullptr;
			OGLShader* particleShader = nullptr;

			//Coursework Meshes
			OGLMesh* gooseMesh = nullptr;
			OGLMesh* keeperMesh = nullptr;
			OGLMesh* appleMesh = nullptr;
			OGLMesh* charA = nullptr;
			OGLMesh* charB = nullptr;

			//Coursework Additional functionality	
			GameObject* lockedObject = nullptr;
			Vector3 lockedOffset = Vector3(0, 14, 20);
			
			void LockCameraToObject(GameObject* o) {
				lockedObject = o;
			}


			AudioEngine audioEngine;
			Vector3 lastCamPos; //store position of the camera in the last frame
			void UpdateListener(float dt);

			bool isPaused;

			GameState state = GameState::MAIN_MENU;
			int level_number = 1;
			LevelBase* level = nullptr;
			std::vector<LevelBase*> levels;


			OGLMesh* particleMesh = nullptr;
      
			LoadingScreen* loadingScreen;

			EnemyAIChase* chaseAI;
			EnemyAIPatrol* patrolAI;

			std::vector<Vector3> points = { Vector3(0,0,0), Vector3(10,0,10), Vector3(10, 0, 40), Vector3(40, 0, 40) };
		};
	}
}

