#include "TutorialGame.h"
#include "../CSC8503Common/GameWorld.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Common/TextureLoader.h"

#include "../CSC8503Common/PositionConstraint.h"
#include <fstream>
#include "../../Common/Assets.h"

#include <functional>


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
}

/*

Each of the little demo scenarios used in the game uses the same 2 meshes, 
and the same texture and shader. There's no need to ever load in anything else
for this module, even in the coursework, but you can add it if you like!

*/
void TutorialGame::ShowScore() {
	string s = std::to_string(score);
	Debug::Print("score is " + s, Vector2(10, 100));


}
void TutorialGame::showtime() {
	string s = std::to_string(caltime);
	Debug::Print("time is " + s, Vector2(10, 80));


}

void TutorialGame::keeperInitpo(void)
{
	keeper->GetTransform().SetWorldPosition(Vector3(30, 2, 0));

}

void TutorialGame::gamemachine()

{
	gamem = new StateMachine();
	StateFunc keeperchase = [](void* data)
	{
		TutorialGame* game = static_cast<TutorialGame*>(data);
		game->chasepath();

	};
	StateFunc dontchase = [](void* data)
	{
		TutorialGame* game = static_cast<TutorialGame*>(data);
		game->walkaround();

	};
	StateFunc looseapples = [](void* data) {
		TutorialGame* game = static_cast<TutorialGame*>(data);
		game->appletoinit();
		
	};
	
	StateFunc swim = [](void* data) {
		TutorialGame* game = static_cast<TutorialGame*>(data);
		game->walkonwater();
	};
	StateFunc getisland = [](void* data) {
		TutorialGame* game = static_cast<TutorialGame*>(data);
		game->throwapple();
	};
	
	chasestate = new GenericState(keeperchase, this);
	 walkstate = new GenericState(dontchase, this);
	 looseapplestate= new GenericState(looseapples, this);
	 waterstate = new GenericState(swim,this);
	 islandstate = new GenericState(getisland, this);

	gamem->AddState(walkstate);
	gamem->AddState(chasestate);
	gamem->AddState(looseapplestate);
	gamem->AddState(waterstate);
	gamem->AddState(islandstate);

stopchase = new MyTransition(chasestate,walkstate);
	startchase = new MyTransition(walkstate,chasestate );
   havecaught=new MyTransition(chasestate, looseapplestate);
   restartcollect = new MyTransition(looseapplestate,walkstate);
   colwater1=new MyTransition(walkstate, waterstate);
   colwater2= new MyTransition(chasestate, waterstate);
   backtoground=new MyTransition(waterstate, walkstate);
   AppletoIsland = new MyTransition(waterstate, islandstate);
   leavehome = new MyTransition(islandstate, waterstate);
	 gamem->AddTransition(stopchase);
	gamem->AddTransition(startchase);
	gamem->AddTransition(havecaught);
	gamem->AddTransition(restartcollect);
	gamem->AddTransition(colwater1);
	gamem->AddTransition(colwater2);
	gamem->AddTransition(backtoground);
	gamem->AddTransition(AppletoIsland);
	gamem->AddTransition(leavehome);

}

/*void TutorialGame::gamemachine()
{
	int applenum = 1;
	//float distance = (goose->GetTransform.GetWorldPosition()-keeper->GetTransform.GetWorldPosition()).length();
	Vector3 distancepos = goose->GetTransform().GetWorldPosition()-keeper->GetTransform().GetWorldPosition();
	float distance = distancepos.Length();
	StateMachine* gamemachine = new StateMachine();

	//StateFunc keeperinit = keeperInitpo;
	
	StateFunc keeperinit = [](void* data) {
		TutorialGame* game = static_cast<TutorialGame*>(data);
		game->keeper->GetTransform().SetWorldPosition(Vector3(30, 2, 0));

		game->InitCamera();
	};

	StateFunc keeperchase = [](void* data) {
		TutorialGame* game = static_cast<TutorialGame*>(data);

		game->keeper->GetTransform().SetWorldPosition(Vector3(30, 2, 0));
	};

	GenericState* keeperinitState = new GenericState(keeperinit, this);

	gamemachine->AddState(keeperinitState);



}*/
Vector3 TutorialGame::ytozero(Vector3& pos) {

	Vector3 yzeropos = Vector3(pos.x, 0, pos.z);
	return yzeropos;

}

