// <Allocators.hpp> -*- C++ -*-

#pragma once

/*!
 * \file Allocators.hpp
  * \brief Defines a general TreeNode that contains all allocators used
 *        in simulation
 */

#include "sparta/simulation/TreeNode.hpp"

#include "core/AtlasInst.hpp"
#include "core/AtlasExtractor.hpp"

namespace atlas
{
    /*!
     * \class Allocators
     * \brief A TreeNode that is actually a functional resource
     *        containing memory allocators
     */
    class Allocators : public sparta::TreeNode
    {
    public:
        static constexpr char name[] = "allocators";

        Allocators(sparta::TreeNode *node) :
            sparta::TreeNode(node, name, "Allocators used in simulation")
        {}

        static Allocators * getAllocators(sparta::TreeNode *node)
        {
            Allocators * allocators = nullptr;
            if(node)
            {
                if(node->hasChild(Allocators::name)) {
                    allocators = node->getChildAs<Allocators>(Allocators::name);
                }
                else {
                    return getAllocators(node->getParent());
                }
            }
            return allocators;
        }

        AtlasInstAllocator      inst_allocator      {3000, 2500};
        AtlasExtractorAllocator extractor_allocator {3000, 2500};
    };
}
