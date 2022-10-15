#ifndef _RTEGUISERIALIZEABLE_
#define _RTEGUISERIALIZEABLE_

#include "GUIReader.h"
#include "GUIWriter.h"

namespace RTE {

	/// <summary>
	/// This base class specifies common creation/destruction patterns associated with reading and writing member data from disk.
	/// Is only intended to be inherited from in one level.
	/// </summary>
	class GUISerializable {

	public:

#pragma region Global Macro Definitions
		/// <summary>
		/// Convenience macro to cut down on duplicate ReadProperty and Save methods in classes that extend GUISerializable.
		/// </summary>
		#define GUISerializableOverrideMethods \
			bool ReadProperty(const std::string_view &propName, GUIReader &reader) override; \
			bool Save(GUIWriter &writer) const override;

		/// <summary>
		/// Convenience macro to cut down on duplicate GetClassName methods in classes that extend GUISerializable.
		/// </summary>
		#define GUISerializableClassNameGetter \
			const std::string & GetClassName() const override { return c_ClassName; }
#pragma endregion

#pragma region Creation
		/// <summary>
		/// Constructor method used to instantiate a GUISerializable object in system memory. Create() should be called before using the object.
		/// </summary>
		GUISerializable() = default;

		/// <summary>
		/// Makes the GUISerializable object ready for use, usually after all necessary properties have been set with Create(GUIReader).
		/// </summary>
		/// <returns>An error return value signaling success or any particular failure.</returns>
		virtual bool Create() { return true; }

		/// <summary>
		/// Makes the GUISerializable object ready for use.
		/// </summary>
		/// <param name="reader">A GUIReader that the GUISerializable will create itself from.</param>
		/// <param name="checkType">Whether there is a class name in the stream to check against to make sure the correct type is being read from the stream.</param>
		/// <param name="doCreate">Whether to do any additional initialization of the object after reading in all the properties from the GUIReader. This is done by calling Create.</param>
		/// <returns>An error return value signaling success or any particular failure.</returns>
		virtual bool Create(GUIReader &reader, bool checkType = true, bool doCreate = true);
#pragma endregion

#pragma region Destruction
		/// <summary>
		/// Destructor method used to clean up a GUISerializable object before deletion from system memory.
		/// </summary>
		virtual ~GUISerializable() = default;

		/// <summary>
		/// Resets the entire GUISerializable, including its inherited members, to their default settings or values.
		/// </summary>
		virtual void Reset() {}
#pragma endregion

#pragma region INI Handling
		/// <summary>
		/// Reads a property value from a GUIReader stream.
		/// If the name isn't recognized by this class, then ReadProperty of the parent class is called.
		/// If the property isn't recognized by any of the base classes, false is returned, and the GUIReader's position is untouched.
		/// </summary>
		/// <param name="reader">A GUIReader lined up to the value of the property to be read.</param>
		/// <param name="propName">The name of the property to be read.</param>
		/// <returns>An error return value signaling whether the property was successfully read or not.</returns>
		virtual bool ReadProperty(GUIReader &reader, const std::string_view &propName);

		/// <summary>
		/// Saves the complete state of this GUISerializable to an output stream for later recreation.
		/// </summary>
		/// <param name="writer">A GUIWriter that the GUISerializable will save itself to.</param>
		/// <returns>An error return value signaling success or any particular failure.</returns>
		virtual bool Save(GUIWriter &writer) const { writer.ObjectStart(GetClassName()); return true; }

		/// <summary>
		/// Replaces backslashes with forward slashes in file paths to eliminate issues with cross-platform compatibility or invalid escape sequences.
		/// </summary>
		/// <param name="pathToCorrect">Reference to the file path string to correct slashes in.</param>
		std::string CorrectBackslashesInPath(const std::string &pathToCorrect) const { return std::filesystem::path(pathToCorrect).generic_string(); }
#pragma endregion

#pragma region Logging
		/// <summary>
		/// Gets the file and line that are currently being read. Formatted to be used for logging warnings and errors.
		/// </summary>
		/// <returns>A string containing the currently read file path and the line being read.</returns>
		const std::string & GetFormattedReaderPosition() const { return m_FormattedReaderPosition; }
#pragma endregion

#pragma region Operator Overloads
		/// <summary>
		/// A GUIReader extraction operator for filling a GUISerializable from a GUIReader.
		/// </summary>
		/// <param name="reader">A GUIReader reference as the left hand side operand.</param>
		/// <param name="operand">An GUISerializable reference as the right hand side operand.</param>
		/// <returns>A GUIReader reference for further use in an expression.</returns>
		friend GUIReader & operator>>(GUIReader &reader, GUISerializable &operand);

		/// <summary>
		/// A GUIReader extraction operator for filling an GUISerializable from a GUIReader.
		/// </summary>
		/// <param name="reader">A GUIReader reference as the left hand side operand.</param>
		/// <param name="operand">An GUISerializable pointer as the right hand side operand.</param>
		/// <returns>A GUIReader reference for further use in an expression.</returns>
		friend GUIReader & operator>>(GUIReader &reader, GUISerializable *operand);

		/// <summary>
		/// A GUIWriter insertion operator for sending a GUISerializable to a GUIWriter.
		/// </summary>
		/// <param name="writer">A GUIWriter reference as the left hand side operand.</param>
		/// <param name="operand">A GUISerializable reference as the right hand side operand.</param>
		/// <returns>A GUIWriter reference for further use in an expression.</returns>
		friend GUIWriter & operator<<(GUIWriter &writer, const GUISerializable &operand);

		/// <summary>
		/// A GUIWriter insertion operator for sending a GUISerializable to a GUIWriter.
		/// </summary>
		/// <param name="writer">A GUIWriter reference as the left hand side operand.</param>
		/// <param name="operand">A GUISerializable pointer as the right hand side operand.</param>
		/// <returns>A GUIWriter reference for further use in an expression.</returns>
		friend GUIWriter &operator<<(GUIWriter &writer, const GUISerializable *operand);
#pragma endregion

#pragma region Class Info
		/// <summary>
		/// Gets the class name of this GUISerializable.
		/// </summary>
		/// <returns>A string with the friendly-formatted type name of this GUISerializable.</returns>
		virtual const std::string & GetClassName() const = 0;
#pragma endregion

	private:

		std::string m_FormattedReaderPosition = ""; //!< A string containing the currently read file path and the line being read. Formatted to be used for logging.
	};
}
#endif