/*
 * FilePath.cpp
 *
 *  Created on: 9 Jan 2018
 *      Author: zal
 */

#include <odCore/FilePath.h>

#include <algorithm>
#include <sstream>
#include <fstream>

#include <odCore/StringUtils.h>
#include <odCore/Exception.h>

#if defined (__WIN32__)
#	define OD_FILEPATH_SEPERATOR	    '\\'
#	define OD_FILEPATH_HOST_STYLE		PathRootStyle::DOS
#else
#	define OD_FILEPATH_SEPERATOR	    '/'
#	define OD_FILEPATH_HOST_STYLE		PathRootStyle::POSIX
extern "C"
{
#	include <dirent.h>
}
#endif

namespace od
{

	FilePath::FilePath(const std::string &path)
	: mOriginalPath(path)
	, mRootStyle(PathRootStyle::RELATIVE)
	, mAlreadyBuiltPath(false)
	{
	    _parsePath(path);
	}

	FilePath::FilePath(const std::string &path, const FilePath &relativeTo)
	: mOriginalPath(path)
	, mRootStyle(PathRootStyle::RELATIVE)
	, mAlreadyBuiltPath(false)
	{
	    _parsePath(relativeTo.str() + OD_FILEPATH_SEPERATOR + path);
	}

	FilePath::FilePath(const FilePath &p, size_t omitLastNComponents)
	: mOriginalPath(p.mOriginalPath)
	, mRoot(p.mRoot)
	, mRootStyle(p.mRootStyle)
	, mPathComponents(p.mPathComponents.begin(), p.mPathComponents.begin() + (p.mPathComponents.size() - omitLastNComponents))
	, mAlreadyBuiltPath(false)
	{
	}

	FilePath FilePath::dir() const
	{
	    if(mPathComponents.size() == 0)
	    {
	        throw Exception("Tried to ascend above path root with FilePath::dir()");
	    }

		return FilePath(*this, 1);
	}

	std::string FilePath::str() const
	{
	    return _buildHostPath();
	}

	std::string FilePath::strNoExt() const
	{
		return FilePath(fileStrNoExt(), dir()).str();
	}

	std::string FilePath::fileStr() const
	{
		if(mPathComponents.size() == 0)
		{
			throw Exception("Tried to access file part of empty path");
		}

		return mPathComponents[mPathComponents.size() - 1];
	}

	std::string FilePath::fileStrNoExt() const
	{
		std::string file = fileStr();
		size_t lastDot = file.find_last_of(".");
		return file.substr(0, lastDot);
	}

	std::string FilePath::extStr() const
	{
	    std::string file = fileStr();
        size_t lastDot = file.find_last_of(".");
        return file.substr(lastDot);
	}

	FilePath FilePath::ext(const std::string &newExt) const
	{
	    std::string path = strNoExt();
	    return FilePath(path + newExt);
	}

	FilePath FilePath::adjustCase() const
	{
#if defined (__WIN32__)
		// in a windows environment, paths are case-insensitive, so we don't have to adjust anything.
		//  this is great, because i don't know whether all windows libcs have a dirent.h and i can't be bothered
		//  to look up the right way of listing directories on windows right now.
		return *this;
#else

		FilePath fp(*this);

		std::string adjustedPath((mRootStyle == PathRootStyle::RELATIVE) ? "./" : mRoot);

		for(size_t i = 0; i < fp.mPathComponents.size(); ++i)
		{
		    if(i == 1)
		    {
		        adjustedPath += fp.mPathComponents[i-1];

		    }else if(i > 1)
			{
				adjustedPath += OD_FILEPATH_SEPERATOR + fp.mPathComponents[i-1];
			}

			DIR *dir = opendir(adjustedPath.c_str());
			if(dir == NULL)
			{
				// could not access path. keep this component as-is
				continue;
			}

			std::string &name = fp.mPathComponents[i];

			dirent *entry = readdir(dir);
			while(entry != NULL)
			{
				std::string realName(entry->d_name);

				if(StringUtils::compareIgnoringCase(realName, name))
				{
					fp.mPathComponents[i] = realName;
					break;
				}

				entry = readdir(dir);
			}

			closedir(dir);
		}

		return fp;
#endif
	}

	bool FilePath::exists() const
	{
		// the probably easiest way is to go try and open the file
		std::ifstream in(this->str(), std::ios::in);
		return !in.fail();
	}

	bool FilePath::operator==(const FilePath &right) const
    {
		if(mRoot != right.mRoot)
		{
			return false;
		}

		return std::equal(mPathComponents.begin(), mPathComponents.end(), right.mPathComponents.begin(), right.mPathComponents.end());
    }

	void FilePath::_parsePath(const std::string &path)
	{
		std::string workPath = StringUtils::trim(path);

		if(workPath.empty())
		{
			return;
		}

		size_t startSearchAt = 0;

		if(workPath.size() >= 1 && workPath[0] == '/')
		{
			// POSIX root
			mRoot = "/";
			mRootStyle = PathRootStyle::POSIX;
			startSearchAt = 1;

		}else if(workPath.size() >= 2 && workPath[1] == ':')
		{
			// DOS root
			mRoot = workPath.substr(0,1) + ":\\";
			mRootStyle = PathRootStyle::DOS;
			startSearchAt = 3;
		}

		while(startSearchAt < workPath.size())
		{
			size_t nextSlash = workPath.find_first_of("\\/", startSearchAt);

			size_t componentSize;
			if(nextSlash != std::string::npos)
			{
				componentSize = nextSlash - startSearchAt;

			}else
			{
				componentSize = std::string::npos;
			}


			std::string pathComponent = workPath.substr(startSearchAt, componentSize);

			if(pathComponent == "..")
			{
				if(mPathComponents.empty())
				{
					throw Exception("Invalid path: '..' used beyond path root.");
				}

				mPathComponents.pop_back();

			}else if(!pathComponent.empty() && pathComponent != ".")
			{
				mPathComponents.push_back(pathComponent);
			}


			if(nextSlash != std::string::npos)
			{
				startSearchAt = nextSlash + 1;

			}else
			{
				startSearchAt = std::string::npos;
			}
		}
	}

	std::string FilePath::_buildHostPath() const
	{
		if(mAlreadyBuiltPath)
		{
			return mBuiltPathCache;
		}

		std::ostringstream oss;

		if(mRootStyle != PathRootStyle::RELATIVE)
		{
			if(mRootStyle != OD_FILEPATH_HOST_STYLE)
			{
				throw UnsupportedException("Can't convert absolute path roots right now");
			}

			oss << mRoot;

		}else if(mPathComponents.empty())
		{
			return ".";
		}

		for(size_t i = 0; i < mPathComponents.size(); ++i)
		{
			oss << mPathComponents[i];

			if(i+1 < mPathComponents.size()) // if this is not the last item
			{
				oss << OD_FILEPATH_SEPERATOR;
			}
		}

		mBuiltPathCache = oss.str();
		mAlreadyBuiltPath = true;

		return oss.str();
	}


	std::ostream &operator<<(std::ostream &left, const FilePath &right)
	{
		left << right.str();

		return left;
	}
}





