#pragma once

#include "BsCorePrerequisites.h"
#include "BsResource.h"
#include "BsBounds.h"
#include "BsDrawOps.h"
#include "BsSubMesh.h"

namespace BansheeEngine
{
	/**
	 * @brief	Planned usage for the mesh. These options usually affect performance and 
	 *			you should specify static if you don't plan on modifying the mesh often,
	 *			otherwise specify dynamic.
	 */
	enum MeshUsage
	{
		MU_STATIC, /**< Specify for a mesh that is not often updated from the CPU. */
		MU_DYNAMIC, /**< Specify for a mesh that is often updated from the CPU. */
		MU_CPUCACHED = 0x1000 /**< All mesh data will also be cached in CPU memory, making it readable with GPU reads. */
	};

	/**
	 * @brief	Contains various properties describing a mesh.
	 */
	class BS_CORE_EXPORT MeshProperties
	{
	public:
		MeshProperties();
		MeshProperties(UINT32 numVertices, UINT32 numIndices, DrawOperationType drawOp);
		MeshProperties(UINT32 numVertices, UINT32 numIndices, const Vector<SubMesh>& subMeshes);

		/**
		 * @brief	Retrieves a sub-mesh containing data used for rendering a
		 * 			certain portion of this mesh. If no sub-meshes are specified manually
		 *			a special sub-mesh containing all indices is returned.
		 */
		const SubMesh& getSubMesh(UINT32 subMeshIdx = 0) const;

		/**
		 * @brief	Retrieves a total number of sub-meshes in this mesh.
		 */
		UINT32 getNumSubMeshes() const;

		/**
		 * @brief	Returns maximum number of vertices the mesh may store.
		 */
		UINT32 getNumVertices() const { return mNumVertices; }

		/**
		 * @brief	Returns maximum number of indices the mesh may store.
		 */
		UINT32 getNumIndices() const { return mNumIndices; }

		/**
		 * @brief	Returns bounds of the geometry contained in the vertex buffers for all sub-meshes.
		 */
		const Bounds& getBounds() const { return mBounds; }

	protected:
		friend class MeshBase;
		friend class MeshCoreBase;
		friend class Mesh;
		friend class MeshCore;
		friend class TransientMesh;
		friend class TransientMeshCore;
		friend class MeshBaseRTTI;

		Vector<SubMesh> mSubMeshes;
		UINT32 mNumVertices;
		UINT32 mNumIndices;
		Bounds mBounds;
	};

	/**
	 * @brief	Core version of a class used as a basis for all implemenations of meshes.
	 *
	 * @see		MeshBase
	 *
	 * @note	Core thread.
	 */
	class BS_CORE_EXPORT MeshCoreBase : public CoreObjectCore
	{
	public:
		MeshCoreBase(UINT32 numVertices, UINT32 numIndices, const Vector<SubMesh>& subMeshes);
		virtual ~MeshCoreBase() { }

		/**
		 * @brief	Get vertex data used for rendering.
		 */
		virtual SPtr<VertexData> getVertexData() const = 0;

		/**
		 * @brief	Get index data used for rendering.
		 */
		virtual SPtr<IndexBufferCore> getIndexBuffer() const = 0;

		/**
		 * @brief	Returns an offset into the vertex buffers that is returned
		 * 			by getVertexData that signifies where this meshes vertices
		 * 			begin.
		 * 			
		 * @note	Used when multiple meshes share the same buffers.
		 */
		virtual UINT32 getVertexOffset() const { return 0; }

		/**
		 * @brief	Returns an offset into the index buffer that is returned
		 * 			by getIndexData that signifies where this meshes indices
		 * 			begin.
		 * 			
		 * @note	Used when multiple meshes share the same buffers.
		 */
		virtual UINT32 getIndexOffset() const { return 0; }

		/**
		 * @brief	Returns a structure that describes how are the vertices stored in the mesh's vertex buffer.
		 */
		virtual SPtr<VertexDataDesc> getVertexDesc() const = 0;

		/**
		 * @brief	Called whenever this mesh starts being used on the GPU.
		 * 			
		 * @note	Needs to be called after all commands referencing this 
		 * 			mesh have been sent to the GPU.
		 * 			
		 *			Internal method.
		 */
		virtual void _notifyUsedOnGPU() { }

		/**
		 * @brief	Returns properties that contain information about the mesh.
		 */
		const MeshProperties& getProperties() const { return mProperties; }

	protected:
		/**
		 * @copydoc	CoreObjectCore::syncToCore
		 */
		virtual void syncToCore(const CoreSyncData& data) override;

		MeshProperties mProperties;
	};

	/**
	 * @brief	Base class all mesh implementations derive from. Meshes hold geometry information,
	 *			normally in the form of one or several index or vertex buffers. Different mesh implementations
	 *			might choose to manage those buffers differently.
	 *
	 * @note	Sim thread.
	 */
	class BS_CORE_EXPORT MeshBase : public Resource
	{
	public:
		/**
		 * @brief	Constructs a new mesh with no sub-meshes.
		 *
		 * @param	numVertices		Number of vertices in the mesh.
		 * @param	numIndices		Number of indices in the mesh. 
		 * @param	drawOp			Determines how should the provided indices be interpreted by the pipeline. Default option is triangles,
		 *							where three indices represent a single triangle.
		 */
		MeshBase(UINT32 numVertices, UINT32 numIndices, DrawOperationType drawOp = DOT_TRIANGLE_LIST);

		/**
		 * @brief	Constructs a new mesh with one or multiple sub-meshes. (When using just one sub-mesh it is equivalent
		 *			to using the other overload).
		 *
		 * @param	numVertices		Number of vertices in the mesh.
		 * @param	numIndices		Number of indices in the mesh.
		 * @param	subMeshes		Defines how are indices separated into sub-meshes, and how are those sub-meshes rendered.
		 */
		MeshBase(UINT32 numVertices, UINT32 numIndices, const Vector<SubMesh>& subMeshes);

		virtual ~MeshBase();

		/**
		 * @brief	Returns properties that contain information about the mesh.
		 */
		const MeshProperties& getProperties() const { return mProperties; }

		/**
		 * @brief	Retrieves a core implementation of a mesh usable only from the
		 *			core thread.
		 */
		SPtr<MeshCoreBase> getCore() const;

	protected:
		/**
		 * @copydoc	CoreObject::syncToCore
		 */
		virtual CoreSyncData syncToCore(FrameAlloc* allocator) override;

		MeshProperties mProperties;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	private:
		MeshBase() { } // Serialization only

	public:
		friend class MeshBaseRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;
	};
}