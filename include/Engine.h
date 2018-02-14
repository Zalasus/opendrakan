/*
 * Engine.h
 *
 *  Created on: 14 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_ENGINE_H_
#define INCLUDE_ENGINE_H_

#include <memory>
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
		inline DbManager &getDbManager() { return mDbManager; }
		inline void setInitialLevelFile(const FilePath &level) { mInitialLevelFile = level; }

		void run();


	private:

		DbManager mDbManager;
		FilePath mInitialLevelFile;
		std::unique_ptr<Level> mLevel;
		osg::ref_ptr<osgViewer::Viewer> mViewer;
	};

}

#endif /* INCLUDE_ENGINE_H_ */
