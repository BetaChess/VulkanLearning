#ifndef PHM_FUNCTION_H
#define PHM_FUNCTION_H

#include <functional>

#include "phm_component.h"

#define FUNCTIONCOMPONENTLAMDA(paramCount, paramType) [=](ecs::Entity& e, const std::array<paramType, paramCount>& constParams)
#define FUNCTIONCOMPONENTLAMDA(paramCount, paramType, constParamName) [=](ecs::Entity& e, const std::array<paramType, paramCount>& constParamName)

namespace phm
{
	namespace ecs
	{
		template<size_t numberOfConstParameters = 0, typename arrayT = int>
		class FunctionComponent : public Component
		{
		public:
			const std::array<const arrayT, numberOfConstParameters> constParams;
			
		public:
			FunctionComponent() = delete;
			FunctionComponent(std::function<void(Entity&, const std::array<const arrayT, numberOfConstParameters>& constParams)> function, const std::array<const arrayT, numberOfConstParameters>& constParams = {}) : 
				constParams(constParams),
				function_(function)
			{}

			virtual void update(Entity& entity) override
			{
				function_(entity, constParams);
			}

		private:
			std::function<void(Entity&, const std::array<const arrayT, numberOfConstParameters>)> function_;
		};
	}
}


#endif /* PHM_FUNCTION_H */
