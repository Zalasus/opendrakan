/*
 * Engine.h
 *
 *  Created on: 14 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_ENGINE_H_
#define INCLUDE_ENGINE_H_

#include <osgViewer/Viewer>

#include "DbManager.h"
#include "Level.h"

namespace od
{

	class Engine
	{
	public:

		Engine();

		inline FilePath getInitialLevelFile() const { return mInitialLevelFile; }
		inline void setInitialLevelFile(const FilePath &level) { mInitialLevelFile = level; }

		void run();


	private:

		DbManager mDbManager;
		FilePath mInitialLevelFile;
		osg::ref_ptr<Level> mLevel;
		osg::ref_ptr<osgViewer::Viewer> mViewer;
	};

}

#endif /* INCLUDE_ENGINE_H_ */
