#pragma once

#include <cinttypes>
#include <array>

namespace atlas::translate_types
{
    struct FieldDef
    {
        const uint64_t bitmask;
        const uint32_t msb;
        const uint32_t lsb;

        FieldDef() : bitmask(0xffffffffffffffff), msb(63), lsb(0) {}

        FieldDef(const uint64_t bm, const uint32_t mb, const uint32_t lb) :
            bitmask(bm),
            msb(mb),
            lsb(lb)
        {
        }

        FieldDef(const FieldDef & def) = default;
    };

    namespace Sv32
    {
        namespace PteFields
        {
            static const FieldDef ppn1{0x00000000fff00000, 31, 20};
            static const FieldDef ppn0{0x00000000000ffc00, 19, 10};
            static const FieldDef rsw{0x0000000000000300, 9, 8};
            static const FieldDef dirty{0x0000000000000080, 7, 7};
            static const FieldDef accessed{0x0000000000000040, 6, 6};
            static const FieldDef global{0x0000000000000020, 5, 5};
            static const FieldDef user{0x0000000000000010, 4, 4};
            static const FieldDef execute{0x0000000000000008, 3, 3};
            static const FieldDef write{0x0000000000000004, 2, 2};
            static const FieldDef read{0x0000000000000002, 1, 1};
            static const FieldDef valid{0x0000000000000001, 0, 0};
        } // namespace PteFields

        namespace VAddrFields
        {
            static const FieldDef vpn1{0x00000000ffc00000, 31, 22};
            static const FieldDef vpn0{0x00000000003ff000, 21, 12};
            static const FieldDef page_offset{0x0000000000000fff, 11, 0};
        } // namespace VAddrFields

        static constexpr uint32_t num_ppn_fields = 2;
        static const std::array<FieldDef, num_ppn_fields> ppn_fields{PteFields::ppn0,
                                                                     PteFields::ppn1};
        static constexpr uint32_t num_vpn_fields = 2;
        static const std::array<FieldDef, num_vpn_fields> vpn_fields{VAddrFields::vpn0,
                                                                     VAddrFields::vpn1};
        static constexpr uint32_t num_pagewalk_levels = 2;
    } // namespace Sv32

    namespace Sv39
    {
        namespace PteFields
        {
            static const FieldDef ppn2{0x003ffffff0000000, 53, 28};
            static const FieldDef ppn1{0x000000000ff80000, 27, 19};
            static const FieldDef ppn0{0x000000000007fc00, 18, 10};
            static const FieldDef rsw{0x0000000000000300, 9, 8};
            static const FieldDef dirty{0x0000000000000080, 7, 7};
            static const FieldDef accessed{0x0000000000000040, 6, 6};
            static const FieldDef global{0x0000000000000020, 5, 5};
            static const FieldDef user{0x0000000000000010, 4, 4};
            static const FieldDef execute{0x0000000000000008, 3, 3};
            static const FieldDef write{0x0000000000000004, 2, 2};
            static const FieldDef read{0x0000000000000002, 1, 1};
            static const FieldDef valid{0x0000000000000001, 0, 0};
        } // namespace PteFields

        namespace VAddrFields
        {
            static const FieldDef vpn2{0x0000007fc0000000, 38, 30};
            static const FieldDef vpn1{0x000000003fe00000, 29, 21};
            static const FieldDef vpn0{0x00000000001ff000, 20, 12};
            static const FieldDef page_offset{0x0000000000000fff, 11, 0};
        } // namespace VAddrFields

        static constexpr uint32_t num_ppn_fields = 3;
        static const std::array<FieldDef, num_ppn_fields> ppn_fields;
        static constexpr uint32_t num_vpn_fields = 3;
        static const std::array<FieldDef, num_vpn_fields> vpn_fields;
        static constexpr uint32_t num_pagewalk_levels = 3;
    } // namespace Sv39

    namespace Sv48
    {
        namespace PteFields
        {
            static const FieldDef ppn3{0x003fffe000000000, 53, 37};
            static const FieldDef ppn2{0x0000001ff0000000, 36, 28};
            static const FieldDef ppn1{0x000000000ff80000, 27, 19};
            static const FieldDef ppn0{0x000000000007fc00, 18, 10};
            static const FieldDef rsw{0x0000000000000300, 9, 8};
            static const FieldDef dirty{0x0000000000000080, 7, 7};
            static const FieldDef accessed{0x0000000000000040, 6, 6};
            static const FieldDef global{0x0000000000000020, 5, 5};
            static const FieldDef user{0x0000000000000010, 4, 4};
            static const FieldDef execute{0x0000000000000008, 3, 3};
            static const FieldDef write{0x0000000000000004, 2, 2};
            static const FieldDef read{0x0000000000000002, 1, 1};
            static const FieldDef valid{0x0000000000000001, 0, 0};
        } // namespace PteFields

        namespace VAddrFields
        {
            static const FieldDef vpn3{0x0000ff8000000000, 47, 39};
            static const FieldDef vpn2{0x0000007fc0000000, 38, 30};
            static const FieldDef vpn1{0x000000003fe00000, 29, 21};
            static const FieldDef vpn0{0x00000000001ff000, 20, 12};
            static const FieldDef page_offset{0x0000000000000fff, 11, 0};
        } // namespace VAddrFields

        static constexpr uint32_t num_ppn_fields = 4;
        static const std::array<FieldDef, num_ppn_fields> ppn_fields;
        static constexpr uint32_t num_vpn_fields = 4;
        static const std::array<FieldDef, num_vpn_fields> vpn_fields;
        static constexpr uint32_t num_pagewalk_levels = 4;
    } // namespace Sv48

    namespace Sv57
    {
        namespace PteFields
        {
            static const FieldDef ppn4{0x003fc00000000000, 53, 46};
            static const FieldDef ppn3{0x00003fe000000000, 45, 37};
            static const FieldDef ppn2{0x0000001ff0000000, 36, 28};
            static const FieldDef ppn1{0x000000000ff80000, 27, 19};
            static const FieldDef ppn0{0x000000000007fc00, 18, 10};
            static const FieldDef rsw{0x0000000000000300, 9, 8};
            static const FieldDef dirty{0x0000000000000080, 7, 7};
            static const FieldDef accessed{0x0000000000000040, 6, 6};
            static const FieldDef global{0x0000000000000020, 5, 5};
            static const FieldDef user{0x0000000000000010, 4, 4};
            static const FieldDef execute{0x0000000000000008, 3, 3};
            static const FieldDef write{0x0000000000000004, 2, 2};
            static const FieldDef read{0x0000000000000002, 1, 1};
            static const FieldDef valid{0x0000000000000001, 0, 0};
        } // namespace PteFields

        namespace VAddrFields
        {
            static const FieldDef vpn4{0x01ff000000000000, 56, 48};
            static const FieldDef vpn3{0x0000ff8000000000, 47, 39};
            static const FieldDef vpn2{0x0000007fc0000000, 38, 30};
            static const FieldDef vpn1{0x000000003fe00000, 29, 21};
            static const FieldDef vpn0{0x00000000001ff000, 20, 12};
            static const FieldDef page_offset{0x0000000000000fff, 11, 0};
        } // namespace VAddrFields

        static constexpr uint32_t num_ppn_fields = 5;
        static const std::array<FieldDef, num_ppn_fields> ppn_fields;
        static constexpr uint32_t num_vpn_fields = 5;
        static const std::array<FieldDef, num_vpn_fields> vpn_fields;
        static constexpr uint32_t num_pagewalk_levels = 5;
    } // namespace Sv57
} // namespace atlas::translate_types
