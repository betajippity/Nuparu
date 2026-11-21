# Repair the Windows ARM64 bit-scan helpers in Embree 4.4.0's bundled
# sse2neon. Operate only on the build-local copy and fail if upstream changes.
function(nuparu_patch_embree_win_arm64 header)
    file(READ "${header}" contents)
    # Windows Git checkouts can use CRLF; normalize only this build-local copy.
    string(REPLACE "\r\n" "\n" contents "${contents}")
    set(original [=[FORCE_INLINE int _sse2neon_clz(unsigned int x)
{
#if _MSC_VER
    DWORD cnt = 0;
    if (_BitScanForward(&cnt, x))
        return cnt;
    return 32;
#else
    return x != 0 ? __builtin_clz(x) : 32;
#endif
}

FORCE_INLINE int _sse2neon_ctz(unsigned int x)
{
#if _MSC_VER
    DWORD cnt = 0;
    if (_BitScanReverse(&cnt, x))
        return 31 - cnt;
    return 32;
#else
    return x != 0 ? __builtin_ctz(x) : 32;
#endif
}

FORCE_INLINE int _sse2neon_ctzll(unsigned long long x)
{
#if _MSC_VER
    unsigned long cnt;
#ifdef defined(SSE2NEON_HAS_BITSCAN64)
    (defined(_M_AMD64) || defined(__x86_64__))
        if((_BitScanForward64(&cnt, x))
            return (int)(cnt);
#else
    if (_BitScanForward(&cnt, (unsigned long) (x)))
        return (int) cnt;
    if (_BitScanForward(&cnt, (unsigned long) (x >> 32)))
        return (int) (cnt + 32);
#endif
    return 64;
#else
    return x != 0 ? __builtin_ctzll(x) : 64;
#endif
}
]=])
    set(replacement [=[FORCE_INLINE int _sse2neon_clz(unsigned int x)
{
#if _MSC_VER
    unsigned long cnt = 0;
    if (_BitScanReverse(&cnt, x))
        return 31 - (int) cnt;
    return 32;
#else
    return x != 0 ? __builtin_clz(x) : 32;
#endif
}

FORCE_INLINE int _sse2neon_ctz(unsigned int x)
{
#if _MSC_VER
    unsigned long cnt = 0;
    if (_BitScanForward(&cnt, x))
        return (int) cnt;
    return 32;
#else
    return x != 0 ? __builtin_ctz(x) : 32;
#endif
}

FORCE_INLINE int _sse2neon_ctzll(unsigned long long x)
{
#if _MSC_VER
    unsigned long cnt;
    if (_BitScanForward(&cnt, (unsigned long) (x)))
        return (int) cnt;
    if (_BitScanForward(&cnt, (unsigned long) (x >> 32)))
        return (int) (cnt + 32);
    return 64;
#else
    return x != 0 ? __builtin_ctzll(x) : 64;
#endif
}
]=])
    string(FIND "${contents}" "${original}" original_position)
    string(FIND "${contents}" "${replacement}" replacement_position)
    if(NOT original_position EQUAL -1)
        string(REPLACE "${original}" "${replacement}" contents "${contents}")
        file(WRITE "${header}" "${contents}")
    elseif(replacement_position EQUAL -1)
        message(FATAL_ERROR "Cannot patch Embree's Windows ARM64 bit-scan helpers; check the Embree revision")
    endif()
endfunction()
