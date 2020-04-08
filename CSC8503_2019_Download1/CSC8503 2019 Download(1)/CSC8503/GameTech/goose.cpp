#include "../GameTech/goose.h"

using namespace NCL;
using namespace CSC8503;
Goose::Goose(string name) : GameObject(name)
{

}
void Goose::collisionclear() {

	collisionapple = false;
	collisionwater = false;
	collisionisland = false;
	 collisionkeeper = false;

}