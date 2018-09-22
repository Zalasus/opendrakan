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
#include <osgViewer/ViewerEventHandlers>

#include <odCore/FilePath.h>

namespace od
{

    class Player;
    class Camera;
    class Level;
    class GuiManager;
    class RflManager;
    class InputManager;
    class ShaderManager;
    class LightManager;
    class DbManager;

	class Engine
	{
	public:

	    Engine();
		Engine(Engine &e) = delete;
		Engine(const Engine &e) = delete;
		~Engine();

		inline bool hasInitialLevelOverride() const { return mHasInitialLevelOverride; }
		inline const FilePath &getInitialLevelOverride() const { return mInitialLevelOverride; }
		inline void setInitialLevelOverride(const FilePath &level) { mInitialLevelOverride = level; mHasInitialLevelOverride = true; }
		inline const FilePath &getEngineRootDir() const { return mEngineRootDir; }
		inline DbManager &getDbManager() { return *mDbManager; }
		inline ShaderManager &getShaderManager() { return *mShaderManager; }
		inline RflManager &getRflManager() { return *mRflManager; }
		inline GuiManager &getGuiManager() { return *mGuiManager; }
		inline LightManager &getLightManager() { return *mLightManager; }
		inline Level &getLevel() { return *mLevel; } // FIXME: throw if no level present
		inline Player *getPlayer() { return mPlayer; }
        inline void setPlayer(Player *p) { mPlayer = p; }
		inline void setCamera(Camera *cam) { mCamera = cam; }
		inline Camera *getCamera() { return mCamera; }
		inline double getMaxFrameRate() const { return mMaxFrameRate; }
		inline void setMaxFrameRate(double fps) { mMaxFrameRate = fps; } // 0 for no cap

		void setUp();
		void run();

		void loadLevel(const FilePath &levelFile);

		void setFullScreenGamma(float gamma);


	private:

		void _findEngineRoot(const std::string &rrcFileName);

		std::unique_ptr<DbManager> mDbManager;
		std::unique_ptr<ShaderManager> mShaderManager;
		std::unique_ptr<RflManager> mRflManager;
		osg::ref_ptr<InputManager> mInputManager;
		std::unique_ptr<GuiManager> mGuiManager;
		std::unique_ptr<LightManager> mLightManager;
		bool mHasInitialLevelOverride;
		FilePath mInitialLevelOverride;
		FilePath mEngineRootDir;
		std::unique_ptr<Level> mLevel;
		osg::ref_ptr<osg::Group> mRootNode;
		osg::ref_ptr<osgViewer::Viewer> mViewer;
		osg::ref_ptr<osgViewer::ScreenCaptureHandler> mScreenshotHandler;
		osg::ref_ptr<osg::Uniform> mGammaUniform;
		Camera *mCamera;
		Player *mPlayer;
		double mMaxFrameRate;
		bool mSetUp;
	};

}

#endif /* INCLUDE_ENGINE_H_ */
