"""A minimal implementation of the xxHash fast hash algorithm.

https://cyan4973.github.io/xxHash/

https://github.com/Cyan4973/xxHash/blob/v0.8.1/doc/xxhash_spec.md
"""


# TODO: Use fnv instead, maybe?


_PRIME32_1 = 0x9E3779B1
_PRIME32_2 = 0x85EBCA77
_PRIME32_3 = 0xC2B2AE3D
_PRIME32_4 = 0x27D4EB2F
_PRIME32_5 = 0x165667B1

_INT32_MASK = 0xffffffff


def xxh32_int32(*, seed, input):
    """Return the XXHH32 hash of an integer.

    seed and input are both assumed to be 32-bit unsigned integers.
    """
    acc = seed + _PRIME32_5

    # Add input length:
    acc += 4

    # "Consume remaining input"
    # (which in our case is the entire input):
    acc += input * _PRIME32_3
    # TODO: Decide whose responsibility it is to truncate to 32 bits. Our call site, or this function?
    acc = _circular_left_shift_int32(int32=acc, shift_amount=17) * _PRIME32_4

    # Final mix (avalanche):
    acc ^= acc >> 15
    acc *= _PRIME32_2
    acc ^= acc >> 13
    acc *= _PRIME32_3
    acc ^= acc >> 16

    # TODO: xxhash is specified with modular arithmetic.
    # Is it safe to truncate to 32 bits at the end, not for each intermediate operation?
    return acc & _INT32_MASK


def _circular_left_shift_int32(int32, shift_amount):
    # Save the leftmost shift_amount bits before we rotate them out.
    saved_bits = (int32 & _INT32_MASK) >> (32-shift_amount)

    int32 <<= shift_amount
    int32 &= _INT32_MASK
    int32 |= saved_bits

    return int32
