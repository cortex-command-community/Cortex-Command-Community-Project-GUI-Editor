#ifndef _RTEVECTOR_
#define _RTEVECTOR_

#include "Serializable.h"

namespace RTE {

	/// <summary>
	/// A useful 2D float vector.
	/// </summary>
	class Vector : public Serializable {

	public:

		float m_X; //!< X value of this vector.
		float m_Y; //!< Y value of this vector.

#pragma region Creation
		/// <summary>
		/// Constructor method used to instantiate a Vector object.
		/// </summary>
		Vector() { Clear(); }

		/// <summary>
		/// Constructor method used to instantiate a Vector object from X and Y values.
		/// </summary>
		/// <param name="inputX">Float defining the initial X value of this Vector.</param>
		/// <param name="inputY">Float defining the initial Y value of this Vector.</param>
		Vector(float inputX, float inputY) { Create(inputX, inputY); }

		/// <summary>
		/// Copy constructor method used to instantiate a Vector object identical to an already existing one.
		/// </summary>
		/// <param name="reference">A Vector object which is passed in by reference.</param>
		Vector(const Vector &reference) { Create(reference.m_X, reference.m_Y); }

		/// <summary>
		/// Makes the Vector object ready for use.
		/// </summary>
		/// <param name="inputX">Float defining the initial X value of this Vector.</param>
		/// <param name="inputY">Float defining the initial Y value of this Vector.</param>
		/// <returns>An error return value signaling success or any particular failure. Anything below 0 is an error signal.</returns>
		int Create(float inputX, float inputY) { m_X = inputX; m_Y = inputY; return 0; }
#pragma endregion

#pragma region Destruction
		/// <summary>
		/// Sets both the X and Y of this Vector to zero.
		/// </summary>
		void Reset() { m_X = m_Y = 0.0F; }
#pragma endregion

#pragma region INI Handling
		/// <summary>
		/// Reads a property value from a Reader stream.
		/// If the name isn't recognized by this class, then ReadProperty of the parent class is called.
		/// If the property isn't recognized by any of the base classes, false is returned, and the Reader's position is untouched.
		/// </summary>
		/// <param name="propName">The name of the property to be read.</param>
		/// <param name="reader">A Reader lined up to the value of the property to be read.</param>
		/// <returns>
		/// An error return value signaling whether the property was successfully read or not.
		/// 0 means it was read successfully, and any nonzero indicates that a property of that name could not be found in this or base classes.
		/// </returns>
		int ReadProperty(std::string propName, Reader &reader) override {
			if (propName == "X") {
				reader >> m_X;
			} else if (propName == "Y") {
				reader >> m_Y;
			} else {
				return Serializable::ReadProperty(propName, reader);
			}
			return 0;
		}

		/// <summary>
		/// Saves the complete state of this Serializable to an output stream for later recreation with Create(istream &stream).
		/// </summary>
		/// <param name="writer">A Writer that the Serializable will save itself to.</param>
		/// <returns>An error return value signaling success or any particular failure. Anything below 0 is an error signal.</returns>
		int Save(Writer &writer) const override {
			Serializable::Save(writer);
			writer.NewProperty("X");
			writer << m_X;
			writer.NewProperty("Y");
			writer << m_Y;
			return 0;
		}
#pragma endregion

#pragma region Getters and Setters
		/// <summary>
		/// Sets both the X and Y values of this Vector.
		/// </summary>
		/// <param name="newX">A float value that the X value will be set to.</param>
		/// <param name="newY">A float value that the Y value will be set to.</param>
		void SetXY(const float newX, const float newY) { m_X = newX; m_Y = newY; }
#pragma endregion

#pragma region Operator Overloads
		/// <summary>
		/// An assignment operator for setting one Vector equal to another.
		/// </summary>
		/// <param name="rhs">A Vector reference.</param>
		/// <returns>A reference to the changed Vector.</returns>
		Vector & operator=(const Vector &rhs) {
			if (*this != rhs) {
				m_X = rhs.m_X;
				m_Y = rhs.m_Y;
			}
			return *this;
		}

		/// <summary>
		/// Unary negation overload for single Vectors.
		/// </summary>
		/// <returns>The resulting Vector.</returns>
		Vector operator-() { Vector returnVector(-m_X, -m_Y); return returnVector; }

		/// <summary>
		/// An equality operator for testing if any two Vectors are equal.
		/// </summary>
		/// <param name="lhs">A Vector reference as the left hand side operand.</param>
		/// <param name="rhs">A Vector reference as the right hand side operand.</param>
		/// <returns>A boolean indicating whether the two operands are equal or not.</returns>
		friend bool operator==(const Vector &lhs, const Vector &rhs) { return lhs.m_X == rhs.m_X && lhs.m_Y == rhs.m_Y; }

