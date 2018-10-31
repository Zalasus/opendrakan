/*
 * ShaderFactory.cpp
 *
 *  Created on: Oct 30, 2018
 *      Author: zal
 */

#include <odCore/render/ShaderFactory.h>

#include <odCore/Exception.h>

namespace odRender
{

    ShaderFactory::ShaderFactory(const std::string &shaderSourceRoot)
    : mShaderSourceRoot(shaderSourceRoot)
    {
    }

    osg::Program *ShaderFactory::getProgram(const std::string &namePrefix)
    {
        osg::ref_ptr<osg::Shader> vert = _loadShader(namePrefix + "_vertex.glsl", osg::Shader::VERTEX);
        osg::ref_ptr<osg::Shader> fragment = _loadShader(namePrefix + "_fragment.glsl", osg::Shader::FRAGMENT);
        return _makeProgram(vert, fragment);
    }

    osg::Shader *ShaderFactory::_loadShader(const std::string &shaderFileName, osg::Shader::Type shaderType)
    {
        auto it = mShaderCache.find(shaderFileName);
        if(it != mShaderCache.end())
        {
            return it->second;
        }

        od::FilePath shaderFilePath(shaderFileName, mShaderSourceRoot);

        osg::ref_ptr<osg::Shader> newShader(new osg::Shader(shaderType));
        newShader->loadShaderSourceFromFile(shaderFilePath.str());
        newShader->setName(shaderFileName);
        mShaderCache.insert(std::make_pair(shaderFileName, newShader));
        return newShader;
    }

    osg::Program *ShaderFactory::_makeProgram(osg::Shader *vertexShader, osg::Shader *fragmentShader)
    {
        auto it = mProgramCache.find(std::make_pair(vertexShader, fragmentShader));
        if(it != mProgramCache.end())
        {
            return it->second;
        }

        if(vertexShader == nullptr)
        {
            throw od::Exception("Passed nullptr as vertex shader when making program");
        }

        if(fragmentShader == nullptr)
        {
            throw od::Exception("Passed nullptr as fragment shader when making program");
        }

        osg::ref_ptr<osg::Program> newProgram(new osg::Program);
        newProgram->addShader(vertexShader);
        newProgram->addShader(fragmentShader);
        newProgram->setName(vertexShader->getName() + ":" + fragmentShader->getName());
        mProgramCache.insert(std::make_pair(std::make_pair(vertexShader, fragmentShader), newProgram));
        return newProgram;
    }

}


