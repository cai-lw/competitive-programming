#include <cstdint>

namespace cplib::impl {

template<typename T> struct make_double_width {};
template<> struct make_double_width<uint8_t> { using type = uint16_t; };
template<> struct make_double_width<uint16_t> { using type = uint32_t; };
template<> struct make_double_width<uint32_t> { using type = uint64_t; };
template<> struct make_double_width<uint64_t> { using type = unsigned __int128; };

template<typename T>
using make_double_width_t = typename make_double_width<T>::type;

}  // namespace cplib::impl