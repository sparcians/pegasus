#pragma once

#include <stdint.h>

#include "core/AtlasState.hpp"
#include "core/VecConfig.hpp"
#include "core/VecNums.hpp"
#include "include/AtlasUtils.hpp"

namespace atlas
{
    template <size_t ElemWidth> struct Element
    {
      public:
        using ValueType = UintType<ElemWidth>;

        class BitIterator
        {
          public:
            using value_type = uint8_t;
            using difference_type = std::ptrdiff_t;
            using iterator_category = std::forward_iterator_tag;

            BitIterator(const Element<ElemWidth>* elem_ptr, size_t index) : elem_ptr_(elem_ptr)
            {
                index_ = getNextIndex(index);
            }

            BitIterator & operator++()
            {
                index_ = this->getNextIndex(++index_);
                return *this;
            }

            bool operator==(const BitIterator & other) const
            {
                return index_ == other.index_ && elem_ptr_ == other.elem_ptr_;
            }

            bool operator!=(const BitIterator & other) const { return !(*this == other); }

            size_t getIndex() const { return index_; }

            size_t getNextIndex(size_t index) const
            {
                const size_t end_pos = elem_ptr_->getEndPos();
                index = std::max(index, elem_ptr_->getStartPos());
                if (index >= end_pos)
                {
                    return end_pos;
                }
                while (index < end_pos)
                {
                    if ((elem_ptr_->peekVal() >> index) & 1)
                    {
                        break;
                    }
                    ++index;
                }
                return index;
            }

          private:
            const Element<ElemWidth>* elem_ptr_ = nullptr;
            size_t index_ = 0;
        }; // class BitIterator

        Element() = default;

        Element(AtlasState* state, uint32_t reg_id, uint32_t idx) :
            state_(state),
            reg_id_(reg_id),
            idx_(idx)
        {
        }

        static constexpr size_t getWidth() { return ElemWidth; }

        void setState(AtlasState* state) { state_ = state; }

        void setRegId(uint32_t reg_id) { reg_id_ = reg_id; }

        void setIdx(uint32_t idx) { idx_ = idx; }

        size_t getStartPos() const { return start_pos_; }

        void setStartPos(size_t start_pos) { start_pos_ = start_pos; }

        size_t getEndPos() const { return end_pos_; }

        void setEndPos(size_t end_pos) { end_pos_ = end_pos; }

        ValueType peekVal() const { return val_; }

        ValueType getVal()
        {
            ValueType bitmask = (((ValueType)1 << (end_pos_ - start_pos_)) - 1) << start_pos_;
            val_ = READ_VEC_ELEM<ValueType>(state_, reg_id_, idx_) & bitmask;
            return val_;
        }

        void setVal(ValueType value)
        {
            ValueType bitmask = (((ValueType)1 << (end_pos_ - start_pos_)) - 1) << start_pos_;
            val_ = value & bitmask;
            ValueType val = READ_VEC_ELEM<ValueType>(state_, reg_id_, idx_) & !bitmask;
            WRITE_VEC_ELEM<ValueType>(state_, reg_id_, val | val_, idx_);
        }

        BitIterator begin() { return BitIterator(this, start_pos_); }

        BitIterator end() { return BitIterator(this, end_pos_); }

      private:
        AtlasState* state_ = nullptr;
        uint32_t reg_id_ = 0;
        uint32_t idx_ = 0;
        size_t start_pos_ = 0;
        size_t end_pos_ = ElemWidth;
        ValueType val_ = 0;
    }; // struct Element

    template <bool isMaskElems>
    concept EnableIf = isMaskElems;

