#ifndef HARZ_CCKHASH_SET
#define HARZ_CCKHASH_SET

// Custom implementation of hash set based on cuckoo hashing
// c++14 and newer versions(possibly c++11 also, but not guaranteed)
// made by IHarzI

#include <vector>
#include <functional>

// custom params, experiment with different values for better perfomance
#define HARZ_CCKHASH_SET_MAX_ITERATIONS_MOD (2.5f) 
#define HARZ_CCKHASH_SET_RESIZE_MOD (1.5f)

namespace harz
{

	template<typename V>
	class cuckooHashSet
	{
	public:
		cuckooHashSet(const uint32_t capacity = 64, const uint32_t tablesCount = 2)
			: _capacity(capacity), _tablesCount(tablesCount), _maxIters(tablesCount * HARZ_CCKHASH_SET_MAX_ITERATIONS_MOD + 1)
		{
			_data.resize(tablesCount);
			for (uint32_t iter = 0; iter < tablesCount; iter++)
			{
				_data[iter].resize(capacity);
			}
		}

		struct TableSlot
		{
			V value;
			bool occupied = false;
		};

		// change capacity("rehash" set), be careful if give custom parameter, possible loss of data (if newCapacity < current capacity of set)
		bool resize(uint32_t newCapacity = 0)
		{
			if (newCapacity <= 0)
			{
				newCapacity = _capacity * HARZ_CCKHASH_SET_RESIZE_MOD + 1;
			}
			std::vector<std::vector<TableSlot>> oldData = _data;

			_capacity = newCapacity;

			clear();

			for (std::vector<TableSlot> table : oldData)
			{
				for (auto& slot : table)
				{
					if (slot.occupied)
						insert(slot.value);
				};
			};

			return true;
		}

		// change tables count, be careful if give custom parameter, possible loss of data (if newTablesCount < current tables count of set)
		bool restrain(const uint32_t newTablesCount)
		{
			if (newTablesCount <= 2)
				return false;

			_data.resize(newTablesCount);
			_tablesCount = newTablesCount;
			resize(_capacity);

			_maxIters = _tablesCount * HARZ_CCKHASH_SET_MAX_ITERATIONS_MOD;

			return true;
		}

	private:
		uint32_t _capacity = 64;
		uint32_t _tablesCount = 2;
		uint32_t _maxIters = _tablesCount*HARZ_CCKHASH_SET_MAX_ITERATIONS_MOD + 1;

		std::vector<std::vector<TableSlot>> _data;

		std::function <const uint32_t(V, uint32_t, uint32_t, uint32_t)> _g_CCKHT_l_hashFunction = [](V key, uint32_t cap, uint32_t tablecnt, uint32_t i)-> const uint32_t
		{
			return ((std::hash< uint32_t>()(std::hash<V>()(key) + std::hash< uint32_t>()(i % (tablecnt + cap))))) % cap;
		};

