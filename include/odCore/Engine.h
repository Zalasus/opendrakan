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

namespace odAudio
{
    class SoundManager;
}

namespace odDb
{
    class DbManager;
}

namespace odGui
{
    class GuiManager;
}

namespace odRender
{
    class RenderManager;
}

namespace odRfl
{
    class RflManager;
}

namespace od
{

    class Player;
    class Camera;
    class Level;
    class InputManager;
    class ShaderManager;

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
		inline odDb::DbManager &getDbManager() { return *mDbManager; }
		inline ShaderManager &getShaderManager() { return *mShaderManager; }
		inline odRfl::RflManager &getRflManager() { return *mRflManager; }
		inline odGui::GuiManager &getGuiManager() { return *mGuiManager; }
		inline odRender::RenderManager &getRenderManager() { return *mRenderManager; }
		inline odAudio::SoundManager &getSoundManager() { return *mSoundManager; }
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

		std::unique_ptr<odDb::DbManager> mDbManager;
		std::unique_ptr<ShaderManager> mShaderManager;
		std::unique_ptr<odRfl::RflManager> mRflManager;
		osg::ref_ptr<InputManager> mInputManager;
		std::unique_ptr<odGui::GuiManager> mGuiManager;
		std::unique_ptr<odRender::RenderManager> mRenderManager;
		std::unique_ptr<odAudio::SoundManager> mSoundManager;
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
