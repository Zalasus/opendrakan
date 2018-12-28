/*
 * GeometryBuilder.h
 *
 *  Created on: 26 Mar 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RENDER_GEOMETRYUILDER_H_
#define INCLUDE_ODCORE_RENDER_GEOMETRYUILDER_H_

#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <odCore/db/Asset.h>
#include <odCore/db/Model.h>

namespace odDb
{
	class AssetProvider;
}

namespace odOsg
{

    class Geometry;
    class Renderer;

	/**
	 * TODO: this can potentially be integrated into out Geometry implementation
	 *
	 * Class for constructing geodes from Riot engine model data. It splits models into multiple segments
	 * with only one texture per segment. VBOs are shared among all segments and vertices are reused where possible.
	 * Automatically generates normals and duplicates vertices if neccessary.
	 *
	 * Construction is O(n*log(n)).
	 */
	class GeometryBuilder
	{
	public:

        typedef std::vector<glm::vec3>::const_iterator VertexIterator;
        typedef std::vector<odDb::Model::Polygon>::const_iterator PolygonIterator;
        typedef std::vector<odDb::Model::BoneAffection>::const_iterator BoneAffectionIterator;

		GeometryBuilder(Renderer *renderer, Geometry &geometry, const std::string &geometryName, odDb::AssetProvider &assetProvider);

		inline void setBuildSmoothNormals(bool b) { mSmoothNormals = b; }
		inline void setNormalsFromCcw(bool b) { mNormalsFromCcw = b; }
		inline void setUseClampedTextures(bool b) { mUseClampedTextures = b; }

		void setVertexVector(VertexIterator begin, VertexIterator end);
		void setPolygonVector(PolygonIterator begin, PolygonIterator end);
		void setBoneAffectionVector(BoneAffectionIterator begin, BoneAffectionIterator end);

		void build();


	private:

		struct Triangle
		{
			size_t vertexIndices[3];
			glm::vec2 uvCoords[3];
			odDb::AssetRef texture;
		};

		void _buildNormals();
		void _makeIndicesUniqueAndGenerateUvs();
		void _disambiguateAndGenerateUvs();

		Renderer *mRenderer;

		std::string mGeometryName;
		odDb::AssetProvider &mAssetProvider;

		bool mSmoothNormals;
		bool mNormalsFromCcw;
		bool mUseClampedTextures;

		std::vector<Triangle> mTriangles;

		Geometry &mGeometry;
		std::vector<glm::vec3> &mVertices;
		std::vector<glm::vec3> &mNormals;
		std::vector<glm::vec2> &mUvCoords;
		std::vector<glm::vec4> &mBoneIndices;
		std::vector<glm::vec4> &mBoneWeights;
	};

}

#endif /* INCLUDE_ODCORE_RENDER_GEOMETRYUILDER_H_ */
