#include "tlb.h"
#include <stddef.h>
#include <stdint.h>

// TLB instance
static TLB tlb;

// Hash function to get the set index
static size_t get_set_index(size_t vpn) {
    return (vpn >> POBITS) % NUM_SETS;
}

// Extract VPN from virtual address
static size_t get_vpn(size_t va) {
    return va >> POBITS;
}

// Get page offset from virtual address
static size_t get_page_offset(size_t va) {
    return va & ((1 << POBITS) - 1);
}

// Invalidate all entries in the TLB
void tlb_clear() {
    for (int i = 0; i < NUM_SETS; i++) {
        for (int j = 0; j < WAYS_PER_SET; j++) {
            tlb.sets[i].entries[j].valid = 0;
        }
    }
}

// Update LRU and return the LRU position for a hit entry
static void update_lru(TLBSet *set, int hit_index) {
    int old_lru = set->entries[hit_index].lru;
    for (int i = 0; i < WAYS_PER_SET; i++) {
        if (set->entries[i].valid && set->entries[i].lru < old_lru) {
            set->entries[i].lru++;
        }
    }
    set->entries[hit_index].lru = 1;  // Most-recently used
}

// Find the LRU entry index in a set for replacement
static int find_lru_index(TLBSet *set) {
    int lru_index = 0;
    for (int i = 1; i < WAYS_PER_SET; i++) {
        if (set->entries[i].lru > set->entries[lru_index].lru) {
            lru_index = i;
        }
    }
    return lru_index;
}

// Check if a virtual address is in the TLB and return its LRU position
int tlb_peek(size_t va) {
    size_t vpn = get_vpn(va);
    size_t set_index = get_set_index(vpn);
    TLBSet *set = &tlb.sets[set_index];

    for (int i = 0; i < WAYS_PER_SET; i++) {
        if (set->entries[i].valid && set->entries[i].vpn == vpn) {
            return set->entries[i].lru;
        }
    }
    return 0;  // Not found
}

// Translate virtual address to physical address using TLB
size_t tlb_translate(size_t va) {
    size_t vpn = get_vpn(va);
    size_t set_index = get_set_index(vpn);
    TLBSet *set = &tlb.sets[set_index];
    size_t offset = get_page_offset(va);

    // Check for TLB hit
    for (int i = 0; i < WAYS_PER_SET; i++) {
        if (set->entries[i].valid && set->entries[i].vpn == vpn) {
            update_lru(set, i);
            return (set->entries[i].ppn << POBITS) | offset;
        }
    }

    // TLB miss - call translate() to get physical address
    size_t pa = translate(vpn << POBITS);
    if (pa == (size_t)-1) {
        return -1;  // Invalid translation, do not update TLB
    }

    // Update TLB with new entry
    int replace_index = find_lru_index(set);
    set->entries[replace_index].vpn = vpn;
    set->entries[replace_index].ppn = pa >> POBITS;
    set->entries[replace_index].valid = 1;
    update_lru(set, replace_index);

    return pa;
}

