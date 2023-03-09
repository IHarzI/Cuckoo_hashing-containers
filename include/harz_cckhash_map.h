#ifndef HARZ_CCKHASH_MAP
#define HARZ_CCKHASH_MAP

// Custom implementation of hash map based on cuckoo hashing
// c++14 and newer versions
// repository: https://github.com/IHarzI/Cuckoo_hashing-containers
// made by IHarzI

#include <vector>
#include <functional>
#include <memory>
#include <stdint.h>
// custom params, experiment with different values for better perfomance
#define HARZ_CCKHASH_MAP_MAX_ITERATIONS_MOD (3.7f) 
#define HARZ_CCKHASH_MAP_RESIZE_MOD (1.75f)

namespace harz
{

	template<typename K, typename V>
	class cuckooHashMap
	{
	public:

		cuckooHashMap(const uint32_t capacity = 64, const uint32_t tablesCount = 2)
			: _capacity(capacity), _tablesCount(tablesCount), _maxIters((uint32_t)(tablesCount* HARZ_CCKHASH_MAP_MAX_ITERATIONS_MOD) + 1)
		{

			_data.resize(tablesCount);
			for (uint32_t iter = 0; iter < tablesCount; iter++)
			{
				_data[iter].resize(capacity);
			}
		}

		struct TableSlot
		{

			K key;
			V value;
			bool occupied = false;
		};

		struct K_V_pair
		{
			K key;
			V value;
		};

		// Change capacity("rehash") container, possible recursive call to resize while inserting elements from old _data
		const bool resize(uint32_t newCapacity = 0)
		{
			if (newCapacity <= 0)
			{
				newCapacity = (uint32_t)(_capacity * HARZ_CCKHASH_MAP_RESIZE_MOD) + 1;
			}

			std::vector<std::vector<TableSlot>> oldData = _data;

			_capacity = newCapacity;

			clear();

			for (auto& table : oldData)
			{
				for (auto& slot : table)
				{
					if (slot.occupied)
						_insert({ slot.key, slot.value });
				};
			};

			return true;
		};

		// Change tables count, possible recursive call to resize while inserting elements from old _data
		const bool restrain(const uint32_t newTablesCount)
		{
			if (newTablesCount <= 2)
				return false;

			_tablesCount = newTablesCount;
			_maxIters = _tablesCount * HARZ_CCKHASH_MAP_MAX_ITERATIONS_MOD;
			resize(_capacity);

			return true;
		};


	private:
		uint32_t _capacity = 64;
		uint32_t _tablesCount = 2;
		uint32_t _maxIters = _tablesCount * HARZ_CCKHASH_MAP_MAX_ITERATIONS_MOD + 1;

		std::vector<std::vector<TableSlot>> _data;

		const std::function <const uint32_t(K, uint32_t, uint32_t, uint32_t)> _g_CCKHT_l_hashFunction = [](const K& key, uint32_t cap, uint32_t tablecnt, uint32_t i)-> const uint32_t
		{
			return ((std::hash< uint32_t>()(std::hash<K>()(key) + std::hash< uint32_t>()(i % (tablecnt + cap))))) % cap;
		};

		const bool _CCKHT_insertData(const K& key, const V& value, uint32_t iterations = 0)
		{
			while (true)
			{
				while (iterations < _maxIters)
				{
					const uint32_t currentTable = iterations % _tablesCount;
					const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, iterations);

					if (_data[currentTable][hashedKey].occupied)
					{
						K_V_pair temp;
						temp.key = _data[currentTable][hashedKey].key;
						temp.value = _data[currentTable][hashedKey].value;
						_data[currentTable][hashedKey].key = key;
						_data[currentTable][hashedKey].value = value;
						return _CCKHT_insertData(std::move(temp), iterations);
					}
					else
					{
						_data[currentTable][hashedKey].key = key;
						_data[currentTable][hashedKey].value = value;
						_data[currentTable][hashedKey].occupied = true;
						return true;
					}
					iterations++;
				}
				resize();
				iterations = 0;
			}
		}

