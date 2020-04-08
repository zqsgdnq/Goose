#pragma once
#include "Transform.h"
#include "CollisionVolume.h"

#include "PhysicsObject.h"
#include "RenderObject.h"
#include "NetworkObject.h"

#include <vector>

using std::vector;

namespace NCL {
	namespace CSC8503 {
		class NetworkObject;

		class GameObject	{
		public:
			GameObject(string name = "");
			~GameObject();
			void SetInitColour(Vector4& initcolour) {
				this->initcolor = initcolour;

			}
			Vector4 Getinitcolour() {

				return this->initcolor;
			}
			void SetBoundingVolume(CollisionVolume* vol) {
				boundingVolume = vol;
			}

			const CollisionVolume* GetBoundingVolume() const {
				return boundingVolume;
			}

			bool IsActive() const {
				return isActive;
			}

			const Transform& GetConstTransform() const {
				return transform;
			}

			Transform& GetTransform() {
				return transform;
			}

			RenderObject* GetRenderObject() const {
				return renderObject;
			}

			PhysicsObject* GetPhysicsObject() const {
				return physicsObject;
			}

			NetworkObject* GetNetworkObject() const {
				return networkObject;
			}

			void SetRenderObject(RenderObject* newObject) {
				renderObject = newObject;
			}

			void SetPhysicsObject(PhysicsObject* newObject) {
				physicsObject = newObject;
			}

			const string& GetName() const {
				return name;
			}

			virtual void OnCollisionBegin(GameObject* otherObject) {
				

				//std::cout << "OnCollisionBegin event occured!\n";
			}

			virtual void OnCollisionEnd(GameObject* otherObject) {
				//std::cout << "OnCollisionEnd event occured!\n";
			}

		/*	bool Oncollisionsphere(GameObject* otherObject) {
			
				const CollisionVolume* currentob= this->GetBoundingVolume();
			const CollisionVolume* otherob = otherObject->GetBoundingVolume();
			if (!currentob || !otherob) {
				return false;
			}
			SphereVolume* volumeA = (SphereVolume*) currentob;
			SphereVolume* volumeB = (SphereVolume*) otherob;

			//const Transform& transformA = currentob->GetConstTransform();
			//const Transform& transformB = otherob->GetConstTransform();

			float radii = currentob->GetRadius() + volumeB->GetRadius();
			Vector3 positiona= this->GetConstTransform().GetWorldPosition();
			Vector3 positionb = otherObject->GetConstTransform().GetWorldPosition();
			Vector3 delta = positiona-positionb;

			float deltaLength = delta.Length();

			if (deltaLength < radii) {
			
			return true;// we ’re colliding !
			}
			else { return false; }

			}*/

			bool GetBroadphaseAABB(Vector3&outsize) const;

			void UpdateBroadphaseAABB();
			Vector4 initcolor=Vector4(1,1,1,1);
		protected:
			Transform			transform;

			CollisionVolume*	boundingVolume;
			PhysicsObject*		physicsObject;
			RenderObject*		renderObject;
			NetworkObject*		networkObject;

			bool	isActive;
			string	name;

			Vector3 broadphaseAABB;
		};
	}
}

