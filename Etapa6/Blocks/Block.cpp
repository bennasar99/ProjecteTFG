#include "Block.h"
#include "../World.h"
#include <iostream>

Block::Block(World* world, Bloc id, Block* parent) {
	this->world = world;
	this->id = id;
	this->parent = parent;
}

Block::Block() {
	this->world = NULL;
	this->id = Bloc::RES;
	this->parent = 0;
}

void Block::update(int delta) {

}

Block* Block::getParent() {
	return this->parent;
}

Bloc Block::getId() {
	return this->id;
}

void Block::setId(Bloc id) {
	this->id = id;
}

void Block::destroy(){
	if (parent != 0) {
		this->parent->destroy();
	}
};

void Block::interact() {
	if (parent != 0) {
		this->parent->interact();
	}
};

bool Block::isTransparent(Bloc tipus) {
	if (tipus == Bloc::RES || tipus == Bloc::AIRE || tipus == Bloc::HERBA || tipus == Bloc::VIDRE || tipus == Bloc::TORXA
		|| tipus == Bloc::HERBAFULL || tipus == Bloc::AIGUA) {
		return true;
	}
	return false;
}

bool Block::isSolid(Bloc tipus) {
	if (tipus != Bloc::RES && tipus != Bloc::AIRE && tipus != Bloc::HERBA && tipus != Bloc::TORXA && tipus != Bloc::HERBAFULL 
		&& tipus != Bloc::AIGUA) { //Exclude list
		return true;
	}
	return false;
}