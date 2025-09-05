#define PNSLR_IMPLEMENTATION
#include "Network.h"

b8 PNSLR_GetInterfaceIPAddresses(ArraySlice(PNSLR_IPNetwork)* networks, PNSLR_Allocator allocator)
{
    if (!networks) return false;
    *networks = (ArraySlice(PNSLR_IPNetwork)) {0};

    #if PNSLR_WINDOWS
    {
        ULONG outBufLen = 15000;
        ArraySlice(u8) b = PNSLR_MakeSlice(u8, outBufLen, false, PNSLR_GetAllocator_DefaultHeap(), CURRENT_LOC(), nil);

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

        ArraySlice(PNSLR_IPNetwork) networksTemp = PNSLR_MakeSlice(PNSLR_IPNetwork, 0, true, PNSLR_GetAllocator_DefaultHeap(), CURRENT_LOC(), nil);
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
                        PNSLR_Intrinsic_MemCopy(ipAddr.data, &(((SOCKADDR_IN*) addr)->sin_addr), 4);
                        break;
                    }
                    case AF_INET6:
                    {
                        ipAddr = PNSLR_MakeSlice(u8, 16, false, allocator, CURRENT_LOC(), nil);
                        PNSLR_Intrinsic_MemCopy(ipAddr.data, &(((SOCKADDR_IN6*) addr)->sin6_addr), 16);
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
                        PNSLR_Intrinsic_MemCopy(ipAddr.data, &(((SOCKADDR_IN*) addr)->sin_addr), 4);
                        break;
                    }
                    case AF_INET6:
                    {
                        ipAddr = PNSLR_MakeSlice(u8, 16, false, allocator, CURRENT_LOC(), nil);
                        PNSLR_Intrinsic_MemCopy(ipAddr.data, &(((SOCKADDR_IN6*) addr)->sin6_addr), 16);
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

        ArraySlice(PNSLR_IPNetwork) networksTemp = PNSLR_MakeSlice(PNSLR_IPNetwork, 0, true, allocator, CURRENT_LOC(), nil);
        i64 countTrack = 0;

        for (struct ifaddrs* ifa = ifaddr; ifa != nil; ifa = ifa->ifa_next)
        {
            if (!ifa->ifa_addr) continue;

            i64 ipLen = 0;
            const u8* ipBytes = nil;

            switch (ifa->ifa_addr->sa_family)
            {
                case AF_INET:
                {
                    struct sockaddr_in* ipv4 = (struct sockaddr_in*) ifa->ifa_addr;
                    ipLen = 4;
                    ipBytes = (const u8*) &ipv4->sin_addr;
                    break;
                }
                case AF_INET6:
                {
                    struct sockaddr_in6* ipv6 = (struct sockaddr_in6*) ifa->ifa_addr;
                    ipLen = 16;
                    ipBytes = (const u8*) &ipv6->sin6_addr;
                    break;
                }
                default: continue;
            }

            PNSLR_IPAddress ipAddr = PNSLR_MakeSlice(u8, ipLen, false, allocator, CURRENT_LOC(), nil);
            PNSLR_IPMask    ipMask = PNSLR_MakeSlice(u8, ipLen, false, allocator, CURRENT_LOC(), nil);
            PNSLR_Intrinsic_MemCopy(ipAddr.data, (rawptr) ipBytes, (i32) ipLen);
            PNSLR_Intrinsic_MemCopy(ipMask.data, (rawptr) &ifa->ifa_netmask, (i32) ipLen);

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
