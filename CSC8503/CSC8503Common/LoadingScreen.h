#pragma once
#include "../GameTech/GameTechRenderer.h"

namespace NCL {
	namespace CSC8503 {
		class LoadingScreen {
		public:
			void StartLoadingScreen(GameWorld& gameWorld, GameTechRenderer& renderer) {
				gameWorld.GetMainCamera()->SetCameraType(CameraType::Orthographic);
			}

			// is possible using threads rather than calling this every so often... but much harder
			void UpdateLoadingScreen(GameTechRenderer& renderer) {
				if(loadCount == 0)
					renderer.DrawString("LOADING.", Vector2(50, 50));
				else if(loadCount == 1)
					renderer.DrawString("LOADING..", Vector2(50, 50));
				else {
					renderer.DrawString("LOADING...", Vector2(50, 50));
					loadCount = 0;
				}

				renderer.Render();
				loadCount++;
			}

			void EndLoadingScreen(GameWorld& gameWorld) {
				gameWorld.GetMainCamera()->SetCameraType(CameraType::Perspective);
			}
		private:
			int loadCount = 0;
		};
	}
}