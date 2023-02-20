#ifndef HARZ_CCKHASH_MAP
#define HARZ_CCKHASH_MAP

// Custom implementation of hash map based on cuckoo hashing
// c++14 and newer versions(possibly c++11 also, but not guaranteed)
// made by IHarzI

#include <vector>
#include <functional>

// custom params, experiment with different values for better perfomance
#define HARZ_CCKHASH_MAP_MAX_ITERATIONS_MOD (2.5f) 
#define HARZ_CCKHASH_MAP_RESIZE_MOD (1.5f)

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

		// ("rehash") container,  be careful if give custom parameter, possible loss of data (if newCapacity < current capacity)
		bool resize(uint32_t newCapacity = 0)
		{
			if (newCapacity <= 0)
			{
				newCapacity = (uint32_t)(_capacity * HARZ_CCKHASH_MAP_RESIZE_MOD) + 1;
			}

			std::vector<std::vector<TableSlot>> oldData = _data;

			_capacity = newCapacity;

			clear();

			for (std::vector<TableSlot> table : oldData)
			{
				for (auto& slot : table)
				{
					if (slot.occupied)
						insert(slot.key, slot.value);
				};
			};

			return true;
		};

		// change tables count, be careful if give custom parameter, possible loss of data (if newTablesCount < current tables count)
		bool restrain(const uint32_t newTablesCount)
		{
			if (newTablesCount <= 2)
				return false;

			_data.resize(newTablesCount);
			_tablesCount = newTablesCount;
			resize(_capacity);

			_maxIters = _tablesCount * HARZ_CCKHASH_MAP_MAX_ITERATIONS_MOD;
			return true;
		};


	private:
		uint32_t _capacity = 64;
		uint32_t _tablesCount = 2;
		uint32_t _maxIters = _tablesCount * HARZ_CCKHASH_MAP_MAX_ITERATIONS_MOD + 1;

		std::vector<std::vector<TableSlot>> _data;

		std::function <const uint32_t(K, uint32_t, uint32_t, uint32_t)> _g_CCKHT_l_hashFunction = [](K key, uint32_t cap, uint32_t tablecnt, uint32_t i)-> const uint32_t
		{
			return ((std::hash< uint32_t>()(std::hash<K>()(key) + std::hash< uint32_t>()(i % (tablecnt + cap))))) % cap;
		};

		bool _CCKHT_insertData(const K& key, const V& value, uint32_t iterations = 0)
		{
			while (true)
			{
				while (iterations < _maxIters)
				{
					const uint32_t currentTable = iterations % _tablesCount;
					const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, iterations);

					if (_data[currentTable][hashedKey].key == key)
					{
						return false;
					}
					else if (_data[currentTable][hashedKey].occupied)
					{
						K_V_pair temp;
						temp.key = _data[currentTable][hashedKey].key;
						temp.value = _data[currentTable][hashedKey].value;
						_data[currentTable][hashedKey].key = key;
						_data[currentTable][hashedKey].value = value;
						return _CCKHT_insertData(std::move(temp), iterations);
					}
					else if (!_data[currentTable][hashedKey].occupied)
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


		bool _CCKHT_insertData(K&& key, V&& value, uint32_t iterations = 0)
		{
			while (true)
			{
				while (iterations < _maxIters)
				{
					const uint32_t currentTable = iterations % _tablesCount;
					const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, iterations);

					if (_data[currentTable][hashedKey].key == key)
					{
						return false;
					}
					else if (_data[currentTable][hashedKey].occupied)
					{
						K_V_pair temp;
						temp.key = _data[currentTable][hashedKey].key;
						temp.value = _data[currentTable][hashedKey].value;
						_data[currentTable][hashedKey].key = std::move(key);
						_data[currentTable][hashedKey].value = std::move(value);
						return _CCKHT_insertData(std::move(temp), iterations);
					}
					else if (!_data[currentTable][hashedKey].occupied)
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


		bool _CCKHT_insertData(const K&& key, const V&& value, uint32_t iterations = 0)
		{
			while (true)
			{
				while (iterations < _maxIters)
				{
					const uint32_t currentTable = iterations % _tablesCount;
					const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, iterations);

					if (_data[currentTable][hashedKey].key == key)
					{
						return false;
					}
					else if (_data[currentTable][hashedKey].occupied)
					{
						K_V_pair temp;
						temp.key = _data[currentTable][hashedKey].key;
						temp.value = _data[currentTable][hashedKey].value;
						_data[currentTable][hashedKey].key = std::move(key);
						_data[currentTable][hashedKey].value = std::move(value);
						return _CCKHT_insertData(std::move(temp), iterations);
					}
					else if (!_data[currentTable][hashedKey].occupied)
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

		bool _CCKHT_insertData(const K_V_pair& k_v_pair, uint32_t iterations = 0)
		{
			while (true)
			{
				while (iterations < _maxIters)
				{
					const uint32_t currentTable = iterations % _tablesCount;
					const uint32_t hashedKey = _g_CCKHT_l_hashFunction(k_v_pair.key, _capacity, _tablesCount, iterations);

					if (_data[currentTable][hashedKey].key == k_v_pair.key)
					{
						return false;
					}
					else if (_data[currentTable][hashedKey].occupied)
					{
						K_V_pair temp;
						temp.key = _data[currentTable][hashedKey].key;
						temp.value = _data[currentTable][hashedKey].value;
						_data[currentTable][hashedKey].key = k_v_pair.key;
						_data[currentTable][hashedKey].value = k_v_pair.value;
						return _CCKHT_insertData(std::move(temp), iterations);
					}
					else if (!_data[currentTable][hashedKey].occupied)
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

		bool _CCKHT_insertData(K_V_pair&& k_v_pair, uint32_t iterations = 0)
		{
			while (true)
			{

				while (iterations < _maxIters)
				{
					const uint32_t currentTable = iterations % _tablesCount;
					const uint32_t hashedKey = _g_CCKHT_l_hashFunction(k_v_pair.key, _capacity, _tablesCount, iterations);

					if (_data[currentTable][hashedKey].key == k_v_pair.key)
					{
						return false;
					}
					else if (_data[currentTable][hashedKey].occupied)
					{
						K_V_pair temp;
						temp.key = _data[currentTable][hashedKey].key;
						temp.value = _data[currentTable][hashedKey].value;
						_data[currentTable][hashedKey].key = std::move(k_v_pair.key);
						_data[currentTable][hashedKey].value = std::move(k_v_pair.value);
						k_v_pair = temp;
					}
					else if (!_data[currentTable][hashedKey].occupied)
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

	public:

		K_V_pair extract(K& key)
		{
			uint32_t iters = 0;
			while (iters < _maxIters)
			{
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, iters);
				const uint32_t currentTable = iters % _tablesCount;

				if (_data[currentTable][hashedKey].occupied)
					if (_data[currentTable][hashedKey].key == key)
					{
						K_V_pair temp(_data[currentTable][hashedKey].key, _data[currentTable][hashedKey].value);
						erase(key);
						return std::move(temp);
					}
				iters++;
			}
			return K_V_pair();
		}

		K_V_pair extract(K&& key)
		{
			uint32_t iters = 0;
			while (iters < _maxIters)
			{
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, iters);
				const uint32_t currentTable = iters % _tablesCount;
				if (_data[currentTable][hashedKey].occupied)
					if (_data[currentTable][hashedKey].key == key)
					{
						K_V_pair temp(_data[currentTable][hashedKey].key, _data[currentTable][hashedKey].value);
						erase(key);
						return std::move(temp);
					}
				iters++;
			}
			return K_V_pair();
		}

		std::vector<K_V_pair> extract(std::initializer_list<K> l)
		{
			std::vector<K_V_pair> results;
			results.reserve(l.size());
			uint32_t index = 0;
			for (index < l.size(); index++;)
			{
				auto& element = l[index];
				uint32_t iters = 0;
				while (iters < _maxIters)
				{
					const uint32_t hashedKey = _g_CCKHT_l_hashFunction(element, _capacity, _tablesCount, iters);

					if (_data[iters % _tablesCount][hashedKey].occupied)
						if (_data[iters % _tablesCount][hashedKey].key == element)
						{
							results.push_back(_data[iters % _tablesCount][hashedKey].key, _data[iters % _tablesCount][hashedKey].value);
							erase(element);
						}
					iters++;
				}
			}
			return results;
		}

		// delete all elements
		void clear()
		{
			_data = std::vector<std::vector<TableSlot>>();
			_data.resize(_tablesCount);
			for (uint32_t tables = 0; tables < _tablesCount; tables++)
			{
				_data[tables].resize(_capacity);
			}
		}

		std::vector<bool> erase(std::initializer_list<K> l)
		{
			std::vector<bool> results(l.size(), false);
			uint32_t iters = 0;
			for (auto key : l)
			{
				for (uint32_t inTableIndex = 0; inTableIndex < _maxIters; inTableIndex++)
				{
					const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, inTableIndex);
					const uint32_t currentTable = inTableIndex % _tablesCount;

					if (_data[currentTable][hashedKey].key == key && _data[currentTable][hashedKey].occupied)
					{
						_data[currentTable][hashedKey].key = K();
						_data[currentTable][hashedKey].value = V();
						_data[currentTable][hashedKey].occupied = false;
						results[iters] = true;
					}
				}
				iters++;
			}
			return results;
		}

		bool erase(const K& key)
		{
			for (uint32_t iters = 0; iters < _maxIters; iters++)
			{
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, iters);
				const uint32_t currentTable = iters % _tablesCount;

				if (_data[currentTable][hashedKey].key == key && _data[currentTable][hashedKey].occupied)
				{
					_data[currentTable][hashedKey].key = K();
					_data[currentTable][hashedKey].value = V();
					_data[currentTable][hashedKey].occupied = false;
					return true;
				}
			}
			return false;
		}

		bool erase(const K&& key)
		{
			for (uint32_t iters = 0; iters < _maxIters; iters++)
			{
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, iters);
				const uint32_t currentTable = iters % _tablesCount;

				if (_data[currentTable][hashedKey].key == key && _data[currentTable][hashedKey].occupied)
				{
					_data[currentTable][hashedKey].key = K();
					_data[currentTable][hashedKey].value = V();
					_data[currentTable][hashedKey].occupied = false;
					return true;
				}
			}
			return false;
		}

		V* find(const K& key)
		{
			uint32_t iters = 0;
			while (iters < _maxIters)
			{
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, iters);
				const uint32_t currentTable = iters % _tablesCount;

				if (_data[currentTable][hashedKey].occupied)
					if (_data[currentTable][hashedKey].key == key)
						return &_data[currentTable][hashedKey].value;
				iters++;
			}
			return nullptr;
		}


		V* find(const K&& key)
		{
			uint32_t iters = 0;
			while (iters < _maxIters)
			{
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, iters);
				const uint32_t currentTable = iters % _tablesCount;

				if (_data[currentTable][hashedKey].occupied)
					if (_data[currentTable][hashedKey].key == key)
						return &_data[currentTable][hashedKey].value;
				iters++;
			}
			return nullptr;
		}

		bool insert(const K& key, const V& value)
		{
			return _CCKHT_insertData(key, value);
		}

		bool insert(K& key, V& value)
		{
			return _CCKHT_insertData(key, value);
		}

		bool insert(const K&& key, const V&& value)
		{
			return _CCKHT_insertData(key, value);
		}

		bool insert(K&& key, V&& value)
		{
			return _CCKHT_insertData(key, value);
		}

		bool insert(K_V_pair& k_v_pair)
		{
			return _CCKHT_insertData(k_v_pair);
		}

		bool insert(K_V_pair&& k_v_pair)
		{
			return _CCKHT_insertData(k_v_pair);
		}

		std::vector<bool> insert(std::initializer_list<K_V_pair> l) {
			std::vector<bool> results(l.size(), false);
			uint32_t iter = 0;
			for (auto element : l)
			{
				results[iter] = _CCKHT_insertData(element);
				iter++;
			}
			return results;
		}

		// get internal container (std::vector<std::vector<TableSlot<K,V>>)
		const std::vector<std::vector<TableSlot>>& rawData()
		{
			return _data;
		}

		const uint32_t tablesCount()
		{
			return _tablesCount;
		}
		// return capacity 
		const uint32_t capacity()
		{
			return _capacity;
		}
		// return capacity * tables count
		const uint32_t totalCapacity()
		{
			return _capacity * _tablesCount;
		}

		V* operator [](const K& key)
		{
			return find(key);
		}

		V* operator [](const K&& key)
		{
			return find(key);
		}


	};
}
#endif // !HARZ_CCKHASH