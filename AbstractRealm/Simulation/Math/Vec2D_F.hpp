/*
Vector 2D Floating Point 

Last Modified: 5/18/2020

Think about: Should I just make these templated? (That way there is one class definition)
*/



#pragma once



#include "Math/Units.hpp"



namespace Math
{
	/*
	2-Dimensional Vector

	Provides a encapsulation of a 2D vector, with intended use in simulated objects.
	*/
	class Vec2D_F
	{
	public:
		// Enums

		enum Axis { X, Y };

		
		// Constructors

		Vec2D_F              ();
		Vec2D_F(DecN X, DecN Y);


		// Functions

		Vec2D_F Abs        () const;   // Absolute value vector (all component values positive).
		DecN    Angle      () const;   // Vector's angle in radians.
		DecN    AspectRatio() const;   // The aspect ratio of the components (X:Y).

		Vec2D_F Signed() const;   // Provides a vector with each component set to a unit value sign (+ or - 1).

		// RelationTo

		DecN AngleToward     (const Vec2D_F& OtherVector2D) const;   // Provides an angle in radians between the vector and the other provided.
		DecN AngleTowardPoint(const Vec2D_F& OtherPoint2D ) const;   // Provides the angle in radians betwen the line connecting the two points and the X coordinate. 

		bool ApproxEqual(const Vec2D_F& OtherVector2D) const;   // Determines if both vectors are approximately equal.

		DecN DistanceTo        (const Vec2D_F& OtherVector2D) const;   // Distance to the other vector.
		DecN DistanceTo_Squared(const Vec2D_F& OtherVector2D) const;   // Distance to the other vector squared.

		// Directional (Noramlization)

		bool    IsDirectionalVector() const;   // IsNormalized
		void    Directionalize     ()      ;   // Normalize
		Vec2D_F Directionalized    () const;   // Normalized

		Vec2D_F DirectionToward(const Vec2D_F& OtherVector2D) const;   // Provides a normalized vector pointing towards the vector provide from this vector.

		// Magnitude

		DecN Magnitude      ();   // The magnitude (length) of this vector.
		DecN SquareMagnitude();   // The magnituded (length) of this vector squared.

		// Operations

		Vec2D_F Ceiling() const;   // All components rounded up.

		DecN Cross(const Vec2D_F& OtherVector2D);   // Normal Vector producing product between two vector.
		DecN Dot  (const Vec2D_F& OtherVector2D);   // Inner Product.

		Vec2D_F Floor() const;   // All components rounded down.

		Vec2D_F Modulus_Positive(const DecN     Modulus      ) const;   // TODO: Make a good description.
		Vec2D_F Modulus_Positive(const Vec2D_F& VectorModulus) const;   // TODO: Make a good description.
		
		DecN ProjectOnto(const Vec2D_F& Vector2D_ToProjectOnto);   // Proivdes the vector projection onto the vector to project to.

		Vec2D_F PlaneProjection(DecN NotSure_D, const Vec2D_F& DirectionPossibly) const;   // TODO: Make a descrption.

		Vec2D_F Round() const;   // The vector with its components rounded to the nearest integer. (Halfway case round away from zero)

		Vec2D_F ToClamped(DecN ClampingMagnitude) const;   // Returns the vector with its magnitude clamped to the specified value.

		// Transformations

		/*
		The resulting vector bounced off from a plane.

		The plane is defined via the provided normal.
		*/
		Vec2D_F Bounce(const Vec2D_F& NormalOfPlane) const;

		Vec2D_F Lerp          (const Vec2D_F& OtherVector2D, DecN DeltaTime) const;   // Interpolate toward vector (Not sure what type).
		Vec2D_F Lerp_Spherical(const Vec2D_F& OtherVector2D, DecN DeltaTime) const;   // Interpolate toward vector (Sphereical approach).

		/*
		Interpolates between this vector and the other provided cubically.

		TODO: Elaborate.
		*/
		Vec2D_F Lerp_Cubic    
		(
			const Vec2D_F& OtherVector2D  , 
			const Vec2D_F& PreA_CheckThis , 
			const Vec2D_F& PostB_CheckThis, 
			      DecN     DeltaTime
		) const;

		Vec2D_F MoveToward(const Vec2D_F& VectorToMoveToward, const DecN DeltaTime) const;

		/*
		The vector reflected from a plane.

		The plane is defined via the provided normal.
		*/
		Vec2D_F Reflect(const Vec2D_F& NormalOfPlane) const;

		/*
		The vector rotated by the provided angle value in radians.
		*/
		Vec2D_F Rotated(const DecN AngleAmountInRadians) const;

		/*
		Normal of the vector along a plane.

		The plane is defined via the provided normal.
		*/
		Vec2D_F Slide(const Vec2D_F& NormalOfPlane_CheckThis) const;

		/*
		The vector when snapped to a specified grid.
		*/
		Vec2D_F SnappedTo(const Vec2D_F& GridVector) const;

		/*
		The vector perpendicular to the vector.
		*/
		Vec2D_F Tanget() const;

		// Operator Overloading

		bool operator == (const Vec2D_F& OtherVector2D) const;
		bool operator != (const Vec2D_F& OtherVector2D) const;
		bool operator <= (const Vec2D_F& OtherVector2D) const;
		bool operator >= (const Vec2D_F& OtherVector2D) const;
		bool operator <  (const Vec2D_F& OtherVector2D) const;
		bool operator >  (const Vec2D_F& OtherVector2D) const;

		Vec2D_F operator - () const;

		Vec2D_F operator + (const Vec2D_F& OtherVector2D) const;
		Vec2D_F operator - (const Vec2D_F& OtherVector2D) const;
		Vec2D_F operator * (const Vec2D_F& OtherVector2D) const;
		Vec2D_F operator / (const Vec2D_F& OtherVector2D) const;

		void operator += (const Vec2D_F& OtherVector2D) const;
		void operator -= (const Vec2D_F& OtherVector2D) const;
		void operator *= (const Vec2D_F& OtherVector2D) const;
		void operator /= (const Vec2D_F& OtherVector2D) const;

		      DecN& operator [] (int index)      ;
		const DecN& operator [] (int index) const;

		// TODO: Figure out own string implementation...
		// operator String() 


		// Components

		union   // First
		{
			DecN X = DecN(0);

			DecN Width;
		};

		union   // Second
		{
			DecN Y = DecN(0);

			DecN Height;
		};
	};

	using Size2D_F  = Vec2D_F;
	using Point2D_F = Vec2D_F;
}