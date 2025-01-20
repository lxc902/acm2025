/*
Implement an LRU cache which includes the following features:
  Expire Time - after which an entry in the cache is invalid
  Priority - lower priority entries should be evicted before higher priority entries

The Cache eviction strategy should be as follows:
  Evict expired entries first
  If there are no expired items to evict then evict the lowest priority entries
    Tie breaking among entries with the same priority is done via least recently used.


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
*/

#include <functional>
#include <iostream>
#include <list>
#include <queue>
#include <set>
#include <unordered_map>
#include <vector>
#include <chrono>

int g_Time = 0;
typedef int CacheData;

class PriorityExpiryCache
{
  private:
  int maxItems;

  struct CacheItem
  {
    std::string key;
    CacheData value;
    int priority;
    int expiryTime;
    int lastAccessTime;

    CacheItem()
        : key(""), value(0), priority(0), expiryTime(0), lastAccessTime(0) {}

    CacheItem(std::string k, CacheData v, int p, int e, int last = g_Time)
        : key(k), value(v), priority(p), expiryTime(e), lastAccessTime(last) {}

    bool isExpired() const
    {
      return expiryTime < g_Time;
    }
  };

  // Comparator to sort CacheItem by expiry time, last access time
  struct CacheItemComparatorForExpiry
  {
    bool operator()(const CacheItem &a, const CacheItem &b) const
    {
      if (a.key == b.key)
      {
        // This is needed so that expirySet.insert(newItem) will remove the old key
        return false;
      }
      if (a.expiryTime == b.expiryTime)
      {
        // This helps reduct time, because the tie is less and sorting algo works better
        return a.lastAccessTime < b.lastAccessTime; // LRU tie breaker
      }
      return a.expiryTime < b.expiryTime;
    }
  };

  // Cache storage and data structures
  std::unordered_map<std::string, CacheItem> cache;

  std::set<int> priorityQueue;                                                       // Store only the priority numbers
  std::unordered_map<int, std::list<std::string>> priorityLRU;                       // LRU tracking for each priority
  std::unordered_map<std::string, std::list<std::string>::iterator> cacheItemLRUMap; // Item position in its priority LRU list
  std::set<CacheItem, CacheItemComparatorForExpiry> expirySet;

  public:
  // Constructor
  PriorityExpiryCache(int maxItems)
      : maxItems(maxItems) {}

  // Get the value of the key if it exists and is not expired
  CacheData *Get(std::string key)
  {
    if (cache.find(key) == cache.end() || cache[key].isExpired())
    {
      return nullptr; // Cache miss or expired
    }

    // Update last access time to reflect recent usage (LRU)
    CacheItem &item = cache[key];
    item.lastAccessTime = g_Time;

    // Move the key to the front of the LRU list for its priority
    auto &lruList = priorityLRU[item.priority];
    lruList.erase(cacheItemLRUMap[key]);    // Remove the item from its current position in the LRU list
    lruList.push_front(key);                // Move it to the front (most recently used)
    cacheItemLRUMap[key] = lruList.begin(); // Update the map to point to the new position

    return &item.value;
  }

  // Set the key-value pair with priority and expiry time
  void Set(std::string key, CacheData value, int priority, int expiryInSecs)
  {
    if (cache.count(key)) // Remove if old key exists.
    {
      CacheItem oldItem = cache[key];

      // Evict the old item first
      auto &lruList = priorityLRU[oldItem.priority];
      lruList.erase(cacheItemLRUMap[key]);  // Remove from LRU list
      cacheItemLRUMap.erase(key);           // Remove from the LRU map
      expirySet.erase(oldItem);             // Remove from expiry set
      cache.erase(key);                     // Remove from cache
      
      // If all items for this priority have been removed, delete the priority from the queue
      if (lruList.empty())
      {
        priorityQueue.erase(oldItem.priority);
        priorityLRU.erase(oldItem.priority); // Clean up empty priority
      }
    }

    // Insert the new item into cache and tracking structures
    CacheItem newItem(key, value, priority, g_Time + expiryInSecs, g_Time);

    // Add this priority to the priorityQueue if not already present
    if (priorityQueue.find(priority) == priorityQueue.end())
    {
      priorityQueue.insert(priority);
    }

    // Insert the item into its priority LRU list and map
    auto &lruList = priorityLRU[priority];
    lruList.push_front(key);                // Add to the front of the LRU list
    cacheItemLRUMap[key] = lruList.begin(); // Map the key to the list iterator
    expirySet.insert(newItem);
    cache[key] = newItem;

    EvictItems(); // Evict if needed after adding new item
  }

  // Set the max cache size and evict items accordingly
  void SetMaxItems(int numItems)
  {
    maxItems = numItems;
    EvictItems();
  }

