#include "world.h"
#include "renderer/graphics.h"

World::World(Color background) : background(background)
{

}

World::~World()
{
	
}

void World::Render()
{
	Graphics::SetClearColor(background);
}