		/// <summary>
		/// An inequality operator for testing if any two Vectors are unequal.
		/// </summary>
		/// <param name="lhs">A Vector reference as the left hand side operand.</param>
		/// <param name="rhs">A Vector reference as the right hand side operand.</param>
		/// <returns>A boolean indicating whether the two operands are unequal or not.</returns>
		friend bool operator!=(const Vector &lhs, const Vector &rhs) { return lhs.m_X != rhs.m_X || lhs.m_Y != rhs.m_Y; }

		/// <summary>
		/// A stream insertion operator for sending a Vector to an output stream.
		/// </summary>
		/// <param name="stream">An ostream reference as the left hand side operand.</param>
		/// <param name="operand">A Vector reference as the right hand side operand.</param>
		/// <returns>An ostream reference for further use in an expression.</returns>
		friend std::ostream & operator<<(std::ostream &stream, const Vector &operand) { stream << "{" << operand.m_X << ", " << operand.m_Y << "}"; return stream; }

		/// <summary>
		/// Addition operator overload for a Vector and a float.
		/// </summary>
		/// <param name="rhs">A float reference as the right hand side operand.</param>
		/// <returns>The resulting Vector.</returns>
		Vector operator+(const float &rhs) const { Vector returnVector(m_X + rhs, m_Y + rhs); return returnVector; }

		/// <summary>
		/// Addition operator overload for Vectors.
		/// </summary>
		/// <param name="lhs">A Vector reference as the left hand side operand.</param>
		/// <param name="rhs">A Vector reference as the right hand side operand.</param>
		/// <returns>The resulting Vector.</returns>
		friend Vector operator+(const Vector &lhs, const Vector &rhs) { Vector returnVector(lhs.m_X + rhs.m_X, lhs.m_Y + rhs.m_Y); return returnVector; }

		/// <summary>
		/// Subtraction operator overload for a Vector and a float.
		/// </summary>
		/// <param name="rhs">A float reference as the right hand side operand.</param>
		/// <returns>The resulting Vector.</returns>
		Vector operator-(const float &rhs) const { Vector returnVector(m_X - rhs, m_Y - rhs); return returnVector; }

		/// <summary>
		/// Subtraction operator overload for Vectors.
		/// </summary>
		/// <param name="lhs">A Vector reference as the left hand side operand.</param>
		/// <param name="rhs">A Vector reference as the right hand side operand.</param>
		/// <returns>The resulting Vector.</returns>
		friend Vector operator-(const Vector &lhs, const Vector &rhs) { Vector returnVector(lhs.m_X - rhs.m_X, lhs.m_Y - rhs.m_Y); return returnVector; }

		/// <summary>
		/// Multiplication operator overload for a Vector and a float.
		/// </summary>
		/// <param name="rhs">A float reference as the right hand side operand.</param>
		/// <returns>The resulting Vector.</returns>
		Vector operator*(const float &rhs) const { Vector returnVector(m_X * rhs, m_Y * rhs); return returnVector; }

		/// <summary>
		/// Multiplication operator overload for Vectors.
		/// </summary>
		/// <param name="lhs">A Vector reference as the left hand side operand.</param>
		/// <param name="rhs">A Vector reference as the right hand side operand.</param>
		/// <returns>The resulting Vector.</returns>
		friend Vector operator*(const Vector &lhs, const Vector &rhs) { Vector returnVector(lhs.m_X * rhs.m_X, lhs.m_Y * rhs.m_Y); return returnVector; }

		/// <summary>
		/// Division operator overload for a Vector and a float.
		/// </summary>
		/// <param name="rhs">A float reference as the right hand side operand.</param>
		/// <returns>The resulting Vector.</returns>
		Vector operator/(const float &rhs) const {
			Vector returnVector(0, 0);
			if (rhs) { returnVector.SetXY(m_X / rhs, m_Y / rhs); }
			return returnVector;
		}

		/// <summary>
		/// Division operator overload for Vectors.
		/// </summary>
		/// <param name="lhs">A Vector reference as the left hand side operand.</param>
		/// <param name="rhs">A Vector reference as the right hand side operand.</param>
		/// <returns>The resulting Vector.</returns>
		friend Vector operator/(const Vector &lhs, const Vector &rhs) {
			Vector returnVector(0, 0);
			if (rhs.m_X && rhs.m_Y) { returnVector.SetXY(lhs.m_X / rhs.m_X, lhs.m_Y / rhs.m_Y); }
			return returnVector;
		}

