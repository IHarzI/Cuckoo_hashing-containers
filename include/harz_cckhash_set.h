#ifndef HARZ_CCKHASH_SET
#define HARZ_CCKHASH_SET

// Custom implementation of hash set based on cuckoo hashing
// c++14 and newer versions
// repository: https://github.com/IHarzI/Cuckoo_hashing-containers
// made by IHarzI

#include <vector>
#include <functional>
#include <memory>
#include <stdint.h>
// custom params, experiment with different values for better perfomance
#define HARZ_CCKHASH_SET_MAX_ITERATIONS_MOD (3.7f) 
#define HARZ_CCKHASH_SET_RESIZE_MOD (1.75f)

namespace harz
{
	template<typename V>
	class cuckooHashSet
	{
	public:
		cuckooHashSet(const uint32_t capacity = 64, const uint32_t tablesCount = 2)
			: _capacity(capacity), _tablesCount(tablesCount), _maxIters((uint32_t)(tablesCount* HARZ_CCKHASH_SET_MAX_ITERATIONS_MOD) + 1)
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

		// Change capacity("rehash" set), possible recursive call to resize while inserting elements from old _data
		const bool resize(uint32_t newCapacity = 0)
		{
			if (newCapacity <= 0)
			{
				newCapacity = (uint32_t)(_capacity * HARZ_CCKHASH_SET_RESIZE_MOD) + 1;
			}

			std::vector<std::vector<TableSlot>> oldData = _data;

			_capacity = newCapacity;

			clear();

			for (auto& table : oldData)
			{
				for (auto& slot : table)
				{
					if (slot.occupied)
						_insert(slot.value);
				};
			};

			return true;
		}

		// Change tables count, possible recursive call to resize while inserting elements from old _data
		const bool restrain(const uint32_t newTablesCount)
		{
			if (newTablesCount <= 2)
				return false;

			_tablesCount = newTablesCount;
			_maxIters = _tablesCount * HARZ_CCKHASH_SET_MAX_ITERATIONS_MOD;
			resize(_capacity);


			return true;
		}

	private:
		uint32_t _capacity = 64;
		uint32_t _tablesCount = 2;
		uint32_t _maxIters = _tablesCount * HARZ_CCKHASH_SET_MAX_ITERATIONS_MOD + 1;

		std::vector<std::vector<TableSlot>> _data;

		const std::function <const uint32_t(V, uint32_t, uint32_t, uint32_t)> _g_CCKHT_l_hashFunction = [](const V& key, uint32_t cap, uint32_t tablecnt, uint32_t i)-> const uint32_t
		{
			return ((std::hash< uint32_t>()(std::hash<V>()(key) + std::hash< uint32_t>()(i % (tablecnt + cap))))) % cap;
		};

