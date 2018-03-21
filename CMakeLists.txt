

cmake_minimum_required(VERSION 2.6.3)

project(OpenDrakan)


# project sources
include_directories("include")
set(SOURCES 
        "src/StringUtils.cpp"
        "src/db/Skeleton.cpp"
        "src/db/Asset.cpp"
        "src/db/Sound.cpp"
        "src/db/Class.cpp"
        "src/db/ClassFactory.cpp"
        "src/db/TextureFactory.cpp"
        "src/db/Model.cpp"
        "src/db/Texture.cpp"
        "src/db/Sequence.cpp"
        "src/db/ModelFactory.cpp"
        "src/db/Database.cpp"
        "src/db/SequenceFactory.cpp"
        "src/SrscFile.cpp"
        "src/Player.cpp"
        "src/rfl/RflClass.cpp"
        "src/rfl/Rfl.cpp"
        "src/rfl/RflFieldProbe.cpp"
        "src/rfl/general/RflAbstractItem.cpp"
        "src/rfl/general/RflMaterial.cpp"
        "src/rfl/general/RflCamera.cpp"
        "src/rfl/general/RflSky.cpp"
        "src/rfl/general/RflPlayerControl.cpp"
        "src/rfl/RflField.cpp"
        "src/rfl/dragon/RflHealthPack.cpp"
        "src/rfl/dragon/RflKey.cpp"
        "src/rfl/dragon/RflDragon.cpp"
        "src/Camera.cpp"
        "src/Layer.cpp"
        "src/OsgSerializers.cpp"
        "src/DbManager.cpp"
        "src/ZStream.cpp"
        "src/Engine.cpp"
        "src/DataStream.cpp"
        "src/SegmentedGeode.cpp"
        "src/TextureAtlas.cpp"
        "src/Logger.cpp"
        "src/InputManager.cpp"
        "src/Level.cpp"
        "src/Exception.cpp"
        "src/LevelObject.cpp"
        "src/FilePath.cpp"
        "src/ShaderManager.cpp"
        "src/Main.cpp")


# dependencies
find_package(ZLIB REQUIRED)
find_package(OpenSceneGraph 3.4.0 REQUIRED osgDB osgViewer osgGA osgUtil)


# targets
add_executable(opendrakan ${SOURCES})
target_link_libraries(opendrakan ${OPENSCENEGRAPH_LIBRARIES} ${ZLIB_LIBRARIES})


