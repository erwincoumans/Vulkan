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

#ifndef VKTS_SCENEFACTORY_HPP_
#define VKTS_SCENEFACTORY_HPP_

#include <vkts/scenegraph/vkts_scenegraph.hpp>

namespace vkts
{

class SceneFactory : public ISceneFactory
{

private:

	const ISceneRenderFactorySP sceneRenderFactory;

	const VkBool32 gpu;

public:

	SceneFactory() = delete;

	SceneFactory(const ISceneRenderFactorySP& sceneRenderFactory, const VkBool32 gpu);

    virtual ~SceneFactory();

    virtual ISceneRenderFactorySP getSceneRenderFactory() const override;

    //

    virtual IPhongMaterialSP createPhongMaterial(const ISceneManagerSP& sceneManager, const VkBool32 forwardRendering) override;

    //

    virtual IBSDFMaterialSP createBSDFMaterial(const ISceneManagerSP& sceneManager, const VkBool32 forwardRendering) override;

    //

    virtual ISubMeshSP createSubMesh(const ISceneManagerSP& sceneManager) override;

    virtual IMeshSP createMesh(const ISceneManagerSP& sceneManager) override;

    //

    virtual IChannelSP createChannel(const ISceneManagerSP& sceneManager) override;

    virtual IMarkerSP createMarker(const ISceneManagerSP& sceneManager) override;

    virtual IAnimationSP createAnimation(const ISceneManagerSP& sceneManager) override;

    //

    virtual ICameraSP createCamera(const ISceneManagerSP& sceneManager) override;

    virtual ILightSP createLight(const ISceneManagerSP& sceneManager) override;

    //

    virtual IConstraintSP createCopyConstraint(const ISceneManagerSP& sceneManager, const enum CopyConstraintType copyConstraintType) override;

    virtual IConstraintSP createLimitConstraint(const ISceneManagerSP& sceneManager, const enum LimitConstraintType limitConstraintType) override;

    //

    virtual INodeSP createNode(const ISceneManagerSP& sceneManager) override;

    //

    virtual IObjectSP createObject(const ISceneManagerSP& sceneManager) override;

    //

    virtual ISceneSP createScene(const ISceneManagerSP& sceneManager) override;

    //

    virtual VkBool32 useGPU() const override;

};

} /* namespace vkts */

#endif /* VKTS_SCENEFACTORY_HPP_ */
