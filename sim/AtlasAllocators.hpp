// <AtlasAllocators.hpp> -*- C++ -*-

#pragma once

/*!
 * \file AtlasAllocators.hpp
 * \brief Defines a general TreeNode that contains all allocators used
 *        in simulation
 */

#include "sparta/simulation/TreeNode.hpp"

#include "core/AtlasInst.hpp"
#include "core/AtlasExtractor.hpp"

namespace atlas
{
    /*!
     * \class AtlasAllocators
     * \brief A TreeNode that is actually a functional resource
     *        containing memory allocators
     */
    class AtlasAllocators : public sparta::TreeNode
    {
      public:
        static constexpr char name[] = "allocators";

        AtlasAllocators(sparta::TreeNode* node) :
            sparta::TreeNode(node, name, "Allocators used in simulation")
        {
        }

        static AtlasAllocators* getAllocators(sparta::TreeNode* node)
        {
            AtlasAllocators* allocators = nullptr;
            if (node)
            {
                if (node->hasChild(AtlasAllocators::name))
                {
                    allocators = node->getChildAs<AtlasAllocators>(AtlasAllocators::name);
                }
                else
                {
                    return getAllocators(node->getParent());
                }
            }
            return allocators;
        }

        AtlasInstAllocator inst_allocator{1000, 500};
        AtlasExtractorAllocator extractor_allocator{5000, 4500};
    };
} // namespace atlas
