#ifndef _GUIPROPERTIES_
#define _GUIPROPERTIES_

namespace RTE {

	class GUIWriter;

	/// <summary>
	/// A class containing properties for controls and skins.
	/// </summary>
	class GUIProperties {

	public:

#pragma region Creation
		/// <summary>
		/// Constructor method used to instantiate a GUIProperties object in system memory.
		/// </summary>
		/// <param name="name">Name of section.</param>
		GUIProperties() = default;

		/// <summary>
		/// Constructor method used to instantiate a GUIProperties object in system memory.
		/// </summary>
		/// <param name="name">Name of section.</param>
		explicit GUIProperties(const std::string_view &name) : m_Name(name) {}
#pragma endregion

#pragma region Destruction
		/// <summary>
		/// Clears the properties.
		/// </summary>
		void ClearProperties() { m_PropertyEntries.clear(); }
#pragma endregion

#pragma region Getters and Setters
		/// <summary>
		/// Gets the property name.
		/// </summary>
		/// <returns></returns>
		std::string GetName() const { return m_Name; }

		/// <summary>
		/// 
		/// </summary>
		/// <param name="newName"></param>
		void SetName(const std::string_view &newName) { m_Name = newName; }
#pragma endregion

#pragma region Property Entry Handling
		/// <summary>
		/// Adds a new property entry to the properties.
		/// </summary>
		/// <param name="propName">Property name.</param>
		/// <param name="propValue">Property value.</param>
		void AddProperty(const std::string &propName, const std::string &propValue);

		/// <summary>
		/// Adds a new property entry to the properties.
		/// </summary>
		/// <param name="propName">Property name.</param>
		/// <param name="propValue">Property value.</param>
		void AddProperty(const std::string &propName, const char *propValue) { AddProperty(propName, std::string(propValue)); }

		/// <summary>
		/// Adds a new property entry to the properties.
		/// </summary>
		/// <param name="propName">Property name.</param>
		/// <param name="propValue">Property value.</param>
		void AddProperty(const std::string &propName, int propValue) { AddProperty(propName, std::to_string(propValue)); }

		/// <summary>
		/// Adds a new property entry to the properties.
		/// </summary>
		/// <param name="propName">Property name.</param>
		/// <param name="propValue">Property value.</param>
		void AddProperty(const std::string &propName, bool propValue) { AddProperty(propName, std::to_string(propValue)); }

		/// <summary>
		/// Gets a string value.
		/// </summary>
		/// <param name="propName">Property name.</param>
		/// <param name="propValue">String pointer.</param>
		/// <returns></returns>
		bool GetPropertyValue(const std::string_view &propName, std::string *propValue) const;

		/// <summary>
		/// Gets an integer value.
		/// </summary>
		/// <param name="propName">Property name.</param>
		/// <param name="propValue">Integer pointer.</param>
		/// <returns></returns>
		bool GetPropertyValue(const std::string &propName, int *propValue) const;

		/// <summary>
		/// Gets a boolean value.
		/// </summary>
		/// <param name="propName">Property name.</param>
		/// <param name="propValue">Boolean pointer.</param>
		/// <returns></returns>
		bool GetPropertyValue(const std::string &propName, bool *propValue) const;

		/// <summary>
		/// Gets a string array of values.
		/// </summary>
		/// <param name="propName">Property name.</param>
		/// <param name="propValueArray">String array pointer.</param>
		/// <param name="MaxArraySize">Size of array.</param>
		/// <returns>Number of elements read.</returns>
		int GetPropertyValue(const std::string &propName, std::string *propValueArray, int arraySize);

		/// <summary>
		/// Gets an integer array of values.
		/// </summary>
		/// <param name="propName">Property name.</param>
		/// <param name="propValueArray">Integer array pointer</param>
		/// <param name="MaxArraySize">Size of array.</param>
		/// <returns>Number of elements read.</returns>
		int GetPropertyValue(const std::string &propName, int *propValueArray, int arraySize);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="propName"></param>
		/// <returns></returns>
		template <typename Type> Type GetPropertyValue(const std::string &propName) const { Type propValue; GetPropertyValue(propName, &propValue); return propValue; }

		/// <summary>
		/// Changes the value of a property.
		/// </summary>
		/// <param name="propName">Property name.</param>
		/// <param name="newPropValue">New property value.</param>
		/// <returns>True if the property was set. Otherwise false.</returns>
		bool SetPropertyValue(const std::string_view &propName, const std::string_view &newPropValue);

		/// <summary>
		/// Changes the value of a property.
		/// </summary>
		/// <param name="propName">Property name.</param>
		/// <param name="newPropValue">New property value.</param>
		/// <returns>True if the property was set. Otherwise false.</returns>
		bool SetPropertyValue(const std::string_view &propName, int newPropValue) { return SetPropertyValue(propName, std::to_string(newPropValue)); }
#pragma endregion

#pragma region Concrete Methods
		/// <summary>
		/// Updates the properties with properties from another instance.
		/// </summary>
		/// <param name="reference">Pointer to a Properties class.</param>
		/// <param name="appendUndefinedProps">Whether to add variables.</param>
		void OverwriteProperties(const GUIProperties *reference, bool appendUndefinedProps = false);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="writer"></param>
		void Save(GUIWriter &writer);
#pragma endregion

#pragma region PropertyPage Control Handling
		/// <summary>
		/// Gets the variable count in the properties.
		/// </summary>
		/// <returns></returns>
		int GetPropertyCount() const { return m_PropertyEntries.size(); }

		/// <summary>
		/// Gets a variable based on index.
		/// </summary>
		/// <param name="index"></param>
		/// <param name="propName"></param>
		/// <param name="propValue"></param>
		/// <returns></returns>
		bool GetProperty(int index, std::string *propName, std::string *propValue);

		/// <summary>
		/// Sets a variable based on index.
		/// </summary>
		/// <param name="index"></param>
		/// <param name="propName"></param>
		/// <param name="propValue"></param>
		/// <returns></returns>
		bool SetProperty(int index, const std::string_view &propName, const std::string_view &propValue);
#pragma endregion

	private:

		/// <summary>
		/// Property structure.
		/// </summary>
		struct GUIPropEntry {
			std::string Name = "";
			std::string Value = "";

			GUIPropEntry(const std::string &name, const std::string &value) : Name(name), Value(value) {}
		};

		std::string m_Name = "";
		std::vector<GUIPropEntry> m_PropertyEntries = {};

		// Disallow the use of some implicit methods.
		GUIProperties(const GUIProperties &reference) = delete;
		GUIProperties & operator=(const GUIProperties &rhs) = delete;
	};
};
#endif