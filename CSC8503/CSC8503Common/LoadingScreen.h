#pragma once
#include "../GameTech/GameTechRenderer.h"
#include <thread>

/*
*	Example Usage: 
*
*	LoadingScreen ls = new LoadingScreen();
*
*	// anything that needs to load goes here
*
*	delete ls;
*	ls = nullptr;
*/

namespace NCL {
	namespace CSC8503 {
		class LoadingScreen {
		public:
			LoadingScreen() {
				loadingThread = std::thread([this] {this->Loading(); });

				isLoading = true;
			}

			~LoadingScreen() {
				isLoading = false;
				loadingThread.join();

				delete renderer;
				delete world;
				renderer = nullptr;
				world = nullptr;
			}

			void Loading() {
				// since it's a new thread, we need a new renderer
				world = new GameWorld();
				renderer = new GameTechRenderer(*world);
				StartLoadingScreen();
				while (isLoading) {
					UpdateLoadingScreen();
					Sleep(600);
				}
				EndLoadingScreen();
			}

			void StartLoadingScreen() {
				this->renderer = renderer;
				world->GetMainCamera()->SetCameraType(CameraType::Orthographic);
				glClearColor(0, 0, 0, 0);
			}

			void UpdateLoadingScreen() {
				glClearColor(0, 0, 0, 0);

				if (loadCount < 4) {
					loadingText += ".";
					renderer->DrawString(loadingText, textPos);
					loadCount++;
				}
				else {
					loadingText = "Loading.";
					renderer->DrawString(loadingText, textPos);
					loadCount = 0;
				}
				
				renderer->Render();
			}

			void EndLoadingScreen() {
				world->GetMainCamera()->SetCameraType(CameraType::Perspective);
				renderer = nullptr;
			}

		private:
			int loadCount = 0;
			Vector2 textPos = Vector2(50, 50);
			string loadingText = "Loading.";

			bool isLoading;

			std::thread loadingThread;

			GameWorld* world;
			GameTechRenderer* renderer;
		};
	}
}