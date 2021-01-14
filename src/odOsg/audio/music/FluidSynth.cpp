/*
 * FluidSynth.cpp
 *
 *  Created on: 25 Jun 2019
 *      Author: zal
 */

#include <odOsg/audio/music/FluidSynth.h>

#include <sstream>
#include <cstdio>

#include <odCore/Logger.h>
#include <odCore/Panic.h>

#include <odCore/db/MusicContainer.h>

namespace odOsg
{

    class DlsLoaderWrapper
    {
    public:

        DlsLoaderWrapper(odDb::MusicContainer &container, const od::Guid &guid)
        : mContainer(container)
        , mGuid(guid)
        , mInputCursor(mContainer.getCursorForDls(guid))
        , mReader(mInputCursor.getReader())
        , mDataOffset(mInputCursor.getDirIterator()->dataOffset)
        , mDataSize(mInputCursor.getDirIterator()->dataSize)
        {
        }

        void read(void *buf, int count)
        {
            mReader.read(reinterpret_cast<char*>(buf), count);
        }

        void seek(long offset, int origin)
        {
            switch(origin)
            {
            case SEEK_SET:
                mReader.seek(offset + mDataOffset);
                break;

            case SEEK_CUR:
                mReader.seek(offset + mReader.tell());
                break;

            case SEEK_END:
                mReader.seek(offset + mDataOffset + mDataSize);
                break;

            default:
                OD_PANIC() << "Unsupported seek offset";
            }
        }

        long tell()
        {
            return mReader.tell() - mDataOffset;
        }

        static void *_dlsLoader_open(const char *filename)
        {
            if(filename[0] != '&')
            {
                Logger::error() << "Tried to load invalid DLS " << filename << " using the memory wrapper. Filename should be a pointer prefixed by &";
                return nullptr;
            }

            Logger::debug() << "Opened DLS loader wrapper with filename " << filename;

            void *ptr = nullptr;
            std::istringstream ss(filename+1);
            ss >> ptr;

            if(ss.fail())
            {
                return nullptr;
            }

            return ptr;
        }

        static int _dlsLoader_read(void *buf, int count, void *handle)
        {
            if(handle == nullptr) return FLUID_FAILED;

            DlsLoaderWrapper *loader = reinterpret_cast<DlsLoaderWrapper*>(handle);
            loader->read(buf, count);

            return FLUID_OK;
        }

        static int _dlsLoader_seek(void *handle, long offset, int origin)
        {
            if(handle == nullptr) return FLUID_FAILED;

            DlsLoaderWrapper *loader = reinterpret_cast<DlsLoaderWrapper*>(handle);
            loader->seek(offset, origin);

            return FLUID_OK;
        }

        static int _dlsLoader_close(void *handle)
        {
            return FLUID_OK;
        }

        static long _dlsLoader_tell(void *handle)
        {
            if(handle == nullptr) return FLUID_FAILED;

            DlsLoaderWrapper *loader = reinterpret_cast<DlsLoaderWrapper*>(handle);
            return loader->tell();
        }


    private:

        odDb::MusicContainer &mContainer;
        od::Guid mGuid;
        od::SrscFile::RecordInputCursor mInputCursor;
        od::DataReader mReader;
        size_t mDataOffset;
        size_t mDataSize;

    };


    static void _errorCheck(int errorCode, const char *errorMessage)
    {
        if(errorCode != FLUID_OK)
        {
            throw od::Exception(errorMessage);
        }
    }


    FluidSynth::Settings::Settings()
    {
        mInstance = new_fluid_settings();
        if(mInstance == nullptr)
        {
            OD_PANIC() << "Failed to create fluid synth settings";
        }
    }

    FluidSynth::Settings::~Settings()
    {
        delete_fluid_settings(mInstance);
    }

    void FluidSynth::Settings::setString(const char *key, const char *value)
    {
        fluid_settings_setstr(mInstance, key, value);
    }


