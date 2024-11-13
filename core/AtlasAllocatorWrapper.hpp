#pragma once

#include "sparta/utils/SpartaSharedPointerAllocator.hpp"

namespace atlas
{
    template <typename AllocatorT> class AtlasInstAllocatorWrapper
    {
      public:
        ~AtlasInstAllocatorWrapper()
        {
            std::cout << "AtlasInstAllocatorWrapper: " << allocator_.getNumAllocated()
                      << " objects allocated/created" << std::endl;
        }

        explicit AtlasInstAllocatorWrapper(AllocatorT & allocator) : allocator_(allocator) {}

        // Used by Mavis for the type allocated
        using InstTypePtr = sparta::SpartaSharedPointer<typename AllocatorT::element_type>;

        // Called by Mavis when creating a new instruction
        template <typename... Args> InstTypePtr operator()(Args &&... args)
        {
            return sparta::allocate_sparta_shared_pointer<typename AllocatorT::element_type>(
                allocator_, std::forward<Args>(args)...);
        }

      private:
        AllocatorT & allocator_;
    };

    template <typename AllocatorT> class AtlasExtractorAllocatorWrapper
    {
      public:
        ~AtlasExtractorAllocatorWrapper()
        {
            std::cout << "AtlasExtractorAllocatorWrapper: " << allocator_.getNumAllocated()
                      << " objects allocated/created" << std::endl;
        }

        AtlasExtractorAllocatorWrapper(AllocatorT & allocator, AtlasState* state) :
            allocator_(allocator),
            state_(state)
        {
        }

        // Used by Mavis for the type allocated
        using InstTypePtr = sparta::SpartaSharedPointer<typename AllocatorT::element_type>;

        // Called by Mavis when creating a new instruction
        template <typename... Args> InstTypePtr operator()(Args &&... args)
        {
            return sparta::allocate_sparta_shared_pointer<typename AllocatorT::element_type>(
                allocator_, std::forward<Args>(args)..., state_);
        }

      private:
        AllocatorT & allocator_;
        AtlasState* state_ = nullptr;
    };
} // namespace atlas
