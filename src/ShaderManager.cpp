/*
 * ShaderManager.cpp
 *
 *  Created on: 21 Mar 2018
 *      Author: zal
 */

#include "ShaderManager.h"

namespace od
{

	ShaderManager::ShaderManager(const FilePath &shaderSourceRoot)
	: mShaderSourceRoot(shaderSourceRoot)
	{
	}

	osg::ref_ptr<osg::Shader> ShaderManager::loadShader(const std::string &shaderFileName, osg::Shader::Type shaderType)
	{
		auto it = mShaderCache.find(shaderFileName);
		if(it != mShaderCache.end())
		{
			return it->second;
		}

		// TODO: implement binary loading if compiling shaders at startup takes too long

		FilePath shaderFilePath(shaderFileName, mShaderSourceRoot);

		osg::ref_ptr<osg::Shader> newShader(new osg::Shader(shaderType));
		newShader->loadShaderSourceFromFile(shaderFilePath.str());
		mShaderCache.insert(std::make_pair(shaderFileName, newShader));
		return newShader;
	}

	osg::ref_ptr<osg::Program> ShaderManager::makeProgram(osg::ref_ptr<osg::Shader> vertexShader, osg::ref_ptr<osg::Shader> fragmentShader)
	{
		auto it = mProgramCache.find(std::make_pair(vertexShader, fragmentShader));
		if(it != mProgramCache.end())
		{
			return it->second;
		}

		osg::ref_ptr<osg::Program> newProgram(new osg::Program);
		newProgram->addShader(vertexShader);
		newProgram->addShader(fragmentShader);
		mProgramCache.insert(std::make_pair(std::make_pair(vertexShader, fragmentShader), newProgram));
		return newProgram;
	}

}
