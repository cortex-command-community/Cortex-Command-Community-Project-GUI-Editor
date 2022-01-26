#ifndef _GUIPROPERTIES_
#define _GUIPROPERTIES_

namespace RTE {

/// <summary>
/// A class containing properties for controls and skins.
/// </summary>
class GUIProperties {

public:


//////////////////////////////////////////////////////////////////////////////////////////
// Constructor:     GUIProperties
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Constructor method used to instantiate a GUIProperties object in
//                  system memory.
// Arguments:       Name of section.

    explicit GUIProperties(const std::string &Name);


//////////////////////////////////////////////////////////////////////////////////////////
// Constructor:     GUIProperties
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Constructor method used to instantiate a GUIProperties object in
//                  system memory.
// Arguments:       None.

    GUIProperties();


//////////////////////////////////////////////////////////////////////////////////////////
// Destructor:      GUIProperties
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Destructor method used to free a GUIProperties object in system
//                  memory.
// Arguments:       None.

	~GUIProperties() { Clear(); }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Clear
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Clears the properties.
// Arguments:       None.

    void Clear();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          AddVariable
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Adds a new variable to the properties
// Arguments:       Variable, Value

    void AddVariable(const std::string &Variable, const std::string &Value);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          AddVariable
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Adds a new variable to the properties
// Arguments:       Variable, Value

    void AddVariable(const std::string &Variable, char *Value);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          AddVariable
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Adds a new variable to the properties
// Arguments:       Variable, Value

    void AddVariable(const std::string &Variable, int Value);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          AddVariable
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Adds a new variable to the properties
// Arguments:       Variable, Value

    void AddVariable(const std::string &Variable, bool Value);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetValue
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Changes the value of a property.
// Arguments:       Variable, Value
// Returns:         True if the variable was set. Otherwise false.

    bool SetValue(const std::string &Variable, const std::string &Value);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetValue
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Changes the value of a property.
// Arguments:       Variable, Value
// Returns:         True if the variable was set. Otherwise false.

    bool SetValue(const std::string &Variable, int Value);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Update
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Updates the properties with properties from another instance.
// Arguments:       Pointer to a Properties class, whether to add variables.

    void Update(GUIProperties *Props, bool Add = false);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetValue
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets a string value
// Arguments:       Variable, String pointer

    bool GetValue(const std::string &Variable, std::string *Value);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetValue
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets a string array of values
// Arguments:       Variable, String array, max size of array
// Returns:         Number of elements read

    int GetValue(const std::string &Variable, std::string *Array, int MaxArraySize);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetValue
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets an integer array of values
// Arguments:       Variable, Integer array, max size of array
// Returns:         Number of elements read

    int GetValue(const std::string &Variable, int *Array, int MaxArraySize);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetValue
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets a single interger
// Arguments:       Variable, Integer pointer

    bool GetValue(const std::string &Variable, int *Value);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetValue
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets a single unsigned interger
// Arguments:       Variable, Unsigned Integer pointer

    bool GetValue(const std::string &Variable, unsigned long *Value);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetValue
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets a boolean value
// Arguments:       Variable, Boolean pointer

    bool GetValue(const std::string &Variable, bool *Value);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetName
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the property name

    std::string GetName() const { return m_Name; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ToString
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Converts the properties to a string

    std::string ToString();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetCount
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the variable count in the properties

    int GetCount() const { return m_VariableList.size(); }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetVariable
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets a variable based on index

    bool GetVariable(int Index, std::string *Name, std::string *Value);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetVariable
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets a variable based on index

    bool SetVariable(int Index, const std::string &Name, const std::string &Value);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Sort
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sorts the list by variable name
// Arguments:       True for ascending, False for descending

    void Sort(bool Ascending);

private:

    // Variable structure
    typedef struct {
        std::string m_Name;
        std::string m_Value;
    } PropVariable;

    std::string m_Name;

    std::vector<PropVariable *> m_VariableList;
};
};
#endif