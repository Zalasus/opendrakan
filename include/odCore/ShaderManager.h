/*
 * ShaderManager.h
 *
 *  Created on: 21 Mar 2018
 *      Author: zal
 */

#ifndef INCLUDE_SHADERMANAGER_H_
#define INCLUDE_SHADERMANAGER_H_

#include <map>
#include <string>
#include <osg/Shader>
#include <osg/Program>

#include <odCore/FilePath.h>

namespace od
{

	class ShaderManager
	{
	public:

		ShaderManager(const FilePath &shaderSourceRoot);

		/**
		 * Creates a shader object from a shader source. If the shader is found in local cache,
		 * that version is used instead of creating a new shader object.
		 *
		 * @param[in]  shaderFileName    Filename of the shader source, relative to shader source root
		 * @param[in]  shaderType        Type of the shader
		 */
		osg::Shader *loadShader(const std::string &shaderFileName, osg::Shader::Type shaderType);

		/**
		 * Returns program from cache if available. If not, links shaders and caches the result.
		 */
		osg::Program *makeProgram(osg::Shader *vertexShader, osg::Shader *fragmentShader);

		/**
		 * Convenience method. Loads the shaders namePrefix_vertex.glsl and namePrefix_fragment.glsl
		 * and returns a program linked against both.
		 */
		osg::Program *makeProgram(const std::string &namePrefix);


	private:

		FilePath mShaderSourceRoot;
		std::map<std::string, osg::ref_ptr<osg::Shader>> mShaderCache;
		std::map<std::pair<osg::Shader*,osg::Shader*>, osg::ref_ptr<osg::Program>> mProgramCache;
	};

}

#endif /* INCLUDE_SHADERMANAGER_H_ */
