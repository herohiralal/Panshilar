#ifndef PNSLR_NETWORK_H // =========================================================
#define PNSLR_NETWORK_H
#include "__Prelude.h"
#include "Allocators.h"
EXTERN_C_BEGIN

/**
 * Represents an IP address in binary form.
 * For IPv4, it's 4 bytes. For IPv6, it's 16 bytes.
 */
typedef PNSLR_ArraySlice(u8) PNSLR_IPAddress;

/**
 * Represents a subnet mask in binary form.
 * For IPv4, it's 4 bytes. For IPv6, it's 16 bytes.
 */
typedef PNSLR_ArraySlice(u8) PNSLR_IPMask;

/**
 * Represents an IP network, consisting of an IP address and a subnet mask.
 */
typedef struct PNSLR_IPNetwork
{
    PNSLR_IPAddress address; // The IP address
    PNSLR_IPMask    mask;    // The subnet mask
} PNSLR_IPNetwork;

PNSLR_DECLARE_ARRAY_SLICE(PNSLR_IPNetwork);

b8 PNSLR_GetInterfaceIPAddresses(PNSLR_ArraySlice(PNSLR_IPNetwork)* networks, PNSLR_Allocator allocator);

EXTERN_C_END
#endif // PNSLR_NETWORK_H ==========================================================