		bool _CCKHT_insertData(const V& value, uint32_t iterations = 0)
		{
			while (true)
			{
				while (iterations < _maxIters)
				{
					const uint32_t currentTable = iterations % _tablesCount;
					const uint32_t hashedKey = _g_CCKHT_l_hashFunction(value, _capacity, _tablesCount, iterations);

					if (_data[currentTable][hashedKey].value == value)
					{
						return false;
					}
					else if (_data[currentTable][hashedKey].occupied)
					{
						V temp( _data[currentTable][hashedKey].value);
						_data[currentTable][hashedKey].value = value;
						return _CCKHT_insertData(std::move(temp), iterations);
					}
					else if (!_data[currentTable][hashedKey].occupied)
					{
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

		bool _CCKHT_insertData(V&& value, uint32_t iterations = 0)
		{
			while (true)
			{
				while (iterations < _maxIters)
				{
					const uint32_t currentTable = iterations % _tablesCount;
					const uint32_t hashedKey = _g_CCKHT_l_hashFunction(value, _capacity, _tablesCount, iterations);

					if (_data[currentTable][hashedKey].value == value)
					{
						return false;
					}
					else if (_data[currentTable][hashedKey].occupied)
					{
						V temp(_data[currentTable][hashedKey].value);
						_data[currentTable][hashedKey].value = std::move(value);
						value = temp;
					}
					else if (!_data[currentTable][hashedKey].occupied)
					{
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

	public:

		V extract(V& value)
		{
			uint32_t iters = 0;
			while (iters < _maxIters)
			{
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(value, _capacity, _tablesCount, iters);
				const uint32_t currentTable = iters % _tablesCount;

				if (_data[currentTable][hashedKey].occupied)
					if (_data[currentTable][hashedKey].value == value)
					{
						V temp(_data[currentTable][hashedKey].value);
						erase(value);
						return std::move(temp);
					}
				iters++;
			}
			return V();
		}

		V extract(V&& value)
		{
			uint32_t iters = 0;
			while (iters < _maxIters)
			{
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(value, _capacity, _tablesCount, iters);
				const uint32_t currentTable = iters % _tablesCount;

				if (_data[currentTable][hashedKey].occupied)
					if (_data[currentTable][hashedKey].value == value)
					{
						V temp(_data[currentTable][hashedKey].value);
						erase(value);
						return std::move(temp);
					}
				iters++;
			}
			return V();
		}

		// delete all elements
		void clear()
		{
			_data = std::vector<std::vector<TableSlot>>();
			_data.resize(_tablesCount);
			for (int tables = 0; tables < _tablesCount; tables++)
			{
				_data[tables].resize(_capacity);
			}
		}

		std::vector<bool> erase(std::initializer_list<V> l)
		{
			std::vector<bool> results(l.size(), false);
			uint32_t iters = 0;
			for (auto element : l)
			{
				for (uint32_t inTableIndex = 0; inTableIndex < _tablesCount; inTableIndex++)
				{
					const uint32_t hashedKey = _g_CCKHT_l_hashFunction(element, _capacity, _tablesCount, inTableIndex);
					const uint32_t currentTable = inTableIndex % _tablesCount;

					if (_data[currentTable][hashedKey].value == element)
					{
						_data[currentTable][hashedKey].value == V();
						_data[currentTable][hashedKey].occupied = false;
						results[iters] = true;
					}
				}
				iters++;
			}
			return results;
		}

		bool erase(const V& value)
		{
			for (uint32_t iters = 0; iters < _tablesCount; iters++)
			{
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(value, _capacity, _tablesCount, iters);
				const uint32_t currentTable = iters % _tablesCount;

				if (_data[currentTable][hashedKey].value == value)
				{
					_data[currentTable][hashedKey].value == V();
					_data[currentTable][hashedKey].occupied = false;
					return true;
				}
			}
			return false;
		}

		bool erase(const V&& value)
		{
			for (uint32_t iters = 0; iters < _tablesCount; iters++)
			{
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(value, _capacity,_tablesCount, iters);
				const uint32_t currentTable = iters % _tablesCount;

				if (_data[currentTable][hashedKey].value == value)
				{
					_data[currentTable][hashedKey].value == V();
					_data[currentTable][hashedKey].occupied = false;
					return true;
				}
			}
			return false;
		}

	
		V* find(const V& value)
		{
			uint32_t iters = 0;
			while (iters < _tablesCount)
			{
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(value, _capacity, _tablesCount, iters);
				const uint32_t currentTable = iters % _tablesCount;

				if (_data[currentTable][hashedKey].occupied)
					if (_data[currentTable][hashedKey].value == value)
						return &_data[currentTable][hashedKey].value;
				iters++;
			}
			return nullptr;
		}

		V* find(const V&& value)
		{
			uint32_t iters = 0;
			while (iters < _tablesCount)
			{
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(value, _capacity, _tablesCount, iters);
				const uint32_t currentTable = iters % _tablesCount;

				if (_data[currentTable][hashedKey].occupied)
					if (_data[currentTable][hashedKey].value == value)
						return &_data[currentTable][hashedKey].value;
				iters++;
			}
			return nullptr;
		}

		bool insert(const V& value)
		{
			return _CCKHT_insertData(value);
		}

		bool insert(const V&& value)
		{
			return _CCKHT_insertData(value);
		}

		std::vector<bool> insert(std::initializer_list<V> l) {
			std::vector<bool> results(l.size(), false);
			uint32_t iter = 0;
			for (auto element : l)
			{
				if (_CCKHT_insertData(element))
					results[iter] = true;
				iter++;
			}
			return results;
		}

		// get internal container (std::vector<std::vector<TableSlot<V>>)
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

		V* operator [](V& value)
		{
			return find(value);
		}

		V* operator [](V&& value)
		{
			return find(value);
		}

	};
}
#endif // !HARZ_CCKHASH