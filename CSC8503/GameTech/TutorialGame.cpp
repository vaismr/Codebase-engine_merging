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
	audioEngine.Set3DListenerAndOrientation(Vec3{ 0,10,0 });
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

	loadFunc("cube.msh"		 , &cubeMesh);
	loadFunc("sphere.msh"	 , &sphereMesh);
	loadFunc("goose.msh"	 , &gooseMesh);
	loadFunc("CharacterA.msh", &keeperMesh);
	loadFunc("CharacterM.msh", &charA);
	loadFunc("CharacterF.msh", &charB);
	loadFunc("Apple.msh"	 , &appleMesh);

	basicTex	= (OGLTexture*)TextureLoader::LoadAPITexture("checkerboard.png");
	basicShader = new OGLShader("GameTechVert.glsl", "GameTechFrag.glsl");

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontDefault();
	std::string pathMainDlgFont = Assets::FONTSSDIR + "/FiraSans-Regular.otf";
	fontMainDlg = io.Fonts->AddFontFromFileTTF(pathMainDlgFont.c_str(), 16);

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

	audioEngine.Shutdown();
}

void TutorialGame::UpdateGame(float dt) {
lastCamPos = world->GetMainCamera()->GetPosition(); //get this before camera is updated below

	if (!isPaused) {
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
        //Updateballco();
		physics->Update(dt);
		renderHUD(dt);
		UpdateListener(dt);
		audioEngine.Update();
		UpdateArrow();
		Debug::FlushRenderables();
		renderer->Render();
	}
	else {
		UpdatePauseMenu();
		Debug::Print("Game Paused", Vector2(50, 100));
		Debug::FlushRenderables();
		renderer->Render();
	}
}
//void TutorialGame::Updateballco() {
//	isOnWater = ball->onWater;
//}
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
	Vec3 cameraUp = Vec3{ (cameraForward.y * 0) - (cameraForward.z * 1), (cameraForward.z * 0) - (cameraForward.x * 0), (cameraForward.x * 1) - (cameraForward.y * 0) };
	cameraUp = Vec3{ (cameraUp.y * cameraForward.z) - (cameraUp.z * cameraForward.y), (cameraUp.z * cameraForward.x) - (cameraUp.x * cameraForward.z), (cameraUp.x * cameraForward.y) - (cameraUp.y * cameraForward.x) };

	audioEngine.Set3DListenerAndOrientation(cameraPos, cameraVelocity, cameraForward, cameraUp);
}



//@TODO UI stuff - need on screen msg showing "Game Paused", quit game button, also mute audio
void TutorialGame::UpdatePauseMenu() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::P))
	{
		TogglePauseMenu();
		audioEngine.TogglePauseAllChannels();
	}		
}

