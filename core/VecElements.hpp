#pragma once

#include <stdint.h>

#include "core/AtlasState.hpp"
#include "core/VecConfig.hpp"
#include "core/VecNums.hpp"
#include "include/AtlasUtils.hpp"

namespace atlas
{
    /**
     * @class Element
     * @brief A proxy representation for vector element of native integer type size.
     *
     * A local copy of element bits is stored. Neccessary information
     * to extract bits from vector register is kept, e.g. reg_id and idx.
     * It also tracks start and end position of the bits, so that only active
     * bits in Body part is accessible.
     * Bit Iterator is provided for bit iteration.
     *
     * @tparam ElemWidth Number of bits for this *Element* type.
     */
    template <size_t ElemWidth> struct Element
    {
      public:
        using ValueType = UintType<ElemWidth>;

        /**
         * @class BitIterator
         * @brief Used for bit iteration of *Element*.
         */
        class BitIterator
        {
          public:
            using value_type = uint8_t;
            using difference_type = std::ptrdiff_t;
            using iterator_category = std::forward_iterator_tag;

            /**
             * @brief Constructor. When called, *index_* is adjucted to first active bit.
             * @param elem_ptr Poninter to *Element* object upon which this iterator operates.
             * @param index *index_* will be set to the first active bit after this (inclusive).
             */
            BitIterator(const Element<ElemWidth>* elem_ptr, size_t index) : elem_ptr_(elem_ptr)
            {
                index_ = getNextIndex(index);
            }

            /**
             * @brief Pre-increment operator. *index_* will be set to the next active bit.
             * @return Reference to itself.
             */
            BitIterator & operator++()
            {
                index_ = this->getNextIndex(++index_);
                return *this;
            }

            /**
             * @brief Equal operator.
             * @return *true* if iterators point to the same *Element* and bit position.
             */
            bool operator==(const BitIterator & other) const
            {
                return index_ == other.index_ && elem_ptr_ == other.elem_ptr_;
            }

            /**
             * @brief Not equal operator.
             * @return *true* if not equal.
             */
            bool operator!=(const BitIterator & other) const { return !(*this == other); }

            /**
             * @brief Return current *index_*.
             * @return Current *index_*.
             */
            size_t getIndex() const { return index_; }

            /**
             * @brief Set *index_* to the next active bit.
             * @param index *index_* will be set to the next active bit after this (inclusive).
             * @return Index of next active bit.
             */
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
            /**< Pointer to *Element* object upon which this iterator operates. */
            const Element<ElemWidth>* elem_ptr_ = nullptr;
            /**< Index tracking current pointed bit. */
            size_t index_ = 0;
        }; // class BitIterator

        /**
         * @brief Default Constructor.
         */
        Element() = default;

        /**
         * @brief Constructor.
         *
         * @param state Poninter of *AtlasState* object.
         * @param reg_id vector register ID.
         * @param idx Index offset in the Register
         */
        Element(AtlasState* state, uint32_t reg_id, uint32_t idx) :
            state_(state),
            reg_id_(reg_id),
            idx_(idx)
        {
        }

        /**
         * @brief Return number of bits this *Element* type represents.
         * @return Number of bits.
         */
        static constexpr size_t getWidth() { return ElemWidth; }

        /**
         * @brief Set *state_*.
         */
        void setState(AtlasState* state) { state_ = state; }

        /**
         * @brief Set *reg_id_*.
         */
        void setRegId(uint32_t reg_id) { reg_id_ = reg_id; }

        /**
         * @brief Set *idx_*.
         */
        void setIdx(uint32_t idx) { idx_ = idx; }

        /**
         * @brief Get *start_pos_*.
         */
        size_t getStartPos() const { return start_pos_; }

        /**
         * @brief Set *start_pos_*.
         */
        void setStartPos(size_t start_pos) { start_pos_ = start_pos; }

        /**
         * @brief Get *end_pos_*.
         */
        size_t getEndPos() const { return end_pos_; }

        /**
         * @brief Set *end_pos_*.
         */
        void setEndPos(size_t end_pos) { end_pos_ = end_pos; }

        /**
         * @brief Generate active bit mask based on start and end position.
         * @return Bit mask represented in native integer type.
         */
        ValueType getMask() const
        {
            return (((ValueType)1 << (end_pos_ - start_pos_)) - 1) << start_pos_;
        }

        /**
         * @brief Get the active local element bits without retrieving from vector register.
         * @return Local element bits in native integer type.
         */
        ValueType peekVal() const { return val_; }

        /**
         * @brief Set the active local element bits without updating vector register.
         * @param value Value to set to.
         */
        void pokeVal(ValueType value) { val_ = value & getMask(); }

        /**
         * @brief Retrieve the active element bits from vector register.
         * @return Element bits in native integer type.
         */
        ValueType getVal()
        {
            val_ = READ_VEC_ELEM<ValueType>(state_, reg_id_, idx_) & getMask();
            return val_;
        }

        /**
         * @brief Update active element bits in vector register.
         * @param value Value to set to.
         */
        void setVal(ValueType value)
        {
            ValueType bitmask = getMask();
            val_ = value & bitmask;
            ValueType val = READ_VEC_ELEM<ValueType>(state_, reg_id_, idx_) & ~bitmask;
            WRITE_VEC_ELEM<ValueType>(state_, reg_id_, val | val_, idx_);
        }

        /**
         * @brief Return *begin* iterator object.
         * @return *BitIterator* object pointitng to the start position.
         */
        BitIterator begin() { return BitIterator(this, start_pos_); }

        /**
         * @brief Return *end* iterator object.
         * @return *BitIterator* object pointitng to the end position.
         */
        BitIterator end() { return BitIterator(this, end_pos_); }

      private:
        /**< Needed for vector register access. */
        AtlasState* state_ = nullptr;
        /**< vector register ID. */
        uint32_t reg_id_ = 0;
        /**< Index offset for vector register. */
        uint32_t idx_ = 0;
        /**< Start position of active bits. */
        size_t start_pos_ = 0;
        /**< End position of active bits. */
        size_t end_pos_ = ElemWidth;
        /**< Local bits of vector element. */
        ValueType val_ = 0;
    }; // struct Element