		const bool _CCKHT_insertData(K&& key, V&& value, uint32_t iterations = 0)
		{
			while (true)
			{
				while (iterations < _maxIters)
				{
					const uint32_t currentTable = iterations % _tablesCount;
					const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, iterations);

					if (_data[currentTable][hashedKey].occupied)
					{
						K_V_pair temp;
						temp.key = _data[currentTable][hashedKey].key;
						temp.value = _data[currentTable][hashedKey].value;
						_data[currentTable][hashedKey].key = std::move(key);
						_data[currentTable][hashedKey].value = std::move(value);
						return _CCKHT_insertData(std::move(temp), iterations);
					}
					else
					{
						_data[currentTable][hashedKey].key = std::move(key);
						_data[currentTable][hashedKey].value = std::move(value);
						_data[currentTable][hashedKey].occupied = true;
						return true;
					}
					iterations++;
				}
				resize();
				iterations = 0;
			}
		}

		const bool _CCKHT_insertData(const K_V_pair& k_v_pair, uint32_t iterations = 0)
		{
			while (true)
			{
				while (iterations < _maxIters)
				{
					const uint32_t currentTable = iterations % _tablesCount;
					const uint32_t hashedKey = _g_CCKHT_l_hashFunction(k_v_pair.key, _capacity, _tablesCount, iterations);

					if (_data[currentTable][hashedKey].occupied)
					{
						K_V_pair temp;
						temp.key = _data[currentTable][hashedKey].key;
						temp.value = _data[currentTable][hashedKey].value;
						_data[currentTable][hashedKey].key = k_v_pair.key;
						_data[currentTable][hashedKey].value = k_v_pair.value;
						return _CCKHT_insertData(std::move(temp), iterations);
					}
					else
					{
						_data[currentTable][hashedKey].key = k_v_pair.key;
						_data[currentTable][hashedKey].value = k_v_pair.value;
						_data[currentTable][hashedKey].occupied = true;
						return true;
					}
					iterations++;
				}
				resize();
				iterations = 0;
			}
		}

		const bool _CCKHT_insertData(K_V_pair&& k_v_pair, uint32_t iterations = 0)
		{
			while (true)
			{
				while (iterations < _maxIters)
				{
					const uint32_t currentTable = iterations % _tablesCount;
					const uint32_t hashedKey = _g_CCKHT_l_hashFunction(k_v_pair.key, _capacity, _tablesCount, iterations);

					if (_data[currentTable][hashedKey].occupied)
					{
						K_V_pair temp;
						temp.key = _data[currentTable][hashedKey].key;
						temp.value = _data[currentTable][hashedKey].value;
						_data[currentTable][hashedKey].key = std::move(k_v_pair.key);
						_data[currentTable][hashedKey].value = std::move(k_v_pair.value);
						k_v_pair = std::move(temp);
					}
					else
					{
						_data[currentTable][hashedKey].key = std::move(k_v_pair.key);
						_data[currentTable][hashedKey].value = std::move(k_v_pair.value);
						_data[currentTable][hashedKey].occupied = true;
						return true;
					}
					iterations++;
				}
				resize();
				iterations = 0;
			}
		}

		const bool _insert(const K_V_pair& pair)
		{
			auto contains = std::move(_contains_with_place(pair.key));

			switch (contains.second)
			{
			case 0:
				contains.first->key = pair.key;
				contains.first->value = pair.value;
				contains.first->occupied = true;
				return true;
			case 1:
				return false;
			case 2:
				K_V_pair tmp{ contains.first->key,contains.first->value };
				contains.first->key = pair.key;
				contains.first->value = pair.value;
				return _CCKHT_insertData(std::move(tmp), 1);
			}
		}

		const bool _insert(K_V_pair&& pair)
		{
			auto contains = std::move(_contains_with_place(pair.key));

			switch (contains.second)
			{
			case 0:
				contains.first->key = std::move(pair.key);
				contains.first->value = std::move(pair.value);
				contains.first->occupied = true;
				return true;
			case 1:
				return false;
			case 2:
				K_V_pair tmp{ contains.first->key,contains.first->value };
				contains.first->key = pair.key;
				contains.first->value = pair.value;
				return _CCKHT_insertData(std::move(tmp), 1);
			}
		}
		// Check if map contains value on [key], 0 - not occupied, 1 - has same key, 2 collision
		std::pair<TableSlot*, const int> _contains_with_place(const K& key)
		{
			for (uint32_t iterations = 0; iterations < _tablesCount; iterations++)
			{
				const uint32_t currentTable = iterations % _tablesCount;
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, iterations);

				if (_data[currentTable][hashedKey].occupied && _data[currentTable][hashedKey].key == key)
				{
					return std::make_pair(&_data[currentTable][hashedKey], 1);
				}
				else if (!_data[currentTable][hashedKey].occupied)
				{
					return std::make_pair(&_data[currentTable][hashedKey], 0);
				}
			}
			auto* place = &_data[0 % _tablesCount][_g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, 0)];
			return std::make_pair(place, 2);
		}
		// Check if map contains value on [key], 0 - not occupied, 1 - has same key, 2 collision
		std::pair<TableSlot*, const int> _contains_with_place(const K&& key)
		{
			for (uint32_t iterations = 0; iterations < _tablesCount; iterations++)
			{
				const uint32_t currentTable = iterations % _tablesCount;
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, iterations);

				if (_data[currentTable][hashedKey].occupied && _data[currentTable][hashedKey].key == key)
				{
					return  std::make_pair(&_data[currentTable][hashedKey], 1);
				}
				else if (!_data[currentTable][hashedKey].occupied)
				{
					return std::make_pair(&_data[currentTable][hashedKey], 0);
				}
			}
			auto* place = &_data[0 % _tablesCount][_g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, 0)];
			return std::make_pair(place, 2);
		}

	public:
		// Exchanges the content of container with other 
		const bool swap(cuckooHashMap<K, V>& other)
		{
			std::swap(_tablesCount, other._tablesCount);
			std::swap(_capacity, other._capacity);
			std::swap(_maxIters, other._maxIters);
			std::swap(_data, other._data);

			return true;
		}
		// Count all elements that satisfy the predicate
		// Predicate must take parameters in (K key, V value) form
		template <typename PredicateT>
		const uint32_t count_if(const PredicateT& predicate) const
		{
			uint32_t count = 0;
			for (auto& table : _data)
			{
				for (auto& slot : table)
				{
					if (predicate(slot.key, slot.value))
					{
						count += 1;
					}
				}
			}
			return count;
		}
		// Count all elements that satisfy the predicate
		// Predicate must take parameters in (K key, V value) form
		template <typename PredicateT>
		const uint32_t count_if(const PredicateT&& predicate) const
		{
			uint32_t count = 0;
			for (auto& table : _data)
			{
				for (auto& slot : table)
				{
					if (predicate(slot.key, slot.value))
					{
						count += 1;
					}
				}
			}
			return count;
		}
		// Erases all elements that satisfy the predicate from the container
		// Predicate must take parameters in (K key, V value) form
		template <typename PredicateT>
		const uint32_t erase_if(const PredicateT& predicate)
		{
			uint32_t erasuresCount = 0;
			for (auto& table : _data)
			{
				for (auto& slot : table)
				{
					if (predicate(slot.key, slot.value))
					{
						slot.value = V();
						slot.key = K();
						slot.occupied = false;
						erasuresCount += 1;
					}
				}
			}
			return erasuresCount;
		}
		// Erases all elements that satisfy the predicate pred from the container
		// Predicate must take parameters in (K key, V value) form
		template <typename PredicateT>
		const uint32_t erase_if(PredicateT&& predicate)
		{
			uint32_t erasuresCount = 0;
			for (auto& table : _data)
			{
				for (auto& slot : table)
				{
					if (predicate(slot.key, slot.value))
					{
						slot.value = V();
						slot.key = K();
						slot.occupied = false;
						erasuresCount += 1;
					}
				}
			}
			return erasuresCount;
		}

		// Extract element by key
		K_V_pair extract(const K& key)
		{
			uint32_t iters = 0;
			while (iters < _maxIters)
			{
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, iters);
				const uint32_t currentTable = iters % _tablesCount;

				if (_data[currentTable][hashedKey].occupied && _data[currentTable][hashedKey].key == key)
				{
					K_V_pair tmp;
					tmp.key = _data[currentTable][hashedKey].key;
					tmp.value = _data[currentTable][hashedKey].value;
					erase(key);
					return std::move(tmp);
				}
				iters++;
			}
			return K_V_pair();
		}

		// Extract element by key
		K_V_pair extract(const K&& key)
		{
			uint32_t iters = 0;
			while (iters < _maxIters)
			{
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, iters);
				const uint32_t currentTable = iters % _tablesCount;
				if (_data[currentTable][hashedKey].occupied && _data[currentTable][hashedKey].key == key)
				{
					K_V_pair tmp;
					tmp.key = _data[currentTable][hashedKey].key;
					tmp.value = _data[currentTable][hashedKey].value;
					erase(std::move(key));
					return std::move(tmp);
				}
				iters++;
			}
			return K_V_pair();
		}
		// Extract elements by keys from init list
		std::vector<K_V_pair> extract(const std::initializer_list<K>& l)
		{
			std::vector<K_V_pair> results(l.size());
			uint32_t index = 0;
			for (auto& element : l)
			{
				results[index] = std::move(extract(element));
				index++;
			}
			return results;
		}

		// Extract elements by keys from init list
		std::vector<K_V_pair> extract(const std::initializer_list<K>&& l)
		{
			std::vector<K_V_pair> results(l.size());
			uint32_t index = 0;
			for (auto& element : l)
			{
				results[index] = std::move(extract(element));
				index++;
			}
			return results;
		}

		// Erase all elements.
		void clear()
		{
			_data = std::vector<std::vector<TableSlot>>();
			_data.resize(_tablesCount);
			for (uint32_t tables = 0; tables < _tablesCount; tables++)
			{
				_data[tables].resize(_capacity);
			}
		}

		// Erase element by key
		const bool erase(const K& key)
		{
			for (uint32_t iters = 0; iters < _maxIters; iters++)
			{
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, iters);
				const uint32_t currentTable = iters % _tablesCount;

				if (_data[currentTable][hashedKey].occupied && _data[currentTable][hashedKey].key == key)
				{
					_data[currentTable][hashedKey].key = K();
					_data[currentTable][hashedKey].value = V();
					_data[currentTable][hashedKey].occupied = false;
					return true;
				}
			}
			return false;
		}
		// Erase element by key
		const bool erase(const K&& key)
		{
			for (uint32_t iters = 0; iters < _maxIters; iters++)
			{
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, iters);
				const uint32_t currentTable = iters % _tablesCount;

				if (_data[currentTable][hashedKey].occupied && _data[currentTable][hashedKey].key == key)
				{
					_data[currentTable][hashedKey].key = K();
					_data[currentTable][hashedKey].value = V();
					_data[currentTable][hashedKey].occupied = false;
					return true;
				}
			}
			return false;
		}
		// Erase elements by keys from init list.
		std::vector<bool> erase(const std::initializer_list<K>& l)
		{
			std::vector<bool> results(l.size(), false);
			uint32_t index = 0;
			for (auto& key : l)
			{
				results[index] = std::move(erase(key));
				index++;
			}
			return results;
		}

		// Erase elements by keys from init list.
		std::vector<bool> erase(const std::initializer_list<K>&& l)
		{
			std::vector<bool> results(l.size(), false);
			uint32_t index = 0;
			for (auto& key : l)
			{
				results[index] = std::move(erase(key));
				index++;
			}
			return results;
		}

		// Find element by key, returns a pointer to value
		V* find(const K& key)
		{
			uint32_t iters = 0;
			while (iters < _maxIters)
			{
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, iters);
				const uint32_t currentTable = iters % _tablesCount;

				if (_data[currentTable][hashedKey].occupied && _data[currentTable][hashedKey].key == key)
					return &_data[currentTable][hashedKey].value;
				iters++;
			}
			return nullptr;
		}

		// Find element by key, returns a pointer to value
		V* find(const K&& key)
		{
			uint32_t iters = 0;
			while (iters < _maxIters)
			{
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, iters);
				const uint32_t currentTable = iters % _tablesCount;

				if (_data[currentTable][hashedKey].occupied && _data[currentTable][hashedKey].key == key)
					return &_data[currentTable][hashedKey].value;
				iters++;
			}
			return nullptr;
		}

		// Insert element by key and value
		const bool insert(const K& key, const V& value)
		{
			return _insert({ key,value });
		}
		// Insert element by {key} and {value}
		const bool insert(const K&& key, const V&& value)
		{
			return _insert({ std::move(key),std::move(value) });
		}
		// Insert element by key, value
		const bool insert(const K_V_pair& k_v_pair)
		{
			return _insert(k_v_pair);
		}
		// Insert element by {key, value}
		const bool insert(const K_V_pair&& k_v_pair)
		{
			return _insert(std::move(k_v_pair));
		}

		// Insert elements by {{keys, values},{...},...}
		std::vector<bool> insert(const std::initializer_list<K_V_pair>& l) {
			std::vector<bool> results(l.size(), false);
			uint32_t iter = 0;
			for (auto& element : l)
			{
				results[iter] = _insert(element);

				iter++;
			}
			return results;
		}

		// Insert elements by {{keys, values},{...},...}
		std::vector<bool> insert(const std::initializer_list<K_V_pair>&& l) {
			std::vector<bool> results(l.size(), false);
			uint32_t iter = 0;
			for (auto& element : l)
			{
				results[iter] = _insert(element);

				iter++;
			}
			return results;
		}

		const bool insert_or_assign(const K& key, const V& value)
		{
			auto assignPos = find(key);
			if (assignPos)
			{
				*assignPos = value;
				return false;
			}

			return _CCKHT_insertData({ key,value });
		}

		const bool insert_or_assign(const K&& key, const V&& value)
		{
			auto assignPos = find(key);
			if (assignPos)
			{
				*assignPos = value;
				return false;
			}
			return _CCKHT_insertData({ std::move(key),std::move(value) });
		}

		const bool insert_or_assign(const K_V_pair& k_v_pair)
		{
			auto assignPos = find(k_v_pair.key);
			if (assignPos)
			{
				*assignPos = k_v_pair.value;
				return false;
			}

			return _CCKHT_insertData(k_v_pair);
		}

		const bool insert_or_assign(K_V_pair&& k_v_pair)
		{
			auto assignPos = find(k_v_pair.key);
			if (assignPos)
			{
				*assignPos = std::move(k_v_pair.value);
				return false;
			}

			return _CCKHT_insertData(std::move(k_v_pair));
		}

		const std::vector<bool> insert_or_assign(const std::initializer_list<K_V_pair>& l)
		{
			std::vector<bool> results(l.size(), false);
			uint32_t index = 0;
			for (auto& element : l)
			{
				auto assignPos = find(element.key);
				if (assignPos)
				{
					*assignPos = element.value;
				}
				else
				{
					results[index] = _CCKHT_insertData(element);
				}
				index++;
			}
		}

		const std::vector<bool> insert_or_assign(const std::initializer_list<K_V_pair>&& l)
		{
			std::vector<bool> results(l.size(), false);
			uint32_t index = 0;
			for (auto& element : l)
			{
				auto assignPos = find(element.key);
				if (assignPos)
				{
					*assignPos = element.value;
				}
				else
				{
					results[index] = _CCKHT_insertData(element);
				}
				index++;
			}
		}

		const std::vector<bool> insert_from_array(const K keys[], const V values[], const uint32_t size)
		{
			std::vector<bool> results(size, false);
			for (int iter = 0; iter < size; iter++)
			{
				results[iter] = _insert({ keys[iter],values[iter] });
			}
			return results;
		}

		// Get internal container
		const std::vector<std::vector<TableSlot>>& rawData() const
		{
			return _data;
		}
		// Return tables count
		const uint32_t tablesCount() const
		{
			return _tablesCount;
		}
		// Return capacity 
		const uint32_t capacity() const
		{
			return _capacity;
		}
		// Return capacity * tables count
		const uint32_t totalCapacity() const
		{
			return _capacity * _tablesCount;
		}

		// Find element by [key]
		V* operator [](const K& key)
		{
			return find(key);
		}

		// Find element by [key]
		V* operator [](const K&& key)
		{
			return find(key);
		}
		// Get load factor
		const double loadFactor() const
		{
			uint32_t result = 0;
			for (auto table : _data)
			{
				for (auto& slot : table)
					result += slot.occupied;
			}
			return (double)((double)result / (double)totalCapacity());
		}
		// Check if map contains value on [key]
		const bool contains(const K& key) const
		{
			for (uint32_t iterations = 0; iterations < _tablesCount; iterations++)
			{
				const uint32_t currentTable = iterations % _tablesCount;
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, iterations);

				if (_data[currentTable][hashedKey].occupied && _data[currentTable][hashedKey].key == key)
				{
					return true;
				}
			}
			return false;
		}
		// Check if map contains value on [key]
		const bool contains(const K&& key) const
		{
			for (uint32_t iterations = 0; iterations < _tablesCount; iterations++)
			{
				const uint32_t currentTable = iterations % _tablesCount;
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, iterations);

				if (_data[currentTable][hashedKey].occupied && _data[currentTable][hashedKey].key == key)
				{
					return true;
				}
			}
			return false;
		}
		// Return count of values on [key] (1 or 0)
		const int count(const K& key) const
		{
			return contains(key);
		}
		// Return count of values on [key] (1 or 0)
		const int count(const K&& key) const
		{
			return contains(std::move(key));
		}
	};


	// Experimental

	// Node-like version of cuckoo hash map
	// Do not need for default construction of K, V types and occupy less memory(unless your data size is less than size of pointer) with cost of iterating over pointers to elements instead of raw elements in vector
	template<typename K, typename V>
	class cuckooNodeHashMap
	{
	public:

		cuckooNodeHashMap(const uint32_t capacity = 64, const uint32_t tablesCount = 2)
			: _capacity(capacity), _tablesCount(tablesCount), _maxIters((uint32_t)(tablesCount* HARZ_CCKHASH_MAP_MAX_ITERATIONS_MOD) + 1)
		{

			_data.resize(tablesCount);
			for (uint32_t iter = 0; iter < tablesCount; iter++)
			{
				_data[iter].resize(capacity);
			}
		}

		struct K_V_pair
		{
			K key;
			V value;
		};

		struct TableSlot
		{
			std::shared_ptr<K_V_pair> element{ nullptr };
		};

		// Change capacity("rehash") container, possible recursive call to resize while inserting elements from old _data
		const bool resize(uint32_t newCapacity = 0)
		{
			if (newCapacity <= 0)
			{
				newCapacity = (uint32_t)(_capacity * HARZ_CCKHASH_MAP_RESIZE_MOD) + 1;
			}

			std::vector<std::vector<TableSlot>> oldData = _data;

			_capacity = newCapacity;

			clear();

			for (auto& table : oldData)
			{
				for (auto& slot : table)
				{
					if (slot.element)
					{
						_insert(std::move(*slot.element));
						slot.element.reset();
					}
				};
			};

			return true;
		};

		// Change tables count, possible recursive call to resize while inserting elements from old _data
		const bool restrain(const uint32_t newTablesCount)
		{
			if (newTablesCount <= 2)
				return false;

			_tablesCount = newTablesCount;
			_maxIters = _tablesCount * HARZ_CCKHASH_MAP_MAX_ITERATIONS_MOD;

			resize(_capacity);
			return true;
		};


	private:
		uint32_t _capacity = 64;
		uint32_t _tablesCount = 2;
		uint32_t _maxIters = _tablesCount * HARZ_CCKHASH_MAP_MAX_ITERATIONS_MOD + 1;

		std::vector<std::vector<TableSlot>> _data;

		const std::function <const uint32_t(K, uint32_t, uint32_t, uint32_t)> _g_CCKHT_l_hashFunction = [](const K& key, uint32_t cap, uint32_t tablecnt, uint32_t i)-> const uint32_t
		{
			return ((std::hash< uint32_t>()(std::hash<K>()(key) + std::hash< uint32_t>()(i % (tablecnt + cap))))) % cap;
		};

		const bool _CCKHT_insertData(const K& key, const V& value, uint32_t iterations = 0)
		{
			while (true)
			{
				while (iterations < _maxIters)
				{
					const uint32_t currentTable = iterations % _tablesCount;
					const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, iterations);

					if (_data[currentTable][hashedKey].element)
					{
						K_V_pair temp;
						temp.key = _data[currentTable][hashedKey].element->key;
						temp.value = _data[currentTable][hashedKey].element->value;
						_data[currentTable][hashedKey].element->key = key;
						_data[currentTable][hashedKey].element->value = value;
						return _CCKHT_insertData(std::move(temp), iterations);
					}
					else
					{
						_data[currentTable][hashedKey].element.reset(new K_V_pair{ key, value });
						return true;
					}
					iterations++;
				}
				resize();
				iterations = 0;
			}
		}

		const bool _CCKHT_insertData(const K&& key, const V&& value, uint32_t iterations = 0)
		{
			while (true)
			{
				while (iterations < _maxIters)
				{
					const uint32_t currentTable = iterations % _tablesCount;
					const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, iterations);

					if (_data[currentTable][hashedKey].element)
					{
						K_V_pair temp;
						temp.key = _data[currentTable][hashedKey].element->key;
						temp.value = _data[currentTable][hashedKey].element->value;
						_data[currentTable][hashedKey].element->key = std::move(key);
						_data[currentTable][hashedKey].element->value = std::move(value);
						return _CCKHT_insertData(std::move(temp), iterations);
					}
					else
					{
						_data[currentTable][hashedKey].element.reset(new K_V_pair{ key, value });
						return true;
					}
					iterations++;
				}
				resize();
				iterations = 0;
			}
		}

		const bool _CCKHT_insertData(const K_V_pair& k_v_pair, uint32_t iterations = 0)
		{
			while (true)
			{
				while (iterations < _maxIters)
				{
					const uint32_t currentTable = iterations % _tablesCount;
					const uint32_t hashedKey = _g_CCKHT_l_hashFunction(k_v_pair.key, _capacity, _tablesCount, iterations);

					if (_data[currentTable][hashedKey].element)
					{
						K_V_pair temp;
						temp.key = _data[currentTable][hashedKey].element->key;
						temp.value = _data[currentTable][hashedKey].element->value;
						_data[currentTable][hashedKey].element->key = k_v_pair.key;
						_data[currentTable][hashedKey].element->value = k_v_pair.value;
						return _CCKHT_insertData(std::move(temp), iterations);
					}
					else
					{
						_data[currentTable][hashedKey].element.reset(new K_V_pair{ k_v_pair.key, k_v_pair.value });
						return true;
					}
					iterations++;
				}
				resize();
				iterations = 0;
			}
		}

		const bool _CCKHT_insertData(K_V_pair&& k_v_pair, uint32_t iterations = 0)
		{
			while (true)
			{

				while (iterations < _maxIters)
				{
					const uint32_t currentTable = iterations % _tablesCount;
					const uint32_t hashedKey = _g_CCKHT_l_hashFunction(k_v_pair.key, _capacity, _tablesCount, iterations);

					if (_data[currentTable][hashedKey].element)
					{
						K_V_pair temp;
						temp.key = _data[currentTable][hashedKey].element->key;
						temp.value = _data[currentTable][hashedKey].element->value;
						_data[currentTable][hashedKey].element->key = std::move(k_v_pair.key);
						_data[currentTable][hashedKey].element->value = std::move(k_v_pair.value);
						k_v_pair = std::move(temp);
					}
					else
					{
						_data[currentTable][hashedKey].element.reset(new K_V_pair{ std::move(k_v_pair) });
						return true;
					}
					iterations++;
				}
				resize();
				iterations = 0;
			}
		}

		const bool _insert(const K_V_pair& pair)
		{
			auto contains = std::move(_contains_with_place(pair.key));

			switch (contains.second)
			{
			case 0:
				contains.first->element.reset(new K_V_pair(std::move(pair)));
				return true;
			case 1:
				return false;
			case 2:
				K_V_pair tmp{ contains.first->element->key,contains.first->element->value };
				contains.first->element->key = pair.key;
				contains.first->element->value = pair.value;
				return _CCKHT_insertData(std::move(tmp), 1);
			}
		}

		const bool _insert(K_V_pair&& pair)
		{
			auto contains = std::move(_contains_with_place(pair.key));

			switch (contains.second)
			{
			case 0:
				contains.first->element.reset(new K_V_pair(std::move(pair)));
				return true;
			case 1:
				return false;
			case 2:
				std::swap(pair, *contains.first->element);
				return _CCKHT_insertData(std::move(pair), 1);
			}
		}
		// Check if map contains value on [key], 0 - not occupied, 1 - has same key, 2 collision
		std::pair<TableSlot*, const int> _contains_with_place(const K& key)
		{
			for (uint32_t iterations = 0; iterations < _tablesCount; iterations++)
			{
				const uint32_t currentTable = iterations % _tablesCount;
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, iterations);

				if (_data[currentTable][hashedKey].element && _data[currentTable][hashedKey].element->key == key)
				{
					return std::make_pair(&_data[currentTable][hashedKey], 1);
				}
				else if (!_data[currentTable][hashedKey].element)
				{
					return std::make_pair(&_data[currentTable][hashedKey], 0);
				}
			}
			TableSlot* place = &_data[0 % _tablesCount][_g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, 0)];
			return std::make_pair(place, 2);
		}
		// Check if map contains value on [key], 0 - not occupied, 1 - has same key, 2 collision
		std::pair<TableSlot*, const int> _contains_with_place(const K&& key)
		{
			for (uint32_t iterations = 0; iterations < _tablesCount; iterations++)
			{
				const uint32_t currentTable = iterations % _tablesCount;
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, iterations);

				if (_data[currentTable][hashedKey].element && _data[currentTable][hashedKey].element->key == key)
				{
					return std::make_pair(&_data[currentTable][hashedKey], 1);
				}
				else if (!_data[currentTable][hashedKey].element)
				{
					return std::make_pair(&_data[currentTable][hashedKey], 0);
				}
			}
			TableSlot* place = &_data[0 % _tablesCount][_g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, 0)];
			return std::make_pair(place, 2);
		}

	public:

		// Exchanges the content of container with other 
		const bool swap(cuckooNodeHashMap<K, V>& other)
		{
			std::swap(_tablesCount, other._tablesCount);
			std::swap(_capacity, other._capacity);
			std::swap(_maxIters, other._maxIters);
			std::swap(_data, other._data);

			return true;
		}

		// Count all elements that satisfy the predicate
		// Predicate must take parameters in (K key, V value) form
		template <typename PredicateT>
		const uint32_t count_if(const PredicateT& predicate) const
		{
			uint32_t count = 0;
			for (auto& table : _data)
			{
				for (auto& slot : table)
				{
					if (slot.element)
					{
						if (predicate(slot.element->key, slot.element->value))
						{
							count += 1;
						}
					}
				}
			}
			return count;
		}

		// Count all elements that satisfy the predicate
		// Predicate must take parameters in (K key, V value) form
		template <typename PredicateT>
		const uint32_t count_if(const PredicateT&& predicate) const
		{
			uint32_t count = 0;
			for (auto& table : _data)
			{
				for (auto& slot : table)
				{
					if (slot.element)
					{
						if (predicate(slot.element->key, slot.element->value))
						{
							count += 1;
						}
					}
				}
			}
			return count;
		}

		// Erases all elements that satisfy the predicate pred from the container
		// Predicate must take parameters in (K key, V value) form
		template <typename PredicateT>
		const uint32_t erase_if(const PredicateT& predicate)
		{
			uint32_t erasuresCount = 0;
			for (auto& table : _data)
			{
				for (auto& slot : table)
				{
					if (slot.element)
					{
						if (predicate(slot.element->key, slot.element->value))
						{
							slot.element.reset();
							erasuresCount += 1;
						}
					}
				}
			}
			return erasuresCount;
		}
		// Erases all elements that satisfy the predicate pred from the container
		// Predicate must take parameters in (K key, V value) form
		template <typename PredicateT>
		const uint32_t erase_if(const PredicateT&& predicate)
		{
			uint32_t erasuresCount = 0;
			for (auto& table : _data)
			{
				for (auto& slot : table)
				{
					if (slot.element)
					{
						if (predicate(slot.element->key, slot.element->value))
						{
							slot.element.reset();
							erasuresCount += 1;
						}
					}
				}
			}
			return erasuresCount;
		}

		// Extract element by key
		K_V_pair extract(const K& key)
		{
			uint32_t iters = 0;
			while (iters < _maxIters)
			{
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, iters);
				const uint32_t currentTable = iters % _tablesCount;

				if (_data[currentTable][hashedKey].element && _data[currentTable][hashedKey].element->key == key)
				{
					K_V_pair tmp;
					tmp.key = _data[currentTable][hashedKey].element->key;
					tmp.value = _data[currentTable][hashedKey].element->value;
					_data[currentTable][hashedKey].element.reset();
					return std::move(tmp);
				}
				iters++;
			}
			return K_V_pair();
		}

		// Extract element by key
		K_V_pair extract(const K&& key)
		{
			uint32_t iters = 0;
			while (iters < _maxIters)
			{
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, iters);
				const uint32_t currentTable = iters % _tablesCount;
				if (_data[currentTable][hashedKey].element && _data[currentTable][hashedKey].element->key == key)
				{
					K_V_pair tmp;
					tmp.key = _data[currentTable][hashedKey].element->key;
					tmp.value = _data[currentTable][hashedKey].element->value;
					_data[currentTable][hashedKey].element.reset();
					return std::move(tmp);
				}
				iters++;
			}
			return K_V_pair();
		}
		// Extract elements by keys from init list
		std::vector<K_V_pair> extract(const std::initializer_list<K>& l)
		{
			std::vector<K_V_pair> results(l.size());
			uint32_t index = 0;
			for (auto& element : l)
			{
				results[index] = std::move(extract(element));
				index++;
			}
			return results;
		}

		// Extract elements by keys from init list
		std::vector<K_V_pair> extract(const std::initializer_list<K>&& l)
		{
			std::vector<K_V_pair> results(l.size());
			uint32_t index = 0;
			for (auto& element : l)
			{
				results[index] = std::move(extract(element));
				index++;
			}
			return results;
		}

		// Erase all elements
		void clear()
		{
			for (auto& table : _data)
			{
				for (auto& slot : table)
				{
					slot.element.reset();
				}
			}

			_data.resize(_tablesCount);
			for (uint32_t tables = 0; tables < _tablesCount; tables++)
			{
				_data[tables].resize(_capacity);
			}
		}

		// Erase element by key
		const bool erase(const K& key)
		{
			for (uint32_t iters = 0; iters < _maxIters; iters++)
			{
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, iters);
				const uint32_t currentTable = iters % _tablesCount;

				if (_data[currentTable][hashedKey].element && _data[currentTable][hashedKey].element->key == key)
				{
					_data[currentTable][hashedKey].element.reset();
					return true;
				}
			}
			return false;
		}
		// Erase element by key
		const bool erase(const K&& key)
		{
			for (uint32_t iters = 0; iters < _maxIters; iters++)
			{
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, iters);
				const uint32_t currentTable = iters % _tablesCount;

				if (_data[currentTable][hashedKey].element && _data[currentTable][hashedKey].element->key == key)
				{
					_data[currentTable][hashedKey].element.reset();
					return true;
				}
			}
			return false;
		}
		// Erase elements by keys from init list.
		std::vector<bool> erase(const std::initializer_list<K>& l)
		{
			std::vector<bool> results(l.size(), false);
			uint32_t index = 0;
			for (auto& key : l)
			{
				results[index] = std::move(erase(key));
				index++;
			}
			return results;
		}
		// Erase elements by keys from init list.
		std::vector<bool> erase(const std::initializer_list<K>&& l)
		{
			std::vector<bool> results(l.size(), false);
			uint32_t index = 0;
			for (auto& key : l)
			{
				results[index] = std::move(erase(key));
				index++;
			}
			return results;
		}
		// Find element by key, returns a pointer to value
		V* find(const K& key)
		{
			uint32_t iters = 0;
			while (iters < _maxIters)
			{
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, iters);
				const uint32_t currentTable = iters % _tablesCount;

				if (_data[currentTable][hashedKey].element)
					if (_data[currentTable][hashedKey].element->key == key)
						return &_data[currentTable][hashedKey].element->value;
				iters++;
			}
			return nullptr;
		}

		// Find element by key, returns a pointer to value
		V* find(const K&& key)
		{
			uint32_t iters = 0;
			while (iters < _maxIters)
			{
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, iters);
				const uint32_t currentTable = iters % _tablesCount;

				if (_data[currentTable][hashedKey].element)
					if (_data[currentTable][hashedKey].element->key == key)
						return &_data[currentTable][hashedKey].element->value;
				iters++;
			}
			return nullptr;
		}
		// Insert element by key and value
		const bool insert(const K& key, const V& value)
		{
			return _insert({ key,value });
		}
		// Insert element by {key} and {value}
		const bool insert(const K&& key, const V&& value)
		{
			return _insert({ std::move(key),std::move(value) });
		}
		// Insert element by key, value
		const bool insert(const K_V_pair& k_v_pair)
		{
			return _insert(k_v_pair);
		}
		// Insert element by {key, value}
		const bool insert(const K_V_pair&& k_v_pair)
		{
			return _insert(std::move(k_v_pair));
		}

		// Insert elements by {{keys, values},{...},...}
		std::vector<bool> insert(const std::initializer_list<K_V_pair>& l) {
			std::vector<bool> results(l.size(), false);
			uint32_t index = 0;
			for (auto& element : l)
			{
				results[index] = _insert(element);;
				index++;
			}
			return results;
		}

		// Insert elements by {{keys, values},{...},...}
		std::vector<bool> insert(const std::initializer_list<K_V_pair>&& l) {
			std::vector<bool> results(l.size(), false);
			uint32_t index = 0;
			for (auto& element : l)
			{
				results[index] = _insert(element);;
				index++;
			}
			return results;
		}

		const bool insert_or_assign(const K& key, const V& value)
		{
			auto assignPos = find(key);
			if (assignPos)
			{
				*assignPos = value;
				return false;
			}
			else
				return _CCKHT_insertData(key, value);
		}

		const bool insert_or_assign(const K&& key, const V&& value)
		{
			auto assignPos = find(key);
			if (assignPos)
			{
				*assignPos = std::move(value);
				return false;
			}
			else
				return _CCKHT_insertData(std::move(key), std::move(value));
		}

		const bool insert_or_assign(const K_V_pair& k_v_pair)
		{
			auto assignPos = find(k_v_pair.key);
			if (assignPos)
			{
				*assignPos = k_v_pair.value;
				return false;
			}
			else
				return _CCKHT_insertData(k_v_pair);
		}

		const bool insert_or_assign(const K_V_pair&& k_v_pair)
		{
			auto assignPos = find(k_v_pair.key);
			if (assignPos)
			{
				*assignPos = std::move(k_v_pair.value);
				return false;
			}
			else
				return _CCKHT_insertData(std::move(k_v_pair));
		}

		const std::vector<bool> insert_or_assign(const std::initializer_list<K_V_pair>& l)
		{
			std::vector<bool> results(l.size(), false);
			uint32_t index = 0;
			for (auto& element : l)
			{
				auto assignPos = find(element.key);
				if (assignPos)
				{
					*assignPos = element.value;
				}
				else
				{
					results[index] = (std::move(_CCKHT_insertData(element)));
				}
				index++;
			}
			return results;
		}

		const std::vector<bool> insert_or_assign(const std::initializer_list<K_V_pair>&& l)
		{
			std::vector<bool> results(l.size(), false);
			uint32_t index = 0;
			for (auto& element : l)
			{
				auto assignPos = find(element.key);
				if (assignPos)
				{
					*assignPos = element.value;
				}
				else
				{
					results[index] = (std::move(_CCKHT_insertData(element)));
				}
				index++;
			}
			return results;
		}

		const std::vector<bool> insert_from_array(const K keys[], const V values[], const uint32_t size)
		{
			std::vector<bool> results(size, false);
			for (int iter = 0; iter < size; iter++)
			{
				results[iter] = _insert({ keys[iter],values[iter] });
			}
			return results;
		}

		// Get internal container
		const std::vector<std::vector<TableSlot>>& rawData() const
		{
			return _data;
		}
		// Return tables count
		const uint32_t tablesCount() const
		{
			return _tablesCount;
		}
		// Return capacity 
		const uint32_t capacity() const
		{
			return _capacity;
		}
		// Return capacity * tables count
		const uint32_t totalCapacity() const
		{
			return _capacity * _tablesCount;
		}

		// Find element by [key]
		V* operator [](const K& key)
		{
			return find(key);
		}

		// Find element by [key]
		V* operator [](const K&& key)
		{
			return find(key);
		}
		// Get const shared ptr to element in map by[key]
		const std::shared_ptr<const K_V_pair> getShare(const K& key) const
		{
			for (uint32_t iterations = 0; iterations < _tablesCount; iterations++)
			{
				const uint32_t currentTable = iterations % _tablesCount;
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, iterations);

				if (_data[currentTable][hashedKey].element && _data[currentTable][hashedKey].element->key == key)
				{
					return _data[currentTable][hashedKey].element;
				}
			}
			return nullptr;
		}
		// Get const shared ptr to element in map by[key]
		const std::shared_ptr<const K_V_pair> getShare(const K&& key) const
		{
			for (uint32_t iterations = 0; iterations < _tablesCount; iterations++)
			{
				const uint32_t currentTable = iterations % _tablesCount;
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, iterations);

				if (_data[currentTable][hashedKey].element && _data[currentTable][hashedKey].element->key == key)
				{
					return _data[currentTable][hashedKey].element;
				}
			}
			return nullptr;
		}
		// Get load factor
		const double loadFactor() const
		{
			uint32_t result = 0;
			for (auto& table : _data)
			{
				for (auto& slot : table)
					result += (bool)(slot.element);
			}
			return (double)((double)result / (double)totalCapacity());
		}
		// Check if map contains value on [key]
		const bool contains(const K& key) const
		{
			for (uint32_t iterations = 0; iterations < _tablesCount; iterations++)
			{
				const uint32_t currentTable = iterations % _tablesCount;
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, iterations);

				if (_data[currentTable][hashedKey].element && _data[currentTable][hashedKey].element->key == key)
				{
					return true;
				}
			}
			return false;
		}
		// Check if map contains value on [key]
		const bool contains(const K&& key) const
		{
			for (uint32_t iterations = 0; iterations < _tablesCount; iterations++)
			{
				const uint32_t currentTable = iterations % _tablesCount;
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, iterations);

				if (_data[currentTable][hashedKey].element && _data[currentTable][hashedKey].element->key == key)
				{
					return true;
				}
			}
			return false;
		}
		// Return count of values on [key] (1 or 0)
		const int count(const K& key) const
		{
			return contains(key);
		}
		// Return count of values on [key] (1 or 0)
		const int count(const K&& key) const
		{
			return contains(key);
		}
	};
}
#endif // !HARZ_CCKHASH