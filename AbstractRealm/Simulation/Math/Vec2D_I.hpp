/*
Vector 2D Integer

Last Modified: 5/18/2020

Think about: Should I just make these templated? (That way there is one class definition)

Reference: https://github.com/godotengine/godot/blob/master/core/math/vector2.h
*/



#pragma once



#include "Math/Units.hpp"



namespace Math
{
	/*
		2-Dimensional Vector

		Provides a encapsulation of a 2D vector, with intended use in simulated objects.
	*/
	class Vec2D_I
	{
	public:

		// Enums

		enum Axis { X, Y };


		// Constructor

		Vec2D_I              ();
		Vec2D_I(IntN X, IntN Y);


		// Function

		Vec2D_I Abs() const;   // Absolute value vector (all component values positive).

		DecN AspectRatio() const;   // The aspect ratio of the components (X:Y).

		Vec2D_I Signed() const;   // Provides a vector with each component set to a unit value sign (+ or - 1).

	
		// Operator Overloads

		bool operator == (const Vec2D_I& OtherVector2D) const;
		bool operator != (const Vec2D_I& OtherVector2D) const;
		bool operator <= (const Vec2D_I& OtherVector2D) const;
		bool operator >= (const Vec2D_I& OtherVector2D) const;
		bool operator <  (const Vec2D_I& OtherVector2D) const;
		bool operator >  (const Vec2D_I& OtherVector2D) const;

		Vec2D_I operator + (const Vec2D_I& OtherVector2D) const;
		Vec2D_I operator - (const Vec2D_I& OtherVector2D) const;
		Vec2D_I operator * (const Vec2D_I& OtherVector2D) const;
		Vec2D_I operator / (const Vec2D_I& OtherVector2D) const;

		void operator += (const Vec2D_I& OtherVector2D) const;
		void operator -= (const Vec2D_I& OtherVector2D) const;
		void operator *= (const Vec2D_I& OtherVector2D) const;
		void operator /= (const Vec2D_I& OtherVector2D) const;

		      IntN& operator [] (int Index)      ;
		const IntN& operator [] (int Index) const;


		// Components

		union   // First
		{
			IntN X = IntN(0);

			IntN Width;
		};

		union   // Second
		{
			IntN Y = IntN(0);

			IntN Height;
		};
	};

	using Size2D_I  = Vec2D_I;
	using Point2D_I = Vec2D_I;
}
