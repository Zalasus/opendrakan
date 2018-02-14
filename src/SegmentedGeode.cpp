/*
 * SegmentedGeode.cpp
 *
 *  Created on: 10 Feb 2018
 *      Author: zal
 */

#include "SegmentedGeode.h"

#include <algorithm>
#include <osg/Geometry>
#include <osg/Texture2D>

#include "db/Texture.h"
#include "db/Database.h"
#include "Exception.h"

namespace od
{

	void SegmentedGeode::build(AssetProvider &db, std::vector<osg::Vec3> &vertexArray, std::vector<Face> &faceArray, size_t textureCount)
	{
		auto pred = [](Face &left, Face &right){ return left.texture < right.texture; };
		std::sort(faceArray.begin(), faceArray.end(), pred);

		struct GeomGroup
		{
			std::vector<Face>::iterator facesBegin;
			std::vector<Face>::iterator facesEnd;
			osg::ref_ptr<osg::Geometry> geometry;
			size_t vertexCount;
			AssetRef texture;
		};

		std::vector<GeomGroup> geomGroups;
		if(textureCount > 0) geomGroups.reserve(textureCount);
		for(auto it = faceArray.begin(); it != faceArray.end(); ++it)
		{
			if(geomGroups.size() == 0)
			{
				GeomGroup gg;
				gg.vertexCount = 0;
				gg.geometry = new osg::Geometry;
				gg.facesBegin = it;
				gg.texture = it->texture;
				geomGroups.push_back(gg);

			}else if(geomGroups.back().texture != it->texture)
			{
				geomGroups.back().facesEnd = it;

				GeomGroup gg;
				gg.vertexCount = 0;
				gg.geometry = new osg::Geometry;
				gg.facesBegin = it;
				gg.texture = it->texture;
				geomGroups.push_back(gg);
			}

			if(it->texture.isNullTexture())
			{
				throw Exception("Can't build SegmentedGeode with null textures");
			}

			if(it->vertexCount == 3)
			{
				geomGroups.back().vertexCount += 3;

			}else if(it->vertexCount == 4)
			{
				geomGroups.back().vertexCount += 6; // 6 since we convert quads to triangles

			}else
			{
				throw Exception("Can't build SegmentedGeode with non-quad/non-triangle primitives");
			}
		}
		if(geomGroups.size() > 0)
		{
			geomGroups.back().facesEnd = faceArray.end();
		}

		// faces are now sorted and grouped by texture~ now build geometries from them

		for(auto ggIt = geomGroups.begin(); ggIt != geomGroups.end(); ++ggIt)
		{
			osg::ref_ptr<osg::Vec3Array> vertices(new osg::Vec3Array);
			vertices->reserve(ggIt->vertexCount);
			osg::ref_ptr<osg::Vec2Array> uvCoords(new osg::Vec2Array);
			uvCoords->reserve(ggIt->vertexCount);

			for(auto it = ggIt->facesBegin; it != ggIt->facesEnd; ++it)
			{
				if(it->vertexCount == 3)
				{
					vertices->push_back(vertexArray[it->vertexIndices[0]]);
					vertices->push_back(vertexArray[it->vertexIndices[1]]);
					vertices->push_back(vertexArray[it->vertexIndices[2]]);
					uvCoords->push_back(it->vertexUvCoords[0]);
					uvCoords->push_back(it->vertexUvCoords[1]);
					uvCoords->push_back(it->vertexUvCoords[2]);

				}else if(it->vertexCount == 4) // convert all quads to triangles
				{
					vertices->push_back(vertexArray[it->vertexIndices[0]]);
					vertices->push_back(vertexArray[it->vertexIndices[1]]);
					vertices->push_back(vertexArray[it->vertexIndices[2]]);
					uvCoords->push_back(it->vertexUvCoords[0]);
					uvCoords->push_back(it->vertexUvCoords[1]);
					uvCoords->push_back(it->vertexUvCoords[2]);

					vertices->push_back(vertexArray[it->vertexIndices[2]]);
					vertices->push_back(vertexArray[it->vertexIndices[3]]);
					vertices->push_back(vertexArray[it->vertexIndices[0]]);
					uvCoords->push_back(it->vertexUvCoords[2]);
					uvCoords->push_back(it->vertexUvCoords[3]);
					uvCoords->push_back(it->vertexUvCoords[0]);
				}
			}

			ggIt->geometry->setVertexArray(vertices);
			ggIt->geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES, 0, vertices->size()));
			ggIt->geometry->setTexCoordArray(0, uvCoords);

			TexturePtr textureImage = db.getTextureByRef(ggIt->texture);
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

			if(ggIt->geometry->checkForDeprecatedData())
			{
				Logger::warn() << "Geometry in segmented geode contains deprecated data";
			}

			this->addDrawable(ggIt->geometry);
		}

	}

}