bool TutorialGame::closedistance()
{
	Vector3 kposition = keeper->GetTransform().GetWorldPosition();
	Vector3 gposition = goose->GetTransform().GetWorldPosition();
	float distance = (kposition - gposition).Length();
	if (distance < 40) {
		
		return true;
	}
	else
	{ return false; }




}

bool TutorialGame::fardistance()
{
	Vector3 kposition = keeper->GetTransform().GetWorldPosition();
	Vector3 gposition = goose->GetTransform().GetWorldPosition();
	float distance = (kposition - gposition).Length();
	if (distance > 40) {
		
		return true;
	}
	else
	{
		return false;
	}
}
bool TutorialGame::iscarrying() {

	if (carrylist.size() > 0) {
		return true;
	}
	else { return false; }
}
void TutorialGame::walkaround()
{
	Vector3 kposition = keeper->GetTransform().GetWorldPosition();
	Vector3 gposition = goose->GetTransform().GetWorldPosition();
	float distance = (kposition - gposition).Length();
	//std::cout << "Do nothing!Walk around!" << std::endl;
	
}
void TutorialGame::appletoinit() {
	//constraint = nullptr;
	world->RemoveConstraint(constraint);
	world->RemoveAllConstraints();
	delete constraint;
	InitConstraint();
	for (int i = 0; i < carrylist.size(); i++) 
	{
		carrylist[i]->GobackInit();

	}
	
	carrylist.clear();
}

void TutorialGame::chasepath() {

	NavigationGrid grid("TestGrid1.txt");
	NavigationPath outPath;

	Vector3 getkeeper = keeper->GetTransform().GetWorldPosition();
	//Vector3 setyzero =Vector3 (getkeeper.x, 0, getkeeper.z);
	//Vector3 startPos(setyzero);
	Vector3 startPos(getkeeper);
	Vector3 endPos(goose->GetTransform().GetWorldPosition());
	
	bool found = grid.FindPath(startPos, endPos, outPath,upinter);
	chaseNodes.clear();
	Vector3 pos;
	while (outPath.PopWaypoint(pos)) {
		chaseNodes.push_back(pos);

	}
	/*Vector3 testa = chaseNodes[0];
	Vector3 testb = chaseNodes[1];
	if ((testa.x - testb.x) < 0 || (testa.z - testb.z) < 0) 
	{

		bool found = grid.FindPath(startPos, endPos, outPath);
		chaseNodes.clear();
		Vector3 pos;
		while (outPath.PopWaypoint(pos)) {
			chaseNodes.push_back(pos);

		}


	}*/
	
	static int i = 1;
		if (i < chaseNodes.size()) {
			Vector3 a = chaseNodes[i - 1];
			Vector3 b = chaseNodes[i];
			float distanceab = (a - b).Length();

			//Vector3 worldposition = keeper->GetTransform().GetWorldPosition();
			//Vector3 ytozero = Vector3(worldposition.x, 0, worldposition.z);
			//keeper->GetTransform().SetWorldPosition(Vector3(worldposition.x, 0, worldposition.z));

			Vector3 nowposition = keeper->GetTransform().GetWorldPosition();
		
			Vector3 keepertrans = (b - a) /200;
			Vector3 testab = b- a;
			Vector3 nextposition = nowposition + keepertrans;
			Vector3 xzpos = ytozero(nextposition);
			float nextac = (xzpos - a).Length();

			if (nextac >= distanceab)
			{
				keeper->GetTransform().SetWorldPosition(Vector3(b.x,nextposition.y,b.z));
				Vector3 newposition = keeper->GetTransform().GetWorldPosition();
				
				upinter = false;
				//i++;
			}
			else {

				keeper->GetTransform().SetWorldPosition(nextposition);
				Vector3 newposition = keeper->GetTransform().GetWorldPosition();
				
				if (testab.x < 0 || testab.z < 0) {
					upinter = true;
				}

			}

		}
}
	void TutorialGame::chasefunc()
	{
	
	}
	/*void TutorialGame::walkonwater(float ) {


}*/
	void TutorialGame::walkonwater() {
		changingwave = changingwave + 0.1;
		float changedelta = changingwave - floor(changingwave);
		float changeangle = (-30.0f) * changedelta + 30.0f * (1-changedelta);
		goose->GetRenderObject()->SetColour(Vector4(changedelta, changedelta, 1-changedelta,1));
			goose->GetTransform().SetLocalOrientation(Quaternion::AxisAngleToQuaterion(Vector3(1, 0, 0), changeangle));
		
	}

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

	InitCamera();
	InitWorld();
	gamemachine();
	
}
void TutorialGame::throwapple() {

	for (int i = 0; i < carrylist.size(); i++) {

		homelist.push_back(carrylist[i]);

	}
carrylist.clear();
	world->RemoveConstraint(constraint);
	world->RemoveAllConstraints();
	delete constraint;
	InitConstraint();
	
	for (int i = 0; i < homelist.size(); i++) {

		homelist[i]->GetTransform().SetWorldPosition(Vector3(100, 6, 100));
		score = homelist.size() * 50;
	}
	
}
bool TutorialGame::isgetting() {
	
	if (carrylist.size() > 0) 
	{
		if ((goose->collisionisland )== true) 
		{
			if (Window::GetKeyboard()->KeyDown(KeyboardKeys::E)) 
			{

            return true;
			}
			else { return false; }
      

		}
		else { return false; }
			
	}
	else { return false; }

}

