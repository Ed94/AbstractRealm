/*
Vector 2D

Last Modified: 5/18/2020
*/



#pragma once



#include "Simulation/Unit.hpp"




namespace Sim::Math
{
	/*
	2-Dimensional Vector

	Provides a encapsulation of a 2D vector, with intended use in simulated objects.
	*/
	class Vector2D
	{
	public:
		// Enums

		enum Axis { X, Y };

		
		// Constructors

		Vector2D                  ();
		Vector2D(FloatN X, FloatN Y);


		// Functions

		Vector2D Abs        () const; 	// Absolute value vector (all component values positive).
		FloatN   Angle      () const;   // Vector's angle in radians.
		FloatN   AspectRatio() const;   // The aspect ratio of the components (X:Y).

		Vector2D SignValue() const;   // 

		// RelationTo

		FloatN AngleToward     (const Vector2D& OtherVector2D) const;
		FloatN AngleTowardPoint(const Vector2D& OtherPoint2D ) const;

		bool ApproxEqual(const Vector2D& OtherVector2D) const;

		FloatN DistanceTo        (const Vector2D& OtherVector2D) const;
		FloatN DistanceTo_Squared(const Vector2D& OtherVector2D) const;

		// Directional (Noramlization)

		bool     IsDirectionalVector() const;   // IsNormalized
		void     Directionalize     ()      ;   // Normalize
		Vector2D Directionalized    () const;   // Normalized

		Vector2D DirectionToward(const Vector2D& OtherVector2D) const;

		// Magnitude

		FloatN Magnitude      ();   // The magnitude (length) of this vector.
		FloatN SquareMagnitude();   // The magnituded (length) of this vector squared.

		// Operations

		Vector2D Ceiling() const;   // All components rounded up.

		FloatN Cross(const Vector2D& OtherVector2D);   // Normal Vector producing product between two vector.
		FloatN Dot  (const Vector2D& OtherVector2D);   // Inner Product.

		Vector2D Floor() const;   // All components rounded down.

		Vector2D Modulus_Positive(const FloatN          Modulus) const;
		Vector2D Modulus_Positive(const Vector2D& VectorModulus) const;
		
		FloatN ProjectOnto(const Vector2D& Vector2D_ToProjectOnto);

		Vector2D PlaneProjection(FloatN NotSure_D, const Vector2D& DirectionPossibly) const;

		Vector2D Round() const;   // The vector with its components rounded to the nearest integer. (Halfway case round away from zero)

		Vector2D ToClamped(FloatN ClampingMagnitude) const;   // Returns the vector with its magnitude clamped to the specified value.

		// Transformations

		/*
		The resulting vector bounced off from a plane.

		The plane is defined via the provided normal.
		*/
		Vector2D Bounce(const Vector2D& NormalOfPlane) const;

		Vector2D Lerp          (const Vector2D& OtherVector2D, FloatN DeltaTime) const;
		Vector2D Lerp_Spherical(const Vector2D& OtherVector2D, FloatN DeltaTime) const;

		Vector2D Lerp_Cubic    
		(
			const Vector2D& OtherVector2D  , 
			const Vector2D& PreA_CheckThis , 
			const Vector2D& PostB_CheckThis, 
			      FloatN    DeltaTime
		) const;

		Vector2D MoveToward(const Vector2D& VectorToMoveToward, const FloatN DeltaTime) const;

		/*
		The vector reflected from a plane.

		The plane is defined via the provided normal.
		*/
		Vector2D Reflect(const Vector2D& NormalOfPlane) const;

		/*
		The vector rotated by the provided angle value in radians.
		*/
		Vector2D Rotated(const FloatN AngleAmountInRadians) const;

		/*
		Normal of the vector along a plane.

		The plane is defined via the provided normal.
		*/
		Vector2D Slide(const Vector2D& NormalOfPlane_CheckThis) const;

		/*
		The vector when snapped to a specified grid.
		*/
		Vector2D SnappedTo(const Vector2D& GridVector) const;

		/*
		The vector perpendicular to the vector.
		*/
		Vector2D Tanget() const;

		// Operator Overloading






		// Components

		union   // First
		{
			FloatN X = FloatN(0);

			FloatN Width;
		};

		union   // Second
		{
			FloatN Y = FloatN(0);

			FloatN Height;
		};
	};
}
