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
#include "InputManager.h"
#include "Level.h"
#include "Camera.h"
#include "Player.h"

namespace od
{

	class Engine
	{
	public:

		Engine();
		Engine(Engine &e) = delete;
		Engine(const Engine &e) = delete;
		~Engine() = default;

		inline FilePath getInitialLevelFile() const { return mInitialLevelFile; }
		inline void setInitialLevelFile(const FilePath &level) { mInitialLevelFile = level; }
		inline DbManager &getDbManager() { return mDbManager; }
		inline Level &getLevel() { return *mLevel; } // FIXME: throw if no level present
		inline Player &getPlayer() { return getLevel().getPlayer(); }

		Camera &getCamera();
		void run();


	private:

		DbManager mDbManager;
		osg::ref_ptr<InputManager> mInputManager;
		FilePath mInitialLevelFile;
		std::unique_ptr<Level> mLevel;
		osg::ref_ptr<osgViewer::Viewer> mViewer;
		std::unique_ptr<Camera> mCamera;
	};

}

#endif /* INCLUDE_ENGINE_H_ */