    template <bool isMaskElems>
    concept EnableIf = isMaskElems;

    /**
     * @class Elements
     * @brief A proxy representation for vector register group.
     *
     * This class focuses on efficient native integer type representation of
     * both vector register group and vector mask register.
     * Information to correctly represent vector register group is kept,
     * e.g. reg_id and *VectorConfig*, which includes LMUL, SEW, VLEN ... etc.
     * It also tracks start and end position of the active element based on
     * VSTART and VL from *VectorConfig*. Depending on what it's representing
     * (vector register groups or vector mask), start and end position are
     * calculated differently.
     * When representing vector mask register, it is in view of mutiple chunks
     * of native integer size (*Element*) instead of bit set. In this way
     * bit-wise operation of vector mask registers can be more efficient
     * performed.
     * Element iterator is provided for element iteration; bit iterator for
     * vector mask regitster bit iteration.
     *
     * @tparam ElemType Element type for individual element.
     * @tparam isMaskElems true if this *Elements* represents vector mask.
     */
    template <typename ElemType, bool isMaskElems> class Elements
    {
      public:
        /**
         * @class ElementIterator
         * @brief Used for *Element* iteration of *Elements*.
         */
        class ElementIterator
        {
          public:
            using value_type = ElemType;
            using pointer = ElemType*;
            using reference = ElemType &;
            using difference_type = std::ptrdiff_t;
            using iterator_category = std::forward_iterator_tag;

            /**
             * @brief Constructor. When called, *index_* is adjucted to first active *Element*.
             * @param elems_ptr Poninter to *Elements* object upon which this iterator operates.
             * @param index *index_* will be set to the first active *Element* after this
             * (inclusive).
             */
            ElementIterator(const Elements* elems_ptr, size_t index) : elems_ptr_(elems_ptr)
            {
                index_ = getNextIndex(index);
            }

            /**
             * @brief Dereference operator.
             * @return A newly constructed *Element* pointed to by current *index_*.
             */
            value_type operator*() { return elems_ptr_->getElement(index_); }

            /**
             * @brief Pointer version of dereference operator.
             * @return A pointer to newly constructed *Element* pointed to by current *index_*.
             */
            pointer operator->() { return &(operator*()); }

            /**
             * @brief Pre-increment operator. *index_* will be set to the next active *Element*.
             * @return Reference to itself.
             */
            ElementIterator & operator++()
            {
                index_ = this->getNextIndex(++index_);
                return *this;
            }

            /**
             * @brief Equal operator.
             * @return *true* if iterators point to the same *Elements* and *Element* position.
             */
            bool operator==(const ElementIterator & other) const
            {
                return index_ == other.index_ && elems_ptr_ == other.elems_ptr_;
            }

            /**
             * @brief Not equal operator.
             * @return *true* if not equal.
             */
            bool operator!=(const ElementIterator & other) const { return !(*this == other); }

            /**
             * @brief Return current *index_*.
             * @return Current *index_*.
             */
            size_t getIndex() const { return index_; }

            /**
             * @brief Set *index_* to the next active *Element*. Only range checks are performed
             * here.
             * @param index *index_* will be set to the next active *Element* after this
             * (inclusive).
             * @return Index of next active *Element*.
             */
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
            /**< Pointer to *Elements* object upon which this iterator operates. */
            const Elements* elems_ptr_ = nullptr;
            /**< Index tracking current pointed *Element*. */
            size_t index_ = 0;
        }; // class ElementIterator

        template <bool M = isMaskElems>
        requires EnableIf<M>

        /**
         * @class MaskBitIterator
         * @brief Used for bit iteration of *MastElements*.
         */
        class MaskBitIterator
        {
          public:
            using value_type = uint8_t;
            using difference_type = std::ptrdiff_t;
            using iterator_category = std::forward_iterator_tag;

            /**
             * @brief Constructor. When called, *index_* is adjucted to first active bit.
             * @param elems_ptr Poninter to *Elements* object upon which this iterator operates.
             * @param index *index_* will be set to the first active bit after this (inclusive).
             */
            MaskBitIterator(const Elements* elems_ptr, size_t index) : elems_ptr_(elems_ptr)
            {
                index_ = getNextIndex(index);
            }

            /**
             * @brief Pre-increment operator. *index_* will be set to the next active bit.
             * @return Reference to itself.
             */
            MaskBitIterator & operator++()
            {
                index_ = this->getNextIndex(++index_);
                return *this;
            }

            /**
             * @brief Equal operator.
             * @return *true* if iterators point to the same *Elements* and bit position.
             */
            bool operator==(const MaskBitIterator & other) const
            {
                return index_ == other.index_ && elems_ptr_ == other.elems_ptr_;
            }

            /**
             * @brief Not equal operator.
             * @return *true* if not equal.
             */
            bool operator!=(const MaskBitIterator & other) const { return !(*this == other); }

            /**
             * @brief Return current *index_*.
             * @return Current *index_*.
             */
            size_t getIndex() const { return index_; }

            /**
             * @brief Set *index_* to the next active bit.
             * @param index *index_* will be set to the next active bit after this (inclusive).
             * @return Index of next active bit.
             */
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
            /**< Pointer to *Elements* object upon which this iterator operates. */
            const Elements* elems_ptr_ = nullptr;
            /**< Index tracking current pointed bit. */
            size_t index_ = 0;
        }; // class MaskBitIterator

        /**
         * @brief Constructor for vector register group *Elements*.
         *
         * @param state Poninter to *AtlasState* object.
         * @param config Poninter to *VectorConfig* object.
         * @param reg_id vector register ID.
         */
        Elements(AtlasState* state, VectorConfig* config, uint32_t reg_id)
        requires EnableIf<!isMaskElems>
            :
            state_(state),
            config_(config),
            start_pos_(config_->getVSTART()),
            end_pos_(config_->getVL()),
            reg_id_(reg_id)
        {
        }

        /**
         * @brief Constructor for vector mask *Elements*.
         *
         * @param state Poninter to *AtlasState* object.
         * @param config Poninter to *VectorConfig* object.
         * @param reg_id vector register ID.
         */
        Elements(AtlasState* state, VectorConfig* config, uint32_t reg_id)
        requires EnableIf<isMaskElems>
            :
            state_(state),
            config_(config),
            start_pos_(config_->getVSTART() / VLEN_MIN),
            end_pos_((config_->getVL() + VLEN_MIN - 1) / VLEN_MIN),
            reg_id_(reg_id)
        {
        }

        /**
         * @brief Return *begin* *ElementIterator* object.
         * @return *ElementIterator* object pointitng to the starting *Element*.
         */
        ElementIterator begin() const { return ElementIterator(this, start_pos_); }

        /**
         * @brief Return *end* *ElementIterator* object.
         * @return *ElementIterator* object pointitng to the ending *Element*.
         */
        ElementIterator end() const { return ElementIterator(this, end_pos_); }

        /**
         * @brief Return *begin* *MaskBitIterator* object.
         * @return *MaskBitIterator* object pointitng to the starting bit.
         */
        auto maskBitIterBegin() const
        requires EnableIf<isMaskElems>
        {
            return MaskBitIterator<>(this, config_->getVSTART());
        }

        /**
         * @brief Return *end* *MaskBitIterator* object.
         * @return *MaskBitIterator* object pointitng to the ending bit.
         */
        auto maskBitIterEnd() const
        requires EnableIf<isMaskElems>
        {
            return MaskBitIterator<>(this, config_->getVL());
        }

        /**
         * @brief Get *Element* from *Elements* at *index*.
         *
         * *Element* start and end position will be adjusted depending on whether it's
         * vector register group *Element* or vector mask *Element*.
         *
         * @param index The index of requested *Element*.
         * @return A newly constructed *Element* to which *index* points.
         */
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
        /**< Pointer to *AtlasState* object. */
        AtlasState* state_ = nullptr;
        /**< Pointer to *VectorConfig* object. */
        const VectorConfig* config_ = nullptr;
        /**< Starting *Element* position. */
        size_t start_pos_ = 0;
        /**< Ending *Element* position. */
        size_t end_pos_ = 0;
        /**< Vector register ID. */
        uint32_t reg_id_ = 0;
    }; // class Elements

    using MaskElement = Element<VLEN_MIN>;
    using MaskElements = Elements<MaskElement, true>;
    using MaskBitIterator = MaskElements::MaskBitIterator<>;
} // namespace atlas
