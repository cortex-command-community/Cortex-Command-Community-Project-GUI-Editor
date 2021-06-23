#ifndef _RTECONTENTFILE_
#define _RTECONTENTFILE_

#include "Serializable.h"

struct BITMAP;

namespace RTE {

	/// <summary>
	/// A representation of a content file that is stored directly on disk.
	/// </summary>
	class ContentFile : public Serializable {

	public:

		SerializableClassNameGetter
		SerializableOverrideMethods

#pragma region Creation
		/// <summary>
		/// Constructor method used to instantiate a ContentFile object in system memory. Create() should be called before using the object.
		/// </summary>
		ContentFile() { Clear(); }

		/// <summary>
		/// Constructor method used to instantiate a ContentFile object in system memory, and also do a Create() in the same line.
		/// </summary>
		/// <param name="filePath">A string defining the path to where the content file itself is located.</param>
		explicit ContentFile(const char *filePath) { Clear(); Create(filePath); }

		/// <summary>
		/// Makes the ContentFile object ready for use.
		/// </summary>
		/// <param name="filePath">A string defining the path to where the content file itself is located.</param>
		/// <returns>An error return value signaling success or any particular failure. Anything below 0 is an error signal.</returns>
		int Create(const char *filePath);

		/// <summary>
		/// Creates a ContentFile to be identical to another, by deep copy.
		/// </summary>
		/// <param name="reference">A reference to the ContentFile to deep copy.</param>
		/// <returns>An error return value signaling success or any particular failure. Anything below 0 is an error signal.</returns>
		int Create(const ContentFile &reference);
#pragma endregion

#pragma region Destruction
		/// <summary>
		/// Destructor method used to clean up a ContentFile object before deletion from system memory.
		/// </summary>
		~ContentFile() override { Destroy(); }

		/// <summary>
		/// Destroys and resets (through Clear()) the ContentFile object.
		/// </summary>
		void Destroy() { Clear(); }

		/// <summary>
		/// Frees all loaded data used by all ContentFile instances. This should ONLY be done when quitting the app, or after everything else is completely destroyed.
		/// </summary>
		static void FreeAllLoaded();
#pragma endregion

#pragma region Getters and Setters
		/// <summary>
		/// Gets the file path of the content file represented by this ContentFile object.
		/// </summary>
		/// <returns>A string with the file name path.</returns>
		const std::string & GetDataPath() const { return m_DataPath; }

		/// <summary>
		/// Sets the file path of the content file represented by this ContentFile object.
		/// </summary>
		/// <param name="newDataPath">A string with the new file name path.</param>
		void SetDataPath(const std::string &newDataPath);

		/// <summary>
		/// Sets the DataPath combined with the file and line it's being created from. This is used in cases we can't get the file and line from Serializable::Create(&reader).
		/// For example when creating a ContentFile for the sound during the readSound lambda in SoundContainer::ReadAndGetSound.
		/// </summary>
		/// <param name="newPosition">The file and line that are currently being read.</param>
		void SetFormattedReaderPosition(const std::string &newPosition);
#pragma endregion

#pragma region Data Handling
		/// <summary>
		/// Gets the data represented by this ContentFile object as an Allegro BITMAP, loading it into the static maps if it's not already loaded. Note that ownership of the BITMAP is NOT transferred!
		/// </summary>
		/// <param name="conversionMode">The Allegro color conversion mode to use when loading this bitmap.</param>
		/// <param name="storeBitmap">Whether to store the BITMAP in the relevant static map after loading it or not.</param>
		/// <param name="dataPathToSpecificFrame">Path to a specific frame when loading an animation to avoid overwriting the original preset DataPath when loading each frame.</param>
		/// <returns>Pointer to the BITMAP loaded from disk.</returns>
		BITMAP * GetAsBitmap(int conversionMode = 0, bool storeBitmap = true, const std::string &dataPathToSpecificFrame = "");

		/// <summary>
		/// Gets the data represented by this ContentFile object as an array of Allegro BITMAPs, each representing a frame in the animation.
		/// It loads the BITMAPs into the static maps if they're not already loaded. Ownership of the BITMAPs is NOT transferred, but the array itself IS!
		/// </summary>
		/// <param name="frameCount">The number of frames to attempt to load, more than 1 frame will mean 00# is appended to datapath to handle naming conventions.</param>
		/// <param name="conversionMode">The Allegro color conversion mode to use when loading this bitmap.</param>
		/// <returns>Pointer to the beginning of the array of BITMAP pointers loaded from the disk, the length of which is specified with the FrameCount argument.</returns>
		BITMAP ** GetAsAnimation(int frameCount = 1, int conversionMode = 0);
#pragma endregion

	protected:

		/// <summary>
		/// Enumeration for loading BITMAPs by bit depth. NOTE: This can't be lower down because s_LoadedBitmaps relies on this definition.
		/// </summary>
		enum BitDepths { Eight = 0, ThirtyTwo, BitDepthCount };

		static std::array<std::unordered_map<std::string, BITMAP *>, BitDepthCount> s_LoadedBitmaps; //!< Static map containing all the already loaded BITMAPs and their paths for each bit depth.

		std::string m_DataPath; //!< The path to this ContentFile's data file. In the case of an animation, this filename/name will be appended with 000, 001, 002 etc.
		std::string m_DataPathExtension; //!< The extension of the data file of this ContentFile's path.
		std::string m_DataPathWithoutExtension; //!< The path to this ContentFile's data file without the file's extension.

		std::string m_FormattedReaderPosition; //!< A string containing the currently read file path and the line being read. Formatted to be used for logging.
		std::string m_DataPathAndReaderPosition; //!< The path to this ContentFile's data file combined with the ini file and line it is being read from. This is used for logging.

	private:

		static const std::string c_ClassName; //!< A string with the friendly-formatted type name of this object.

#pragma region Data Handling
		/// <summary>
		/// Loads and transfers the data represented by this ContentFile object as an Allegro BITMAP. Ownership of the BITMAP IS transferred!
		/// Note that this is relatively slow since it reads the data from disk each time.
		/// </summary>
		/// <param name="conversionMode">The Allegro color conversion mode to use when loading this bitmap. Only applies the first time a bitmap is loaded from the disk.</param>
		/// <param name="dataPathToSpecificFrame">Path to a specific frame when loading an animation to avoid overwriting the original preset DataPath when loading each frame.</param>
		/// <returns>Pointer to the BITMAP loaded from disk.</returns>
		BITMAP * LoadAndReleaseBitmap(int conversionMode = 0, const std::string &dataPathToSpecificFrame = "");
#pragma endregion

		/// <summary>
		/// Clears all the member variables of this ContentFile, effectively resetting the members of this abstraction level only.
		/// </summary>
		void Clear();
	};
}
#endif