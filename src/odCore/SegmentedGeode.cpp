/*
 * SegmentedGeode.cpp
 *
 *  Created on: 10 Feb 2018
 *      Author: zal
 */

#include <odCore/SegmentedGeode.h>

#include <algorithm>
#include <osg/Geometry>
#include <osg/Texture2D>

#include <odCore/Exception.h>
#include <odCore/db/Texture.h>
#include <odCore/db/Database.h>

namespace od
{
	struct VertexInfo
	{
		VertexInfo() : visited(false), texture(0xffff, 0xffff) {}

		bool visited;
		osg::Vec2f uv;
		AssetRef texture;
		osg::Vec3f vertex;
	};

	void SegmentedGeode::build(AssetProvider &db, VertexIter vertexBegin, VertexIter vertexEnd, FaceIter faceBegin, FaceIter faceEnd, size_t textureCount)
	{
		// FIXME: really bad. doesn't use shared vertices at all, makes unneccessary copies etc.

		auto pred = [](Face &left, Face &right){ return left.texture < right.texture; };
		std::sort(faceBegin, faceEnd, pred); // most models are already sorted by texture, so this is O(n) most of the time

		std::vector<osg::Vec3f> vertexArray(vertexBegin, vertexEnd);
		/*std::vector<VertexInfo> vertexInfoArray;
		vertexInfoArray.reserve(vertexEnd - vertexBegin);
		for(auto it = vertexBegin; it != vertexEnd; ++it)
		{
			VertexInfo info;
			info.vertex = *it;
			info.visited = false;
			vertexInfoArray.push_back(info);
		}*/

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
		for(auto it = faceBegin; it != faceEnd; ++it)
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

			if(it->texture.isNull())
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
			geomGroups.back().facesEnd = faceEnd;
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

			osg::ref_ptr<Texture> textureImage = db.getTextureByRef(ggIt->texture);
			if(textureImage->hasAlpha())
			{
				osg::StateSet *ss = ggIt->geometry->getOrCreateStateSet();
				ss->setMode(GL_BLEND, osg::StateAttribute::ON);
				ss->setRenderBinDetails(1, "DepthSortedBin");
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

	void SegmentedGeode::build(AssetProvider &db, std::vector<osg::Vec3f> &vertexArray, std::vector<Face> &faceArray, size_t textureCount)
	{
		build(db, vertexArray.begin(), vertexArray.end(), faceArray.begin(), faceArray.end(), textureCount);
	}


	/*
	{
		auto pred = [](Face &left, Face &right){ return left.texture < right.texture; };
		if(std::is_sorted(faceBegin, faceEnd, pred))
		{
			Logger::info() << "Begin building, faces are sorted";

		}else
		{
			Logger::info() << "Begin building, faces are unsorted";
		}


		size_t hits = 0;
		size_t uvMisses = 0;
		size_t textureMisses = 0;
		size_t maxUsage = 0;

		std::vector<VertexBums> vbs;
		vbs.resize(vertexEnd-vertexBegin);

		for(FaceIter it = faceBegin; it != faceEnd; ++it)
		{
			for(size_t n = 0; n < it->vertexCount; ++n)
			{
				size_t vertIndex = it->vertexIndices[n];
				osg::Vec2f uv = it->vertexUvCoords[n];
				AssetRef texture = it->texture;

				if(vertIndex >= vbs.size())
				{
					throw Exception("Vertex index out of bounds");
				}

				vbs[vertIndex].usage++;
				maxUsage = std::max(vbs[vertIndex].usage, maxUsage);

				if(vbs[vertIndex].visited == false)
				{
					vbs[vertIndex].uv = uv;
					vbs[vertIndex].texture = texture;
					vbs[vertIndex].visited = true;

				}else
				{
					if(vbs[vertIndex].uv != uv)
					{
						uvMisses++;
						//throw Exception("Buffer UV fucked up");

					}

					if(vbs[vertIndex].texture != texture)
					{
						textureMisses++;
						//throw Exception("Buffer texture fucked up");

					}else
					{
						hits++;
						//Logger::info() << "Buffer seems nice";
					}
				}
			}
		}

		Logger::info() << "textures=" << textureCount << " hits=" << hits << " uv_misses="
				<< uvMisses << " texture_misses=" << textureMisses << " max_usage=" << maxUsage;
	}
	 */

}
