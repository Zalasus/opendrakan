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

namespace odRender
{

    class Geometry;

	/**
	 * Class for constructing geodes from Riot engine model data. It splits models into multiple segments
	 * with only one texture per segment. VBOs are shared among all segments and vertices are reused where possible.
	 * Automatically generates normals and duplicates vertices if neccessary.
	 *
	 * Construction is O(n*log(n)).
	 */
	class GeometryBuilder
	{
	public:

		GeometryBuilder(Geometry &geometry, const std::string &geometryName, odDb::AssetProvider &assetProvider);

		inline void setBuildSmoothNormals(bool b) { mSmoothNormals = b; }
		inline void setNormalsFromCcw(bool b) { mNormalsFromCcw = b; }

		void setVertexVector(std::vector<glm::vec3>::iterator begin, std::vector<glm::vec3>::iterator end);
		void setPolygonVector(std::vector<odDb::Model::Polygon>::iterator begin, std::vector<odDb::Model::Polygon>::iterator end);
		void setBoneAffectionVector(std::vector<odDb::Model::BoneAffection>::iterator begin, std::vector<odDb::Model::BoneAffection>::iterator end);

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

		std::string mGeometryName;
		odDb::AssetProvider &mAssetProvider;

		bool mSmoothNormals;
		bool mNormalsFromCcw;

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
