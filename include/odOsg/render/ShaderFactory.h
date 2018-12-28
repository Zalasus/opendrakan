/*
 * ShaderFactory.h
 *
 *  Created on: Oct 30, 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RENDER_SHADERFACTORY_H_
#define INCLUDE_ODCORE_RENDER_SHADERFACTORY_H_

#include <map>
#include <string>
#include <osg/Shader>
#include <osg/Program>

#include <odCore/FilePath.h>

namespace odOsg
{

    class ShaderFactory
    {
    public:

        ShaderFactory(const std::string &shaderSourceDir);

        /**
         * Loads the shaders namePrefix_vertex.glsl and namePrefix_fragment.glsl and returns a program linked against both.
         * If a program with this name prefix exists in the internal cache, the cached version is returned instead.
         */
        osg::Program *getProgram(const std::string &namePrefix);


    private:

        /**
         * Creates a shader object from a shader source. If the shader is found in local cache,
         * that version is used instead of creating a new shader object.
         *
         * @param[in]  shaderFileName    Filename of the shader source, relative to shader source root
         * @param[in]  shaderType        Type of the shader
         */
        osg::Shader *_loadShader(const std::string &shaderFileName, osg::Shader::Type shaderType);

        /**
         * Returns program from cache if available. If not, links shaders and caches the result.
         */
        osg::Program *_makeProgram(osg::Shader *vertexShader, osg::Shader *fragmentShader);


        od::FilePath mShaderSourceRoot;
        std::map<std::string, osg::ref_ptr<osg::Shader>> mShaderCache;
        std::map<std::pair<osg::Shader*,osg::Shader*>, osg::ref_ptr<osg::Program>> mProgramCache;

    };

}


#endif /* INCLUDE_ODCORE_RENDER_SHADERFACTORY_H_ */
