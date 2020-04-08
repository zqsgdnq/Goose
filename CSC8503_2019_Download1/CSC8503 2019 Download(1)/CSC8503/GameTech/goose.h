#pragma once
#include "..//CSC8503Common/GameObject.h"

namespace NCL {
	namespace CSC8503 
	{
		class Goose :public GameObject 
		{
   
		public:
			Goose(string name = "");
			//~Goose();
			//void swim() {};
			
			 void OnCollisionBegin(GameObject* otherObject) override{
				 if (
					 (this->GetName()=="goose") && (otherObject->GetName() == "apple")
					 ) 
				 {collisionapple = true;}
				 
					 if((this->GetName() == "goose") && (otherObject->GetName() == "island"))
					 {
						 collisionisland = true;
						
					 }

					 if ((this->GetName() == "goose") && (otherObject->GetName() == "water"))
					 {
						 collisionwater = true;
					 }
					 if ((this->GetName() == "goose") && (otherObject->GetName() == "keeper"))
					 {
						 collisionkeeper = true;
					 }
				 

				 

				
			}
			 bool Getcollisionapple() 
			 {

				 return this->collisionapple;
			 }

			 bool collisionapple=false;
			 bool collisionwater=false;
			 bool collisionisland=false;
			 bool collisionkeeper=false;
			 void collisionclear();
	};
	}
}




