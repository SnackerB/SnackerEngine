#pragma once

#include <ostream>

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	/// RGB color
	template<typename T>
	struct Color3
	{
		T r, g, b;
		/// Constructors
		Color3(const T& r, const T& g, const T& b)
			: r(r), g(g), b(b) {}
		Color3(const T& val)
			: r(val), g(val), b(val) {}
		Color3() = default;
		/// Converts from a color vector where each color is in [0, 255]
		template<typename T2>
		static Color3<T> fromColor256(Color3<T2> color256);
		Color3<T> operator*(const T& scalar) const { return Color3<T>(r * scalar, g * scalar, b * scalar); }
	};
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	std::ostream& operator<<(std::ostream& ostream, const Color3<T>& color)
	{
		return ostream << "(" << color.r << ", " << color.g << ", " << color.b << ")";
	}
	//------------------------------------------------------------------------------------------------------
	/// RGBA color (A = alpha = opacity)
	template<typename T>
	struct Color4
	{
		T r, g, b, alpha;
		/// Constructors
		Color4(const T& r, const T& g, const T& b, const T& alpha)
			: r(r), g(g), b(b), alpha(alpha) {}
		Color4(const T& val, const T& alpha = T(1))
			: r(val), g(val), b(val), alpha(alpha) {}
		Color4(const Color3<T>& color, const T& alpha)
			: r(color.r), g(color.g), b(color.b), alpha(alpha) {}
		Color4() = default;
		/// Converts from a color vector where each color is in [0, 255]
		template<typename T2>
		static Color4<T> fromColor256(Color4<T2> color256);
		/// Implicit conversion from Color3
		Color4(Color3<T> color)
			: r(color.r), g(color.g), b(color.b), alpha(T(1)) {}
	};
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	std::ostream& operator<<(std::ostream& ostream, const Color4<T>& color)
	{
		return ostream << "(" << color.r << ", " << color.g << ", " << color.b << ", " << color.alpha << ")";
	}
	//------------------------------------------------------------------------------------------------------
	using Color3f = Color3<float>;
	using Color4f = Color4<float>;
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	template<typename T2>
	inline Color3<T> Color3<T>::fromColor256(Color3<T2> color256)
	{
		return Color3<T>(static_cast<T>(color256.r) / T(255), static_cast<T>(color256.g) / T(255), static_cast<T>(color256.b) / T(255));
	}
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	template<typename T2>
	inline Color4<T> Color4<T>::fromColor256(Color4<T2> color256)
	{
		return Color4<T>(static_cast<T>(color256.r) / T(255), static_cast<T>(color256.g) / T(255), static_cast<T>(color256.b) / T(255), static_cast<T>(color256.alpha) / T(255));
	}
	//------------------------------------------------------------------------------------------------------
}