#pragma once
#include <bench/core/string.hpp>
#include <bench/core/arraylist.hpp>
#include <bench/core/file.hpp>

namespace bench {

/// Loader, parser, and writer for simple key–value INI files.
/// 
/// The format is extremely simple:
/// ```
/// key = value
/// another_key = 123
/// flag = true
/// ```
///
/// Keys are case-sensitive.  
/// The file is stored internally as a list of entries.  
/// Dirty writes are tracked so that `Save()` only writes when needed.
class IniFile {
	class Entry {
	public:
		HeapString key;
		HeapString value;
	};

	/// List of key-value pairs.
	ArrayList<Entry> m_entries = {};

	/// File handle for the .ini file.
	File m_file = {};
	
	/// If true, m_file will remain Open to lock the .ini file for exclusive access.
	bool m_file_locked = {};

	/// Path to the .ini file
	HeapString m_path = {};

	/// Are there changes that aren't saved?
	bool m_dirty = false;

public:

	/// Load an INI file from disk.
	/// 
	/// If `lock_file` is true, the file will be opened with an exclusive lock
	/// and stay open until `Destroy()` or object destruction.
	///
	/// @param path Path to the INI file.
	/// @param lock_file Whether to lock the file for exclusive access.
	/// @param create_disposition Controls whether the file must exist, may be created, etc.
	/// @return True on success, false if loading or opening failed.
	bool Load(String path, bool lock_file, FileCreateDisposition create_disposition);

	/// Save the current entries back to disk.
	/// Only writes if something has changed (`m_dirty == true`).
	void Save();

	/// Release memory, unlock file, and clear entries.
	void Destroy();

	/// Check whether the INI file contains a key.
	bool Contains(String key);

	/// Retrieve a string value for a key, or return fallback if it does not exist.
	String GetString(String key, String fallback = {});

	/// Set or replace a string value for a key.
	void SetString(String key, String value);

	/// Retrieve an integer value for a key, or return fallback if it does not exist or is not a valid integer.
	I32 GetInt(String key, I32 fallback = 0);

	/// Set or replace an integer value.
	void SetInt(String key, I32 value);

	/// Retrieve a boolean value, or fallback if it does not exist or is not a valid boolean.
	bool GetBool(String key, bool fallback = false);

	/// Set a boolean value.
	void SetBool(String key, bool value);
};

} // namespace bench
