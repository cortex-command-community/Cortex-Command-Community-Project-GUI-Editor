#include "GUIWriter.h"

namespace RTE {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIWriter::Create(const std::string &fileName, bool append) {
		m_FilePath = fileName;

		// Extract filename and folder path
		size_t slashPos = m_FilePath.find_last_of("/\\");
		m_FileName = m_FilePath.substr(slashPos + 1);
		m_FolderPath = m_FilePath.substr(0, slashPos + 1);

		m_Stream = std::make_unique<std::ofstream>(fileName, append ? (std::ios::out | std::ios::app | std::ios::ate) : (std::ios::out | std::ios::trunc));
		return m_Stream->good();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIWriter::NewLine(bool toIndent, int lineCount) const {
		for (int lines = 0; lines < lineCount; ++lines) {
			*m_Stream << "\n";
			if (toIndent) { *m_Stream << std::string(m_IndentCount, '\t'); }
		}
	}
}