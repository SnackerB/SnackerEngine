#pragma once

#include <math.h>
#include <ostream>

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	/// 2-dimensional Vector
	template<typename T>
	struct Vec2
	{
		T x, y;
		/// Constructors
		Vec2(const T& x, const T& y)
			: x(x), y(y) {}
		Vec2(const T& val)
			: x(val), y(val) {}
		Vec2() = default;
		/// Operators
		Vec2<T> operator+(const Vec2<T>& other) const { return Vec2<T>(x + other.x, y + other.y); }
		void operator+=(const Vec2<T>& other) { x += other.x; y += other.y; }
		Vec2<T> operator-(const Vec2<T>& other) const { return Vec2<T>(x - other.x, y - other.y); }
		void operator-=(const Vec2<T>& other) { x -= other.x; y -= other.y; }
		T operator*(const Vec2<T>& other) const { return x * other.x + y * other.y; }
		Vec2<T> operator*(const T& scalar) const { return Vec2<T>(x * scalar, y * scalar); }
		void operator*=(const T& scalar) const { x *= scalar; y *= scalar; }
		Vec2<T> operator/(const T& scalar) const { return Vec2<T>(x / scalar, y / scalar); }
		void operator/=(const T& scalar) const { x /= scalar; y /= scalar; }
		bool operator==(const Vec2<T>& other) const { return x == other.x && y == other.y; }
		bool operator!=(const Vec2<T>& other) const { return x != other.x || y != other.y; }
		/// More involved computations
		T squaredMagnitude() const { return x * x + y * y; }
		T magnitude() const { return sqrt(squaredMagnitude()); }
		void normalize();
		T cross(const Vec2<T>& other) const { return x * other.y - y * other.x; }
		T dot(const Vec2<T>& other) const { return x * other.x + y * other.y; }
		/// Explicit conversions
		template<typename T2>
		Vec2<T>(const Vec2<T2>& other);
	};
	//------------------------------------------------------------------------------------------------------
	/// 3-dimensional Vector
	template<typename T>
	struct Vec3
	{
		T x, y, z;
		/// Constructors
		Vec3(const T& x, const T& y, const T& z)
			: x(x), y(y), z(z) {}
		Vec3(const T& val)
			: x(val), y(val), z(val) {}
		Vec3(const Vec2<T>& vec)
			: x(vec.x), y(vec.y), z(T(0)) {}
		Vec3() = default;
		/// Operators
		Vec3<T> operator+(const Vec3<T>& other) const { return Vec3<T>(x + other.x, y + other.y, z + other.z); }
		void operator+=(const Vec3<T>& other) { x += other.x; y += other.y; z += other.z; }
		Vec3<T> operator-(const Vec3<T>& other) const { return Vec3<T>(x - other.x, y - other.y, z - other.z); }
		void operator-=(const Vec3<T>& other) { x -= other.x; y -= other.y; z -= other.z; }
		T operator*(const Vec3<T>& other) const { return x * other.x + y * other.y + z * other.z; }
		Vec3<T> operator*(const T& scalar) const { return Vec3<T>(x * scalar, y * scalar, z * scalar); }
		void operator*=(const T& scalar) { x *= scalar; y *= scalar; z *= scalar; }
		Vec3<T> operator/(const T& scalar) const { return Vec3<T>(x / scalar, y / scalar, z / scalar); }
		void operator/=(const T& scalar) { x /= scalar; y /= scalar; z /= scalar; }
		bool operator==(const Vec3<T>& other) const { return x == other.x && y == other.y && z == other.z; }
		bool operator!=(const Vec3<T>& other) const { return x != other.x || y != other.y || z != other.z; }
		/// More involved computations
		T squaredMagnitude() const { return x * x + y * y + z * z; }
		T magnitude() const { return sqrt(squaredMagnitude()); }
		void normalize();
		Vec3<T> cross(const Vec3<T>& other) const { return Vec3<T>(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x); }
		T dot(const Vec3<T>& other) const { return x * other.x + y * other.y + z * other.z; }
		/// Explicit conversions
		template<typename T2>
		Vec3(const Vec3<T2>& other);
	};
	//------------------------------------------------------------------------------------------------------
	/// 4-dimensional Vector
	template<typename T>
	struct Vec4
	{
		T x, y, z, w;
		/// Constructors
		Vec4(const T& x, const T& y, const T& z, const T& w)
			: x(x), y(y), z(z), w(w) {}
		Vec4(const T& val)
			: x(val), y(val), z(val), w(val) {}
		Vec4() = default;
		/// Operators
		Vec4<T> operator+(const Vec4<T>& other) const { return Vec4<T>(x + other.x, y + other.y, z + other.z, w + other.w); }
		void operator+=(const Vec4<T>& other) { x += other.x; y += other.y; z += other.z; w += other.w; }
		Vec4<T> operator-(const Vec4<T>& other) const { return Vec4<T>(x - other.x, y - other.y, z - other.z, w - other.w); }
		void operator-=(const Vec4<T>& other) { x -= other.x; y -= other.y; z -= other.z; w -= other.w; }
		T operator*(const Vec4<T>& other) const { return x * other.x + y * other.y + z * other.z + w * other.w; }
		Vec4<T> operator*(const T& scalar) const { return Vec4<T>(x * scalar, y * scalar, z * scalar, w * scalar); }
		void operator*=(const T& scalar) const { x *= scalar; y *= scalar; z *= scalar; w *= scalar; }
		Vec4<T> operator/(const T& scalar) const { return Vec4<T>(x / scalar, y / scalar, z / scalar, w / scalar); }
		void operator/=(const T& scalar) const { x /= scalar; y /= scalar; z /= scalar; w /= scalar; }
		bool operator==(const Vec4<T>& other) const { return x == other.x && y == other.y && z == other.z && w == other.w; }
		bool operator!=(const Vec4<T>& other) const { return x != other.x || y != other.y || z != other.z || w != other.w; }
		/// More involved computations
		T squaredMagnitude() const { return x * x + y * y + z * z + w * w; }
		T magnitude() const { return sqrt(squaredMagnitude()); }
		void normalize();
		T dot(const Vec4<T>& other) const { return x * other.x + y * other.y + z * other.z + w + other.w; }
		/// Explicit conversions
		template<typename T2>
		Vec4(const Vec4<T2>& other);
	};
	//------------------------------------------------------------------------------------------------------
	/// Typedefs for ease of use
	using Vec2f = Vec2<float>;
	using Vec2d = Vec2<double>;
	using Vec2i = Vec2<int>;
	using Vec3f = Vec3<float>;
	using Vec3d = Vec3<double>;
	using Vec3i = Vec3<int>;
	using Vec4f = Vec4<float>;
	using Vec4d = Vec4<double>;
	using Vec4i = Vec4<int>;
	//======================================================================================================
	// Vec2 implementation
	//======================================================================================================
	template<typename T>
	Vec2<T> operator*(const T& scalar, const Vec2<T>& vec) 
	{
		return Vec2<T>(scalar * vec.x, scalar * vec.y);
	}
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	inline void Vec2<T>::normalize()
	{
		T norm = squaredMagnitude();
		if (norm == T(1) || norm == T(0)) return;
		norm = sqrt(norm);
		*this /= norm;
	}
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	T cross(const Vec2<T>& v1, const Vec2<T>& v2)
	{
		return v1.x * v2.y - v1.y * v2.x;
	}
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	std::ostream& operator<<(std::ostream& ostream, const Vec2<T>& vec)
	{
		return ostream << "(" << vec.x << ", " << vec.y << ")";
	}
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	template<typename T2>
	inline Vec2<T>::Vec2(const Vec2<T2>& other)
		: x(static_cast<T>(other.x)), y(static_cast<T>(other.y)) {}
	//======================================================================================================
	// Vec3 implementation
	//======================================================================================================
	template<typename T>
	Vec3<T> operator*(const T& scalar, const Vec3<T>& vec)
	{
		return Vec3<T>(scalar * vec.x, scalar * vec.y, scalar * vec.z);
	}
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	inline void Vec3<T>::normalize()
	{
		T norm = squaredMagnitude();
		if (norm == T(1) || norm == T(0)) return;
		norm = sqrt(norm);
		*this /= norm;
	}
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	Vec3<T> cross(const Vec3<T>& v1,const Vec3<T>& v2)
	{
		return Vec3<T>(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
	}
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	std::ostream& operator<<(std::ostream& ostream, const Vec3<T>& vec)
	{
		return ostream << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
	}
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	template<typename T2>
	inline Vec3<T>::Vec3(const Vec3<T2>& other)
		: x(static_cast<T>(other.x)), y(static_cast<T>(other.y)), z(static_cast<T>(other.z)) {}
	//======================================================================================================
	// Vec4 implementation
	//======================================================================================================
	template<typename T>
	Vec4<T> operator*(const T& scalar, const Vec4<T>& vec)
	{
		return Vec4<T>(scalar * vec.x, scalar * vec.y, scalar * vec.z, scalar * vec.w);
	}
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	inline void Vec4<T>::normalize()
	{
		T norm = squaredMagnitude();
		if (norm == T(1) || norm == T(0)) return;
		norm = sqrt(norm);
		*this /= norm;
	}
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	std::ostream& operator<<(std::ostream& ostream, const Vec4<T>& vec)
	{
		return ostream << "(" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ")";
	}
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	template<typename T2>
	inline Vec4<T>::Vec4(const Vec4<T2>& other)
		: x(static_cast<T>(other.x)), y(static_cast<T>(other.y)), z(static_cast<T>(other.z)), w(static_cast<T>(other.w)) {}
	//------------------------------------------------------------------------------------------------------
}