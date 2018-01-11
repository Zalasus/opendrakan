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

		bool operator==(const FilePath &right) const;


	private:

		void _parsePath(const std::string &path);

		std::string mGoodPath;
	};

}

#endif /* INCLUDE_FILEPATH_H_ */
