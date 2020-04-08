#pragma once
#include "GameTechRenderer.h"
#include "../CSC8503Common/PhysicsSystem.h"
#include "../CSC8503Common/StateMachine.h"
#include "../CSC8503Common/StateTransition.h"
#include "../CSC8503Common/State.h"
#include "../CSC8503Common/NavigationGrid.h"
#include"../CSC8503Common/PositionConstraint.h"
#include"goose.h"
#include"apple.h"
#include"water.h"
#include"island.h"

class MyTransition : public StateTransition
{
public:
	MyTransition(State* srcState, State* destState)
	{
		sourceState = srcState;		
		destinationState = destState;
	}

	virtual bool CanTransition() const override {
		return transitionif;
	}
protected:


};

namespace NCL {
	namespace CSC8503 {
		class TutorialGame		{
		public:
			TutorialGame();
			~TutorialGame();
			void keeperInitpo();
			virtual void UpdateGame(float dt);
			void ConstraintAddApple();
			void InitConstraint();
			StateMachine* gamem;
			GenericState* chasestate;
			GenericState* walkstate ;
			GenericState* looseapplestate;
			GenericState* waterstate;
			GenericState* islandstate;
			GenericState* continuestate;
			

			MyTransition* stopchase ;
			MyTransition* startchase;
			MyTransition* havecaught;
			MyTransition* restartcollect;
			MyTransition* colwater1;
			MyTransition* colwater2;
			MyTransition* backtoground;
			MyTransition* AppletoIsland;
			MyTransition* leavehome;
			

void gamemachine();
void chasepath();
void walkaround();
void appletoinit();
bool iscarrying();
void UpdateTransitions();
void walkonwater();
void throwapple();
void showtime();
bool isgetting();
int score;
float caltime;
float changingwave=0;
int changespeed;
bool pause;
		protected:
			void InitialiseAssets();

			void InitCamera();
			void UpdateKeys();

			void InitWorld();
			void ShowScore();
			/*
			These are some of the world/object creation functions I created when testing the functionality
			in the module. Feel free to mess around with them to see different objects being created in different
			test scenarios (constraints, collision types, and so on). 
			*/
			void InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius);
			void InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing);
			void InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims);
			void InitCubeFile(const std::string& filename,const Vector3& cubeDims);
			void InitWater();
			void InitApple();
			void InitIsland();

			std::vector<Apple*> applelist;
			std::vector<Apple*> carrylist;
			std::vector<Apple*> homelist;
			//void BridgeConstraintTest();
			void SimpleGJKTest();

			bool SelectObject();
			void MoveSelectedObject();
			void DebugObjectMovement();
			void LockedObjectMovement();
			void LockedCameraMovement();
			Vector3 ytozero(Vector3& pos);
			
			//void keeperInitpo();
			
			void chasefunc();
			
			
			bool closedistance();
			bool fardistance();
			vector<Vector3> chaseNodes;
            
			bool upinter = false;

			GameObject* AddFloorToWorld(const Vector3& position);
			GameObject* AddWaterToWorld(const Vector3& position);
			GameObject* AddSphereToWorld(const Vector3& position, float radius, float inverseMass = 10.0f);
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);
			//IT'S HAPPENING
			GameObject* AddGooseToWorld(const Vector3& position);
			GameObject* AddParkKeeperToWorld(const Vector3& position);
			GameObject* AddCharacterToWorld(const Vector3& position);
			GameObject* AddAppleToWorld(const Vector3& position);
			GameObject* AddIslandToWorld(const Vector3& position);
			Goose* goose;
			GameObject* keeper;
			Apple* apple;
			Water* water;
			Island* island;
			PositionConstraint* constraint;
			GameTechRenderer*	renderer;
			PhysicsSystem*		physics;
			GameWorld*			world;

			GameObject* start;
			GameObject* previous;
			GameObject* next;
			//bool gooseclose;
			//bool goosefar;
			
			
			bool useGravity;
			bool inSelectionMode;

			float		forceMagnitude;

			GameObject* selectionObject = nullptr;

			OGLMesh*	cubeMesh	= nullptr;
			OGLMesh*	sphereMesh	= nullptr;
			OGLTexture* basicTex	= nullptr;
			OGLShader*	basicShader = nullptr;

			//Coursework Meshes
			OGLMesh*	gooseMesh	= nullptr;
			OGLMesh*	keeperMesh	= nullptr;
			OGLMesh*	appleMesh	= nullptr;
			OGLMesh*	charA		= nullptr;
			OGLMesh*	charB		= nullptr;

			//Coursework Additional functionality	
			GameObject* lockedObject	= nullptr;
			Vector3 lockedOffset		= Vector3(0, 14, 20);
			void LockCameraToObject(GameObject* o) {
				lockedObject = o;
			}
		};
	}
}