		const bool _CCKHT_insertData(const V& value, uint32_t iterations = 0)
		{
			while (true)
			{
				while (iterations < _maxIters)
				{
					const uint32_t currentTable = iterations % _tablesCount;
					const uint32_t hashedKey = _g_CCKHT_l_hashFunction(value, _capacity, _tablesCount, iterations);

					if (_data[currentTable][hashedKey].occupied)
					{
						V temp(_data[currentTable][hashedKey].value);
						_data[currentTable][hashedKey].value = value;
						return _CCKHT_insertData(std::move(temp), iterations);
					}
					else
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

		const bool _CCKHT_insertData(V&& value, uint32_t iterations = 0)
		{
			while (true)
			{
				while (iterations < _maxIters)
				{
					const uint32_t currentTable = iterations % _tablesCount;
					const uint32_t hashedKey = _g_CCKHT_l_hashFunction(value, _capacity, _tablesCount, iterations);

					if (_data[currentTable][hashedKey].occupied)
					{
						V temp(_data[currentTable][hashedKey].value);
						_data[currentTable][hashedKey].value = std::move(value);
						value = temp;
					}
					else
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

		const bool _insert(const V& value)
		{
			auto contains = std::move(_contains_with_place(value));

			switch (contains.second)
			{
			case 0:
				contains.first->value = value;
				contains.first->occupied = true;
				return true;
			case 1:
				return false;
			case 2:
				V tmp(value);
				std::swap(tmp, contains.first->value);
				return _CCKHT_insertData(std::move(tmp), 1);
			}
		}

		const bool _insert(V&& value)
		{
			auto contains = std::move(_contains_with_place(value));

			switch (contains.second)
			{
			case 0:
				contains.first->value = std::move(value);
				contains.first->occupied = true;
				return true;
			case 1:
				return false;
			case 2:
				std::swap(value, contains.first->value);
				return _CCKHT_insertData(std::move(value), 1);
			}
		}
		// Check if map contains value on [key], 0 - not occupied, 1 - has same key, 2 collision
		std::pair<TableSlot*, const int> _contains_with_place(const V& value)
		{
			for (uint32_t iterations = 0; iterations < _tablesCount; iterations++)
			{
				const uint32_t currentTable = iterations % _tablesCount;
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(value, _capacity, _tablesCount, iterations);

				if (_data[currentTable][hashedKey].occupied && _data[currentTable][hashedKey].value == value)
				{
					return std::make_pair(&_data[currentTable][hashedKey], 1);
				}
				else if (!_data[currentTable][hashedKey].occupied)
				{
					return std::make_pair(&_data[currentTable][hashedKey], 0);
				}
			}
			auto* place = &_data[0 % _tablesCount][_g_CCKHT_l_hashFunction(value, _capacity, _tablesCount, 0)];
			return std::make_pair(place, 2);
		}
		// Check if map contains value on [key], 0 - not occupied, 1 - has same key, 2 collision
		std::pair<TableSlot*, const int> _contains_with_place(const V&& value)
		{
			for (uint32_t iterations = 0; iterations < _tablesCount; iterations++)
			{
				const uint32_t currentTable = iterations % _tablesCount;
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(value, _capacity, _tablesCount, iterations);

				if (_data[currentTable][hashedKey].occupied && _data[currentTable][hashedKey].value == value)
				{
					return  std::make_pair(&_data[currentTable][hashedKey], 1);
				}
				else if (!_data[currentTable][hashedKey].occupied)
				{
					return std::make_pair(&_data[currentTable][hashedKey], 0);
				}
			}
			auto* place = &_data[0 % _tablesCount][_g_CCKHT_l_hashFunction(value, _capacity, _tablesCount, 0)];
			return std::make_pair(place, 2);
		}
	public:
		// Exchanges the content of container with other 
		const bool swap(cuckooHashSet<V>& other)
		{
			std::swap(_tablesCount, other._tablesCount);
			std::swap(_capacity, other._capacity);
			std::swap(_maxIters, other._maxIters);
			std::swap(_data, other._data);

			return true;
		}

		// Erases all elements that satisfy the predicate pred from the container
		template <typename PredicateT>
		const uint32_t erase_if(const PredicateT& predicate)
		{
			uint32_t erasuresCount = 0;
			for (auto& table : _data)
			{
				for (auto& slot : table)
				{
					if (slot.occupied)
						if (predicate(slot.value))
						{
							slot.value = V();
							slot.occupied = false;
							erasuresCount += 1;
						}
				}
			}
			return erasuresCount;
		}
		// Erases all elements that satisfy the predicate pred from the container
		template <typename PredicateT>
		const uint32_t erase_if(const PredicateT&& predicate)
		{
			uint32_t erasuresCount = 0;
			for (auto& table : _data)
			{
				for (auto& slot : table)
				{
					if (slot.occupied)
						if (predicate(slot.value))
						{
							slot.value = V();
							slot.occupied = false;
							erasuresCount += 1;
						}
				}
			}
			return erasuresCount;
		}

		// Count all elements that satisfy the predicate
		template <typename PredicateT>
		const uint32_t count_if(const PredicateT& predicate) const
		{
			uint32_t count = 0;
			for (auto& table : _data)
			{
				for (auto& slot : table)
				{
					if (slot.occupied)
						if (predicate(slot.value))
						{
							count += 1;
						}
				}
			}
			return count;
		}

		// Count all elements that satisfy the predicate
		template <typename PredicateT>
		const uint32_t count_if(const PredicateT&& predicate) const
		{
			uint32_t count = 0;
			for (auto& table : _data)
			{
				for (auto& slot : table)
				{
					if (slot.occupied)
						if (predicate(slot.value))
						{
							count += 1;
						}
				}
			}
			return count;
		}
		// Extract element by value
		V extract(const V& value)
		{
			uint32_t iters = 0;
			while (iters < _maxIters)
			{
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(value, _capacity, _tablesCount, iters);
				const uint32_t currentTable = iters % _tablesCount;

				if (_data[currentTable][hashedKey].occupied && _data[currentTable][hashedKey].value == value)
				{
					V temp(_data[currentTable][hashedKey].value);
					erase(value);
					return std::move(temp);
				}
				iters++;
			}
			return V();
		}

		// Extract element by value
		V extract(const V&& value)
		{
			uint32_t iters = 0;
			while (iters < _maxIters)
			{
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(value, _capacity, _tablesCount, iters);
				const uint32_t currentTable = iters % _tablesCount;

				if (_data[currentTable][hashedKey].occupied && _data[currentTable][hashedKey].value == value)
				{
					V temp(_data[currentTable][hashedKey].value);
					erase(value);
					return std::move(temp);
				}
				iters++;
			}
			return V();
		}

		// Extract elements by values from init list
		std::vector<V> extract(const std::initializer_list<V>& l)
		{
			std::vector<V> results(l.size());
			uint32_t index = 0;
			for (auto& element : l)
			{
				results[index] = (std::move(extract(element)));
				index++;
			}
			return results;
		}
		// Extract elements by values from init list
		std::vector<V> extract(const std::initializer_list<V>&& l)
		{
			std::vector<V> results(l.size());
			uint32_t index = 0;
			for (auto& element : l)
			{
				results[index] = (std::move(extract(element)));
				index++;
			}
			return results;
		}
		// Erase all elements
		void clear()
		{
			_data = std::vector<std::vector<TableSlot>>();
			_data.resize(_tablesCount);
			for (uint32_t tables = 0; tables < _tablesCount; tables++)
			{
				_data[tables].resize(_capacity);
			}
		}
		// Erase elements by values from init list
		std::vector<bool> erase(const std::initializer_list<V>& l)
		{
			std::vector<bool> results(l.size(), false);
			uint32_t index = 0;
			for (auto& element : l)
			{
				results[index] = (std::move(erase(element)));
				index++;
			}
			return results;
		}
		// Erase elements by values from init list
		std::vector<bool> erase(const std::initializer_list<V>&& l)
		{
			std::vector<bool> results(l.size(), false);
			uint32_t index = 0;
			for (auto& element : l)
			{
				results[index] = (std::move(erase(element)));
				index++;
			}
			return results;
		}
		// Erase element by value
		const bool erase(const V& value)
		{
			for (uint32_t iters = 0; iters < _tablesCount; iters++)
			{
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(value, _capacity, _tablesCount, iters);
				const uint32_t currentTable = iters % _tablesCount;

				if (_data[currentTable][hashedKey].occupied && _data[currentTable][hashedKey].value == value)
				{
					_data[currentTable][hashedKey].value = V();
					_data[currentTable][hashedKey].occupied = false;
					return true;
				}
			}
			return false;
		}
		// Erase element by value
		const bool erase(const V&& value)
		{
			for (uint32_t iters = 0; iters < _tablesCount; iters++)
			{
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(value, _capacity, _tablesCount, iters);
				const uint32_t currentTable = iters % _tablesCount;

				if (_data[currentTable][hashedKey].occupied && _data[currentTable][hashedKey].value == value)
				{
					_data[currentTable][hashedKey].value = V();
					_data[currentTable][hashedKey].occupied = false;
					return true;
				}
			}
			return false;
		}
		// Find element by value, returns a const pointer to the value
		const V* find(const V& value) const
		{
			uint32_t iters = 0;
			while (iters < _tablesCount)
			{
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(value, _capacity, _tablesCount, iters);
				const uint32_t currentTable = iters % _tablesCount;

				if (_data[currentTable][hashedKey].occupied && _data[currentTable][hashedKey].value == value)
					return &_data[currentTable][hashedKey].value;
				iters++;
			}
			return nullptr;
		}
		// Find element by value, returns a const pointer to the value
		const V* find(const V&& value) const
		{
			uint32_t iters = 0;
			while (iters < _tablesCount)
			{
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(value, _capacity, _tablesCount, iters);
				const uint32_t currentTable = iters % _tablesCount;

				if (_data[currentTable][hashedKey].occupied && _data[currentTable][hashedKey].value == value)
					return &_data[currentTable][hashedKey].value;
				iters++;
			}
			return nullptr;
		}

		// Insert element by value
		const bool insert(const V& value)
		{
			return _insert(value);
		}

		// Insert element by {value}
		const bool insert(const V&& value)
		{
			return _insert(std::move(value));
		}
		// Insert elements by {values, ....}
		std::vector<bool> insert(const std::initializer_list<V>& l) {
			std::vector<bool> results(l.size(), false);
			uint32_t index = 0;
			for (auto& element : l)
			{
				results[index] = _insert(element);
				index++;
			}
			return results;
		}

		// Insert elements by {values, ....}
		std::vector<bool> insert(const std::initializer_list<V>&& l) {
			std::vector<bool> results(l.size(), false);
			uint32_t index = 0;
			for (auto& element : l)
			{
				results[index] = _insert(element);
				index++;
			}
			return results;
		}

		const std::vector<bool> insert_from_array(const V values[], const uint32_t values_size)
		{
			std::vector<bool> results(values_size, false);
			for (int iter = 0; iter < values_size; iter++)
			{
				results[iter] = _insert(values[iter]);
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
		// Find element by [value]
		const V* operator [](const V& value) const
		{
			return find(value);
		}
		// Find element by [value]
		const V* operator [](const V&& value) const
		{
			return find(value);
		}
		// Get load factor
		const double loadFactor() const
		{
			uint32_t result = 0;
			for (auto& table : _data)
			{
				for (auto& slot : table)
					result += slot.occupied;
			}
			return (double)((double)result / (double)totalCapacity());
		}
		// Check if map contains value on [key]
		const bool contains(const V& value) const
		{
			for (uint32_t iterations = 0; iterations < _tablesCount; iterations++)
			{
				const uint32_t currentTable = iterations % _tablesCount;
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(value, _capacity, _tablesCount, iterations);

				if (_data[currentTable][hashedKey].occupied && _data[currentTable][hashedKey].value == value)
				{
					return true;
				}
			}
			return false;
		}
		// Check if map contains value on [key]
		const bool contains(const V&& value) const
		{
			for (uint32_t iterations = 0; iterations < _tablesCount; iterations++)
			{
				const uint32_t currentTable = iterations % _tablesCount;
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(value, _capacity, _tablesCount, iterations);

				if (_data[currentTable][hashedKey].occupied && _data[currentTable][hashedKey].value == value)
				{
					return true;
				}
			}
			return false;
		}
		// Return count of values on [key] (1 or 0)
		const int count(const V& value) const
		{
			return contains(value);
		}
		// Return count of values on [key] (1 or 0)
		const int count(const V&& value) const
		{
			return contains(value);
		}

	};

	// Experimental

	// Node-like version of cuckoo hash set
	// Do not need for default construction of V types and occupy less memory(unless your data size is less than size of pointer) with cost of iterating over pointers to elements instead of raw elements in vector
	template<typename V>
	class cuckooNodeHashSet
	{
	public:
		cuckooNodeHashSet(const uint32_t capacity = 64, const uint32_t tablesCount = 2)
			: _capacity(capacity), _tablesCount(tablesCount), _maxIters((uint32_t)(tablesCount* HARZ_CCKHASH_SET_MAX_ITERATIONS_MOD) + 1)
		{
			_data.resize(tablesCount);
			for (uint32_t iter = 0; iter < tablesCount; iter++)
			{
				_data[iter].resize(capacity);
			}
		}

		struct TableSlot
		{
			std::shared_ptr<V> value{ nullptr };
		};

		// Change capacity("rehash" set), possible recursive call to resize while inserting elements from old _data
		bool resize(uint32_t newCapacity = 0)
		{
			if (newCapacity <= 0)
			{
				newCapacity = (uint32_t)(_capacity * HARZ_CCKHASH_SET_RESIZE_MOD) + 1;
			}
			std::vector<std::vector<TableSlot>> oldData = _data;

			_capacity = newCapacity;

			clear();

			for (auto& table : oldData)
			{
				for (auto& slot : table)
				{
					if (slot.value)
						_insert(std::move(*slot.value));
				};
			};

			return true;
		}

		// Change tables count, possible recursive call to resize while inserting elements from old _data
		bool restrain(const uint32_t newTablesCount)
		{
			if (newTablesCount <= 2)
				return false;

			_tablesCount = newTablesCount;
			_maxIters = _tablesCount * HARZ_CCKHASH_SET_MAX_ITERATIONS_MOD;
			resize(_capacity);


			return true;
		}

	private:
		uint32_t _capacity = 64;
		uint32_t _tablesCount = 2;
		uint32_t _maxIters = _tablesCount * HARZ_CCKHASH_SET_MAX_ITERATIONS_MOD + 1;

		std::vector<std::vector<TableSlot>> _data;

		const std::function <const uint32_t(V, uint32_t, uint32_t, uint32_t)> _g_CCKHT_l_hashFunction = [](const V& key, uint32_t cap, uint32_t tablecnt, uint32_t i)-> const uint32_t
		{
			return ((std::hash< uint32_t>()(std::hash<V>()(key) + std::hash< uint32_t>()(i % (tablecnt + cap))))) % cap;
		};

		const bool _CCKHT_insertData(const V& value, uint32_t iterations = 0)
		{
			while (true)
			{
				while (iterations < _maxIters)
				{
					const uint32_t currentTable = iterations % _tablesCount;
					const uint32_t hashedKey = _g_CCKHT_l_hashFunction(value, _capacity, _tablesCount, iterations);

					if (_data[currentTable][hashedKey].value)
					{
						V temp(*_data[currentTable][hashedKey].value);
						*_data[currentTable][hashedKey].value = value;
						return _CCKHT_insertData(std::move(temp), iterations);
					}
					else
					{
						_data[currentTable][hashedKey].value.reset(new V{ value });
						return true;
					}
					iterations++;
				}
				resize();
				iterations = 0;
			}
		}

		const bool _CCKHT_insertData(V&& value, uint32_t iterations = 0)
		{
			while (true)
			{
				while (iterations < _maxIters)
				{
					const uint32_t currentTable = iterations % _tablesCount;
					const uint32_t hashedKey = _g_CCKHT_l_hashFunction(value, _capacity, _tablesCount, iterations);

					if (_data[currentTable][hashedKey].value)
					{
						V temp(*_data[currentTable][hashedKey].value);
						*_data[currentTable][hashedKey].value = std::move(value);
						value = temp;
					}
					else
					{
						_data[currentTable][hashedKey].value.reset(new V{ std::move(value) });
						return true;
					}
					iterations++;
				}
				resize();
				iterations = 0;
			}
		}
		const bool _insert(const V& value)
		{
			auto contains = std::move(_contains_with_place(value));

			switch (contains.second)
			{
			case 0:
				contains.first->value.reset(new V(std::move(value)));
				return true;
			case 1:
				return false;
			case 2:
				V tmp(*contains.first->value);
				*contains.first->value = value;
				return _CCKHT_insertData(std::move(tmp), 1);
			}
		}

		const bool _insert(V&& value)
		{
			auto contains = std::move(_contains_with_place(value));

			switch (contains.second)
			{
			case 0:
				contains.first->value.reset(new V(std::move(value)));
				return true;
			case 1:
				return false;
			case 2:
				std::swap(value, *contains.first->value);
				return _CCKHT_insertData(std::move(value), 1);
			}
		}
		// Check if map contains value on [key], 0 - not occupied, 1 - has same key, 2 collision
		std::pair<TableSlot*, const int> _contains_with_place(const V& value)
		{
			for (uint32_t iterations = 0; iterations < _tablesCount; iterations++)
			{
				const uint32_t currentTable = iterations % _tablesCount;
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(value, _capacity, _tablesCount, iterations);

				if (_data[currentTable][hashedKey].value && *_data[currentTable][hashedKey].value == value)
				{
					return std::make_pair(&_data[currentTable][hashedKey], 1);
				}
				else if (!_data[currentTable][hashedKey].value)
				{
					return std::make_pair(&_data[currentTable][hashedKey], 0);
				}
			}
			auto* place = &_data[0 % _tablesCount][_g_CCKHT_l_hashFunction(value, _capacity, _tablesCount, 0)];
			return std::make_pair(place, 2);
		}
		// Check if map contains value on [key], 0 - not occupied, 1 - has same key, 2 collision
		std::pair<TableSlot*, const int> _contains_with_place(const V&& value)
		{
			for (uint32_t iterations = 0; iterations < _tablesCount; iterations++)
			{
				const uint32_t currentTable = iterations % _tablesCount;
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(value, _capacity, _tablesCount, iterations);

				if (_data[currentTable][hashedKey].value && *_data[currentTable][hashedKey].value == value)
				{
					return  std::make_pair(&_data[currentTable][hashedKey], 1);
				}
				else if (!_data[currentTable][hashedKey].value)
				{
					return std::make_pair(&_data[currentTable][hashedKey], 0);
				}
			}
			auto* place = &_data[0 % _tablesCount][_g_CCKHT_l_hashFunction(value, _capacity, _tablesCount, 0)];
			return std::make_pair(place, 2);
		}
	public:
		// Exchanges the content of container with other 
		const bool swap(cuckooNodeHashSet<V>& other)
		{
			std::swap(_tablesCount, other._tablesCount);
			std::swap(_capacity, other._capacity);
			std::swap(_maxIters, other._maxIters);
			std::swap(_data, other._data);

			return true;
		}
		// Erases all elements that satisfy the predicate pred from the container
		template <typename PredicateT>
		const uint32_t erase_if(const PredicateT& predicate)
		{
			uint32_t erasuresCount = 0;
			for (auto& table : _data)
			{
				for (auto& slot : table)
				{
					if (slot.value)
					{
						if (predicate(*slot.value))
						{
							slot.value.reset();
							erasuresCount += 1;
						}
					}
				}
			}
			return erasuresCount;
		}


		// Erases all elements that satisfy the predicate pred from the container
		template <typename PredicateT>
		const uint32_t erase_if(const PredicateT&& predicate)
		{
			uint32_t erasuresCount = 0;
			for (auto& table : _data)
			{
				for (auto& slot : table)
				{
					if (slot.value)
					{
						if (predicate(*slot.value))
						{
							slot.value.reset();
							erasuresCount += 1;
						}
					}
				}
			}
			return erasuresCount;
		}

		// Count all elements that satisfy the predicate
		template <typename PredicateT>
		const uint32_t count_if(const PredicateT& predicate) const
		{
			uint32_t count = 0;
			for (auto& table : _data)
			{
				for (auto& slot : table)
				{
					if (slot.value)
					{
						if (predicate(slot.value))
						{
							count += 1;
						}
					}
				}
			}
			return count;
		}

		// Count all elements that satisfy the predicate
		template <typename PredicateT>
		const uint32_t count_if(const PredicateT&& predicate) const
		{
			uint32_t count = 0;
			for (auto& table : _data)
			{
				for (auto& slot : table)
				{
					if (slot.value)
					{
						if (predicate(slot.value))
						{
							count += 1;
						}
					}
				}
			}
			return count;
		}

		// Extract element by value
		V extract(const V& value)
		{
			uint32_t iters = 0;
			while (iters < _maxIters)
			{
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(value, _capacity, _tablesCount, iters);
				const uint32_t currentTable = iters % _tablesCount;

				if (_data[currentTable][hashedKey].value && *_data[currentTable][hashedKey].value == value)
				{
					V temp(*_data[currentTable][hashedKey].value);
					_data[currentTable][hashedKey].value.reset();
					return std::move(temp);
				}
				iters++;
			}
			return V();
		}

		// Extract element by value
		V extract(const V&& value)
		{
			uint32_t iters = 0;
			while (iters < _maxIters)
			{
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(value, _capacity, _tablesCount, iters);
				const uint32_t currentTable = iters % _tablesCount;

				if (_data[currentTable][hashedKey].value && *_data[currentTable][hashedKey].value == value)
				{
					V temp(*_data[currentTable][hashedKey].value);
					_data[currentTable][hashedKey].value.reset();
					return std::move(temp);
				}
				iters++;
			}
			return V();
		}

		// Extract elements by values from init list
		std::vector<V> extract(const std::initializer_list<V>& l)
		{
			std::vector<V> results(l.size());
			uint32_t index = 0;
			for (auto& element : l)
			{
				results[index] = (std::move(extract(element)));
				index++;
			}
			return results;
		}

		// Extract elements by values from init list
		std::vector<V> extract(const std::initializer_list<V>&& l)
		{
			std::vector<V> results(l.size());
			uint32_t index = 0;
			for (auto& element : l)
			{
				results[index] = (std::move(extract(element)));
				index++;
			}
			return results;
		}

		// Erase all elements.
		void clear()
		{
			for (auto& table : _data)
			{
				for (auto& slot : table)
				{
					if (slot.value)
					{
						slot.value.reset();
					}
				}
			}

			_data.resize(_tablesCount);
			for (uint32_t tables = 0; tables < _tablesCount; tables++)
			{
				_data[tables].resize(_capacity);
			}
		}

		// Erase element by value
		const bool erase(const V& value)
		{
			for (uint32_t iters = 0; iters < _tablesCount; iters++)
			{
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(value, _capacity, _tablesCount, iters);
				const uint32_t currentTable = iters % _tablesCount;

				if (_data[currentTable][hashedKey].value && *_data[currentTable][hashedKey].value == value)
				{

					_data[currentTable][hashedKey].value.reset();
					return true;
				}
			}
			return false;
		}
		// Erase element by value
		const bool erase(const V&& value)
		{
			for (uint32_t iters = 0; iters < _tablesCount; iters++)
			{
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(value, _capacity, _tablesCount, iters);
				const uint32_t currentTable = iters % _tablesCount;

				if (_data[currentTable][hashedKey].value && *_data[currentTable][hashedKey].value == value)
				{

					_data[currentTable][hashedKey].value.reset();
					return true;
				}
			}
			return false;
		}

		// Erase elements by values from init list
		std::vector<bool> erase(const std::initializer_list<V>& l)
		{
			std::vector<bool> results(l.size(), false);
			uint32_t index = 0;
			for (auto& element : l)
			{
				results[index] = (std::move(erase(element)));
				index++;
			}
			return results;
		}

		// Erase elements by values from init list
		std::vector<bool> erase(const std::initializer_list<V>&& l)
		{
			std::vector<bool> results(l.size(), false);
			uint32_t index = 0;
			for (auto& element : l)
			{
				results[index] = (std::move(erase(element)));
				index++;
			}
			return results;
		}

		// Find element by value, returns a const pointer to the value
		const V* find(const V& value) const
		{
			uint32_t iters = 0;
			while (iters < _tablesCount)
			{
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(value, _capacity, _tablesCount, iters);
				const uint32_t currentTable = iters % _tablesCount;

				if (_data[currentTable][hashedKey].value && *_data[currentTable][hashedKey].value == value)
					return _data[currentTable][hashedKey].value.get();
				iters++;
			}
			return nullptr;
		}
		// Find element by value, returns a const pointer to the value
		const V* find(const V&& value) const
		{
			uint32_t iters = 0;
			while (iters < _tablesCount)
			{
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(value, _capacity, _tablesCount, iters);
				const uint32_t currentTable = iters % _tablesCount;

				if (_data[currentTable][hashedKey].value && *_data[currentTable][hashedKey].value == value)
					return _data[currentTable][hashedKey].value.get();
				iters++;
			}
			return nullptr;
		}

		// Insert element by value
		const bool insert(const V& value)
		{
			return _insert(value);
		}

		// Insert element by {value}
		const bool insert(const V&& value)
		{
			return _insert(std::move(value));
		}
		// Insert elements by {values, ....}
		std::vector<bool> insert(const std::initializer_list<V>& l) {
			std::vector<bool> results(l.size(), false);
			uint32_t index = 0;
			for (auto& element : l)
			{
				results[index] = _insert(element);
				index++;
			}
			return results;
		}

		// Insert elements by {values, ....}
		std::vector<bool> insert(const std::initializer_list<V>&& l) {
			std::vector<bool> results(l.size(), false);
			uint32_t index = 0;
			for (auto& element : l)
			{
				results[index] = _insert(element);
				index++;
			}
			return results;
		}

		const std::vector<bool> insert_from_array(const V values[], const uint32_t values_size)
		{
			std::vector<bool> results(values_size, false);
			for (int iter = 0; iter < values_size; iter++)
			{
				results[iter] = _insert(values[iter]);
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
		// Find element by [value]
		const V* operator [](const V& value) const
		{
			return find(value);
		}
		// Find element by [value]
		const V* operator [](const V&& value) const
		{
			return find(value);
		}
		// Get load factor
		const double loadFactor() const
		{
			uint32_t result = 0;
			for (auto& table : _data)
			{
				for (auto& slot : table)
					result += (bool)(slot.value);
			}
			return (double)((double)result / (double)totalCapacity());
		}
		// Check if map contains value on [key]
		const bool contains(const V& value) const
		{
			for (uint32_t iterations = 0; iterations < _tablesCount; iterations++)
			{
				const uint32_t currentTable = iterations % _tablesCount;
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(value, _capacity, _tablesCount, iterations);

				if (_data[currentTable][hashedKey].value && *_data[currentTable][hashedKey].value == value)
				{
					return true;
				}
			}
			return false;
		}
		// Check if map contains value on [key]
		const bool contains(const V&& value) const
		{
			for (uint32_t iterations = 0; iterations < _tablesCount; iterations++)
			{
				const uint32_t currentTable = iterations % _tablesCount;
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(value, _capacity, _tablesCount, iterations);

				if (_data[currentTable][hashedKey].value && *_data[currentTable][hashedKey].value == value)
				{
					return true;
				}
			}
			return false;
		}
		// Return count of values on [key] (1 or 0)
		const int count(const V& value) const
		{
			return contains(value);
		}
		// Return count of values on [key] (1 or 0)
		const int count(const V&& value) const
		{
			return contains(value);
		}

	};
}
#endif // !HARZ_CCKHASH