		/// <summary>
		/// Self-addition operator overload for a Vector and a float.
		/// </summary>
		/// <param name="rhs">A float reference as the right hand side operand.</param>
		/// <returns>A reference to the resulting Vector.</returns>
		Vector & operator+=(const float &rhs) { m_X += rhs; m_Y += rhs; return *this; }

		/// <summary>
		/// Self-addition operator overload for Vectors.
		/// </summary>
		/// <param name="lhs">A Vector reference as the left hand side operand.</param>
		/// <param name="rhs">A Vector reference as the right hand side operand.</param>
		/// <returns>A reference to the resulting Vector (the left one).</returns>
		friend Vector & operator+=(Vector &lhs, const Vector &rhs) { lhs.m_X += rhs.m_X; lhs.m_Y += rhs.m_Y; return lhs; }

		/// <summary>
		/// Self-subtraction operator overload for a Vector and a float.
		/// </summary>
		/// <param name="rhs">A float reference as the right hand side operand.</param>
		/// <returns>A reference to the resulting Vector.</returns>
		Vector & operator-=(const float &rhs) { m_X -= rhs; m_Y -= rhs; return *this; }

		/// <summary>
		/// Self-subtraction operator overload for Vectors.
		/// </summary>
		/// <param name="lhs">A Vector reference as the left hand side operand.</param>
		/// <param name="rhs">A Vector reference as the right hand side operand.</param>
		/// <returns>A reference to the resulting Vector (the left one).</returns>
		friend Vector & operator-=(Vector &lhs, const Vector &rhs) { lhs.m_X -= rhs.m_X; lhs.m_Y -= rhs.m_Y; return lhs; }

		/// <summary>
		/// Self-multiplication operator overload for a Vector and a float.
		/// </summary>
		/// <param name="rhs">A float reference as the right hand side operand.</param>
		/// <returns>A reference to the resulting Vector.</returns>
		Vector & operator*=(const float &rhs) { m_X *= rhs; m_Y *= rhs; return *this; }

		/// <summary>
		/// Self-multiplication operator overload for Vectors.
		/// </summary>
		/// <param name="lhs">A Vector reference as the left hand side operand.</param>
		/// <param name="rhs">A Vector reference as the right hand side operand.</param>
		/// <returns>A reference to the resulting Vector (the left one).</returns>
		friend Vector & operator*=(Vector &lhs, const Vector &rhs) { lhs.m_X *= rhs.m_X; lhs.m_Y *= rhs.m_Y; return lhs; }

		/// <summary>
		/// self-division operator overload for a Vector and a float.
		/// </summary>
		/// <param name="rhs">A float reference as the right hand side operand.</param>
		/// <returns>A reference to the resulting Vector.</returns>
		Vector & operator/=(const float &rhs) { if (rhs) { m_X /= rhs; m_Y /= rhs; } return *this; }

		/// <summary>
		/// Self-division operator overload for Vectors.
		/// </summary>
		/// <param name="lhs">A Vector reference as the left hand side operand.</param>
		/// <param name="rhs">A Vector reference as the right hand side operand.</param>
		/// <returns>A reference to the resulting Vector (the left one).</returns>
		friend Vector & operator/=(Vector &lhs, const Vector &rhs) {
			if (rhs.m_X) { lhs.m_X /= rhs.m_X; }
			if (rhs.m_Y) { lhs.m_Y /= rhs.m_Y; }
			return lhs;
		}

		/// <summary>
		/// Array subscripting to access either the X or Y element of this Vector.
		/// </summary>
		/// <param name="rhs">An int index indicating which element is requested (X = 0, Y = 1).</param>
		/// <returns>The requested element.</returns>
		const float & operator[](const int &rhs) const { return (rhs == 0) ? m_X : m_Y; }

		/// <summary>
		/// Array subscripting to access either the X or Y element of this Vector.
		/// </summary>
		/// <param name="rhs">An int index indicating which element is requested (X = 0, Y = 1).</param>
		/// <returns>The requested element.</returns>
		float & operator[](const int &rhs) { return (rhs == 0) ? m_X : m_Y; }
#pragma endregion

#pragma region Class Info
		/// <summary>
		/// Gets the class name of this Vector.
		/// </summary>
		/// <returns>A string with the friendly-formatted type name of this Vector.</returns>
		const std::string & GetClassName() const override { return c_ClassName; }
#pragma endregion

	protected:

		static const std::string c_ClassName; //!< A string with the friendly-formatted type name of this.

	private:

		/// <summary>
		/// Clears all the member variables of this Vector, effectively resetting the members of this abstraction level only.
		/// </summary>
		void Clear() { m_X = m_Y = 0; }
	};

	const std::string Vector::c_ClassName = "Vector";
}
#endif
