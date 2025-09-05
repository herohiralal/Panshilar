#ifndef PNSLR_NETWORK_H // =========================================================
#define PNSLR_NETWORK_H
#include "__Prelude.h"
#include "Allocators.h"
EXTERN_C_BEGIN

/**
 * Represents an IP address in binary form.
 * For IPv4, it's 4 bytes. For IPv6, it's 16 bytes.
 */
typedef ArraySlice(u8) PNSLR_IPAddress;

DECLARE_ARRAY_SLICE(PNSLR_IPAddress);

b8 PNSLR_GetInterfaceIPAddresses(ArraySlice(PNSLR_IPAddress)* addresses, PNSLR_Allocator allocator);

EXTERN_C_END
#endif // PNSLR_NETWORK_H ==========================================================
