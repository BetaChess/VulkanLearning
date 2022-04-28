#ifndef PHM_COMPONENT_H
#define PHM_COMPONENT_H

#include <array>
#include <bitset>
#include <cmath>


namespace phm
{
	namespace ecs
	{
		class Entity;
		class Component;
		

		// Defining the maximum number of components any entity can have.
		constexpr size_t maxComponents = 32;

		// Defining types
		using ComponentTypeID = uint64_t;
		using ComponentBitSet = std::bitset<maxComponents>;
		using ComponentArray = std::array<Component*, maxComponents>;

		// Defining helper functions
		inline ComponentTypeID getComponentTypeID()
		{
			static ComponentTypeID lastID = 0;
			return lastID++;
		}

		template<typename T>
		inline ComponentTypeID getComponentTypeID() noexcept
		{
			static ComponentTypeID typeID = getComponentTypeID();
			return typeID;
		}

		// --- Class definitions ---

		class Component
		{
		public:
			virtual ~Component() = default;

			//virtual void init() = 0;
			virtual void update(Entity& entity) {};

		protected:
			// Non-owning pointer to the parent entity.
			Entity* entityPtr_;

			friend Entity;
		};
	}
}




#endif /* PHM_COMPONENT_H */