void NCL::CSC8503::TutorialGame::renderHUD(float dt)
{

	ImGui_ImplOpenGL3_NewFrame();
#if _WIN32
	ImGui_ImplWin32_NewFrame();
#elif __ORBIS__
	// TODO: PS4 New Frame?? GLFW + gl3w?
	ImGui_Impl????_NewFrame();
#endif
	ImGui::NewFrame();

	// Start the Dear ImGui frame
	static float f = 100.0f;
	f -= dt;

	float power = 0.5f;

	const ImU32 red = 0xFF0000FF;
	const ImU32 col = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
	const ImU32 bg = ImGui::GetColorU32(ImGuiCol_Button);

	ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_Always);

	ImGuiWindowFlags flags = 
		ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoScrollbar
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoNav
		| ImGuiWindowFlags_NoBackground
		| ImGuiWindowFlags_NoBringToFrontOnFocus;

	//Time window
	ImGui::Begin("Hello, world!", nullptr, flags); // Create a window called "Hello, world!" and append into it.
	ImGui::SetWindowFontScale(2.5f);
	
	ImGui::PushFont(fontMainDlg);
	ImGui::Text(" Time : %.2fs", f);               // Display some text (you can use a format strings too)
	ImGui::PopFont();


	ImGui::End();

	//power window
	ImGui::SetNextWindowPos(ImVec2(800, 450), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(1000, 1000), ImGuiCond_Always);

	ImGui::Begin("Hello, world!", nullptr, flags); // Create a window called "Hello, world!" and append into it.
	
	ImGui::PushFont(fontMainDlg);
	ImGui::Text("Power Bar");               // Display some text (you can use a format strings too)
	ImGui::PopFont();
	ImGui::ProgressBar("##progress_bar1", power, ImVec2(500, 25), red, col);

	ImGui::SetWindowFontScale(2.5f);
	ImGui::End();

	//Name window
	ImGui::SetNextWindowPos(ImVec2(50, 550), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(1000, 1000), ImGuiCond_Always);
	ImGui::Begin("Name", nullptr, flags); // Create a window called "Hello, world!" and append into it.
	ImGui::SetWindowFontScale(2.5f);

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

	ImGui::End();
	ImGui::Render();
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
		audioEngine.PlaySounds("../../Assets/Sounds/jaguar.wav", Vec3{0,10,0}, 10.0f);
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
	Vector3 floatingforce(0, 0, 40.0f);
	float force = 30.0f;
	//if (selectionObject->GetIsOnBridge() == true) {
	//	selectionObject->GetPhysicsObject()->ApplyLinearImpulse(floatingforce);
	//}

	Vector3 rightAxis = Vector3(camWorld.GetColumn(0)); //view is inverse of model!

	//forward is more tricky -  camera forward is 'into' the screen...
	//so we can take a guess, and use the cross of straight up, and
	//the right axis, to hopefully get a vector that's good enough!

	Vector3 fwdAxis = Vector3::Cross(Vector3(0, 1, 0), rightAxis);


	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W)) {
		Vector3 dir = Matrix4::Rotation(world->GetMainCamera()->GetYaw(), Vector3(0, 1, 0)) * Vector3(0, 0, -1);
		selectionObject->GetPhysicsObject()->AddForce(dir * force);

	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S)) {
		Vector3 dir = Matrix4::Rotation(world->GetMainCamera()->GetYaw(), Vector3(0, 1, 0)) * Vector3(0, 0, 1);
		selectionObject->GetPhysicsObject()->AddForce(dir * force);

	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A)) {
		Vector3 dir = Matrix4::Rotation(world->GetMainCamera()->GetYaw(), Vector3(0, 1, 0)) * Vector3(-1, 0, 0);
		selectionObject->GetPhysicsObject()->AddForce(dir * force);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D)) {
		Vector3 dir = Matrix4::Rotation(world->GetMainCamera()->GetYaw(), Vector3(0, 1, 0)) * Vector3(1, 0, 0);
		selectionObject->GetPhysicsObject()->AddForce(dir * force);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::SPACE)) {
		Vector3 dir = Matrix4::Rotation(world->GetMainCamera()->GetYaw(), Vector3(0, 1, 0)) * Vector3(0, 0, -1);
		selectionObject->GetPhysicsObject()->ApplyLinearImpulse(dir * force);
		ball->isSlowDown = false;
		ball->isSpeedUp = false;
		ball->isTelePorted = false;
	}
}


void TutorialGame::UpdateArrow() {
	Impulsedir= Matrix4::Rotation(world->GetMainCamera()->GetYaw(), Vector3(0, 1, 0)) * Vector3(0, 0, -1);
	Impulsesize+= Window::GetMouse()->GetWheelMovement();
	if(Impulsesize <= 0){
		totalImpulse = Vector3(0, 0, 0);
	}
	else {
		Arrowlength = Impulsedir * Impulsesize * 10;
		//Vector3 dir = Matrix4::Rotation(world->GetMainCamera()->GetYaw(), Vector3(0, 1, 0)) * Vector3(0, 0, -1);
		totalImpulse = Impulsedir * 20 + Vector3(0, 1, 0) * Impulsesize;
	}
	renderer->DrawLine(ball->GetTransform().GetWorldPosition(), (ball->GetTransform().GetWorldPosition()) + Arrowlength, Vector4(1, 0, 0, 1));

}