TutorialGame::~TutorialGame()	{
	delete cubeMesh;
	delete sphereMesh;
	delete gooseMesh;
	delete basicTex;
	delete basicShader;
	delete goose;
	delete keeper;
	delete physics;
	delete renderer;
	delete world;
	delete gamem;
	delete island;
}

void TutorialGame::UpdateGame(float dt) {
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::P)) { pause = true; }
	if(pause==false){
	caltime += dt;
	if (!inSelectionMode) {
		world->GetMainCamera()->UpdateCamera(dt);
	}
	if (lockedObject != nullptr) {
		LockedCameraMovement();
	}

	UpdateKeys();

	if (useGravity) {
		Debug::Print("(G)ravity on", Vector2(10, 40));
	}
	else {
		Debug::Print("(G)ravity off", Vector2(10, 40));
	}

	SelectObject();
	MoveSelectedObject();

	world->UpdateWorld(dt);
	renderer->Update(dt);
	physics->Update(dt);
	UpdateTransitions();
	gamem->Update();
	//chasepath();
	//chasefunc();
	//constraint->UpdateConstraint(dt);
	goose->collisionclear();
	
	ConstraintAddApple();
	constraint->UpdateConstraint(dt);
	for (int i = 0; i < applelist.size(); i++) {
		applelist[i]->claercol();
	}
	Debug::FlushRenderables();

	renderer->Render();
ShowScore();
showtime();

if (caltime >180||(homelist.size()==applelist.size())) { 
	
	Debug::Print("Game Restart !" , Vector2(200, 200));


	if(caltime >185){throw 20;}
	 }
if (Window::GetKeyboard()->KeyDown(KeyboardKeys::R)) {
	throw 20;
}
	}else
	{
		renderer->Update(dt);
		renderer->Render();
		renderer->DrawString("Press C to continue.", Vector2(300, 450), Vector4(1, 0, 0, 1));
		
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::C)) {
			pause = false;
		}
	
	}
}
void TutorialGame::UpdateTransitions() {
	stopchase->transitionif = this->fardistance();
	startchase->transitionif = ((this->closedistance()) && (this->iscarrying()));
	havecaught->transitionif = goose->collisionkeeper;
	restartcollect->transitionif = true;
	colwater1->transitionif = goose->collisionwater;
	colwater2->transitionif = goose->collisionwater;
	backtoground->transitionif = !(goose->collisionwater || goose->collisionisland);
	AppletoIsland->transitionif = this->isgetting();
	leavehome->transitionif = goose->collisionwater;
	//std::cout << this->iscarrying() << std::endl;
	//std::cout << goose->collisionisland << std::endl;
	//std::cout << AppletoIsland->transitionif << std::endl;
}

