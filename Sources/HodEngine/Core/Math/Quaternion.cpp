#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Math/Quaternion.hpp"

#include <cstring>
#include <cmath>

namespace hod
{
	const Quaternion Quaternion::Identity;

	/// @brief 
	Quaternion::Quaternion()
	: _x(0.0f)
	, _y(0.0f)
	, _z(0.0f)
	, _w(1.0f)
	{

	}

	/// @brief 
	Quaternion::Quaternion(float x, float y, float z, float w)
	: _x(x)
	, _y(y)
	, _z(z)
	, _w(w)
	{

	}

	/// @brief 
	/// @return 
	float Quaternion::GetAngleZ() const
	{
		return atan2(2.0f * (_w * _z + _x * _y), 1.0f - 2.0f * (_y * _y + _z * _z));
	}

	Quaternion Quaternion::LookRotation(const Vector2& direction)
    {
        Vector2 normalized = Vector2::Normalize(direction);

        float angle = atan2(normalized.GetY(), normalized.GetX());

        float halfAngle = angle / 2.0f;
        float sinHalfAngle = sin(halfAngle);
        float cosHalfAngle = cos(halfAngle);

        return Quaternion(0.0f, 0.0f, sinHalfAngle, cosHalfAngle);
    }

	Vector2 Quaternion::operator*(const Vector2& vec) const
    {
        // Traiter le vecteur 2D comme un vecteur 3D (z = 0)
        float vx = vec.GetX();
        float vy = vec.GetY();
        float vz = 0.0f;

        // Appliquer la rotation via quaternion : q * v * q^-1
        Quaternion qv(vx, vy, vz, 0.0f); // Quaternion représentant le vecteur
        Quaternion qConjugate(-_x, -_y, -_z, _w); // Conjugué du quaternion

        // Rotation : q * v * q^-1
        Quaternion result = (*this) * qv * qConjugate;

        // Renvoyer le vecteur 2D résultant (composantes x et y seulement)
        return Vector2(result._x, result._y);
    }

	Quaternion Quaternion::operator*(const Quaternion& q) const
    {
        return Quaternion(
            _w * q._x + _x * q._w + _y * q._z - _z * q._y,
            _w * q._y - _x * q._z + _y * q._w + _z * q._x,
            _w * q._z + _x * q._y - _y * q._x + _z * q._w,
            _w * q._w - _x * q._x - _y * q._y - _z * q._z
        );
    }
}
