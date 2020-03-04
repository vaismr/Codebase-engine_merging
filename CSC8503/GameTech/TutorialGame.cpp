#include "TutorialGame.h"
#include "../CSC8503Common/GameWorld.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Common/TextureLoader.h"

#include "../../Common/Assets.h"

#include "../CSC8503Common/PositionConstraint.h"

#include <iostream>


#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_win32.h>

#include "imgui_progressbar.h"

#include "LevelTest.h"

#include "Level1.h"

#include "../CSC8503Common/LoadingScreen.h"


using namespace NCL;
using namespace CSC8503;



TutorialGame::TutorialGame()	{
	world		= new GameWorld();
	renderer	= new GameTechRenderer(*world);
	physics		= new PhysicsSystem(*world);

	forceMagnitude	= 10.0f;
	useGravity		= false;
	inSelectionMode = false;

	Debug::SetRenderer(renderer);


	InitialiseAssets();

	audioEngine.Init();
	audioEngine.LoadSound("../../Assets/Sounds/jaguar.wav");
	audioEngine.LoadSound("../../Assets/Sounds/wave.mp3");
	/*audioEngine.Set3DListenerAndOrientation(Vec3{ 0,10,0 });*/


	//levels.push_back(new LevelTest()); // level 0
	//levels.push_back(new Level1());
	//levels.push_back(new Level1());

	//;
	//levels.push_back(new Level1());
	//levels.push_back(new Level1()); // level 5


}

/*

Each of the little demo scenarios used in the game uses the same 2 meshes, 
and the same texture and shader. There's no need to ever load in anything else
for this module, even in the coursework, but you can add it if you like!

*/
void TutorialGame::InitialiseAssets() {
	auto loadFunc = [](const string& name, OGLMesh** into) {
		*into = new OGLMesh(name);
		(*into)->SetPrimitiveType(GeometryPrimitive::Triangles);
		(*into)->UploadToGPU();
	};

	loadFunc("Particle.msh"		 , &particleMesh);
	loadFunc("cube.msh"		 , &cubeMesh);
	loadFunc("sphere.msh"	 , &sphereMesh);
	loadFunc("goose.msh"	 , &gooseMesh);
	loadFunc("CharacterA.msh", &keeperMesh);
	loadFunc("CharacterM.msh", &charA);
	loadFunc("CharacterF.msh", &charB);
	loadFunc("Apple.msh"	 , &appleMesh);

	particleMesh->SetPrimitiveType(GeometryPrimitive::Points);

	basicTex	= (OGLTexture*)TextureLoader::LoadAPITexture("checkerboard.png");
	basicShader = new OGLShader("GameTechVert.glsl", "GameTechFrag.glsl");
	particleShader = new OGLShader("particleVert.glsl", "particleFrag.glsl", "particleGeom.glsl");


	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontDefault();
	std::string pathMainDlgFont = Assets::FONTSSDIR + "/FiraSans-Regular.otf";
	fontMainDlg = io.Fonts->AddFontFromFileTTF(pathMainDlgFont.c_str(), 40);
	fontbutton = io.Fonts->AddFontFromFileTTF(pathMainDlgFont.c_str(), 36);
	fontHeader = io.Fonts->AddFontFromFileTTF(pathMainDlgFont.c_str(), 60);


	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);


	InitCamera();
	InitWorld();

}

TutorialGame::~TutorialGame()	{
	delete cubeMesh;
	delete sphereMesh;
	delete gooseMesh;
	delete basicTex;
	delete basicShader;

	delete physics;
	delete renderer;
	delete world;

	delete particleMesh;
	delete particleShader;

	audioEngine.Shutdown();
}

