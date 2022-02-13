#include <cstdint>
#include <cstring>
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

static inline uint64_t wyhash_default(const void *key, size_t len) {
    return wyhash(key, len, 0, _wyp);
}

template<typename T>
struct WyHashBitwise {
    size_t operator()(const T& t) const {
        return wyhash_default((void*)&t, sizeof(T));
    }
};

}  // namespace impl

template<typename T>
struct WyHash {};

template<> struct WyHash<bool> : impl::WyHashBitwise<bool> {};
template<> struct WyHash<char> : impl::WyHashBitwise<char> {};
template<> struct WyHash<unsigned char> : impl::WyHashBitwise<unsigned char> {};
template<> struct WyHash<signed char> : impl::WyHashBitwise<signed char> {};
template<> struct WyHash<short> : impl::WyHashBitwise<short> {};
template<> struct WyHash<int> : impl::WyHashBitwise<int> {};
template<> struct WyHash<long> : impl::WyHashBitwise<long> {};
template<> struct WyHash<long long> : impl::WyHashBitwise<long long> {};
template<> struct WyHash<unsigned short> : impl::WyHashBitwise<unsigned short> {};
template<> struct WyHash<unsigned int> : impl::WyHashBitwise<unsigned int> {};
template<> struct WyHash<unsigned long> : impl::WyHashBitwise<unsigned long> {};
template<> struct WyHash<unsigned long long> : impl::WyHashBitwise<unsigned long long> {};

template<>
struct WyHash<std::string> {
    size_t operator()(const std::string& s) const {
        return impl::wyhash_default(s.c_str(), s.size());
    }
};

template<typename T1, typename T2>
struct WyHash<std::pair<T1, T2>> {
    WyHash<T1> first_hash;
    WyHash<T2> second_hash;
    size_t operator()(const std::pair<T1, T2>& p) const {
        return wyhash64(first_hash(p.first), second_hash(p.second));
    }
};

}  // namespace cplib