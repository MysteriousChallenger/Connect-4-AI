#pragma once

#include <cmath>

class TranspositionTable {
   public:
    struct Entry {
        uint32_t sig;
        uint32_t val;
        constexpr Entry() : sig(), val() {}
    };

    static constexpr uint64_t num_entries = 99999989; //prime number
    static uint64_t set_count;
    static uint64_t overwrite_count;
    static uint64_t get_count;
    static uint64_t cache_hit;
    static uint64_t cache_miss;
    static Entry data[num_entries];

    static void init() {
        for (uint64_t i = 0; i < num_entries; i++) {
            data[i] = Entry();
        }
    }
    static inline void set(uint64_t key, uint32_t val) {
        #if debug_stats == true
        set_count++;
        if(data[key % num_entries].sig != 0 && data[key % num_entries].sig != (uint32_t)key){
            overwrite_count++;
        }
        #endif
        data[key % num_entries].sig = (uint32_t)key;
        data[key % num_entries].val = val;
        
    }
    static inline uint32_t get(uint64_t key) {
        #if debug_stats == true
        get_count++;
        #endif
        if (data[key % num_entries].sig == (uint32_t)key) {
            #if debug_stats == true
            cache_hit++;
            #endif
            return data[key % num_entries].val;
        } else {
            #if debug_stats == true
            cache_miss++;
            #endif
            return 0;
        }
    }
    static inline uint64_t count_fullness(){
        uint64_t count = 0;
        for (uint64_t i = 0; i < num_entries; i++) {
            if (data[i].sig != 0){
                count++;
            }
        }
        return count;
    }
};

TranspositionTable::Entry TranspositionTable::data[TranspositionTable::num_entries] = {};
uint64_t TranspositionTable::get_count = 0;
uint64_t TranspositionTable::set_count = 0;
uint64_t TranspositionTable::overwrite_count = 0;
uint64_t TranspositionTable::cache_hit = 0;
uint64_t TranspositionTable::cache_miss = 0;