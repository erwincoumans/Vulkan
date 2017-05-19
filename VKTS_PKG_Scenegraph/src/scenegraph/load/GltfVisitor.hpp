/**
 * VKTS - VulKan ToolS.
 *
 * The MIT License (MIT)
 *
 * Copyright (c) since 2014 Norbert Nopper
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef VKTS_GLTFVISITOR_HPP_
#define VKTS_GLTFVISITOR_HPP_

#include <vkts/scenegraph/vkts_scenegraph.hpp>

namespace vkts
{

enum GltfSubState {
	GltfSubState_Start,
	GltfSubState_End,

	GltfSubState_Error,

	GltfSubState_NormalTexture,
	GltfSubState_OcclusionTexture
};

enum GltfState {
	GltfState_Start,
	GltfState_End,

	GltfState_Error,

	GltfState_ExtensionsRequired,
	GltfState_ExtensionsUsed,
	GltfState_Asset,
	GltfState_Buffers,
	GltfState_BufferViews,
	GltfState_Accessors,
	GltfState_Images,
	GltfState_Samplers,
	GltfState_Textures,
	GltfState_Materials,
	GltfState_Meshes,
	GltfState_Cameras,
	GltfState_Skins,
	GltfState_Nodes,
	GltfState_Animations,
	GltfState_Scenes,

	GltfState_Buffer,
	GltfState_BufferView,
	GltfState_Accessor,
	GltfState_Image,
	GltfState_Sampler,
	GltfState_Texture,
	GltfState_Material,
	GltfState_Mesh,
	GltfState_Camera,
	GltfState_Camera_Orthographic,
	GltfState_Camera_Perspective,
	GltfState_Skin,
	GltfState_Node,
	GltfState_Animation,
	GltfState_Scene,

	GltfState_Accessor_Sparse,
	GltfState_Accessor_Sparse_Indices,
	GltfState_Accessor_Sparse_Values,
	GltfState_Material_PbrMetallicRoughness,
	GltfState_Material_Extensions,
	GltfState_Material_Extensions_PbrSpecularGlossiness,
	GltfState_Material_TextureInfo,
	GltfState_Mesh_Primitive,
	GltfState_Mesh_Primitive_Attributes,
	GltfState_Skin_Joints,
	GltfState_Node_Children,
	GltfState_Animation_Sampler,
	GltfState_Animation_Channel,
	GltfState_Animation_Channel_Target,
	GltfState_Scene_Node
};

typedef struct _GltfExtensions {
	VkBool32 required_pbrSpecularGlossiness;
	VkBool32 used_pbrSpecularGlossiness;
} GltfExtensions;

typedef struct _GltfBuffer {
	IBinaryBufferSP binaryBuffer;
	uint32_t byteLength;
	std::string name;
} GltfBuffer;

typedef struct _GltfBufferView {
	GltfBuffer* buffer;
    uint32_t byteOffset;
    uint32_t byteLength;
    uint32_t byteStride;
    int32_t target;
	std::string name;
} GltfBufferView;

typedef struct _GltfSparseIndex {
	uint32_t bufferView;
	uint32_t byteOffset;
	int32_t componentType;
	std::string name;
} GltfSparseIndex;

typedef struct _GltfSparseValue {
	uint32_t bufferView;
	uint32_t byteOffset;
	std::string name;
} GltfSparseValue;

typedef struct _GltfSparse {
	uint32_t count;
	std::vector<GltfSparseIndex> indices;
	std::vector<GltfSparseValue> values;
	std::string name;
} GltfSparse;

typedef struct _GltfAccessor {
	GltfBufferView* bufferView;
    uint32_t byteOffset;
    int32_t componentType;
    VkBool32 normalized;
    uint32_t count;
    std::string type;
    Vector<float> max;
    Vector<float> min;
    GltfSparse* sparse;
	std::string name;
} GltfAccessor;

typedef struct _GltfImage {
	IImageDataSP imageData;
	std::string name;
} GltfImage;

typedef struct _GltfSampler {
	int32_t magFilter;
	int32_t minFilter;
	int32_t wrapS;
	int32_t wrapT;
	std::string name;
} GltfSampler;

typedef struct _GltfTexture {
	int32_t internalFormat;
	int32_t format;
	GltfSampler* sampler;
	GltfImage* source;
	int32_t target;
	int32_t type;
	std::string name;
	//
	int32_t texCoord;
} GltfTexture;

typedef struct _GltfTextureInfo {
	int32_t index;
	int32_t texCoord;
	float normalScale;
	float occlusionStrength;
} GltfTextureInfo;

typedef struct _GltfPbrMetallicRoughness {
	float baseColorFactor[4];
	GltfTexture* baseColorTexture;
	float metallicFactor;
	float roughnessFactor;
	GltfTexture* metallicRoughnessTexture;
} GltfPbrMetallicRoughness;

typedef struct _GltfPbrSpecularGlossiness {
	float diffuseFactor[4];
	GltfTexture* diffuseTexture;
	float specularFactor[3];
	float glossinessFactor;
	GltfTexture* specularGlossinessTexture;
} GltfPbrSpecularGlossiness;

typedef struct _GltfMaterial {
	std::string alphaMode;
	float alphaCutoff;
	VkBool32 doubleSided;

	VkBool32 useSpecularGlossiness;
	GltfPbrMetallicRoughness pbrMetallicRoughness;
	GltfPbrSpecularGlossiness pbrSpecularGlossiness;

	float normalScale;
	GltfTexture* normalTexture;
	float occlusionStrength;
	GltfTexture* occlusionTexture;
	float emissiveFactor[3];
	GltfTexture* emissiveTexture;

	std::string name;
} GltfMaterial;

typedef struct _GltfPrimitive {
	GltfAccessor* position;
	GltfAccessor* normal;
	GltfAccessor* tangent;
	GltfAccessor* texCoord0;
	GltfAccessor* texCoord1;
	GltfAccessor* joints0;
	GltfAccessor* joints1;
	GltfAccessor* weights0;
	GltfAccessor* weights1;
	GltfAccessor* color0;
	GltfAccessor* color1;
	GltfAccessor* indices;
	int32_t mode;

	GltfMaterial* material;
	std::string name;
} GltfPrimitive;

typedef struct _GltfMesh {
	Vector<GltfPrimitive> primitives;
	std::string name;
} GltfMesh;

struct _GltfNode;

typedef struct _GltfOrthograpicCamera {
	float xmag;
	float ymag;
} GltfOrthograpicCamera;

typedef struct _GltfPerspectiveCamera {
	float aspectRatio;
	float yfov;
	//
	VkBool32 infinite;
} GltfPerspectiveCamera;

typedef struct _GltfCamera {
	GltfOrthograpicCamera orthograpic;
	GltfPerspectiveCamera perspective;

	float zfar;
	float znear;
	std::string type;
	std::string name;
} GltfCamera;

typedef struct _GltfSkin {
	GltfAccessor* inverseBindMatrices;
	uint32_t skeleton;
	Vector<uint32_t> joints;
	std::string name;
} GltfSkin;

typedef struct _GltfNode {
	Vector<uint32_t> children;
	GltfCamera* camera;
	GltfSkin* skin;
	float matrix[16];
	GltfMesh* mesh;
	float rotation[4];
	float scale[3];
	float translation[3];
	std::string name;
} GltfNode;

typedef struct _GltfAnimation_Sampler {
	GltfAccessor* input;
    std::string interpolation;
    GltfAccessor* output;
	std::string name;
} GltfAnimation_Sampler;

typedef struct _GltfChannel {
	GltfAnimation_Sampler* sampler;
	GltfNode* targetNode;
	std::string targetPath;
	std::string name;
} GltfChannel;

typedef struct _GltfAnimation {
	Vector<GltfAnimation_Sampler> samplers;
	Vector<GltfChannel> channels;
	std::string name;
} GltfAnimation;

typedef struct _GltfScene {
	Vector<GltfNode*> nodes;
	std::string name;
} GltfScene;

class GltfVisitor : public JsonVisitor
{

private:

	const std::string directory;

	const IBinaryBufferSP binaryBuffer;

	std::stack<enum GltfState> state;
	std::stack<enum GltfSubState> subState;

	VkBool32 gltfBool;
	std::string gltfString;
	int32_t gltfInteger;
	float gltfFloat;
	int32_t gltfIntegerArray[16];
	float gltfFloatArray[16];

	uint32_t arrayIndex;
	uint32_t arraySize;
	VkBool32 numberArray;

	VkBool32 objectArray;

	GltfExtensions gltfExtensions;

	GltfBuffer gltfBuffer;
	GltfBufferView gltfBufferView;
	GltfSparse gltfSparse;
	GltfSparseIndex gltfSparseIndex;
	GltfSparseValue gltfSparseValue;
	GltfAccessor gltfAccessor;
	GltfPrimitive gltfPrimitive;
	GltfImage gltfImage;
	GltfSampler gltfSampler;
	GltfTexture gltfTexture;
	GltfTextureInfo gltfTextureInfo;
	GltfMaterial gltfMaterial;
	GltfMesh gltfMesh;
	GltfCamera gltfCamera;
	GltfSkin gltfSkin;
	GltfNode gltfNode;
	GltfAnimation_Sampler gltfAnimation_Sampler;
	GltfChannel gltfChannel;
	GltfAnimation gltfAnimation;
	GltfScene gltfScene;

	Vector<GltfBuffer> allGltfBuffers;
	Vector<GltfBufferView> allGltfBufferViews;
	Vector<GltfSparse> allGltfSparses;
	Vector<GltfAccessor> allGltfAccessors;
	Vector<GltfImage> allGltfImages;
	Vector<GltfSampler> allGltfSamplers;
	Vector<GltfTexture> allGltfTextures;
	Vector<GltfMaterial> allGltfMaterials;
	Vector<GltfMesh> allGltfMeshes;
	Vector<GltfCamera> allGltfCameras;
	Vector<GltfSkin> allGltfSkins;
	Vector<GltfNode> allGltfNodes;
	Vector<GltfAnimation> allGltfAnimations;
	Vector<GltfScene> allGltfScenes;
	GltfScene* defaultScene;

	void visitBuffer(JSONobject& jsonObject);
	void visitBufferView(JSONobject& jsonObject);
	void visitAccessor(JSONobject& jsonObject);
	void visitImage(JSONobject& jsonObject);
	void visitSampler(JSONobject& jsonObject);
	void visitTexture(JSONobject& jsonObject);
	void visitMaterial(JSONobject& jsonObject);
	void visitMesh(JSONobject& jsonObject);
	void visitCameraOrthographic(JSONobject& jsonObject);
	void visitCameraPerspective(JSONobject& jsonObject);
	void visitCamera(JSONobject& jsonObject);
	void visitSkin(JSONobject& jsonObject);
	void visitNode(JSONobject& jsonObject);
	void visitAnimation(JSONobject& jsonObject);
	void visitScene(JSONobject& jsonObject);

	void visitAccessorSparse(JSONobject& jsonObject);
	void visitAccessorSparseIndex(JSONobject& jsonObject);
	void visitAccessorSparseValue(JSONobject& jsonObject);
	void visitMaterial_PbrMetallicRoughness(JSONobject& jsonObject);
	void visitMaterial_Extensions(JSONobject& jsonObject);
	void visitMaterial_Extensions_PbrSpecularGlossiness(JSONobject& jsonObject);
	void visitMaterial_TextureInfo(JSONobject& jsonObject);
	void visitMesh_Primitive(JSONobject& jsonObject);
	void visitMesh_Primitive_Attributes(JSONobject& jsonObject);
	void visitAnimation_Sampler(JSONobject& jsonObject);
	void visitAnimation_Channel(JSONobject& jsonObject);
	void visitAnimation_Channel_Target(JSONobject& jsonObject);

public:

	GltfVisitor() = delete;

	explicit GltfVisitor(const std::string& directory, const IBinaryBufferSP& binaryBuffer);

	virtual ~GltfVisitor();

	//

	virtual void visit(JSONnull& jsonNull) override;

	virtual void visit(JSONfalse& jsonFalse) override;

	virtual void visit(JSONtrue& jsonTrue) override;

	virtual void visit(JSONfloat& jsonFloat) override;

	virtual void visit(JSONinteger& jsonInteger) override;

	virtual void visit(JSONstring& jsonString) override;

	virtual void visit(JSONarray& jsonArray) override;

	virtual void visit(JSONobject& jsonObject) override;

	//

	const std::string& getDirectory() const;

	enum GltfState getState() const;

	//

	const Vector<GltfBuffer>& getAllGltfBuffers() const;

	const Vector<GltfBufferView>& getAllGltfBufferViews() const;

	const Vector<GltfAccessor>& getAllGltfAccessors() const;

	const Vector<GltfImage>& getAllGltfImages() const;

	const Vector<GltfSampler>& getAllGltfSamplers() const;

	const Vector<GltfTexture>& getAllGltfTextures() const;

	const Vector<GltfMaterial>& getAllGltfMaterials() const;

	const Vector<GltfMesh>& getAllGltfMeshes() const;

	const Vector<GltfSkin>& getAllGltfSkins() const;

	const Vector<GltfNode>& getAllGltfNodes() const;

	const Vector<GltfAnimation>& getAllGltfAnimations() const;

	const Vector<GltfScene>& getAllGltfScenes() const;

	//

	const GltfScene* getDefaultScene() const;

	//

	VkBool32 isByte(const int32_t componentType) const;

	VkBool32 isUnsignedByte(const int32_t componentType) const;

	VkBool32 isShort(const int32_t componentType) const;

	VkBool32 isUnsignedShort(const int32_t componentType) const;

	VkBool32 isUnsignedInt(const int32_t componentType) const;

	VkBool32 isFloat(const int32_t componentType) const;

	uint32_t getBytesPerComponent(const int32_t componentType) const;

	uint32_t getComponentsPerType(const std::string& type) const;

	const void* getBufferPointer(const GltfBufferView& bufferView) const;

	const void* getBufferPointer(const GltfAccessor& accessor, const uint32_t element) const;

	const int8_t* getBytePointer(const GltfAccessor& accessor, const uint32_t element) const;

	const uint8_t* getUnsignedBytePointer(const GltfAccessor& accessor, const uint32_t element) const;

	const int16_t* getShortPointer(const GltfAccessor& accessor, const uint32_t element) const;

	const uint16_t* getUnsignedShortPointer(const GltfAccessor& accessor, const uint32_t element) const;

	const uint32_t* getUnsignedIntPointer(const GltfAccessor& accessor, const uint32_t element) const;

	const float* getFloatPointer(const GltfAccessor& accessor, const uint32_t element) const;

};

}

#endif /* VKTS_GLTFVISITOR_HPP_ */
