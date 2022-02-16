#include <cstdint>
#include <cstring>
#include <random>
#include <string>

namespace cplib {

namespace impl {
/* wyhash.h adopted from https://github.com/wangyi-fudan/wyhash/blob/master/wyhash.h
 * by removing code for other platforms, only leaving support for x86_64 gcc.
 */

//128bit multiply function
static inline void _wymum(uint64_t *A, uint64_t *B){
  __uint128_t r=*A; r*=*B; 
  *A=(uint64_t)r; *B=(uint64_t)(r>>64);
}

//multiply and xor mix function, aka MUM
static inline uint64_t _wymix(uint64_t A, uint64_t B){ _wymum(&A,&B); return A^B; }

//read functions
static inline uint64_t _wyr8(const uint8_t *p) { uint64_t v; memcpy(&v, p, 8); return v;}
static inline uint64_t _wyr4(const uint8_t *p) { uint32_t v; memcpy(&v, p, 4); return v;}
static inline uint64_t _wyr3(const uint8_t *p, size_t k) { return (((uint64_t)p[0])<<16)|(((uint64_t)p[k>>1])<<8)|p[k-1];}

//wyhash main function
static inline uint64_t wyhash(const void *key, size_t len, uint64_t seed, const uint64_t *secret){
  const uint8_t *p=(const uint8_t *)key; seed^=*secret;	uint64_t	a,	b;
  if(__builtin_expect(len<=16,true)){
    if(__builtin_expect(len>=4,true)){ a=(_wyr4(p)<<32)|_wyr4(p+((len>>3)<<2)); b=(_wyr4(p+len-4)<<32)|_wyr4(p+len-4-((len>>3)<<2)); }
    else if(__builtin_expect(len>0,true)){ a=_wyr3(p,len); b=0;}
    else a=b=0;
  }
  else{
    size_t i=len; 
    if(__builtin_expect(i>48,false)){
      uint64_t see1=seed, see2=seed;
      do{
        seed=_wymix(_wyr8(p)^secret[1],_wyr8(p+8)^seed);
        see1=_wymix(_wyr8(p+16)^secret[2],_wyr8(p+24)^see1);
        see2=_wymix(_wyr8(p+32)^secret[3],_wyr8(p+40)^see2);
        p+=48; i-=48;
      }while(__builtin_expect(i>48,true));
      seed^=see1^see2;
    }
    while(__builtin_expect(i>16,false)){  seed=_wymix(_wyr8(p)^secret[1],_wyr8(p+8)^seed);  i-=16; p+=16;  }
    a=_wyr8(p+i-16);  b=_wyr8(p+i-8);
  }
  return _wymix(secret[1]^len,_wymix(a^secret[1],b^seed));
}

//the default secret parameters
static const uint64_t _wyp[4] = {0xa0761d6478bd642full, 0xe7037ed1a0b428dbull, 0x8ebc6af09c88c6e3ull, 0x589965cc75374cc3ull};

//a useful 64bit-64bit mix function to produce deterministic pseudo random numbers that can pass BigCrush and PractRand
static inline uint64_t wyhash64(uint64_t A, uint64_t B){ A^=0xa0761d6478bd642full; B^=0xe7037ed1a0b428dbull; _wymum(&A,&B); return _wymix(A^0xa0761d6478bd642full,B^0xe7037ed1a0b428dbull);}

/* end of wyhash.h */

uint64_t gen_random_seed() {
    std::random_device rd;
    std::uniform_int_distribution<uint64_t> dis(0, std::numeric_limits<uint64_t>::max());
    return dis(rd);
}

}  // namespace impl

/**
 * \relates WyHash
 * \brief Hash function for arbitrary bytes using wyhash.
 * 
 * Calls wyhash from https://github.com/wangyi-fudan/wyhash/blob/master/wyhash.h, with a random seed initialized
 * from system random source on startup.
 * 
 * Wyhash is chosen for several characteristics that are specially suited for competitive programming:
 * * One of the fastest general purpose hash functions as of 2022.
 * * Very short implementation, little concern about code length limit.
 * * No known simple attack that can be reasonably carried out during a contest.
 * * Largely unknown in the competitive programming community, further reducing the likelihood of being hacked.
 * 
 * \see WyHash<std::string> an example of implementing WyHash specialization using this function.
 */
static inline uint64_t wyhash_bytes(const void *key, size_t len) {
    static const uint64_t seed = impl::gen_random_seed();
    return impl::wyhash(key, len, seed, impl::_wyp);
}

/**
 * \relates WyHash
 * \brief Combine two hash values to produce a new hash value.
 * 
 * Calls wyhash64 from https://github.com/wangyi-fudan/wyhash/blob/master/wyhash.h.
 * 
 * Use this to implement hash function for types with multiple fields that are recursively hashed, similar to how
 * [boost::hash_combine](https://www.boost.org/doc/libs/1_78_0/doc/html/hash/combine.html) is used for the same purpose.
 * 
 * \see WyHash<std::pair<T1, T2>> an example of implementing WyHash specialization using this function.
 */
static inline uint64_t wyhash_combine(uint64_t a, uint64_t b) {
    return impl::wyhash64(a, b);
}

/**
 * \brief Hash function class like `std::hash` but uses wyhash.
 * 
 * Specializations for all [integral types](https://en.cppreference.com/w/cpp/types/is_integral) as well as
 * `std::string` and `std::pair` are provided. For all other types, it must be specialized by the user.
 * Specializations should be implemented in terms of ::wyhash_bytes, ::wyhash_combine, and other WyHash specializations.
 * 
 * \see WyHash<std::string>, WyHash<std::pair<T1, T2>>
 */
template<typename T>
struct WyHash {};

/** \brief WyHash specialization for `std::string`. */
template<>
struct WyHash<std::string> {
    size_t operator()(const std::string& s) const {
        return wyhash_bytes(s.c_str(), s.size());
    }
};

/**
 * \brief WyHash specialization for `std::pair`.
 * 
 * Both types in the pair must also have a WyHash specialization. Their hash values are combined using ::wyhash_combine.
 */
template<typename T1, typename T2>
struct WyHash<std::pair<T1, T2>> {
    WyHash<T1> first_hash;
    WyHash<T2> second_hash;
    size_t operator()(const std::pair<T1, T2>& p) const {
        return wyhash_combine(first_hash(p.first), second_hash(p.second));
    }
};

#define _wyhash_for_integral_type(T) \
template<> struct WyHash<T> { \
    size_t operator()(T t) const { \
        return wyhash_bytes((void*)&t, sizeof(T)); \
    } \
};

_wyhash_for_integral_type(bool)
_wyhash_for_integral_type(char)
_wyhash_for_integral_type(unsigned char)
_wyhash_for_integral_type(signed char)
_wyhash_for_integral_type(short)
_wyhash_for_integral_type(unsigned short)
_wyhash_for_integral_type(int)
_wyhash_for_integral_type(unsigned int)
_wyhash_for_integral_type(long)
_wyhash_for_integral_type(unsigned long)
_wyhash_for_integral_type(long long)
_wyhash_for_integral_type(unsigned long long)

#undef _wyhash_for_integral_type

}  // namespace cplib