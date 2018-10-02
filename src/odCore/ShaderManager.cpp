/*
 * ShaderManager.cpp
 *
 *  Created on: 21 Mar 2018
 *      Author: zal
 */

#include <odCore/ShaderManager.h>

#include <odCore/OdDefines.h>
#include <odCore/Exception.h>

namespace od
{

	ShaderManager::ShaderManager(const FilePath &shaderSourceRoot)
	: mShaderSourceRoot(shaderSourceRoot)
	{
	}

	osg::Shader *ShaderManager::loadShader(const std::string &shaderFileName, osg::Shader::Type shaderType)
	{
		auto it = mShaderCache.find(shaderFileName);
		if(it != mShaderCache.end())
		{
			return it->second;
		}

		FilePath shaderFilePath(shaderFileName, mShaderSourceRoot);

		osg::ref_ptr<osg::Shader> newShader(new osg::Shader(shaderType));
		newShader->loadShaderSourceFromFile(shaderFilePath.str());
		newShader->setName(shaderFileName);
		mShaderCache.insert(std::make_pair(shaderFileName, newShader));
		return newShader;
	}

	osg::Program *ShaderManager::makeProgram(osg::Shader *vertexShader, osg::Shader *fragmentShader)
	{
		auto it = mProgramCache.find(std::make_pair(vertexShader, fragmentShader));
		if(it != mProgramCache.end())
		{
			return it->second;
		}

		if(vertexShader == nullptr)
		{
			throw Exception("Passed nullptr as vertex shader when making program");
		}

		if(fragmentShader == nullptr)
		{
			throw Exception("Passed nullptr as fragment shader when making program");
		}

		osg::ref_ptr<osg::Program> newProgram(new osg::Program);
		newProgram->addShader(vertexShader);
		newProgram->addShader(fragmentShader);
		newProgram->setName(vertexShader->getName() + ":" + fragmentShader->getName());
		mProgramCache.insert(std::make_pair(std::make_pair(vertexShader, fragmentShader), newProgram));
		return newProgram;
	}

	osg::Program *ShaderManager::makeProgram(const std::string &namePrefix)
	{
	    osg::ref_ptr<osg::Shader> vert = loadShader(namePrefix + "_vertex.glsl", osg::Shader::VERTEX);
	    osg::ref_ptr<osg::Shader> fragment = loadShader(namePrefix + "_fragment.glsl", osg::Shader::FRAGMENT);
	    return makeProgram(vert, fragment);
	}

}
