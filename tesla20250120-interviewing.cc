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

保证 Get 的时间复杂度是 O(1)，Set 的是 O(n logn) , Evict 是渐进式 O(1)
*/

#include <iostream>
#include <chrono>
#include <thread>

int g_Time = 0;

typedef int CacheData;

struct PriorityExpiryCache {
  int maxItems;
  // TODO(interviewee): implement this
  typedef DataKey string;
  typedef DataValue int;
  HashSet<DataKey, DataValue> values; // Get O(1)

  // priority == 1
  // 10000 elements: <p1, e10000>
  // Get("A"). A
  // Get("C"). A->C
  // Get("B"). A->C->B
  // Get("D"). A->C->B->D
  // HashSet<Key, LinkedListPointer> pointerHash. 
  // Get("A"). A->C->B->D <-- A's pointer = pointerHash["A"] O(1)
  // Get("A"). C->B->D->A O(1)
  // Evict(size=2),   BD, remove AC

  std::map, std::unordered_set, set::

  struct PriorityKey {
    int priority;
    int lastTimeUsed; // replace with linkedlist
    DataKey dataKey;
  };
  PriorityQueue<PriorityKey> heapOfPriority; // log(n)
  // 在 PriorityQueue 内部，支持 delete 元素

    // 1
    // 1.  2
    // 1 3.  2 4
    // remove 1:
    // 3
    // 3.  2
    // 4 3.  2 nil // O(log n)
    // index_of[4] = 4
    // index_of[1] = 7


  struct ExpirationKey {
    int expireTime;
    DataKey dataKey;
  };
  PriorityQueue<ExpirationKey> heapOfExpiration;


  
  CacheData* Get(std::string key) {
    // ... the interviewee does not need to implement this now.
    // Assume that this will return the value for the key in the cache
    return nullptr;
  }
  
  void Set(std::string key, CacheData value, int priority, int expiryInSecs) {
    // ... the interviewee does not need to implement this now.
    // Assume that this will add this key, value pair to the cache
    EvictItems();
  }
  
  void SetMaxItems(int numItems) {
    maxItems = numItems;
    EvictItems();
  }
  
  void DebugPrintKeys() {
    // ... the interviewee does not need to implement this now.
    // Prints all the keys in the cache for debugging purposes
  }
  
  void EvictItems() {
    // TODO(interviewee): implement this
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

