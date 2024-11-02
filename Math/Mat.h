#pragma once

#include "Vec.h"

#include <array>
#include <math.h>

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	/// 3x3 Matrix class
	template<typename T>
	struct Mat3
	{
		std::array<T, 9> data;
		/// Constructors
		Mat3(T val = 0)
			: data(val) {}
		Mat3(const std::array<T, 9>& data)
			: data(data) {}
		Mat3(const T& m11, const T& m12, const T& m13, const T& m21, const T& m22, const T& m23, const T& m31, const T& m32, const T& m33)
			: data{ m11, m12, m13, m21, m22, m23, m31, m32, m33 } {}
		/// Operators
		Mat3<T> operator*(const Mat3<T>& other) const;
		void operator*=(const Mat3<T>& other);
		Vec3<T> operator*(const Vec3<T>& vec) const;
		Mat3<T> operator*(const T& scalar) const;
		void operator*=(const T& scalar);
		Mat3<T> operator/(const T& scalar) const;
		void operator/=(const T& scalar);
		/// Advanced constructors
		static Mat3<T> Identity() {
			return { T(1), T(0), T(0), T(0), T(1), T(0), T(0), T(0), T(1) };
		}
	};
	//------------------------------------------------------------------------------------------------------
	/// 4x4 Matrix class
	template<typename T>
	struct Mat4
	{
		std::array<T, 16> data;
		/// Constructors
		Mat4(T val = 0)
			: data(val) {}
		Mat4(const std::array<T, 16>& data)
			: data(data) {}
		Mat4(const T& m11, const T& m12, const T& m13, const T& m14, const T& m21, const T& m22, const T& m23, const T& m24, const T& m31, const T& m32, const T& m33, const T& m34, const T& m41, const T& m42, const T& m43, const T& m44)
			: data{ m11, m12, m13, m14, m21, m22, m23, m24, m31, m32, m33, m34, m41, m42, m43, m44 } {}
		/// Operators
		Mat4<T> operator*(const Mat4<T>& other) const;
		void operator*=(const Mat4<T>& other);
		Vec4<T> operator*(const Vec4<T>& vec) const;
		Mat4<T> operator*(const T& scalar) const;
		void operator*=(const T& scalar);
		Mat4<T> operator/(const T& scalar) const;
		void operator/=(const T& scalar);
		/// Advanced constructors
		static Mat4<T> Identity() {
			return Mat4<T>{ T(1), T(0), T(0), T(0), T(0), T(1), T(0), T(0), T(0), T(0), T(1), T(0), T(0), T(0), T(0), T(1) };
		}
		static Mat4<T> Translate(const Vec3<T>& offset);
		static Mat4<T> Scale(const T& scale);
		static Mat4<T> TranslateAndScale(const Vec3<T>& offset, const T& scale);
		static Mat4<T> Scale(const Vec3<T>& scale);
		static Mat4<T> TranslateAndScale(const Vec3<T>& offset, const Vec3<T>& scale);
		/// Rotation matrix for rotation around x-axis
		static Mat4<T> RotateX(const T& angle);
		/// Rotation matrix for rotation around x-axis
		static Mat4<T> RotateY(const T& angle);
		/// Rotation matrix for rotation around x-axis
		static Mat4<T> RotateZ(const T& angle);
		/// Perspective projection matrix
		/// fovy:	field of view in y direction
		/// aspect:	aspect ration (image width / image height)
		/// nearPlane:	distance to the nearPlane clipping plane
		/// farPlane:		distance to the farPlane clipping plane
		static Mat4<T> PerspectiveProjection(const T& fovy, const T& aspect, const T& nearPlane, const T& farPlane);
		/// View matrix, looking from position to target.
		/// position:	position of the camera
		/// target:		position of the target
		/// up:			vector that points "up" from the perspective of the camera
		static Mat4<T> ViewLookAt(const Vec3<T> position, const Vec3<T> target, const Vec3<T> up);
		/// View matrix of camera at position looking in direction.
		/// position:	position of the camera
		/// direction:	direction the camera is looking
		/// up:			vector that points "up" from the perspective of the camera
		static Mat4<T> ViewPositionDirection(const Vec3<T>& position, const Vec3<T>& direction, const Vec3<T>& up);

	};
	//------------------------------------------------------------------------------------------------------
	/// Typedefs for ease of use
	using Mat3f = Mat3<float>;
	using Mat3d = Mat3<double>;
	using Mat3i = Mat3<int>;
	using Mat4f = Mat4<float>;
	using Mat4d = Mat4<double>;
	using Mat4i = Mat4<int>;
	//======================================================================================================
	// Mat3 implementation
	//======================================================================================================
	template<typename T>
	inline Mat3<T> Mat3<T>::operator*(const Mat3<T>& other) const
	{
		return Mat3<T>{ 
			data[0] * other.data[0] + data[1] * other.data[3] + data[2] * other.data[6],
			data[0] * other.data[1] + data[1] * other.data[4] + data[2] * other.data[7],
			data[0] * other.data[2] + data[1] * other.data[5] + data[2] * other.data[8],

			data[3] * other.data[0] + data[4] * other.data[3] + data[5] * other.data[6],
			data[3] * other.data[1] + data[4] * other.data[4] + data[5] * other.data[7],
			data[3] * other.data[2] + data[4] * other.data[5] + data[5] * other.data[8],

			data[6] * other.data[0] + data[7] * other.data[3] + data[8] * other.data[6],
			data[6] * other.data[1] + data[7] * other.data[4] + data[8] * other.data[7],
			data[6] * other.data[2] + data[7] * other.data[5] + data[8] * other.data[8],
			};
	}
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	inline void Mat3<T>::operator*=(const Mat3<T>& other)
	{
		*this = *this * other;
	}
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	inline Vec3<T> Mat3<T>::operator*(const Vec3<T>& vec) const
	{
		return Vec3<T>{
			data[0] * vec.x + data[1] * vec.y + data[2] * vec.z,
			data[3] * vec.x + data[4] * vec.y + data[5] * vec.z,
			data[6] * vec.x + data[7] * vec.y + data[8] * vec.z,
			};
	}
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	inline Mat3<T> Mat3<T>::operator*(const T& scalar) const
	{
		return Mat3<T>{ 
			data[0] * scalar, data[1] * scalar, data[2] * scalar,
			data[3] * scalar, data[4] * scalar, data[5] * scalar,
			data[6] * scalar, data[7] * scalar, data[8] * scalar,
			};
	}
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	Mat3<T> operator*(const T& scalar, const Mat3<T>& mat)
	{
		return mat * scalar;
	}
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	inline void Mat3<T>::operator*=(const T& scalar)
	{
		data[0] *= scalar; data[1] *= scalar; data[2] *= scalar;
		data[3] *= scalar; data[4] *= scalar; data[5] *= scalar;
		data[6] *= scalar; data[7] *= scalar; data[8] *= scalar;
	}
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	inline Mat3<T> Mat3<T>::operator/(const T& scalar) const
	{
		return Mat3<T>{ 
			data[0] / scalar, data[1] / scalar, data[2] / scalar,
			data[3] / scalar, data[4] / scalar, data[5] / scalar,
			data[6] / scalar, data[7] / scalar, data[8] / scalar, 
			};
	}
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	inline void Mat3<T>::operator/=(const T& scalar)
	{
		data[0] /= scalar; data[1] /= scalar; data[2] /= scalar;
		data[3] /= scalar; data[4] /= scalar; data[5] /= scalar;
		data[6] /= scalar; data[7] /= scalar; data[8] /= scalar;
	}
	//======================================================================================================
	// Mat4 implementation
	//======================================================================================================
	template<typename T>
	inline Mat4<T> Mat4<T>::operator*(const Mat4<T>& other) const
	{
		return Mat4<T>{ 
			data[0] * other.data[0] + data[1] * other.data[4] + data[2] * other.data[8] + data[3] * other.data[12],
			data[0] * other.data[1] + data[1] * other.data[5] + data[2] * other.data[9] + data[3] * other.data[13],
			data[0] * other.data[2] + data[1] * other.data[6] + data[2] * other.data[10] + data[3] * other.data[14],
			data[0] * other.data[3] + data[1] * other.data[7] + data[2] * other.data[11] + data[3] * other.data[15],

			data[4] * other.data[0] + data[5] * other.data[4] + data[6] * other.data[8] + data[7] * other.data[12],
			data[4] * other.data[1] + data[5] * other.data[5] + data[6] * other.data[9] + data[7] * other.data[13],
			data[4] * other.data[2] + data[5] * other.data[6] + data[6] * other.data[10] + data[7] * other.data[14],
			data[4] * other.data[3] + data[5] * other.data[7] + data[6] * other.data[11] + data[7] * other.data[15],
			
			data[8] * other.data[0] + data[9] * other.data[4] + data[10] * other.data[8] + data[11] * other.data[12],
			data[8] * other.data[1] + data[9] * other.data[5] + data[10] * other.data[9] + data[11] * other.data[13],
			data[8] * other.data[2] + data[9] * other.data[6] + data[10] * other.data[10] + data[11] * other.data[14],
			data[8] * other.data[3] + data[9] * other.data[7] + data[10] * other.data[11] + data[11] * other.data[15],

			data[12] * other.data[0] + data[13] * other.data[4] + data[14] * other.data[8] + data[15] * other.data[12],
			data[12] * other.data[1] + data[13] * other.data[5] + data[14] * other.data[9] + data[15] * other.data[13],
			data[12] * other.data[2] + data[13] * other.data[6] + data[14] * other.data[10] + data[15] * other.data[14],
			data[12] * other.data[3] + data[13] * other.data[7] + data[14] * other.data[11] + data[15] * other.data[15],
			};
	}
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	inline void Mat4<T>::operator*=(const Mat4<T>& other)
	{
		*this = *this * other;
	}
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	inline Vec4<T> Mat4<T>::operator*(const Vec4<T>& vec) const
	{
		return Vec4<T>{
			data[0] * vec.x + data[1] * vec.y + data[2] * vec.z + data[3] * vec.w,
			data[4] * vec.x + data[5] * vec.y + data[6] * vec.z + data[7] * vec.w,
			data[8] * vec.x + data[9] * vec.y + data[10] * vec.z + data[11] * vec.w,
			data[12] * vec.x + data[13] * vec.y + data[14] * vec.z + data[15] * vec.w,
			};
	}
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	inline Mat4<T> Mat4<T>::operator*(const T& scalar) const
	{
		return Mat4<T>{ 
			data[0] * scalar, data[1] * scalar, data[2] * scalar, data[3] * scalar,
			data[4] * scalar, data[5] * scalar, data[6] * scalar, data[7] * scalar,
			data[8] * scalar, data[9] * scalar, data[10] * scalar, data[11] * scalar,
			data[12] * scalar, data[13] * scalar, data[14] * scalar, data[15] * scalar,
			};
	}
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	Mat4<T> operator*(const T& scalar, const Mat4<T>& mat)
	{
		return mat * scalar;
	}
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	inline void Mat4<T>::operator*=(const T& scalar)
	{
		data[0] *= scalar; data[1] *= scalar; data[2] *= scalar; data[3] *= scalar;
		data[4] *= scalar; data[5] *= scalar; data[6] *= scalar; data[7] *= scalar;
		data[8] *= scalar; data[9] *= scalar; data[10] *= scalar; data[11] *= scalar;
		data[12] *= scalar; data[13] *= scalar; data[14] *= scalar; data[15] *= scalar;
	}
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	inline Mat4<T> Mat4<T>::operator/(const T& scalar) const
	{
		return Mat4<T>{ 
			data[0] / scalar, data[1] / scalar, data[2] / scalar, data[3] / scalar,
			data[4] / scalar, data[5] / scalar, data[6] / scalar, data[7] / scalar,
			data[8] / scalar, data[9] / scalar, data[10] / scalar, data[11] / scalar,
			data[12] / scalar, data[13] / scalar, data[14] / scalar, data[15] / scalar,
			};
	}
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	inline void Mat4<T>::operator/=(const T& scalar)
	{
		data[0] /= scalar; data[1] /= scalar; data[2] /= scalar; data[3] /= scalar;
		data[4] /= scalar; data[5] /= scalar; data[6] /= scalar; data[7] /= scalar;
		data[8] /= scalar; data[9] /= scalar; data[10] /= scalar; data[11] /= scalar;
		data[12] /= scalar; data[13] /= scalar; data[14] /= scalar; data[15] /= scalar;
	}
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	inline Mat4<T> Mat4<T>::Translate(const Vec3<T>& offset)
	{
		return Mat4<T>{
			T(1), T(0), T(0), offset.x,
			T(0), T(1), T(0), offset.y,
			T(0), T(0), T(1), offset.z,
			T(0), T(0), T(0), T(1),
			};
	}
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	inline Mat4<T> Mat4<T>::Scale(const T& scale)
	{
		return Mat4<T>{
			T(scale), T(0), T(0), T(0),
			T(0), T(scale), T(0), T(0),
			T(0), T(0), T(scale), T(0),
			T(0), T(0), T(0), T(1),
			};
	}
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	inline Mat4<T> Mat4<T>::TranslateAndScale(const Vec3<T>& offset, const T& scale)
	{
		return Mat4<T>{
			T(scale), T(0), T(0), offset.x,
			T(0), T(scale), T(0), offset.y,
			T(0), T(0), T(scale), offset.z,
			T(0), T(0), T(0), T(1),
			};
	}
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	inline Mat4<T> Mat4<T>::Scale(const Vec3<T>& scale)
	{
		return Mat4<T>{
			T(scale.x), T(0), T(0), T(0),
			T(0), T(scale.y), T(0), T(0),
			T(0), T(0), T(scale.z), T(0),
			T(0), T(0), T(0), T(1),
			};
	}
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	inline Mat4<T> Mat4<T>::TranslateAndScale(const Vec3<T>& offset, const Vec3<T>& scale)
	{
		return Mat4<T>{
			T(scale.x), T(0), T(0), offset.x,
			T(0), T(scale.y), T(0), offset.y,
			T(0), T(0), T(scale.z), offset.z,
			T(0), T(0), T(0), T(1),
			};
	}
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	inline Mat4<T> Mat4<T>::RotateX(const T& angle)
	{
		return Mat4<T>{
			T(1), T(0), T(0), T(0),
			T(0), cos(angle), -sin(angle), T(0),
			T(0), sin(angle), cos(angle), T(0),
			T(0), T(0), T(0), T(1)
			};
	}
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	inline Mat4<T> Mat4<T>::RotateY(const T& angle)
	{
		return Mat4<T>{
			cos(angle), T(0), sin(angle), T(0),
			T(0), T(1), T(0), T(0),
			-sin(angle), T(0), cos(angle), T(0),
			T(0), T(0), T(0), T(1)
			};
	}
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	inline Mat4<T> Mat4<T>::RotateZ(const T& angle)
	{
		return Mat4<T>{
			cos(angle), -sin(angle), T(0), T(0),
			sin(angle), cos(angle), T(0), T(0),
			T(0), T(0), T(1), T(0),
			T(0), T(0), T(0), T(1)
			};
	}
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	inline Mat4<T> Mat4<T>::PerspectiveProjection(const T& fovy, const T& aspect, const T& nearPlane, const T& farPlane)
	{
		T temp = tan(fovy / T(2));
		return Mat4<T>{
			T(1) / (temp * aspect), T(0), T(0), T(0),
			T(0), T(1) / temp, T(0), T(0),
			T(0), T(0), T(-1) * (farPlane + nearPlane) / (farPlane - nearPlane), T(-2) * farPlane * nearPlane / (farPlane - nearPlane),
			T(0), T(0), T(-1), T(0)
			};
	}
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	inline Mat4<T> Mat4<T>::ViewLookAt(const Vec3<T> position, const Vec3<T> target, const Vec3<T> up)
	{
		Vec3<T> direction = target - position;
		direction.normalize();
		return ViewPositionDirection(position, direction, up);
	}
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	inline Mat4<T> Mat4<T>::ViewPositionDirection(const Vec3<T>& position, const Vec3<T>& direction, const Vec3<T>& up)
	{
		// Documentation: https://www.3dgep.com/understanding-the-view-matrix
		Vec3<T> right = cross(up, direction); // The "right" vector.
		right.normalize();
		Vec3<T> upTemp = cross(direction, right); // The "up" vector.
		return Mat4<T>({
			right.x, right.y, right.z, -(right * position),
			upTemp.x, upTemp.y, upTemp.z, -(upTemp * position),
			-direction.x, -direction.y, -direction.z, direction* position,
			T(0), T(0), T(0), T(1),
			});
	}
	//------------------------------------------------------------------------------------------------------
}