void  TutorialGame::LockedCameraMovement() {
	/*if ( != nullptr) {
		Vector3 objPos = lockedObject->GetTransform().GetWorldPosition();
		Vector3 camPos = objPos + lockedOffset;

		Matrix4 temp = Matrix4::BuildViewMatrix(camPos, objPos, Vector3(0, 1, 0));

		Matrix4 modelMat = temp.Inverse();

		Quaternion q(modelMat);
		Vector3 angles = q.ToEuler(); //nearly there now!

		world->GetMainCamera()->SetPosition(camPos);
		world->GetMainCamera()->SetPitch(angles.x);
		world->GetMainCamera()->SetYaw(angles.y);
	//}*/
	if (lockedObject != nullptr) {
		float deltaX = Window::GetMouse()->GetRelativePosition().x;
		
		selectionObject->GetTransform().SetLocalOrientation(Quaternion::AxisAngleToQuaterion(Vector3(0, 1, 0), world->GetMainCamera()->GetYaw() - deltaX + 180));
		

		Window::GetWindow()->ShowOSPointer(false);
		Window::GetWindow()->LockMouseToWindow(true);

		Vector3 gooseForward = selectionObject->GetTransform().GetLocalOrientation() * Vector3(0,0,1);

		Vector3 objPos = lockedObject->GetTransform().GetWorldPosition();
		Vector3 camPos = objPos + gooseForward * (-20) + Vector3(0, 5, 0);

		Matrix4 temp = Matrix4::BuildViewMatrix(camPos, objPos, Vector3(0, 1, 0));

		Matrix4 modelMat = temp.Inverse();

		Quaternion q(modelMat);
		Vector3 angles = q.ToEuler(); //nearly there now!

		//float temppitch;
		temppitch-= (Window::GetMouse()->GetRelativePosition().y);

		world->GetMainCamera()->SetPosition(camPos);
		//world->GetMainCamera()->SetPitch(angles.x);
		world->GetMainCamera()->SetPitch(temppitch);
		world->GetMainCamera()->SetYaw(angles.y);
		
		
		//temppitch = std::min(pitch, 90.0f);
		//temppitch = std::max(pitch, -90.0f);
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
			//lockedObject = ball;
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
	world->ClearAndErase();
	physics->Clear();

	Vector3 Portal1 = Vector3(30, 0.1f, 30);
	Vector3 Portal2 = Vector3(10, 0.1f, 10);

	//InitMixedGridWorld(10, 10, 3.5f, 3.5f);
	//AddGooseToWorld(Vector3(30, 2, 0));
	//AddAppleToWorld(Vector3(35, 2, 0));

	//AddParkKeeperToWorld(Vector3(40, 2, 0));
	//AddCharacterToWorld(Vector3(45, 2, 0));

	AddFloorToWorld(Vector3(0, -2, 0));
	AddWaterToWorld(Vector3(70, 0.1f, 70));
	AddIceToWorld(Vector3(40, 6.0f, 40), Vector3(5.0f, 6.0f, 1.0f));
	AddFirePowerUpToWorld(Vector3(50, 0.1f, 50));
	AddIcePowerUpToWorld(Vector3(55, 0.1f, 55));
	AddPortalToWorld(Portal1)->SetName("PORTAL1");
	AddPortalToWorld(Portal2)->SetName("PORTAL2");

	Ball* tempball = AddSphereToWorld(Vector3(80, 6, 80), 2, 1);
	ball = (Ball*)tempball;

	

}

//From here on it's functions to add in objects to the world!

/*

A single function to add a large immoveable cube to the bottom of our world

*/
GameObject* TutorialGame::AddFloorToWorld(const Vector3& position) {
	GameObject* floor = new GameObject("FLOOR");

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
GameObject* TutorialGame::AddBridgeToWorld(const Vector3& position) {
	GameObject* Bridge = new GameObject();
	Vector3 bridgeSize = Vector3(40, 10, 10);
	AABBVolume* volume = new AABBVolume(bridgeSize);
	Bridge->SetBoundingVolume((CollisionVolume*)volume);
	Bridge->GetTransform().SetWorldScale(bridgeSize);

	Vector3 velocity= Vector3(300, 0, 0);
	Bridge->GetPhysicsObject()->SetLinearVelocity(velocity);
	Bridge->



}*/
/*

Builds a game object that uses a sphere mesh for its graphics, and a bounding sphere for its
rigid body representation. This and the cube function will let you build a lot of 'simple' 
physics worlds. You'll probably need another function for the creation of OBB cubes too.

*/
Ball* TutorialGame::AddSphereToWorld(const Vector3& position, float radius, float inverseMass) {
	Ball* sphere = new Ball("BALL");

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);
	sphere->GetTransform().SetWorldScale(sphereSize);
	sphere->GetTransform().SetWorldPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, 0, basicShader));
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

GameObject* TutorialGame::AddWaterToWorld(const Vector3& position)
{

	GameObject* water = new GameObject("WATER");
	Vector3 dimensions = Vector3(10.0f, 0.1f, 10.0f);
	AABBVolume* volume = new AABBVolume(Vector3(dimensions));

	water->SetBoundingVolume((CollisionVolume*)volume);

	water->GetTransform().SetWorldPosition(position);
	water->GetTransform().SetWorldScale(dimensions);

	water->SetRenderObject(new RenderObject(&water->GetTransform(), cubeMesh, 0, basicShader));
	water->SetPhysicsObject(new PhysicsObject(&water->GetTransform(), water->GetBoundingVolume()));
	water->GetRenderObject()->SetColour(Vector4(0.2f, 0.2f, 1.0f, 1.0f));

	water->GetPhysicsObject()->SetInverseMass(0);
	water->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(water);

	return water;
}