void TutorialGame::UpdateGame(float dt) {

	lastCamPos = world->GetMainCamera()->GetPosition(); //get this before camera is updated below

	if (!isPaused) {
		ImGui_ImplOpenGL3_NewFrame();
#if _WIN32
		ImGui_ImplWin32_NewFrame();
#elif __ORBIS__
		// TODO: PS4 New Frame?? GLFW + gl3w?
		ImGui_Impl ? ? ? ? _NewFrame();
#endif
		ImGui::NewFrame();

		switch (state) {
		case GameState::MAIN_MENU:
			RenderMainGameMenu(dt);
			break;

		case GameState::LOADING:

			loadingScreen = new LoadingScreen();

			//level = levels[level_number];
			InitWorld();
			//Sleep(200);
			state = GameState::IN_GAME;
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);

			delete loadingScreen;
			loadingScreen = nullptr;
			break;

		case GameState::IN_GAME:

			if (!inSelectionMode) {
				world->GetMainCamera()->UpdateCamera(dt);
			}
			if (lockedObject != nullptr) {
				LockedCameraMovement();
			}

			UpdateKeys();

			/*if (useGravity) {
				Debug::Print("(G)ravity on", Vector2(10, 40));
			}
			else {
				Debug::Print("(G)ravity off", Vector2(10, 40));
			}*/

			SelectObject();
			MoveSelectedObject();

			world->UpdateWorld(dt);
			renderer->Update(dt);
			physics->Update(dt);


			Debug::FlushRenderables();
			UpdateListener(dt);
			audioEngine.Update();

			RenderInGameHud(dt);
			break;

		case GameState::PAUSED:
			
			//Debug::Print("Game Paused", Vector2(50, 100));
			UpdatePauseMenu();

			RenderInGameHud(0);
			RenderPauseMenu(dt);
			
			break;

		case GameState::END_GAME:

			UpdateEndgameMenu();

			RenderEndgameMenu(dt);

			break;
		}
	}
	ImGui::Render();

	Debug::FlushRenderables();
	renderer->Render();
}


void TutorialGame::UpdateListener(float dt)
{
	//update audio

	Vec3 cameraPos = Vec3{ world->GetMainCamera()->GetPosition().x, world->GetMainCamera()->GetPosition().y , world->GetMainCamera()->GetPosition().z };

	//calculate distance between camera in this frame and last frame, using dt to get velocity - used for doppler effect
	Vec3 cameraVelocity = Vec3{ (world->GetMainCamera()->GetPosition().x - lastCamPos.x) / dt, (world->GetMainCamera()->GetPosition().y - lastCamPos.y) / dt, (world->GetMainCamera()->GetPosition().z - lastCamPos.z) / dt };

	

	float cosPitch = cos(world->GetMainCamera()->GetPitch());
	float cosYaw = cos(world->GetMainCamera()->GetYaw());
	float sinPitch = sin(world->GetMainCamera()->GetPitch());
	float sinYaw = sin(world->GetMainCamera()->GetYaw());

	float comp0 = cosPitch * cosYaw;
	float comp1 = cosPitch * sinYaw;
	float comp2 = sinPitch;

	float forwardMagnitude = sqrt((comp0 * comp0) + (comp1 * comp1) + (comp2 * comp2));

	Vec3 cameraForward = Vec3{ comp0 / forwardMagnitude, comp1 / forwardMagnitude, comp2 / forwardMagnitude }; //forwards orientation, unit vector and perpendicular to up

	//up = (forward x 0,1,0) x forward
	Vec3 cameraUpTemp = Vec3{ (cameraForward.y * 0) - (cameraForward.z * 1), (cameraForward.z * 0) - (cameraForward.x * 0), (cameraForward.x * 1) - (cameraForward.y * 0) };
	Vec3 cameraUp = Vec3{ (cameraUpTemp.y * cameraForward.z) - (cameraUpTemp.z * cameraForward.y), (cameraUpTemp.z * cameraForward.x) - (cameraUpTemp.x * cameraForward.z), (cameraUpTemp.x * cameraForward.y) - (cameraUpTemp.y * cameraForward.x) };

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::N))
	{
		cout << cameraUp.x << " " << cameraUp.y << " " << cameraUp.z << endl;
	}

	//camerup giving negative values, not allowed so not setting listener, maybe normalise? maybe magnitude value?
	audioEngine.Set3DListenerAndOrientation(cameraPos, cameraVelocity, cameraForward, cameraUp);
	//audioEngine.Set3DListenerAndOrientation(cameraPos, cameraVelocity, cameraForward, Vec3{0,1,0});

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::N))
	{
		cout << lastCamPos.x << endl;
	}		
}


