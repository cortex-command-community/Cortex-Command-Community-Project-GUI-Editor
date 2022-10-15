#include "GUISerializable.h"

namespace RTE {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUISerializable::Create(GUIReader &reader, bool checkType, bool doCreate) {
		if (checkType && reader.ReadPropValue() != GetClassName()) {
			reader.ReportError("Wrong type in GUIReader when passed to GUISerializable::Create");
			return false;
		}
		while (reader.NextProperty()) {
			m_FormattedReaderPosition = ("in file " + reader.GetCurrentFilePath() + " on line " + reader.GetCurrentFileLine());

			// We need to check if !propName.empty() because ReadPropName may return "" when it reads an IncludeFile without any properties in case they are all commented out or it's the last line in file.
			// Also ReadModuleProperty may return "" when it skips IncludeFile till the end of file.
			/*
			std::string propName = reader.ReadPropName();
			if (!propName.empty() && !ReadProperty(propName, reader)) {
				// TODO: Could not match property. Log here!
			}
			*/
		}
		return doCreate ? Create() : true;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUISerializable::ReadProperty(GUIReader &reader, const std::string_view &propName) {
		reader.ReadPropValue();
		reader.ReportError("Could not match property");
		return false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIReader & operator>>(GUIReader &reader, GUISerializable &operand) {
		operand.Create(reader);
		return reader;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIReader & operator>>(GUIReader &reader, GUISerializable *operand) {
		if (operand) { operand->Create(reader); }
		return reader;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIWriter & operator<<(GUIWriter &writer, const GUISerializable &operand) {
		operand.Save(writer);
		writer.ObjectEnd();
		return writer;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIWriter & operator<<(GUIWriter &writer, const GUISerializable *operand) {
		if (operand) {
			operand->Save(writer);
			writer.ObjectEnd();
		} else {
			writer.NoObject();
		}
		return writer;
	}
}