#ifndef _RTECONTENTFILE_
#define _RTECONTENTFILE_

#include "Serializable.h"

struct BITMAP;

namespace RTE {

/// <summary>
/// A representation of a content file that is stored either directly on disk or packaged in another file.
/// </summary>
class ContentFile : public Serializable {

public:

	/// <summary>
	/// Constructor method used to instantiate a ContentFile object in system memory. Create() should be called before using the object.
	/// </summary>
    ContentFile() { Clear(); }

	/// <summary>
	/// Constructor method used to instantiate a ContentFile object in system memory, and also do a Create() in the same line. Create() should therefore not be called after using this constructor.
	/// </summary>
	/// <param name="filePath">A string defining the path to where the content file itself is located, either within the package file, or directly on the disk.</param>
    ContentFile(const char *filePath) { Clear(); Create(filePath); }

	/// <summary>
	/// Destructor method used to clean up a ContentFile object before deletion from system memory.
	/// </summary>
    ~ContentFile() { Destroy(); }

//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Create
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Makes the ContentFile object ready for use.
// Arguments:       A string defining the path to where the content file itself is located,
//                  either within the package file, or directly on the disk.
// Return value:    An error return value signaling success or any particular failure.
//                  Anything below 0 is an error signal.

    int Create(const char *filePath);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Create
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Creates a ContentFile to be identical to another, by deep copy.
// Arguments:       A reference to the ContentFile to deep copy.
// Return value:    An error return value signaling success or any particular failure.
//                  Anything below 0 is an error signal.

    int Create(const ContentFile &reference);


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Destroy
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Destroys and resets (through Clear()) the ContentFile object.
// Arguments:       Whether to only destroy the members defined in this derived class, or
//                  to destroy all inherited members also.
// Return value:    None.

    void Destroy() { Clear(); }


//////////////////////////////////////////////////////////////////////////////////////////
// Static method:   FreeAllLoaded
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Frees all loaded data used by all ContentFile instances. This should
//                  ONLY be done when quitting the app, or after everything else is
//                  completely destroyed
// Arguments:       None.
// Return value:    None.

    static void FreeAllLoaded();

//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  GetClassName
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the class name of this Entity.
// Arguments:       None.
// Return value:    A string with the friendly-formatted type name of this object.

    const std::string & GetClassName() const { return m_ClassName; }


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  SetDataPath
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the file path of the content file represented by this ContentFile
//                  object.
// Arguments:       A string with the datafile object name path
// Return value:    None.

    void SetDataPath(std::string newDataPath) { m_DataPath = newDataPath; }


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  GetDataPath
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the file path of the content file represented by this ContentFile
//                  object.
// Arguments:       None.
// Return value:    A string with the datafile object name path

    const std::string & GetDataPath() const { return m_DataPath; };


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  GetAsBitmap
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Loads and gets the data represented by this ConentFile object as an
//                  Allegro BITMAP. Note that ownership of the BITMAP IS NOT TRANSFERRED!
//                  Also, this should only be done once upon startup, since getting the
//                  BITMAP again is slow. 
// Arguments:       The Allegro color conversion mode to use when loading this bitmap.
//                  Note it will only apply the first time you get a bitmap since it is
//                  only loaded from disk the first time. See allegro docs for the modes.
// Return value:    The pointer to the beginning of the data object loaded from the allegro
//                  .dat datafile. file.
//                  Ownership is NOT transferred! If 0, the file could not be found/loaded.

    BITMAP * GetAsBitmap(int conversionMode = 0);



//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  LoadAndReleaseBitmap
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Loads and transfers the data represented by this ConentFile object as
//                  an Allegro BITMAP. Note that ownership of the BITMAP IS TRANSFERRED!
//                  Also, this is relatively slow since it reads the data from disk each time.
// Arguments:       The Allegro color conversion mode to use when loading this bitmap.
//                  Note it will only apply the first time you get a bitmap since it is
//                  only loaded from disk the first time. See allegro docs for the modes.
// Return value:    The pointer to the BITMAP loaded from disk Ownership IS transferred!
//                  If 0, the file could not be found/loaded.

    BITMAP * LoadAndReleaseBitmap(int conversionMode = 0);


protected:

    static const std::string m_ClassName;
    static std::unordered_map<std::string, BITMAP *> m_LoadedBitmaps; // Static map containing all the already loaded BITMAP:s and their paths
    std::string m_DataPath; // Path to this ContentFile's Datafile Object's path.

private:

	/// <summary>
	/// Clears all the member variables of this ContentFile, effectively resetting the members of this abstraction level only.
	/// </summary>
    void Clear() { m_DataPath.erase(); }
};
}
#endif