void TutorialGame::UpdatePauseMenu() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::P))
	{
		TogglePauseMenu();
		audioEngine.TogglePauseAllChannels();
	}		

}

void TutorialGame::UpdateEndgameMenu() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::E))
		ToggleEndgameMenu();

}

void TutorialGame::RenderMainGameMenu(float dt) {

	ImGuiWindowFlags Miainflags =
		ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoScrollbar
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoNav
			| ImGuiWindowFlags_NoBringToFrontOnFocus;
	
	auto dl = ImGui::GetBackgroundDrawList();
	dl->AddImage(basicTex, ImVec2(0, 0), ImVec2(1920, 1080));

	 ImGui::SetNextWindowPos(ImVec2(454,190), ImGuiCond_Once);
	 ImGui::SetNextWindowSize(ImVec2(336,371), ImGuiCond_Once);

	ImGui::Begin("Main Menu",nullptr,Miainflags );

	ImGui::PushItemWidth(ImGui::GetWindowSize().x * 1.0f);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 25);
	ImGui::PushFont(fontMainDlg);
	if (ImGui::Button("START GAME", ImVec2(-1.0f, 0.0f))) {
		state = GameState::LOADING;
		level_number = 0;
	}


	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 25);
	if (ImGui::Button("CHOICE LEVEL", ImVec2(-1.0f, 0.0f))) {
		// LOAD_LEVEL
		state = GameState::LOADING;
	/*	level_number = level_number - 1;*/
	}
	//ImGui::SliderInt("level", &level_number, 1, levels.size());

	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 25);
	if (ImGui::Button("OPTIONS", ImVec2(-1.0f, 0.0f))) {
		
	}

	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 25);
	if (ImGui::Button("QUIT GAME", ImVec2(-1.0f, 0.0f))) {
		closed = true;
	}

	ImGui::PopFont();
	ImGui::End();
}

void TutorialGame::RenderPauseMenu(float dt) {
	
	ImGuiWindowFlags pausedflags =
		ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoScrollbar
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoNav
		| ImGuiWindowFlags_NoBringToFrontOnFocus;

	ImGui::SetNextWindowPos(ImVec2(511,314), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(229,176), ImGuiCond_Once);


	ImGui::Begin("Pause menu",nullptr, pausedflags);
	ImGui::PushItemWidth(ImGui::GetWindowSize().x * 1.0f);
	// RGBA
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 1000));
	ImGui::PushFont(fontMainDlg);
	if (ImGui::Button("MAIN MENU", ImVec2(-1.0f, 0.0f))) {
		 state = GameState::MAIN_MENU;
	}
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20);
	if (ImGui::Button("QUIT GAME", ImVec2(-1.0f, 0.0f))) {
		closed = true;
	}
	ImGui::PopFont();
	ImGui::PopStyleColor();

	ImGui::End();
}

void TutorialGame::RenderEndgameMenu(float dt) {
	
	ImGuiWindowFlags endflags =
		ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoScrollbar
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoNav
		| ImGuiWindowFlags_NoBringToFrontOnFocus;

	ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoScrollbar
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoNav
		| ImGuiWindowFlags_NoBackground
		| ImGuiWindowFlags_NoBringToFrontOnFocus;
	
	ImGui::SetNextWindowPos(ImVec2(471,168), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(318,89), ImGuiCond_Always);


	ImGui::Begin("Endgamehud", nullptr, flags);
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 1000));
	ImGui::PushFont(fontHeader);
	ImGui::Text(" GAME OVER "); 
	ImGui::PopFont();
	ImGui::PopStyleColor();
	ImGui::End();

	ImGui::SetNextWindowPos(ImVec2(518,331), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(224,161), ImGuiCond_Always);

	ImGui::Begin("Endgame", nullptr, endflags);
	ImGui::PushItemWidth(ImGui::GetWindowSize().x * 1.0f);
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 255, 1000));
	ImGui::PushFont(fontMainDlg);
	
	if (ImGui::Button("MAIN MENU", ImVec2(-1.0f, 0.0f))) {
		state = GameState::MAIN_MENU;
	}
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20);
	if (ImGui::Button("quit game", ImVec2(-1.0f, 0.0f))) {
		closed = true;
	}
	ImGui::SetWindowFontScale(1.0f);
	ImGui::PopFont();
	ImGui::PopStyleColor();
	ImGui::End();

}

