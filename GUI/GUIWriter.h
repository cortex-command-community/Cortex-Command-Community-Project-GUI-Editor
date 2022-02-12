#ifndef _RTEGUIWRITER_
#define _RTEGUIWRITER_

namespace RTE {

	/// <summary>
	/// Writes GUI objects to std::ostreams.
	/// </summary>
	class GUIWriter {

	public:

#pragma region Creation
		/// <summary>
		/// Constructor method used to instantiate a GUIWriter object in system memory. Create() should be called before using the object.
		/// </summary>
		GUIWriter() = default;

		/// <summary>
		/// Makes the GUIWriter object ready for use.
		/// </summary>
		/// <param name="filename">Path to the file to open for writing. If the directory doesn't exist the stream will fail to open.</param>
		/// <param name="append">Whether to append to the file if it exists, or to overwrite it.</param>
		/// <returns>An error return value signaling success or any particular failure. Anything below 0 is an error signal.</returns>
		int Create(const std::string &fileName, bool append = false);
#pragma endregion

#pragma region Getters
		/// <summary>
		/// Gets the path to the file being written.
		/// </summary>
		/// <returns>The full path to the file being written.</returns>
		std::string GetFilePath() const { return m_FilePath; }

		/// <summary>
		/// Gets the name (without path) of the file being written.
		/// </summary>
		/// <returns>The name of file being written.</returns>
		std::string GetFileName() const { return m_FileName; }

		/// <summary>
		/// Gets the folder path (without filename) to where the file is being written.
		/// </summary>
		/// <returns>The name of folder being written in.</returns>
		std::string GetFolderPath() const { return m_FolderPath; }
#pragma endregion

#pragma region Writing Operations
		/// <summary>
		/// Used to specify the start of an object to be written.
		/// </summary>
		/// <param name="className">The class name of the object about to be written.</param>
		void ObjectStart(const std::string &className) { *m_Stream << className; ++m_IndentCount; }

		/// <summary>
		/// Used to specify the end of an object that has just been written.
		/// </summary>
		void ObjectEnd() { --m_IndentCount; if (m_IndentCount == 0) { NewLine(false, 2); } }

		/// <summary>
		/// Creates a new line that can be properly indented.
		/// </summary>
		/// <param name="toIndent">Whether to indent the new line or not.</param>
		/// <param name="lineCount">How many new lines to create.</param>
		void NewLine(bool toIndent = true, int lineCount = 1) const;

		/// <summary>
		/// Creates a new line and writes the specified string to it.
		/// </summary>
		/// <param name="textString">The text string to write to the new line.</param>
		/// <param name="toIndent">Whether to indent the new line or not.</param>
		void NewLineString(const std::string &textString, bool toIndent = true) const { NewLine(toIndent); *m_Stream << textString; }

		/// <summary>
		/// Creates a new line and fills it with slashes to create a divider line for INI.
		/// </summary>
		/// <param name="toIndent">Whether to indent the new line or not.</param>
		/// <param name="dividerLength">The length of the divider (number of slashes).</param>
		void NewDivider(bool toIndent = true, int dividerLength = 72) const { NewLine(toIndent); *m_Stream << std::string(dividerLength, '/'); }

		/// <summary>
		/// Creates a new line and writes the name of the property in preparation to writing it's value.
		/// </summary>
		/// <param name="propName">The name of the property to be written.</param>
		void NewProperty(const std::string &propName) const { NewLine(); *m_Stream << propName + " = "; }
#pragma endregion

#pragma region Writer Status
		/// <summary>
		/// Shows whether the writer is ready to start accepting data streamed to it.
		/// </summary>
		/// <returns>Whether the writer is ready to start accepting data streamed to it or not.</returns>
		bool WriterOK() const { return m_Stream.get() && !m_Stream->fail() && m_Stream->is_open(); }

		/// <summary>
		/// Flushes and closes the output stream of this GUIWriter. This happens automatically at destruction but needs to be called manually if a written file must be read from in the same scope.
		/// </summary>
		void EndWrite() const { m_Stream->flush(); m_Stream->close(); }
#pragma endregion

#pragma region Operator Overloads
		/// <summary>
		/// Elemental types stream insertions. Stream insertion operator overloads for all the elemental types.
		/// </summary>
		/// <param name="var">A reference to the variable that will be written to the ostream.</param>
		/// <returns>A GUIWriter reference for further use in an expression.</returns>
		GUIWriter & operator<<(const bool &var) { *m_Stream << var; return *this; }
		GUIWriter & operator<<(const char &var) { *m_Stream << var; return *this; }
		GUIWriter & operator<<(const unsigned char &var) { int temp = var; *m_Stream << temp; return *this; }
		GUIWriter & operator<<(const short &var) { *m_Stream << var; return *this; }
		GUIWriter & operator<<(const unsigned short &var) { *m_Stream << var; return *this; }
		GUIWriter & operator<<(const int &var) { *m_Stream << var; return *this; }
		GUIWriter & operator<<(const unsigned int &var) { *m_Stream << var; return *this; }
		GUIWriter & operator<<(const long &var) { *m_Stream << var; return *this; }
		GUIWriter & operator<<(const long long &var) { *m_Stream << var; return *this; }
		GUIWriter & operator<<(const unsigned long &var) { *m_Stream << var; return *this; }
		GUIWriter & operator<<(const unsigned long long &var) { *m_Stream << var; return *this; }
		GUIWriter & operator<<(const float &var) { *m_Stream << var; return *this; }
		GUIWriter & operator<<(const double &var) { *m_Stream << var; return *this; }
		GUIWriter & operator<<(const char *var) { *m_Stream << var; return *this; }
		GUIWriter & operator<<(const std::string &var) { *m_Stream << var; return *this; }
#pragma endregion

	protected:

		std::unique_ptr<std::ofstream> m_Stream = nullptr; //!< Stream used for writing to files.
		std::string m_FilePath = ""; //!< Currently used stream's filepath.
		std::string m_FolderPath = ""; //!< Only the path to the folder that we are writing a file in, excluding the filename.
		std::string m_FileName = ""; //!< Only the name of the currently read file, excluding the path.
		int m_IndentCount = 0; //!< Indentation counter.

	private:

		// Disallow the use of some implicit methods.
		GUIWriter(const GUIWriter &reference) = delete;
		GUIWriter & operator=(const GUIWriter &rhs) = delete;
	};
}
#endif