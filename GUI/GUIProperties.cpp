#include "GUI.h"
#include "GUIWriter.h"

namespace RTE {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIProperties::ClearProperties() {
		for (const PropEntry *property : m_PropertyEntries) {
			delete property;
		}
		m_PropertyEntries.clear();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIProperties::AddProperty(const std::string &propName, const std::string &propValue) {
		if (GetPropertyValue(propName, &std::string())) {
			SetPropertyValue(propName, propValue);
		} else {
			m_PropertyEntries.emplace_back(new PropEntry(propName, propValue));
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIProperties::GetPropertyValue(const std::string_view &propName, std::string *propValue) const {
		GUIAssert(propValue, "");

		for (const PropEntry *property : m_PropertyEntries) {
			if (property->Name == propName) {
				*propValue = property->Value;
				return true;
			}
		}
		return false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIProperties::GetPropertyValue(const std::string &propName, int *propValue) const {
		GUIAssert(propValue, "");

		std::string returnValue;
		if (!GetPropertyValue(propName, &returnValue)) {
			return false;
		}
		*propValue = std::stoi(returnValue);
		return true;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIProperties::GetPropertyValue(const std::string &propName, bool *propValue) const {
		GUIAssert(propValue, "");

		std::string returnValue;
		if (!GetPropertyValue(propName, &returnValue)) {
			return false;
		}
		*propValue = (returnValue == "True") ? true : false;

		return true;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int GUIProperties::GetPropertyValue(const std::string &propName, std::string *propValueArray, int arraySize) {
		GUIAssert(propValueArray, "");

		std::string returnValue;
		if (!GetPropertyValue(propName, &returnValue)) {
			return 0;
		}

		// Create a c string version of the value for tokenizing
		char *str = new char[returnValue.length() + 1];
		if (!str) {
			return 0;
		}
		// Tokenize the string
		strcpy(str, returnValue.c_str());
		char *tok = strtok(str, ",");
		int count = 0;

		while (tok && count < arraySize) {
			propValueArray[count++] = GUIUtil::TrimString(tok);
			tok = strtok(nullptr, ",");
		}
		delete[] str;

		return count;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int GUIProperties::GetPropertyValue(const std::string &propName, int *propValueArray, int arraySize) {
		GUIAssert(propValueArray, "");

		std::string returnValue;
		if (!GetPropertyValue(propName, &returnValue)) {
			return 0;
		}
		// Create a c string version of the value for tokenizing
		char *str = new char[returnValue.length() + 1];
		if (!str) {
			return 0;
		}
		// Tokenize the string
		strcpy(str, returnValue.c_str());
		char *tok = strtok(str, ",");
		int count = 0;

		while (tok && count < arraySize) {
			propValueArray[count++] = std::stoi(tok);
			tok = strtok(nullptr, ",");
		}
		delete[] str;

		return count;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIProperties::SetPropertyValue(const std::string_view &propName, const std::string_view &propValue) const {
		for (PropEntry *property : m_PropertyEntries) {
			if (property->Name == propName) {
				property->Value = propValue;
				return true;
			}
		}
		return false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIProperties::OverwriteProperties(const GUIProperties *reference, bool appendUndefinedProps) {
		GUIAssert(reference, "");

		for (const PropEntry *property : reference->m_PropertyEntries) {
			if (!SetPropertyValue(property->Name, property->Value) && appendUndefinedProps) { AddProperty(property->Name, property->Value); }
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIProperties::Save(GUIWriter &writer) {
		for (const PropEntry *property : m_PropertyEntries) {
			if (!property->Value.empty()) { writer.NewPropertyWithValue(property->Name, property->Value); }
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIProperties::GetProperty(int index, std::string *propName, std::string *propValue) {
		if (index < 0 || index >= m_PropertyEntries.size()) {
			return false;
		}
		const PropEntry *property = m_PropertyEntries.at(index);
		if (propName) { *propName = property->Name; }
		if (propValue) { *propValue = property->Value; }

		return true;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIProperties::SetProperty(int index, const std::string_view &propName, const std::string_view &propValue) {
		if (index < 0 || index >= m_PropertyEntries.size()) {
			return false;
		}
		PropEntry *property = m_PropertyEntries.at(index);
		property->Name = propName;
		property->Value = propValue;

		return true;
	}
}