void TutorialGame::RenderInGameHud(float dt) {
	// Start the Dear ImGui frame
	static float f = 100.0f;
	f -= dt;
	float power = 0.5f;

	const ImU32 red = 0xFF0000FF;
	const ImU32 col = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
	const ImU32 bg = ImGui::GetColorU32(ImGuiCol_Button);

	
	ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoScrollbar
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoNav
		| ImGuiWindowFlags_NoBackground
		| ImGuiWindowFlags_NoBringToFrontOnFocus;
	
	

	//Time window
	ImGui::SetNextWindowPos(ImVec2(466,6), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(317,95), ImGuiCond_Always);
	ImGui::Begin("Time Window", nullptr, flags);
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 255, 500));
	ImGui::PushFont(fontHeader);

	ImGui::Text(" Time : %.2fs", f);             
	ImGui::PopFont();
	ImGui::PopStyleColor();

	ImGui::End();

	//power window
	ImGui::SetNextWindowPos(ImVec2(16,575), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(344,109), ImGuiCond_Always);

	ImGui::Begin("Power window", nullptr, flags); 
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 255, 500));
	ImGui::PushFont(fontMainDlg);
	ImGui::Text("Power Bar");               // Display some text (you can use a format strings too)
	ImGui::PopFont();
	ImGui::ProgressBar("##progress_bar1", power, ImVec2(500, 25), red, col);
	ImGui::PopStyleColor();

	ImGui::End();

	//Name window
	ImGui::SetNextWindowPos(ImVec2(966,593), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(276,71), ImGuiCond_Always);
	ImGui::Begin("Name window", nullptr, flags); 
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 255, 500));
	ImGui::PushFont(fontMainDlg);
	static char name[32] = "Unknown";
	char buf[64]; sprintf_s(buf, IM_ARRAYSIZE(buf), "Name: %s###ButtonChangeName", name);

	if (ImGui::Button(buf))
	{
		ImGui::OpenPopup("PopupNameEditor");
	}

	// Popup

	if (ImGui::BeginPopup("PopupNameEditor")) {
		ImGui::Text("Your name:");
		ImGui::InputText("##edit", name, IM_ARRAYSIZE(name));
		if (ImGui::Button("Close"))
			ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}
	ImGui::PopFont();
	ImGui::PopStyleColor();

	ImGui::End();
}

void TutorialGame::UpdateKeys() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1)) {
		InitWorld(); //We can reset the simulation at any time with F1
		selectionObject = nullptr;
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F2)) {
		InitCamera(); //F2 will reset the camera to a specific default place
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::P))
	{
		TogglePauseMenu();
		audioEngine.TogglePauseAllChannels();
	}
		

	
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::E))
		ToggleEndgameMenu();

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::G)) {
		useGravity = !useGravity; //Toggle gravity!
		physics->UseGravity(useGravity);
	}
	//Running certain physics updates in a consistent order might cause some
	//bias in the calculations - the same objects might keep 'winning' the constraint
	//allowing the other one to stretch too much etc. Shuffling the order so that it
	//is random every frame can help reduce such bias.
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F9)) {
		world->ShuffleConstraints(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F10)) {
		world->ShuffleConstraints(false);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F7)) {
		world->ShuffleObjects(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F8)) {
		world->ShuffleObjects(false);
	}
	
	//sounds testing
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::O)) 
	{
		//audioEngine.PlaySounds("../../Assets/Sounds/jaguar.wav", Vec3{0,10,0}, 10.0f);
		audioEngine.PrintListenerPos();
	}
	
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::L)) 
	{
		audioEngine.PlaySounds("../../Assets/Sounds/wave.mp3", Vec3{-60,40,60}, 20.0f);
	}

	if (lockedObject) {
		LockedObjectMovement();
	}
	else {
		DebugObjectMovement();
	}
}