    template <typename ElemType, bool isMaskElems> class Elements
    {
      public:
        class ElementIterator
        {
          public:
            using value_type = ElemType;
            using pointer = ElemType*;
            using reference = ElemType &;
            using difference_type = std::ptrdiff_t;
            using iterator_category = std::forward_iterator_tag;

            ElementIterator(const Elements* elems_ptr, size_t index) : elems_ptr_(elems_ptr)
            {
                index_ = getNextIndex(index);
            }

            value_type operator*() { return elems_ptr_->getElement(index_); }

            pointer operator->() { return &(operator*()); }

            ElementIterator & operator++()
            {
                index_ = this->getNextIndex(++index_);
                return *this;
            }

            bool operator==(const ElementIterator & other) const
            {
                return index_ == other.index_ && elems_ptr_ == other.elems_ptr_;
            }

            bool operator!=(const ElementIterator & other) const { return !(*this == other); }

            size_t getIndex() const { return index_; }

            size_t getNextIndex(size_t index) const
            {
                const size_t end_pos = elems_ptr_->end_pos_;
                index = std::max(index, elems_ptr_->start_pos_);
                if (index >= end_pos)
                {
                    return end_pos;
                }
                return index;
            }

          private:
            const Elements* elems_ptr_ = nullptr;
            size_t index_ = 0;
        }; // class ElementIterator

        template <bool M = isMaskElems>
        requires EnableIf<M>

        class MaskBitIterator
        {
          public:
            using value_type = uint8_t;
            using difference_type = std::ptrdiff_t;
            using iterator_category = std::forward_iterator_tag;

            MaskBitIterator(const Elements* elems_ptr, size_t index) : elems_ptr_(elems_ptr)
            {
                index_ = getNextIndex(index);
            }

            MaskBitIterator & operator++()
            {
                index_ = this->getNextIndex(++index_);
                return *this;
            }

            bool operator==(const MaskBitIterator & other) const
            {
                return index_ == other.index_ && elems_ptr_ == other.elems_ptr_;
            }

            bool operator!=(const MaskBitIterator & other) const { return !(*this == other); }

            size_t getIndex() const { return index_; }

            size_t getNextIndex(size_t index) const
            {
                const size_t vl = elems_ptr_->config_->getVL();
                const size_t elem_offset = index / VLEN_MIN;
                index = std::max(index, elems_ptr_->config_->getVSTART());
                if (index >= vl)
                {
                    return vl;
                }
                for (auto mask_elem_iter = ElementIterator{elems_ptr_, elem_offset};
                     mask_elem_iter != ElementIterator{elems_ptr_, elems_ptr_->end_pos_};
                     ++mask_elem_iter)
                {
                    auto mask_elem = *mask_elem_iter;
                    mask_elem.getVal();
                    // If we don't have a hit in the first Element for this index, then we shall
                    // start from bit 0 of next Element.
                    const size_t bit_offset =
                        mask_elem_iter.getIndex() == elem_offset ? index % VLEN_MIN : 0;
                    for (auto bit_iter = Element<VLEN_MIN>::BitIterator{&mask_elem, bit_offset};
                         bit_iter != mask_elem.end(); ++bit_iter)
                    {
                        return mask_elem_iter.getIndex() * VLEN_MIN + bit_iter.getIndex();
                    }
                }
                return vl;
            }

          private:
            const Elements* elems_ptr_ = nullptr;
            size_t index_ = 0;
        }; // class MaskBitIterator

        Elements(AtlasState* state, VectorConfig* config,
                 uint32_t reg_id) requires EnableIf<!isMaskElems> :
            state_(state),
            config_(config),
            start_pos_(config_->getVSTART()),
            end_pos_(config_->getVL()),
            reg_id_(reg_id)
        {
        }

        Elements(AtlasState* state, VectorConfig* config,
                 uint32_t reg_id) requires EnableIf<isMaskElems> :
            state_(state),
            config_(config),
            start_pos_(config_->getVSTART() / VLEN_MIN),
            end_pos_((config_->getVL() + VLEN_MIN - 1) / VLEN_MIN),
            reg_id_(reg_id)
        {
        }

        ElementIterator begin() const { return ElementIterator(this, start_pos_); }

        ElementIterator end() const { return ElementIterator(this, end_pos_); }

        auto maskBitIterBegin() const requires EnableIf<isMaskElems>
        {
            return MaskBitIterator<>(this, config_->getVSTART());
        }

        auto maskBitIterEnd() const requires EnableIf<isMaskElems>
        {
            return MaskBitIterator<>(this, config_->getVL());
        }

        ElemType getElement(size_t index) const
        {
            uint32_t reg_id = reg_id_ + index / (config_->getVLEN() / ElemType::getWidth());
            uint32_t idx = index % (config_->getVLEN() / ElemType::getWidth());
            ElemType elem{state_, reg_id, idx};
            if constexpr (isMaskElems)
            {
                // Fix *start_pos_* if first element.
                if (index == start_pos_)
                {
                    elem.setStartPos(config_->getVSTART() % VLEN_MIN);
                }
                else
                {
                    elem.setStartPos(0);
                }
                // Fix *end_pos_* if last element.
                if (index == end_pos_ - 1)
                {
                    const size_t vl = config_->getVL();
                    if (vl && vl % VLEN_MIN == 0)
                    {
                        elem.setEndPos(VLEN_MIN);
                    }
                    else
                    {
                        elem.setEndPos(vl % VLEN_MIN);
                    }
                }
                else
                {
                    elem.setEndPos(VLEN_MIN);
                }
            }
            return elem;
        }

      private:
        AtlasState* state_ = nullptr;
        const VectorConfig* config_ = nullptr;
        size_t start_pos_ = 0;
        size_t end_pos_ = 0;
        uint32_t reg_id_ = 0;
    }; // class Elements

    using MaskElements = Elements<Element<VLEN_MIN>, true>;
    using MaskBitIterator = MaskElements::MaskBitIterator<>;
} // namespace atlas
