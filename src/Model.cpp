/*
 * Model.cpp
 *
 *  Created on: 2 Feb 2018
 *      Author: zal
 */

#include "Model.h"

#include <algorithm>
#include <osg/Geometry>
#include <osgUtil/SmoothingVisitor>

#include "Asset.h"
#include "Exception.h"
#include "Database.h"
#include "ModelFactory.h"
#include "Texture.h"

namespace od
{

	Model::Model(Database &db, RecordId modelId)
	: Asset(db, modelId)
	, mModelName("")
	, mTriangleCount(0)
	, mQuadCount(0)
	{
	}

	void Model::loadNameAndShading(ModelFactory &factory, DataReader dr)
	{
		dr >> mModelName;

		uint32_t shadingType;
		dr >> shadingType;
	}

	void Model::loadVertices(ModelFactory &factory, DataReader dr)
	{
		uint16_t vertexCount;
		dr >> vertexCount;

		mVertices.reserve(vertexCount);
		for(size_t i = 0; i < vertexCount; ++i)
		{
			float x;
			float y;
			float z;

			dr >> x
			   >> y
			   >> z;

			mVertices.push_back(osg::Vec3(x,y,z));
		}
	}

	void Model::loadFaces(ModelFactory &factory, DataReader dr)
	{
		uint16_t faceCount;
		dr >> faceCount;

		mFaces.reserve(faceCount);
		for(size_t i = 0; i < faceCount; ++i)
		{
			Face f;

			dr >> DataReader::Ignore(2)
			   >> f.vertexCount
			   >> f.textureIndex;

			if(f.vertexCount == 3)
			{
				++mTriangleCount;

			}else if(f.vertexCount == 4)
			{
				++mTriangleCount;

			}else
			{
				throw UnsupportedException("Can't load model with non-triangle/non-quad primitives");
			}

			for(size_t i = 0; i < f.vertexCount; ++i)
			{
				float u;
				float v;

				dr >> f.vertexIndices[i]
				   >> u
				   >> v;

				f.vertexUvCoords[i] = osg::Vec2(u,v);
			}

			mFaces.push_back(f);
		}
	}

	void Model::loadTextures(ModelFactory &factory, DataReader dr)
	{
		uint32_t textureCount;
		dr >> textureCount;

		mTextureRefs.reserve(textureCount);
		for(size_t i = 0; i < textureCount; ++i)
		{
			AssetRef textureRef;
			dr >> textureRef;

			mTextureRefs.push_back(textureRef);
		}
	}

	void Model::buildGeometry()
	{
		Logger::debug() << "Building geometry for model " << std::hex << getAssetId() << std::dec;

		// one geometry per texture to avoid multitexturing
		// experimental.

		auto pred = [](Face &left, Face &right){ return left.textureIndex < right.textureIndex; };
		std::sort(mFaces.begin(), mFaces.end(), pred);

		struct GeomGroup
		{
			std::vector<Face>::iterator facesBegin;
			std::vector<Face>::iterator facesEnd;
			osg::ref_ptr<osg::Geometry> geometry;
			uint16_t textureIndex;
		};

		std::vector<GeomGroup> geomGroups;
		geomGroups.reserve(mTextureRefs.size());
		for(auto it = mFaces.begin(); it != mFaces.end(); ++it)
		{
			if(geomGroups.size() == 0)
			{
				GeomGroup gg;
				gg.geometry = new osg::Geometry;
				gg.facesBegin = it;
				gg.textureIndex = it->textureIndex;
				geomGroups.push_back(gg);

			}else if(geomGroups.back().textureIndex != it->textureIndex)
			{
				geomGroups.back().facesEnd = it;

				GeomGroup gg;
				gg.geometry = new osg::Geometry;
				gg.facesBegin = it;
				gg.textureIndex = it->textureIndex;
				geomGroups.push_back(gg);
			}
		}
		if(geomGroups.size() > 0)
		{
			geomGroups.back().facesEnd = mFaces.end();
		}


		for(auto ggIt = geomGroups.begin(); ggIt != geomGroups.end(); ++ggIt)
		{
			osg::ref_ptr<osg::Vec3Array> vertices(new osg::Vec3Array);
			osg::ref_ptr<osg::Vec2Array> uvCoords(new osg::Vec2Array);

			for(auto it = ggIt->facesBegin; it != ggIt->facesEnd; ++it)
			{
				if(it->vertexCount == 3)
				{
					vertices->push_back(mVertices[it->vertexIndices[0]]);
					vertices->push_back(mVertices[it->vertexIndices[1]]);
					vertices->push_back(mVertices[it->vertexIndices[2]]);
					uvCoords->push_back(it->vertexUvCoords[0]);
					uvCoords->push_back(it->vertexUvCoords[1]);
					uvCoords->push_back(it->vertexUvCoords[2]);

				}else if(it->vertexCount == 4) // convert all quads to triangles
				{
					vertices->push_back(mVertices[it->vertexIndices[0]]);
					vertices->push_back(mVertices[it->vertexIndices[1]]);
					vertices->push_back(mVertices[it->vertexIndices[2]]);
					uvCoords->push_back(it->vertexUvCoords[0]);
					uvCoords->push_back(it->vertexUvCoords[1]);
					uvCoords->push_back(it->vertexUvCoords[2]);

					vertices->push_back(mVertices[it->vertexIndices[2]]);
					vertices->push_back(mVertices[it->vertexIndices[3]]);
					vertices->push_back(mVertices[it->vertexIndices[0]]);
					uvCoords->push_back(it->vertexUvCoords[2]);
					uvCoords->push_back(it->vertexUvCoords[3]);
					uvCoords->push_back(it->vertexUvCoords[0]);
				}
			}

			ggIt->geometry->setVertexArray(vertices);
			ggIt->geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES, 0, vertices->size()));
			ggIt->geometry->setTexCoordArray(0, uvCoords);

			TexturePtr textureImage = getDatabase().getAssetAsTexture(mTextureRefs[ggIt->textureIndex]);
			if(textureImage->hasAlpha())
			{
				ggIt->geometry->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
			}

			osg::ref_ptr<osg::Texture2D> texture(new osg::Texture2D(textureImage));
			texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT); // this is the default for all drakan textures
			texture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
			ggIt->geometry->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture);

			osg::ref_ptr<osg::Vec4Array> colorArray(new osg::Vec4Array);
			colorArray->push_back(osg::Vec4(1,1,1,1));
			ggIt->geometry->setColorArray(colorArray);
			ggIt->geometry->setColorBinding(osg::Geometry::BIND_OVERALL);

			this->addDrawable(ggIt->geometry);
		}

		osgUtil::SmoothingVisitor sm;
        this->accept(sm);
	}
}