void TutorialGame::UpdateKeys() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1)) {
		InitWorld(); //We can reset the simulation at any time with F1
		selectionObject = nullptr;
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F2)) {
		InitCamera(); //F2 will reset the camera to a specific default place
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
		//Quaternion objangle = lockedObject->GetTransform().GetLocalOrientation();
		//Vector3 objangles = objangle.ToEuler();
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
	/*	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(-10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}
		*/
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM7)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, 10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM8)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, -10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(-50, 0, 0));
			selectionObject->GetTransform().SetLocalOrientation(Quaternion::AxisAngleToQuaterion(Vector3(0, 1, 0), -90.0f));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(50, 0, 0));
			selectionObject->GetTransform().SetLocalOrientation(Quaternion::AxisAngleToQuaterion(Vector3(0, 1, 0), 90.0f));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, -50));
			selectionObject->GetTransform().SetLocalOrientation(Quaternion::AxisAngleToQuaterion(Vector3(0, 1, 0), 180.0f));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, 50));
			selectionObject->GetTransform().SetLocalOrientation(Quaternion::AxisAngleToQuaterion(Vector3(0, 1, 0), 0.0f));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM5)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, -50, 0));
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

				selectionObject->GetRenderObject()->SetColour(selectionObject->Getinitcolour());
				selectionObject = nullptr;
			}

			Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

			RayCollision closestCollision;
			if (world->Raycast(ray, closestCollision, true)) {
				selectionObject = (GameObject*)closestCollision.node;
				if(selectionObject==goose)
				{ selectionObject->GetRenderObject()->SetColour(Vector4(1, 1, 0, 1)); }
				else
				{selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));}
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
		renderer->DrawString("Press Q to change to select mode!", Vector2(10, 0));
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
	renderer -> DrawString(" Click Force :" + std::to_string(forceMagnitude),
		Vector2(10, 20)); // Draw debug text at 10 ,20
	forceMagnitude += Window::GetMouse()->GetWheelMovement() * 100.0f;

	if (!selectionObject) {
		return;// we haven ’t selected anything !
	}
	// Push the selected object !
	if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::RIGHT)) {
		Ray ray = CollisionDetection::BuildRayFromMouse(
			*world->GetMainCamera());

		RayCollision closestCollision;
		if (world->Raycast(ray, closestCollision, true)) {
			if (closestCollision.node == selectionObject) {
				selectionObject->GetPhysicsObject() ->AddForceAtPosition(ray.GetDirection() * forceMagnitude,closestCollision.collidedAt);
			}
		}
	}
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
	caltime = 0.0;
	pause = false;
	world->ClearAndErase();
	physics->Clear();
	score = 0;
	Vector3 cubeDims = Vector3(5, 5, 5);
	//InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims)
	//InitCubeGridWorld(10,10,2.0f,2.0f,cubeDims);
	InitCubeFile("TestGrid1.txt", cubeDims);
	//InitMixedGridWorld(10, 10, 3.5f, 3.5f);
	GameObject* tempgoose=AddGooseToWorld(Vector3(100, 6,100));
	goose = (Goose*)tempgoose;
	
	InitApple();

	keeper=AddParkKeeperToWorld(Vector3(80, 2, 10));
	Vector3 keeperposition = keeper->GetTransform().GetWorldPosition();
	//std::cout << "nowposition" << keeperposition << std::endl;
	AddCharacterToWorld(Vector3(45, 2, 0));
	InitWater();
	InitIsland();
	InitConstraint();
	//BridgeConstraintTest();
	//AddFloorToWorld(Vector3(0, -2, 0));
}

//From here on it's functions to add in objects to the world!

/*

A single function to add a large immoveable cube to the bottom of our world

*/
GameObject* TutorialGame::AddFloorToWorld(const Vector3& position) {
	GameObject* floor = new GameObject();

	Vector3 floorSize = Vector3(300, 2, 300);
	AABBVolume* volume = new AABBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform().SetWorldScale(floorSize);
	floor->GetTransform().SetWorldPosition(position);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh,nullptr, basicShader));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();
	floor->GetRenderObject()->SetColour(Vector4(0,0.8,0.2,1));
	floor->SetInitColour(Vector4(0, 0.8, 0.2, 1));
	world->AddGameObject(floor);

	return floor;
}

GameObject* TutorialGame::AddWaterToWorld(const Vector3& position) {
	GameObject* floor = new GameObject("water");

	Vector3 floorSize = Vector3(20, 0.4, 20);
	AABBVolume* volume = new AABBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform().SetWorldScale(floorSize);
	floor->GetTransform().SetWorldPosition(position);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, nullptr, basicShader));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(floor);

	return floor;
}
GameObject* TutorialGame::AddIslandToWorld(const Vector3& position) {
	GameObject* floor = new GameObject("island");

	Vector3 floorSize = Vector3(10, 0.5, 10);
	AABBVolume* volume = new AABBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform().SetWorldScale(floorSize);
	floor->GetTransform().SetWorldPosition(position);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, nullptr, basicShader));
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

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, nullptr, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(0);
	cube->GetPhysicsObject()->InitCubeInertia();
	cube->GetRenderObject()->SetColour(Vector4(0.7,0.2,0.3,1));
	cube->SetInitColour(Vector4(0.7, 0.2, 0.3, 1));
	world->AddGameObject(cube);

	return cube;
}


