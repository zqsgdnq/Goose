#pragma once
#include "..//CSC8503Common/GameObject.h"

namespace NCL {
	namespace CSC8503
	{
		class Apple :public GameObject
		{

		public:
			Apple(string name = "");
			int appleid;
			void SetAppleid(int a) {
				this->appleid = a;

			}
			int Getappleid() {

				return this->appleid;

			}
			void OnCollisionBegin(GameObject* otherObject) override {
				if (
					(this->GetName() == "apple") && (otherObject->GetName() == "goose")
					)
				{

					currentcolid = this->appleid;
					collisiongoose = true;
					
				}


			}
			bool Getcollisiongoose()
			{

				return this->collisiongoose;
			}
			bool collisiongoose=false;
			void SetInitposition(Vector3& a );
			void GobackInit();
			void claercol();
			int currentcolid;
			Vector3 initposition;
		};
	}
}