void TutorialGame::LockedObjectMovement() {
	Matrix4 view		= world->GetMainCamera()->BuildViewMatrix();
	Matrix4 camWorld	= view.Inverse();

	Vector3 rightAxis = Vector3(camWorld.GetColumn(0)); //view is inverse of model!

	//forward is more tricky -  camera forward is 'into' the screen...
	//so we can take a guess, and use the cross of straight up, and
	//the right axis, to hopefully get a vector that's good enough!

	Vector3 fwdAxis = Vector3::Cross(Vector3(0, 1, 0), rightAxis);

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
		selectionObject->GetPhysicsObject()->AddForce(-rightAxis);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
		selectionObject->GetPhysicsObject()->AddForce(rightAxis);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
		selectionObject->GetPhysicsObject()->AddForce(fwdAxis);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
		selectionObject->GetPhysicsObject()->AddForce(-fwdAxis);
	}
}

void  TutorialGame::LockedCameraMovement() {
	if (lockedObject != nullptr) {
		Vector3 objPos = lockedObject->GetTransform().GetWorldPosition();
		Vector3 camPos = objPos + lockedOffset;

		Matrix4 temp = Matrix4::BuildViewMatrix(camPos, objPos, Vector3(0, 1, 0));

		Matrix4 modelMat = temp.Inverse();

		Quaternion q(modelMat);
		Vector3 angles = q.ToEuler(); //nearly there now!

		world->GetMainCamera()->SetPosition(camPos);
		world->GetMainCamera()->SetPitch(angles.x);
		world->GetMainCamera()->SetYaw(angles.y);
	}
}

void TutorialGame::DebugObjectMovement() {
//If we've selected an object, we can manipulate it with some key presses
	if (inSelectionMode && selectionObject) {
		//Twist the selected object!
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(-10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM7)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, 10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM8)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, -10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, -10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, 10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM5)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, -10, 0));
		}
	}
}

/*

Every frame, this code will let you perform a raycast, to see if there's an object
underneath the cursor, and if so 'select it' into a pointer, so that it can be
manipulated later. Pressing Q will let you toggle between this behaviour and instead
letting you move the camera around.

*/
bool TutorialGame::SelectObject() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::Q)) {
		inSelectionMode = !inSelectionMode;
		if (inSelectionMode) {
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(false);
		}
		else {
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
		}
	}
	if (inSelectionMode) {
		renderer->DrawString("Press Q to change to camera mode!", Vector2(10, 0));

		if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::LEFT)) {
			if (selectionObject) {	//set colour to deselected;
				selectionObject->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
				selectionObject = nullptr;
			}

			Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

			RayCollision closestCollision;
			if (world->Raycast(ray, closestCollision, true)) {
				selectionObject = (GameObject*)closestCollision.node;
				selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
				return true;
			}
			else {
				return false;
			}
		}
		if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::L)) {
			if (selectionObject) {
				if (lockedObject == selectionObject) {
					lockedObject = nullptr;
				}
				else {
					lockedObject = selectionObject;
				}
			}
		}
	}
	else {
		//renderer->DrawString("Press Q to change to select mode!", Vector2(10, 0));
	}
	return false;
}

/*
If an object has been clicked, it can be pushed with the right mouse button, by an amount
determined by the scroll wheel. In the first tutorial this won't do anything, as we haven't
added linear motion into our physics system. After the second tutorial, objects will move in a straight
line - after the third, they'll be able to twist under torque aswell.
*/

void TutorialGame::MoveSelectedObject() {

}

void TutorialGame::InitCamera() {
	world->GetMainCamera()->SetNearPlane(0.5f);
	world->GetMainCamera()->SetFarPlane(500.0f);
	world->GetMainCamera()->SetPitch(-15.0f);
	world->GetMainCamera()->SetYaw(315.0f);
	world->GetMainCamera()->SetPosition(Vector3(-60, 40, 60));
	lockedObject = nullptr;
}

void TutorialGame::InitWorld() {
	bool closed = false;
	InitCamera();

	world->ClearAndErase();
	physics->Clear();


	InitMixedGridWorld(10, 10, 3.5f, 3.5f);
	AddGooseToWorld(Vector3(30, 2, 0));
	AddAppleToWorld(Vector3(35, 2, 0));

	AddParkKeeperToWorld(Vector3(40, 2, 0));
	AddCharacterToWorld(Vector3(45, 2, 0));

	AddParticleToWorld(Vector3(40, 20, 0), basicTex, 0.5);

	//AddFloorToWorld(Vector3(0, -2, 0));

	if (level) {
		level->init(this);
	}

}

