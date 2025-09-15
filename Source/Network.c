#define PNSLR_IMPLEMENTATION
#include "Network.h"

b8 PNSLR_GetInterfaceIPAddresses(PNSLR_ArraySlice(PNSLR_IPNetwork)* networks, PNSLR_Allocator allocator)
{
    if (!networks) return false;
    *networks = (PNSLR_ArraySlice(PNSLR_IPNetwork)) {0};

    #if PNSLR_WINDOWS
    {
        ULONG outBufLen = 15000;
        PNSLR_ArraySlice(u8) b = PNSLR_MakeSlice(u8, outBufLen, false, PNSLR_GetAllocator_DefaultHeap(), CURRENT_LOC(), nil);

        while (true)
        {
            ULONG err = GetAdaptersAddresses(
                AF_UNSPEC,
                GAA_FLAG_INCLUDE_PREFIX | GAA_FLAG_INCLUDE_GATEWAYS,
                nil,
                (IP_ADAPTER_ADDRESSES_LH*) b.data,
                &outBufLen
            );

            if (err == ERROR_SUCCESS && outBufLen != 0) { break; } // success

            if (err == ERROR_BUFFER_OVERFLOW)
            {
                PNSLR_ResizeSlice(u8, &b, outBufLen, false, PNSLR_GetAllocator_DefaultHeap(), CURRENT_LOC(), nil);
                continue; // try again with the new size
            }

            PNSLR_FreeSlice(&b, PNSLR_GetAllocator_DefaultHeap(), CURRENT_LOC(), nil);
            return false; // some other error
        }

        PNSLR_ArraySlice(PNSLR_IPNetwork) networksTemp = PNSLR_MakeSlice(PNSLR_IPNetwork, 0, true, PNSLR_GetAllocator_DefaultHeap(), CURRENT_LOC(), nil);
        i64 countTrack = 0;

        for (IP_ADAPTER_ADDRESSES_LH* adapter = (IP_ADAPTER_ADDRESSES_LH*) b.data; adapter != nil; adapter = adapter->Next)
        {
            u32 idx = adapter->IfIndex;
            if (idx == 0) idx = adapter->Ipv6IfIndex;

            for (IP_ADAPTER_UNICAST_ADDRESS_LH* puni = adapter->FirstUnicastAddress; puni != nil; puni = puni->Next)
            {
                SOCKADDR* addr = puni->Address.lpSockaddr;
                if (!addr) continue;

                PNSLR_IPAddress ipAddr = {0};
                PNSLR_IPMask    ipMask = {0};
                switch (addr->sa_family)
                {
                    case AF_INET:
                    {
                        ipAddr = PNSLR_MakeSlice(u8, 4, false, allocator, CURRENT_LOC(), nil);
                        ipMask = PNSLR_MakeSlice(u8, 4, false, allocator, CURRENT_LOC(), nil);
                        PNSLR_MemCopy(ipAddr.data, &(((SOCKADDR_IN*) addr)->sin_addr), 4);
                        if (puni->OnLinkPrefixLength <= 32)
                        {
                            *(DWORD*) ipMask.data = htonl((puni->OnLinkPrefixLength == 0) ? 0 : (0xFFFFFFFF << (32 - puni->OnLinkPrefixLength)));
                        }
                        else
                        {
                            *(DWORD*) ipMask.data = 0; // invalid prefix length, set mask to 0
                        }
                        break;
                    }
                    case AF_INET6:
                    {
                        ipAddr = PNSLR_MakeSlice(u8, 16, false, allocator, CURRENT_LOC(), nil);
                        ipMask = PNSLR_MakeSlice(u8, 16, false, allocator, CURRENT_LOC(), nil);
                        PNSLR_MemCopy(ipAddr.data, &(((SOCKADDR_IN6*) addr)->sin6_addr), 16);
                        if (puni->OnLinkPrefixLength <= 128)
                        {
                            u8 prefix = (u8) puni->OnLinkPrefixLength;
                            for (i32 i = 0; i < 16; i++)
                            {
                                if (prefix >= 8) { ipMask.data[i] = 0xFF; prefix -= 8; }
                                else if (prefix > 0) { ipMask.data[i] = (u8)(0xFF << (8 - prefix)); prefix = 0; }
                                else { ipMask.data[i] = 0x00; }
                            }
                        }
                        else
                        {
                            for (i32 i = 0; i < 16; i++) ipMask.data[i] = 0x00;
                        }
                        break;
                    }
                    default: continue;
                }

                if (countTrack >= networksTemp.count)
                {
                    PNSLR_ResizeSlice(PNSLR_IPNetwork, &networksTemp, networksTemp.count + 8, true, PNSLR_GetAllocator_DefaultHeap(), CURRENT_LOC(), nil);
                }

                networksTemp.data[countTrack] = (PNSLR_IPNetwork) {.address = ipAddr, .mask = ipMask};
                countTrack += 1;
            }

            for (IP_ADAPTER_ANYCAST_ADDRESS_XP* pany = adapter->FirstAnycastAddress; pany != nil; pany = pany->Next)
            {
                SOCKADDR* addr = pany->Address.lpSockaddr;
                if (!addr) continue;

                PNSLR_IPAddress ipAddr = {0};
                PNSLR_IPMask    ipMask = {0};
                switch (addr->sa_family)
                {
                    case AF_INET:
                    {
                        ipAddr = PNSLR_MakeSlice(u8, 4, false, allocator, CURRENT_LOC(), nil);
                        ipMask = PNSLR_MakeSlice(u8, 4, false, allocator, CURRENT_LOC(), nil);
                        PNSLR_MemCopy(ipAddr.data, &(((SOCKADDR_IN*) addr)->sin_addr), 4);
                        *(DWORD*) ipMask.data = 0xFFFFFFFF; // Windows does not provide a mask for anycast addresses, so we set it to all ones
                        break;
                    }
                    case AF_INET6:
                    {
                        ipAddr = PNSLR_MakeSlice(u8, 16, false, allocator, CURRENT_LOC(), nil);
                        ipMask = PNSLR_MakeSlice(u8, 16, false, allocator, CURRENT_LOC(), nil);
                        PNSLR_MemCopy(ipAddr.data, &(((SOCKADDR_IN6*) addr)->sin6_addr), 16);
                        for (i32 i = 0; i < 16; i++) { ipMask.data[i] = 0xFF; } // Windows does not provide a mask for anycast addresses, so we set it to all ones
                        break;
                    }
                    default: continue;
                }

                if (countTrack >= networksTemp.count)
                {
                    PNSLR_ResizeSlice(PNSLR_IPNetwork, &networksTemp, networksTemp.count + 8, true, PNSLR_GetAllocator_DefaultHeap(), CURRENT_LOC(), nil);
                }

                networksTemp.data[countTrack] = (PNSLR_IPNetwork) {.address = ipAddr, .mask = ipMask};
                countTrack += 1;
            }
        }

        *networks = PNSLR_MakeSlice(PNSLR_IPNetwork, countTrack, false, allocator, CURRENT_LOC(), nil);
        for (i64 i = 0; i < countTrack; i++) { networks->data[i] = networksTemp.data[i]; }

        PNSLR_FreeSlice(&networksTemp, PNSLR_GetAllocator_DefaultHeap(), CURRENT_LOC(), nil);
        PNSLR_FreeSlice(&b, PNSLR_GetAllocator_DefaultHeap(), CURRENT_LOC(), nil);

        return true;
    }
    #elif PNSLR_UNIX
    {
        struct ifaddrs* ifaddr = nil;
        if (getifaddrs(&ifaddr) != 0) return false;

        PNSLR_ArraySlice(PNSLR_IPNetwork) networksTemp = PNSLR_MakeSlice(PNSLR_IPNetwork, 0, true, allocator, CURRENT_LOC(), nil);
        i64 countTrack = 0;

        for (struct ifaddrs* ifa = ifaddr; ifa != nil; ifa = ifa->ifa_next)
        {
            if (!ifa->ifa_addr) continue;

            i64 ipLen = 0;
            const u8* ipBytes = nil;

            PNSLR_IPAddress ipAddr = {0};
            PNSLR_IPMask    ipMask = {0};

            switch (ifa->ifa_addr->sa_family)
            {
                case AF_INET:
                {
                    struct sockaddr_in* ipv4 = (struct sockaddr_in*) ifa->ifa_addr;
                    ipLen = 4;
                    ipBytes = (const u8*) &ipv4->sin_addr;

                    struct sockaddr_in* netmask4 = (struct sockaddr_in*) ifa->ifa_netmask;
                    ipMask = PNSLR_MakeSlice(u8, ipLen, false, allocator, CURRENT_LOC(), nil);
                    PNSLR_MemCopy(ipMask.data, &netmask4->sin_addr, (i32) ipLen);
                    break;
                }
                case AF_INET6:
                {
                    struct sockaddr_in6* ipv6 = (struct sockaddr_in6*) ifa->ifa_addr;
                    ipLen = 16;
                    ipBytes = (const u8*) &ipv6->sin6_addr;

                    struct sockaddr_in6* netmask6 = (struct sockaddr_in6*) ifa->ifa_netmask;
                    ipMask = PNSLR_MakeSlice(u8, ipLen, false, allocator, CURRENT_LOC(), nil);
                    PNSLR_MemCopy(ipMask.data, &netmask6->sin6_addr, (i32) ipLen);
                    break;
                }
                default: continue;
            }

            ipAddr = PNSLR_MakeSlice(u8, ipLen, false, allocator, CURRENT_LOC(), nil);
            PNSLR_MemCopy(ipAddr.data, (rawptr) ipBytes, (i32) ipLen);

            if (countTrack >= networksTemp.count)
            {
                PNSLR_ResizeSlice(PNSLR_IPAddress, &networksTemp, networksTemp.count + 8, true, allocator, CURRENT_LOC(), nil);
            }

            networksTemp.data[countTrack] = (PNSLR_IPNetwork) {.address = ipAddr, .mask = ipMask};
            countTrack += 1;
        }

        if (ifaddr) freeifaddrs(ifaddr);

        if (countTrack == 0)
        {
            PNSLR_FreeSlice(&networksTemp, allocator, CURRENT_LOC(), nil);
            return true; // no addresses found, but success
        }

        *networks = PNSLR_MakeSlice(PNSLR_IPNetwork, countTrack, false, allocator, CURRENT_LOC(), nil);
        for (i64 i = 0; i < countTrack; i++)
        {
            (*networks).data[i] = networksTemp.data[i];
        }

        PNSLR_FreeSlice(&networksTemp, allocator, CURRENT_LOC(), nil);
        return true;
    }
    #else
        return false; // unknown platform
    #endif
}