GameObject* TutorialGame::AddGooseToWorld(const Vector3& position)
{
	float size			= 2.0f;
	float inverseMass	= 2.0f;

	Goose* gamegoose = new Goose("goose");
	

	SphereVolume* volume = new SphereVolume(size);
	gamegoose->SetBoundingVolume((CollisionVolume*)volume);

	gamegoose->GetTransform().SetWorldScale(Vector3(size,size,size) );
	gamegoose->GetTransform().SetWorldPosition(position);

	gamegoose->SetRenderObject(new RenderObject(&gamegoose->GetTransform(), gooseMesh, nullptr, basicShader));
	gamegoose->SetPhysicsObject(new PhysicsObject(&gamegoose->GetTransform(), gamegoose->GetBoundingVolume()));

	gamegoose->GetPhysicsObject()->SetInverseMass(inverseMass);
	gamegoose->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(gamegoose);

	return gamegoose;
}

GameObject* TutorialGame::AddParkKeeperToWorld(const Vector3& position)
{

	float meshSize = 4.0f;
	float inverseMass = 5.0f;

	GameObject* keeper = new GameObject("keeper");

	AABBVolume* volume = new AABBVolume(Vector3(0.3, 0.9f, 0.3) * meshSize);
	keeper->SetBoundingVolume((CollisionVolume*)volume);

	keeper->GetTransform().SetWorldScale(Vector3(meshSize, meshSize, meshSize));
	keeper->GetTransform().SetWorldPosition(position);

	keeper->SetRenderObject(new RenderObject(&keeper->GetTransform(), keeperMesh, nullptr, basicShader));
	//keeper->GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));
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
	Apple* apple = new Apple("apple");

	SphereVolume* volume = new SphereVolume(0.7f);
	apple->SetBoundingVolume((CollisionVolume*)volume);
	apple->GetTransform().SetWorldScale(Vector3(5, 5, 5));
	apple->GetTransform().SetWorldPosition(position);

	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), appleMesh, nullptr, basicShader));
	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));
	apple->GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));
	apple->GetPhysicsObject()->SetInverseMass(1.0f);
	apple->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(apple);

	return apple;
}

