/*
Implement an LRU cache which includes the following features:
  Expire Time - after which an entry in the cache is invalid
  Priority - lower priority entries should be evicted before higher priority entries

The Cache eviction strategy should be as follows:
  Evict expired entries first
  If there are no expired items to evict then evict the lowest priority entries
    Tie breaking among entries with the same priority is done via least recently used.

    (priority, lastTimeUsedTimestamp)
    Get: update lastTimeUsedTimestamp

    1
    1.  2
    1 3.  2 4
    priority_heap
    // N element heap, heap[2*N]
    [1, 1, 2, 1, 3 , 2 , 4]
    // find row 3 col 2, heap[]
    index_of_priority_heap[3]: 5
    index_of_expiration_heap[3]: 5

    // remove 3

You can use any language.

Your task is to implement a PriorityExpiryCache cache with a max capacity.  Specifically please fill out the data structures on the PriorityExpiryCache object and implement the entry eviction method.

You do NOT need to implement the get or set methods.

It should support these operations:
  Get: Get the value of the key if the key exists in the cache and is not expired.
  Set: Update or insert the value of the key with a priority value and expiretime.
    Set should never ever allow more items than maxItems to be in the cache.
    When evicting we need to evict the lowest priority item(s) which are least recently used.

Example:
p5 => priority 5
e10 => expires at 10 seconds since epoch

保证 Get 的时间复杂度是 O(1)
*/

#include <iostream>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <list>
#include <algorithm>
#include <limits>
#include <climits>

int g_Time = 0;

typedef int CacheData;

struct CacheEntry {
    std::string key;
    CacheData value;
    int priority;
    int expiryTime;
    std::list<std::string>::iterator list_iterator;
};

struct PriorityExpiryCache {
  int maxItems;
  // TODO(interviewee): implement this
  std::unordered_map<std::string, CacheEntry> cache;
  std::list<std::string> lru_list;


  PriorityExpiryCache(int max_items) : maxItems(max_items) {}
  
  CacheData* Get(std::string key) {
    auto it = cache.find(key);
    if (it == cache.end()) {
        return nullptr;
    }
    
    if (it->second.expiryTime <= g_Time) {
        // expired
        lru_list.erase(it->second.list_iterator);
        cache.erase(it);
        return nullptr;
    }
    
    // Move to the front of the LRU list
    lru_list.erase(it->second.list_iterator);
    lru_list.push_front(key);
    it->second.list_iterator = lru_list.begin();
    
    return &it->second.value;
  }
  
  void Set(std::string key, CacheData value, int priority, int expiryInSecs) {
    
    auto it = cache.find(key);
    if (it != cache.end()) {
        // update existing entry
        it->second.value = value;
        it->second.priority = priority;
        it->second.expiryTime = g_Time + expiryInSecs;
        
        // Move to the front of the LRU list
        lru_list.erase(it->second.list_iterator);
        lru_list.push_front(key);
        it->second.list_iterator = lru_list.begin();
    } else {
        // insert new entry
        CacheEntry entry;
        entry.key = key;
        entry.value = value;
        entry.priority = priority;
        entry.expiryTime = g_Time + expiryInSecs;
        lru_list.push_front(key);
        entry.list_iterator = lru_list.begin();
        cache[key] = entry;
    }
    
    EvictItems();
  }
  
  void SetMaxItems(int numItems) {
    maxItems = numItems;
    EvictItems();
  }
  
  void DebugPrintKeys() {
    std::cout << "Keys in cache: ";
    for (const auto& key : lru_list) {
        std::cout << key << " ";
    }
    std::cout << std::endl;
  }
  
  
  void EvictItems() {
    // TODO(interviewee): implement this
    while (cache.size() > maxItems) {
        // 1. Evict expired items
        bool evicted = false;
        for (auto it = cache.begin(); it != cache.end(); ) {
            if (it->second.expiryTime <= g_Time) {
                lru_list.erase(it->second.list_iterator);
                it = cache.erase(it);
                evicted = true;
                break;
            } else {
                ++it;
            }
        }
        if (evicted) continue;

        // 2. Evict lowest priority, least recently used
        if (!lru_list.empty()) {
            
            std::string key_to_remove = lru_list.back();
            int min_priority = cache[key_to_remove].priority;
            
            for(const auto& key : lru_list) {
                if(cache[key].priority < min_priority) {
                    min_priority = cache[key].priority;
                    key_to_remove = key;
                }
            }
            
            
            lru_list.remove(key_to_remove);
            cache.erase(key_to_remove);
        }
    }
  }
};


int main() {
  PriorityExpiryCache c(5);
  c.Set("A", 1, 5,  100 );
  c.Set("B", 2, 15, 3   );
  c.Set("C", 3, 5,  10  );
  c.Set("D", 4, 1,  15  );
  c.Set("E", 5, 5,  150 );
  c.Get("C");


  // Current time = 0
  c.SetMaxItems(5);
  // Keys in C = ["A", "B", "C", "D", "E"]
  // space for 5 keys, all 5 items are included
  c.DebugPrintKeys();

  // Sleep for 5 secs
  g_Time += 5;

  // Current time = 5
  c.SetMaxItems(4);
  // Keys in C = ["A", "C", "D", "E"]
  // "B" is removed because it is expired.  e3 < e5
  c.DebugPrintKeys();

  c.SetMaxItems(3);
  // Keys in C = ["A", "C", "E"]
  // "D" is removed because it the lowest priority
  // D's expire time is irrelevant.
  c.DebugPrintKeys();

  c.SetMaxItems(2);
  // Keys in C = ["C", "E"]
  // "A" is removed because it is least recently used."
  // A's expire time is irrelevant.
  c.DebugPrintKeys();

  c.SetMaxItems(1);
  // Keys in C = ["C"]
  // "E" is removed because C is more recently used (due to the Get("C") event).
  c.DebugPrintKeys();
  
  return 0;
}

// pq还是红黑树
// linkedlist，全局还是不全局。
// 用c++现成的库

