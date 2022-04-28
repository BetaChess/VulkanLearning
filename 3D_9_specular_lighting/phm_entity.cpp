#include "pch.h"

#include "phm_entity.h"

namespace phm
{
	namespace ecs
	{
		void Entity::update()
		{
			for (auto& c : components_)
			{
				c->update(*c->entityPtr_);
			}
		}
	}
}