void TutorialGame::InitApple() {
	//GameObject* appletemp1 = AddAppleToWorld(Vector3(10, 2, 10));
	Apple* apple1 = (Apple*)AddAppleToWorld(Vector3(10, 2, 10));
	apple1->SetAppleid(1);
	applelist.push_back(apple1);
	
	
	Apple* apple2 = (Apple*)AddAppleToWorld(Vector3(140, 2, 130));
	apple2->SetAppleid(2);
	applelist.push_back(apple2);

	Apple* apple3 = (Apple*)AddAppleToWorld(Vector3(140, 2, 140));
	apple3->SetAppleid(3);
	applelist.push_back(apple3);
	
	Apple* apple4 = (Apple*)AddAppleToWorld(Vector3(140, 2, 140));
	apple3->SetAppleid(4);
	applelist.push_back(apple4);
	
	Apple* apple5 = (Apple*)AddAppleToWorld(Vector3(100, 2, 190));
	apple3->SetAppleid(5);
	applelist.push_back(apple5);
	for (int i = 0; i < applelist.size(); i++) {
		applelist[i]->SetInitposition(applelist[i]->GetTransform().GetWorldPosition());

	}


}
void TutorialGame::InitIsland() {
	island= (Island*)AddIslandToWorld(Vector3(100, 3, 100));
	island->GetRenderObject()->SetColour(Vector4(0.7, 0.3, 0.2, 1));
	island->SetInitColour(Vector4(0.7, 0.3, 0.2, 1));

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
void TutorialGame::InitWater() 
{
	water = (Water*)AddWaterToWorld(Vector3(100, 1, 100));
	water->GetRenderObject()->SetColour(Vector4(0,0.3,0.8,1));
	water->SetInitColour(Vector4(0, 0.3, 0.8, 1));


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

void TutorialGame::InitCubeFile(const std::string& filename, const Vector3& cubeDims)
{
	std::ifstream infile(Assets::DATADIR + filename);
	int nodeSize, gridWidth, gridHeight;

	infile >> nodeSize;
	infile >> gridWidth;
	infile >> gridHeight;

	for (int y = 0; y < gridHeight; ++y) {
		for (int x = 0; x < gridWidth; ++x) {
			char type = 0;
			infile >> type;
			if (type == 'x') {
             Vector3 position= Vector3((float)(x * 10), 5.5f, (float)(y * 10));
			 AddCubeToWorld(position, cubeDims, 1.0f);

			}
				
		}
	}

	AddFloorToWorld(Vector3(0, -2, 0));

}
void TutorialGame::InitConstraint() {
	previous = goose;
	next = nullptr;
	start = goose;
	constraint = new PositionConstraint(start,next,3);

}
void TutorialGame::ConstraintAddApple() {
	Vector3 previouspos = previous->GetTransform().GetWorldPosition();
	
		for (int i = 0; i < applelist.size(); i++) 
		{bool alreadycarry=false;
			if (applelist[i]->collisiongoose == true) {
				//if (i == 2) { std::cout << "collision" << i << std::endl; }
				for(int a=0;a<carrylist.size();a++){
					if (
						(applelist[i]->Getappleid()) == (carrylist[a]->Getappleid())

						)
					{
						alreadycarry = true;
						//if (i == 2) { std::cout << "already carry:" << i << std::endl; }
						//std::cout << "already carry:"<<i << std::endl;
					}
				}
				for (int a = 0; a < homelist.size(); a++) {
					if (
						(applelist[i]->Getappleid()) == (homelist[a]->Getappleid())

						)
					{
						alreadycarry = true;
						//if (i == 2) { std::cout << "already carry:" << i << std::endl; }
						//std::cout << "already carry:"<<i << std::endl;
					}
				}

				if(alreadycarry==false){
					//std::cout << "have not carry" << i << std::endl;
				carrylist.push_back(applelist[i]);

				//std::cout << "already push:" << i << std::endl;
				if (previous==goose) {
					//previous is goose
					next = applelist[i];
					
					//std::cout <<"next :"<< i << std::endl;
					
					applelist[i]->GetTransform().SetWorldPosition(previouspos+Vector3(4,10,4));
					
					constraint = new PositionConstraint(previous, next, 3);
					//std::cout << "new success" << std::endl;
					world->AddConstraint(constraint);
					//std::cout << "add success"<< std::endl;
					previous = applelist[i];

				}
				else {//previous is apple
					//std::cout << "add apple" << std::endl;
					next = applelist[i];
					applelist[i]->GetTransform().SetWorldPosition(previouspos + Vector3(3, 0, 0));
					constraint = new PositionConstraint(previous, next, 3);
					world->AddConstraint(constraint);
					previous = applelist[i];

				}
				}
			
			}

		}

	



}
/*void TutorialGame::BridgeConstraintTest() {
	Vector3 cubeSize = Vector3(8, 8, 8);

	float	invCubeMass = 2;
	int		numLinks	= 3;
	float	maxDistance	= 6;
	float	cubeDistance = 3;

	//Vector3 startPos = Vector3(500, 100, 500);

	GameObject* start = goose;
	Vector3 startPos = goose->GetTransform().GetWorldPosition();
	GameObject* end = AddAppleToWorld(startPos + Vector3((numLinks + 2) * cubeDistance, 0, 0));

	GameObject* previous = start;

	for (int i = 0; i < numLinks; ++i) {
		GameObject* block = AddAppleToWorld(goose->GetTransform().GetWorldPosition() + Vector3((i + 1) * cubeDistance, 0, 0));
		constraint = new PositionConstraint(previous, block, maxDistance);
		world->AddConstraint(constraint);
		previous = block;
	}

	PositionConstraint* constraint = new PositionConstraint(previous, end, maxDistance);
	world->AddConstraint(constraint);
}*/

void TutorialGame::SimpleGJKTest() {
	Vector3 dimensions		= Vector3(5, 5, 5);
	Vector3 floorDimensions = Vector3(200, 2, 200);

	GameObject* fallingCube = AddCubeToWorld(Vector3(0, 20, 0), dimensions, 10.0f);
	GameObject* newFloor	= AddCubeToWorld(Vector3(0, 0, 0), floorDimensions, 0.0f);

	delete fallingCube->GetBoundingVolume();
	delete newFloor->GetBoundingVolume();

	fallingCube->SetBoundingVolume((CollisionVolume*)new OBBVolume(dimensions));
	newFloor->SetBoundingVolume((CollisionVolume*)new OBBVolume(floorDimensions));

}

