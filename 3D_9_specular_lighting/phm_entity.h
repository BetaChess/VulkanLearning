#ifndef PHM_ENTITY_H
#define PHM_ENTITY_H

#include <vector>

#include "phm_component.h"

#include "phm_transform.h"

namespace phm
{
	namespace ecs
	{
		class Entity
		{
		public:
			// All entities have a transform
			Transform transform{};

		public:


			void update();

			[[nodiscard]] inline bool isAlive() const { return alive_; };
			inline void destroy() { alive_ = false; };

			template<typename T>
			inline bool hasComponent() const
			{
				return componentBitSet_[getComponentTypeID<T>()];
			}

			template<typename T, typename... TArgs>
			T& addComponent(TArgs&&... mArgs)
			{
				T* c(new T(std::forward<TArgs>(mArgs)...));
				c->entityPtr_ = this;
				std::unique_ptr<Component> uPtr{ c };
				components_.emplace_back(std::move(uPtr));

				componentArray_[getComponentTypeID<T>()] = c;
				componentBitSet_[getComponentTypeID<T>()] = true;

				return *c;
			}

			template<typename T>
			inline T& getComponent() const
			{
				auto ptr = componentArray_[getComponentTypeID<T>()];
				return *static_cast<T*>(ptr);
			}

		private:


			// Entities that are not alive will be cleaned by the manager
			bool alive_ = true;

			std::vector<std::unique_ptr<Component>> components_;

			ComponentArray componentArray_{};
			ComponentBitSet componentBitSet_{};
		};

	}
}



#endif /* PHM_ENTITY_H */