/*
 * FilePath.cpp
 *
 *  Created on: 9 Jan 2018
 *      Author: zal
 */

#include "FilePath.h"

#include <algorithm>

#include "StringUtils.h"
#include "Exception.h"

#if defined (__WIN32__)
#	define OD_FILEPATH_SEPERATOR	    '\\'
#	define OD_FILEPATH_SEPERATOR_STR    "\\"
#	define OD_FILEPATH_HOST_STYLE		STYLE_DOS
#else
#	define OD_FILEPATH_SEPERATOR	    '/'
#	define OD_FILEPATH_SEPERATOR_STR    "/"
#	define OD_FILEPATH_HOST_STYLE		STYLE_POSIX
#endif

namespace od
{

	FilePath::FilePath(const std::string &path)
	: mOriginalPath(path)
	, mRootStyle(STYLE_RELATIVE)
	{
	    _parsePath(path);
	}

	FilePath::FilePath(const std::string &path, FilePath relativeTo)
	: mOriginalPath(path)
	, mRootStyle(STYLE_RELATIVE)
	{
	    _parsePath(relativeTo.str() + OD_FILEPATH_SEPERATOR + path);
	}

	FilePath::FilePath(const FilePath &p, size_t omitLastNComponents)
	: mOriginalPath(p.mOriginalPath)
	, mRoot(p.mRoot)
	, mRootStyle(p.mRootStyle)
	, mPathComponents(p.mPathComponents.begin(), p.mPathComponents.begin() + (p.mPathComponents.size() - omitLastNComponents))
	{
	}

	FilePath FilePath::dir() const
	{
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
			mRootStyle = STYLE_POSIX;
			startSearchAt = 1;

		}else if(workPath.size() >= 2 && workPath[1] == ':')
		{
			// DOS root
			mRoot = workPath.substr(0,1) + ":\\";
			mRootStyle = STYLE_DOS;
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
		std::string hostPath;

		if(mRootStyle != STYLE_RELATIVE)
		{
			if(mRootStyle != OD_FILEPATH_HOST_STYLE)
			{
				throw Exception("Can't convert absolute path roots right now");
			}

			hostPath = mRoot;

		}else if(mPathComponents.empty())
		{
			return ".";
		}

		for(size_t i = 0; i < mPathComponents.size(); ++i)
		{
			hostPath.append(mPathComponents[i]);

			if(i+1 < mPathComponents.size()) // if this is not the last item
			{
				hostPath.append(OD_FILEPATH_SEPERATOR_STR);
			}
		}

		return hostPath;
	}
}





