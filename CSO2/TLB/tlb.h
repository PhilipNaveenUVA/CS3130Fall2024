#include "config.h"  // Configuration file
#include "mlpt.h"    // Page table API for `translate()`

#define NUM_SETS 16
#define WAYS_PER_SET 4
#define POBITS 12  // Assume 12 bits for page offset

typedef struct {
    size_t vpn;    // Virtual Page Number
    size_t ppn;    // Physical Page Number
    int valid;     // Valid bit
    int lru;       // LRU counter
} TLBEntry;

typedef struct {
    TLBEntry entries[WAYS_PER_SET];
} TLBSet;

typedef struct {
    TLBSet sets[NUM_SETS];
} TLB;

void tlb_clear();
int tlb_peek(size_t va);
size_t tlb_translate(size_t va);

