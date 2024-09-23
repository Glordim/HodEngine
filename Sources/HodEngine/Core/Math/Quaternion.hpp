#pragma once
#include "HodEngine/Core/Export.hpp"

#include <cstdint>

namespace hod
{	
	class HOD_CORE_API Quaternion final
	{
	public:

		static const Quaternion Identity;

	public:

		Quaternion();
		Quaternion(float x, float y, float z, float w);

		float GetAngleZ() const;

	public:

    	float _x;
		float _y;
		float _z;
		float _w;
	};
}
