/*
 * FilePath.h
 *
 *  Created on: 9 Jan 2018
 *      Author: zal
 */

#ifndef INCLUDE_FILEPATH_H_
#define INCLUDE_FILEPATH_H_

#include <string>
#include <vector>

namespace od
{

	/**
	 * Simple and primitive abstraction for file paths that is able to handle paths with mixed seperators and
	 * can convert paths into the format preferred by the OS.
	 */
	class FilePath
	{
	public:

		enum class PathRootStyle
		{
			DOS,
			POSIX,
			RELATIVE
		};

		/**
		 * Constructs a new FilePath object from the given path. If path is
		 * relative, it is assumed to be relative to the current working directory.
		 */
		FilePath(const std::string &path);

		/**
		 * Constructs a new FilePath object from the given path. If that path is
		 * relative, it is assumed to be relative to relativeTo.
		 */
		FilePath(const std::string &path, FilePath relativeTo);

		FilePath(const FilePath &p, size_t omitLastNComponents = 0);

		/**
		 * If this object represents a path to a file, this method returns a FilePath for the directory
		 * it is contained in. If this object is itself a directory, this method will return a FilePath for it's
		 * parent directory.
		 *
		 * This method can not ascend further in the represented path than the highes level given during construction,
		 * i.e. the current working dir or the filesystem root.
		 */
		FilePath dir() const;

		/**
		 * Returns a string for the path represented by this object compatible with the current platform.
		 *
		 * Note that this can't convert file system roots.
		 */
		std::string str() const;

		/**
		 * Returns the same as str() but with possible extensions removed.
		 */
		std::string strNoExt() const;

		/**
		 * Returns a string containing just the filename without any directories.
		 */
		std::string fileStr() const;

		/**
		 * Returns the same as fileStr() but with possible extensions removed.
		 */
		std::string fileStrNoExt() const;

		/**
		 * Returns the file's extension on it's own, including the extension seperator (.) if present
		 */
		std::string extStr() const;

		/**
		 * @brief Returns true if the represented file exists, false if not.
		 *
		 * This will check for file existence by trying to open that file in read mode. If that fails (for whatever
		 * reason, insufficient permissions included), false will be returned.
		 */
		bool exists() const;

		/**
		 * Creates copy of this path, but changes the file's extension to the passed string
		 * or appends it if no extension is present. The passed string must include the dot.
		 */
		FilePath ext(const std::string &newExt) const;

		/**
		 * Will try to resolve all elements in the path, replacing a directory or filename with whatever
		 * the OS reports should the one stored in this FilePath and the one found in the file system match
		 * when ignoring case.
		 *
		 * If a part of the path can't be accessed (either because it does not exist or because of insufficient
		 * permissions, that part of the path is kept as-is.
		 *
		 * On Windows this returns an exact copy without chaning anything right now.
		 */
		FilePath adjustCase() const;

		bool operator==(const FilePath &right) const;


	private:

		void _parsePath(const std::string &path);
		std::string _buildHostPath() const;

		std::string mOriginalPath;

		std::string mRoot;
		PathRootStyle mRootStyle;
		std::vector<std::string> mPathComponents;
	};

	std::ostream &operator<<(std::ostream &left, const FilePath &right);
}

#endif /* INCLUDE_FILEPATH_H_ */