GameObject* TutorialGame::AddFirePowerUpToWorld(const Vector3& position)
{

	GameObject* FirePowerUp = new GameObject("FIREPOWERUP");
	Vector3 dimensions = Vector3(1.0f, 0.5f, 1.0f);
	AABBVolume* volume = new AABBVolume(Vector3(dimensions));

	FirePowerUp->SetBoundingVolume((CollisionVolume*)volume);

	FirePowerUp->GetTransform().SetWorldPosition(position);
	FirePowerUp->GetTransform().SetWorldScale(dimensions);

	FirePowerUp->SetRenderObject(new RenderObject(&FirePowerUp->GetTransform(), cubeMesh, 0, basicShader));
	FirePowerUp->SetPhysicsObject(new PhysicsObject(&FirePowerUp->GetTransform(), FirePowerUp->GetBoundingVolume()));
	FirePowerUp->GetRenderObject()->SetColour(Vector4(0.5f, 0.1f, 0.1f, 1.0f));

	FirePowerUp->GetPhysicsObject()->SetInverseMass(0);
	FirePowerUp->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(FirePowerUp);

	return FirePowerUp;
}

GameObject* TutorialGame::AddIcePowerUpToWorld(const Vector3& position)
{

	GameObject* IcePowerUp = new GameObject("ICEPOWERUP");
	Vector3 dimensions = Vector3(1.0f, 0.5f, 1.0f);
	AABBVolume* volume = new AABBVolume(Vector3(dimensions));

	IcePowerUp->SetBoundingVolume((CollisionVolume*)volume);

	IcePowerUp->GetTransform().SetWorldPosition(position);
	IcePowerUp->GetTransform().SetWorldScale(dimensions);

	IcePowerUp->SetRenderObject(new RenderObject(&IcePowerUp->GetTransform(), cubeMesh, 0, basicShader));
	IcePowerUp->SetPhysicsObject(new PhysicsObject(&IcePowerUp->GetTransform(), IcePowerUp->GetBoundingVolume()));
	IcePowerUp->GetRenderObject()->SetColour(Vector4(0.0f, 0.8f, 0.8f, 1.0f));

	IcePowerUp->GetPhysicsObject()->SetInverseMass(0);
	IcePowerUp->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(IcePowerUp);

	return IcePowerUp;
}

GameObject* TutorialGame::AddIceToWorld(const Vector3& position, Vector3 dimensions, float inverseMass) {
	GameObject* ice = new GameObject("ICE");

	AABBVolume* volume = new AABBVolume(dimensions);

	ice->SetBoundingVolume((CollisionVolume*)volume);

	ice->GetTransform().SetWorldPosition(position);
	ice->GetTransform().SetWorldScale(dimensions);

	ice->SetRenderObject(new RenderObject(&ice->GetTransform(), cubeMesh, 0, basicShader));
	ice->SetPhysicsObject(new PhysicsObject(&ice->GetTransform(), ice->GetBoundingVolume()));
	ice->GetRenderObject()->SetColour(Vector4(0.8f, 1.0f, 1.0f, 1.0f));

	ice->GetPhysicsObject()->SetInverseMass(0);
	ice->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(ice);

	return ice;
}

GameObject* TutorialGame::AddPortalToWorld(const Vector3& position)
{

	GameObject* portal = new GameObject();
	Vector3 dimensions = Vector3(1.0f, 0.1f, 1.0f);
	AABBVolume* volume = new AABBVolume(Vector3(dimensions));

	portal->SetBoundingVolume((CollisionVolume*)volume);

	portal->GetTransform().SetWorldPosition(position);
	portal->GetTransform().SetWorldScale(dimensions);

	portal->SetRenderObject(new RenderObject(&portal->GetTransform(), cubeMesh, 0, basicShader));
	portal->SetPhysicsObject(new PhysicsObject(&portal->GetTransform(), portal->GetBoundingVolume()));
	portal->GetRenderObject()->SetColour(Vector4(0.0f, 0.0f, 0.0f, 1.0f));

	portal->GetPhysicsObject()->SetInverseMass(0);
	portal->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(portal);

	return portal;
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