  // Evict expired items and low-priority items if the cache exceeds max size
  void EvictItems()
  {
    // std::cout << 41 << std::endl;
    // Evict expired items using the expirySet
    while (!expirySet.empty() && (*expirySet.begin()).isExpired())
    {
      CacheItem expiredItem = *expirySet.begin();
      expirySet.erase(expirySet.begin()); // Remove from expiry set

      priorityLRU[expiredItem.priority].erase(cacheItemLRUMap[expiredItem.key]); // Remove from LRU tracking
      cache.erase(expiredItem.key);                                              // Remove from cache
      cacheItemLRUMap.erase(expiredItem.key);                                    // Remove from the LRU map
    }

    // Evict items if the cache size exceeds maxItems
    while (cache.size() > static_cast<size_t>(maxItems))
    {
      // Find the lowest priority from the priorityQueue
      int lowestPriority = *priorityQueue.begin();

      // Evict least used items of the lowest priority using LRU
      auto &lruList = priorityLRU[lowestPriority];

      while (!lruList.empty() && cache.size() > static_cast<size_t>(maxItems))
      {
        std::string key = lruList.back(); // Least recently used item
        CacheItem &item = cache[key];

        // Evict the valid item
        lruList.pop_back();         // Remove from LRU list
        cacheItemLRUMap.erase(key); // Remove from the LRU map
        expirySet.erase(item);      // Remove from expirySet
        cache.erase(key);           // Remove from cache
      }

      // If all items for this priority have been removed, delete the priority from the queue
      if (priorityLRU[lowestPriority].empty())
      {
        priorityLRU.erase(lowestPriority); // Clean up empty priority
        priorityQueue.erase(priorityQueue.begin());
      }
    }
  }

  // Debug function to print all keys in the cache for debugging
  void DebugPrintKeys()
  {
    std::vector<std::string> keys;
    for (const auto &item : cache)
    {
      keys.push_back(item.first);
    }
    std::sort(keys.begin(), keys.end());
    for (const auto &key : keys)
    {
      std::cout << key << " ";
    }
    std::cout << std::endl;
  }
};

int loadtest()
{
  // Load test
  const int numOps = 300000;
  const int numKeys = 10000;      // Number of unique keys to be used for Set operations
  const int numPrioritys = 20;    // Number of unique keys to be used for Set operations
  const int numCacheSize = 10000; // Number of unique keys to be used for Set operations

  // Initialize the cache with a maximum of items
  PriorityExpiryCache c(numCacheSize); // Use a larger cache size for load testing

  // Measure time for Set operations
  std::cout << "Start loading cache..." << std::endl;
  auto start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < numOps; ++i)
  {
    std::string key = "Key" + std::to_string(rand() % numKeys);
    CacheData value = rand() % 100;
    int priority = rand() % numPrioritys;
    int expiryTime = rand() % 50;
    c.Set(key, value, priority, expiryTime);
    g_Time += 1; // Simulate the passage of time
  }
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> duration = end - start;
  std::cout << "Set operations took: " << duration.count() << " seconds" << std::endl;

  // Measure time for Get operations
  start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < numOps; ++i)
  {
    std::string key = "Key" + std::to_string(rand() % numKeys);
    c.Get(key);
    g_Time += 1; // Simulate the passage of time
  }
  end = std::chrono::high_resolution_clock::now();
  duration = end - start;
  std::cout << "Get operations took: " << duration.count() << " seconds" << std::endl;

  // Load Test: Evictions - Simulating repeated eviction pressures

  std::cout << "Start eviction load test..." << std::endl;
  start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < numOps; ++i)
  {
    // Force eviction by calling EvictItems frequently
    c.EvictItems();
    g_Time += 1; // Simulate the passage of time

    // Optionally, periodically add new items to the cache during eviction cycles
    if (i % 100 == 0) // For example, add a new item every 100 eviction cycles
    {
      std::string key = "Key" + std::to_string(rand() % numKeys);
      CacheData value = rand() % 100;
      int priority = rand() % numPrioritys;
      int expiryTime = rand() % 50;
      c.Set(key, value, priority, expiryTime);
    }
  }
  end = std::chrono::high_resolution_clock::now();
  duration = end - start;
  std::cout << "Eviction load test took: " << duration.count() << " seconds" << std::endl;

  // Optionally, debug the final state of cache keys
  // std::cout << "Final cache keys: ";
  // c.DebugPrintKeys();

  return 0;
}


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

  loadtest();

  return 0;
}

// 1. priorityqueue 用的是红黑树(std::set)，并且只保存priority integer。
// 2. lruList linkedlist ，用了局部变量，对每一个单独操作提高了 map 的速度。
// 3. Get 的时间复杂度是 O(1)，Set 的是 O(logn) , Evict 是渐进式 O(logn)

// g++ -std=c++11 tesla20250120-homework.cc -o a && ./a

// A B C D E
// A C D E
// A C E
// C E
// C
// Start loading cache...
// Set operations took: 0.888077 seconds
// Get operations took: 0.0451756 seconds
// Start eviction load test...
// Eviction load test took: 0.0107144 seconds
