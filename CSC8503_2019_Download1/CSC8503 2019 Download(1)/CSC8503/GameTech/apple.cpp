#include "apple.h"

using namespace NCL;
using namespace CSC8503;
Apple::Apple(string name) : GameObject(name)
{

}
void Apple::SetInitposition(Vector3& a) {
	
	this->initposition = a;

}

void Apple::GobackInit() {

	this->GetTransform().SetWorldPosition(this->initposition);
	this->GetTransform().SetLocalPosition(this->initposition);

}
void Apple::claercol() {
	collisiongoose = false;

}