    FluidSynth::FluidSynth()
    : mSynth(nullptr)
    , mMusicContainer(nullptr)
    {
        mSettings.setString("player.timing-source", "sample");

        mSynth = new_fluid_synth(mSettings.getInstance());
        if(mSynth == nullptr) OD_PANIC() << "Failed to create fluid synth";

        fluid_sfloader_t *sfloader = new_fluid_defsfloader(mSettings.getInstance()); // TODO: do I have to delete this? o.o
        if(sfloader == nullptr) OD_PANIC() << "Failed to create fluid soundfont loader";
        int result = fluid_sfloader_set_callbacks(sfloader,
                                                  &DlsLoaderWrapper::_dlsLoader_open,
                                                  &DlsLoaderWrapper::_dlsLoader_read,
                                                  &DlsLoaderWrapper::_dlsLoader_seek,
                                                  &DlsLoaderWrapper::_dlsLoader_tell,
                                                  &DlsLoaderWrapper::_dlsLoader_close);
        _errorCheck(result, "Failed to set soundfont loader callbacks");
        fluid_synth_add_sfloader(mSynth, sfloader);
    }

    FluidSynth::~FluidSynth()
    {
        delete_fluid_synth(mSynth);
    }

    void FluidSynth::noteOn(uint8_t channel, uint8_t note, uint8_t velocity)
    {
        int result = fluid_synth_noteon(mSynth, channel, note, velocity);
        _errorCheck(result, "Failed to send noteon message");
    }

    void FluidSynth::noteOff(uint8_t channel, uint8_t note)
    {
        int result = fluid_synth_noteoff(mSynth, channel, note);
        _errorCheck(result, "Failed to send noteoff message");
    }

    void FluidSynth::pitchBend(uint8_t channel, uint16_t value)
    {
        int result = fluid_synth_pitch_bend(mSynth, channel, value);
        _errorCheck(result, "Failed to send pitch bend message");
    }

    void FluidSynth::controllerChange(uint8_t channel, uint8_t controller, uint8_t value)
    {
        int result = fluid_synth_cc(mSynth, channel, controller, value);
        _errorCheck(result, "Failed to send controller change message");
    }

    void FluidSynth::channelPressure(uint8_t channel, uint8_t value)
    {
        int result = fluid_synth_channel_pressure(mSynth, channel, value);
        _errorCheck(result, "Failed to send channel pressure message");
    }

    void FluidSynth::keyPressure(uint8_t channel, uint8_t key, uint8_t value)
    {
        int result = fluid_synth_key_pressure(mSynth, channel, key, value);
        _errorCheck(result, "Failed to send key pressure message");
    }

    void FluidSynth::allNotesOff()
    {
    }

    void FluidSynth::fillInterleavedStereoBuffer(int16_t *buffer, size_t size)
    {
        int result = fluid_synth_write_s16(mSynth, size/2, buffer, 0, 2, buffer, 1, 2);
        _errorCheck(result, "Synthesis failed");
    }

    void FluidSynth::preloadDls(const od::Guid &dlsGuid)
    {
        _getOrLoadDls(dlsGuid);
    }

    void FluidSynth::assignPreset(uint8_t channel, uint32_t bank, uint32_t patch, const od::Guid &dlsGuid)
    {
        int sfId = _getOrLoadDls(dlsGuid);

        int result = fluid_synth_program_select(mSynth, channel, sfId, bank, patch);
        _errorCheck(result, "Failed to select program");
    }

    int FluidSynth::_getOrLoadDls(const od::Guid &dlsGuid)
    {
        auto it = mSoundFontIdMap.find(dlsGuid);
        if(mSoundFontIdMap.find(dlsGuid) != mSoundFontIdMap.end())
        {
            return it->second;
        }

        Logger::verbose() << "Loading DLS " << dlsGuid;

        if(mMusicContainer == nullptr)
        {
            OD_PANIC() << "No music container provided to FluidSynth when a DLS needed to be loaded";
        }

        // beware: pretty bad pointer/string abuse. there is much that can go wrong here, but as I see it, fluidsynth
        //         doesn't offer a better solution for this without forcing us into writing our own DLS loader

        DlsLoaderWrapper loaderWrapper(*mMusicContainer, dlsGuid);

        std::ostringstream ss;
        ss << '&' << static_cast<void*>(&loaderWrapper);
        std::string str = ss.str();

        int sfId = fluid_synth_sfload(mSynth, str.c_str(), 0);
        if(sfId == FLUID_FAILED) OD_PANIC() << "Failed to load DLS";

        mSoundFontIdMap.insert(std::make_pair(dlsGuid, sfId));

        Logger::debug() << "Successfully loaded DLS " << dlsGuid;

        return sfId;
    }

}
