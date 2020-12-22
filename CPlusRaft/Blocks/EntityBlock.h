#pragma once
#include "../Vector3.h"
#include "Block.h"
#include "../Entities/Entity.h"
#include "../SoundManager.h"

class World;

/*
	Classe que representa un bloc amb una entitat vinculada. NOMÉS ENTITATS ESTÀTIQUES.
*/
class EntityBlock : public Block {
private:

	Entity*ent; //Entitat vinculada

	Vector3<int> pos; //Posició del bloc

public:
	EntityBlock(World* world, Bloc id, Vector3<int> pos);

	void destroy(World* world);

	void draw();

};
