#pragma once
#include "../GameTech/GameTechRenderer.h"

namespace NCL {
	namespace CSC8503 {
		class LoadingScreen {
		public:
			void StartLoadingScreen(GameWorld& gameWorld, GameTechRenderer& renderer) {
				gameWorld.GetMainCamera()->SetCameraType(CameraType::Orthographic);

				//@TODO UI STUFF HERE - just drawstring for now
				// animated LOADING... bar
				renderer.DrawString("LOADING...", Vector2(50, 100));
			}
			void EndLoadingScreen(GameWorld& gameWorld) {
				gameWorld.GetMainCamera()->SetCameraType(CameraType::Perspective);
			}
		};
	}
}