//From here on it's functions to add in objects to the world!

/*

A single function to add a large immoveable cube to the bottom of our world

*/
GameObject* TutorialGame::AddFloorToWorld(const Vector3& position) {
	GameObject* floor = new GameObject();

	Vector3 floorSize = Vector3(100, 2, 100);
	AABBVolume* volume = new AABBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform().SetWorldScale(floorSize);
	floor->GetTransform().SetWorldPosition(position);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(floor);

	return floor;
}

/*

Builds a game object that uses a sphere mesh for its graphics, and a bounding sphere for its
rigid body representation. This and the cube function will let you build a lot of 'simple' 
physics worlds. You'll probably need another function for the creation of OBB cubes too.

*/
GameObject* TutorialGame::AddSphereToWorld(const Vector3& position, float radius, float inverseMass) {
	GameObject* sphere = new GameObject();

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);
	sphere->GetTransform().SetWorldScale(sphereSize);
	sphere->GetTransform().SetWorldPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(sphere);

	return sphere;
}

GameObject* TutorialGame::AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass) {
	GameObject* cube = new GameObject();

	AABBVolume* volume = new AABBVolume(dimensions);

	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform().SetWorldPosition(position);
	cube->GetTransform().SetWorldScale(dimensions);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(cube);

	return cube;
}

GameObject* TutorialGame::AddGooseToWorld(const Vector3& position)
{
	float size			= 1.0f;
	float inverseMass	= 1.0f;

	GameObject* goose = new GameObject();


	SphereVolume* volume = new SphereVolume(size);
	goose->SetBoundingVolume((CollisionVolume*)volume);

	goose->GetTransform().SetWorldScale(Vector3(size,size,size) );
	goose->GetTransform().SetWorldPosition(position);

	goose->SetRenderObject(new RenderObject(&goose->GetTransform(), gooseMesh, nullptr, basicShader));
	goose->SetPhysicsObject(new PhysicsObject(&goose->GetTransform(), goose->GetBoundingVolume()));

	goose->GetPhysicsObject()->SetInverseMass(inverseMass);
	goose->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(goose);

	return goose;
}

GameObject* TutorialGame::AddParkKeeperToWorld(const Vector3& position)
{
	float meshSize = 4.0f;
	float inverseMass = 0.5f;

	GameObject* keeper = new GameObject();

	AABBVolume* volume = new AABBVolume(Vector3(0.3, 0.9f, 0.3) * meshSize);
	keeper->SetBoundingVolume((CollisionVolume*)volume);

	keeper->GetTransform().SetWorldScale(Vector3(meshSize, meshSize, meshSize));
	keeper->GetTransform().SetWorldPosition(position);

	keeper->SetRenderObject(new RenderObject(&keeper->GetTransform(), keeperMesh, nullptr, basicShader));
	keeper->SetPhysicsObject(new PhysicsObject(&keeper->GetTransform(), keeper->GetBoundingVolume()));

	keeper->GetPhysicsObject()->SetInverseMass(inverseMass);
	keeper->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(keeper);

	return keeper;
}

GameObject* TutorialGame::AddCharacterToWorld(const Vector3& position) {
	float meshSize = 4.0f;
	float inverseMass = 0.5f;

	auto pos = keeperMesh->GetPositionData();

	Vector3 minVal = pos[0];
	Vector3 maxVal = pos[0];

	for (auto& i : pos) {
		maxVal.y = max(maxVal.y, i.y);
		minVal.y = min(minVal.y, i.y);
	}

	GameObject* character = new GameObject();

	float r = rand() / (float)RAND_MAX;


	AABBVolume* volume = new AABBVolume(Vector3(0.3, 0.9f, 0.3) * meshSize);
	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform().SetWorldScale(Vector3(meshSize, meshSize, meshSize));
	character->GetTransform().SetWorldPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), r > 0.5f ? charA : charB, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(character);

	return character;
}

