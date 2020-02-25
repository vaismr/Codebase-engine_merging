#pragma once
#include "../GameTech/GameTechRenderer.h"

namespace NCL {
	namespace CSC8503 {
		class LoadingScreen {
		public:
			void StartLoadingScreen(GameWorld& gameWorld, GameTechRenderer* renderer) {
				this->renderer = renderer;
				gameWorld.GetMainCamera()->SetCameraType(CameraType::Orthographic);
				glClearColor(0, 0, 0, 0);
			}

			// is possible using threads rather than calling this every so often... but much harder
			void UpdateLoadingScreen() {
				glClearColor(0, 0, 0, 0);

				switch (loadCount) {
				case 0:
					renderer->DrawString("LOADING.", Vector2(50, 50)); break;
				case 1:
					renderer->DrawString("LOADING..", Vector2(50, 50)); break;
				case 2:
					renderer->DrawString("LOADING...", Vector2(50, 50)); break;
				case 3:
					renderer->DrawString("LOADING....", Vector2(50, 50)); break;
				default:
					renderer->DrawString("LOADING.....", Vector2(50, 50));
					loadCount = -1;
				}

				renderer->Render();
				loadCount++;
			}

			void EndLoadingScreen(GameWorld& gameWorld) {
				gameWorld.GetMainCamera()->SetCameraType(CameraType::Perspective);
				renderer = nullptr;
			}
		private:
			int loadCount = 0;

			GameTechRenderer* renderer;
		};
	}
}