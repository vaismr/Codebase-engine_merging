#pragma once
#include "GameTechRenderer.h"
#include "../CSC8503Common/PhysicsSystem.h"
#include <imgui/imgui.h>
#include "../CSC8503Common/LoadingScreen.h"

class LevelBase;

namespace NCL {
	namespace CSC8503 {

		enum class GameState {
			MAIN_MENU,
			LOADING,
			IN_GAME,
			PAUSED,
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
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);
			//IT'S HAPPENING
			GameObject* AddGooseToWorld(const Vector3& position);
			GameObject* AddParkKeeperToWorld(const Vector3& position);
			GameObject* AddCharacterToWorld(const Vector3& position);
			GameObject* AddAppleToWorld(const Vector3& position);


			void InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius);
			void InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing);
			void InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims);

			bool closed;

			bool IsClosed()
			{
				return closed;
			}

		protected:
			void InitialiseAssets();

			void InitCamera();
			void UpdateKeys();

			void InitWorld();

			void TogglePauseMenu() {
				if (state == GameState::PAUSED) {
					state = GameState::IN_GAME;
				}
				else if (state == GameState::IN_GAME) {
					state = GameState::PAUSED;
				}
				Window::GetWindow()->ShowOSPointer(state == GameState::PAUSED);
			}
			void UpdatePauseMenu();

			void ToggleEndgameMenu() {
				if (state == GameState::END_GAME) {
					state = GameState::IN_GAME;
				}
				else if (state == GameState::IN_GAME) {
					state = GameState::END_GAME;
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

			void RenderInGameHud(float dt);
			void RenderMainGameMenu(float dt);
			void RenderPauseMenu(float dt);
			void TutorialGame::RenderEndgameMenu(float dt);

			GameTechRenderer* renderer;
			PhysicsSystem* physics;
			GameWorld* world;

			bool useGravity;
			bool inSelectionMode;

			float		forceMagnitude;

			ImFont* fontMainDlg = nullptr;

			GameObject* selectionObject = nullptr;

			OGLMesh* cubeMesh = nullptr;
			OGLMesh* sphereMesh = nullptr;
			OGLTexture* basicTex = nullptr;
			OGLShader* basicShader = nullptr;

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

			GameState state = GameState::MAIN_MENU;
			int level_number = 1;
			LevelBase* level = nullptr;
			std::vector<LevelBase*> levels;

			LoadingScreen* loadingScreen;
		};
	}
}