GameObject* TutorialGame::AddAppleToWorld(const Vector3& position) {
	GameObject* apple = new GameObject();

	SphereVolume* volume = new SphereVolume(0.7f);
	apple->SetBoundingVolume((CollisionVolume*)volume);
	apple->GetTransform().SetWorldScale(Vector3(4, 4, 4));
	apple->GetTransform().SetWorldPosition(position);

	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), appleMesh, nullptr, basicShader));
	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

	apple->GetPhysicsObject()->SetInverseMass(1.0f);
	apple->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(apple);

	return apple;
}

GameObject* TutorialGame::AddParticleToWorld(const Vector3& position, OGLTexture* texture, const float alpha)
{
	GameObject* particle = new GameObject();

	SphereVolume* volume = new SphereVolume(0.7f);
	particle->SetBoundingVolume((CollisionVolume*)volume);

	particle->GetTransform().SetWorldScale(Vector3(10, 10, 10));
	particle->GetTransform().SetWorldPosition(position);

	RenderObject* rObj = new RenderObject(&particle->GetTransform(), particleMesh, texture, particleShader);
	rObj->SetColour(Vector4(0.5, 1, 1, 0.5));

	particle->SetRenderObject(rObj);
	particle->SetPhysicsObject(new PhysicsObject(&particle->GetTransform(), particle->GetBoundingVolume()));

	particle->GetPhysicsObject()->SetInverseMass(1.0f);
	particle->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(particle);

	return particle;
}

void TutorialGame::InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius) {
	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddSphereToWorld(position, radius, 1.0f);
		}
	}
	AddFloorToWorld(Vector3(0, -2, 0));
}

void TutorialGame::InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing) {
	float sphereRadius = 1.0f;
	Vector3 cubeDims = Vector3(1, 1, 1);

	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);

			if (rand() % 2) {
				AddCubeToWorld(position, cubeDims);
			}
			else {
				AddSphereToWorld(position, sphereRadius);
			}
		}
	}
	AddFloorToWorld(Vector3(0, -2, 0));
}

void TutorialGame::InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims) {
	for (int x = 1; x < numCols+1; ++x) {
		for (int z = 1; z < numRows+1; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddCubeToWorld(position, cubeDims, 1.0f);
		}
	}
	AddFloorToWorld(Vector3(0, -2, 0));
}

void TutorialGame::BridgeConstraintTest() {
	Vector3 cubeSize = Vector3(8, 8, 8);

	float	invCubeMass = 5;
	int		numLinks	= 25;
	float	maxDistance	= 30;
	float	cubeDistance = 20;

	Vector3 startPos = Vector3(500, 1000, 500);

	GameObject* start = AddCubeToWorld(startPos + Vector3(0, 0, 0), cubeSize, 0);

	GameObject* end = AddCubeToWorld(startPos + Vector3((numLinks + 2) * cubeDistance, 0, 0), cubeSize, 0);

	GameObject* previous = start;

	for (int i = 0; i < numLinks; ++i) {
		GameObject* block = AddCubeToWorld(startPos + Vector3((i + 1) * cubeDistance, 0, 0), cubeSize, invCubeMass);
		PositionConstraint* constraint = new PositionConstraint(previous, block, maxDistance);
		world->AddConstraint(constraint);
		previous = block;
	}

	PositionConstraint* constraint = new PositionConstraint(previous, end, maxDistance);
	world->AddConstraint(constraint);
}

void TutorialGame::SimpleGJKTest() {
	Vector3 dimensions		= Vector3(5, 5, 5);
	Vector3 floorDimensions = Vector3(100, 2, 100);

	GameObject* fallingCube = AddCubeToWorld(Vector3(0, 20, 0), dimensions, 10.0f);
	GameObject* newFloor	= AddCubeToWorld(Vector3(0, 0, 0), floorDimensions, 0.0f);

	delete fallingCube->GetBoundingVolume();
	delete newFloor->GetBoundingVolume();

	fallingCube->SetBoundingVolume((CollisionVolume*)new OBBVolume(dimensions));
	newFloor->SetBoundingVolume((CollisionVolume*)new OBBVolume(floorDimensions));
}

