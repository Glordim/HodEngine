#pragma once

#include <vector>

namespace HOD
{
	namespace CORE
	{
		class ReflectionTrait;

		///@brief 
		class ReflectionProperty
		{
		public:

													ReflectionProperty(const char* typeName);
													ReflectionProperty(const ReflectionProperty& copy) = default;
													ReflectionProperty(ReflectionProperty&& move) = default;
													~ReflectionProperty() = default;

			ReflectionProperty&						operator = (const ReflectionProperty& copy) = default;
			ReflectionProperty&						operator = (ReflectionProperty&& move) = default;

		public:

			virtual void							Serialize() = 0;
			virtual void							Deserialize() = 0;

			const std::vector<ReflectionTrait*>&	GetTraits() const;
			void									AddTrait(ReflectionTrait* trait);

		private:

			const char*								_typeName;
			std::vector<ReflectionTrait*>			_traits;
		};
	}
}
