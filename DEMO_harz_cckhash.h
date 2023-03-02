#pragma once

// Demo functions and utils for testing custom cckhash containers implementation 
// made by Harz

// INCLUDE BEFORE harz_cckhash.h
/*
	....

	#include "DEMO_harz_cckhash.h"
	#include "harz_cckhash.h"

	....
*/

#define DEMO_CCKHASH_

#include <iostream>
#include <chrono>
#include <iomanip>

namespace harz {

	template<typename V>
	class cuckooHashSet;

	template<typename K, typename V>
	class cuckooHashMap;

	template<typename K, typename V>
	class cuckooNodeHashMap;

	template<typename V>
	class cuckooNodeHashSet;

	namespace demo
	{
		// demo, only works for types with defined stream << operators
		template<typename V>
		void print_CCKHSH_container(cuckooHashSet<V>& cckhs_cntr)
		{
			std::cout << std::setw(80)<< "START PRINT SECTION [SET]"  << std::endl;
			uint32_t freeSlotsCnt = 0; 
			uint32_t OccpSlotsCnt = 0;
			auto _data = cckhs_cntr.rawData();
			for (uint32_t tablesI = 0; tablesI < cckhs_cntr.tablesCount(); tablesI++)
			{
				for (uint32_t i = 0; i < cckhs_cntr.capacity(); i++)
				{
					if (_data[tablesI][i].occupied)
					{
						std::cout << "  " << " Value: " <<  _data[tablesI][i].value << "  ";
						OccpSlotsCnt++;
					}
					else
					{
						std::cout << " " << "[-]" << " ";
						freeSlotsCnt++;
					}
				}
				std::cout << std::endl;
			}
			std::cout << " Total slots count: " << freeSlotsCnt + OccpSlotsCnt << " of which free slots: " << freeSlotsCnt << " occupied slots : " << OccpSlotsCnt << std::endl;
			std::cout << std::setw(80) << "END PRINT SECTION [SET]"<< std::endl;
		}

		// demo, only works for types with defined stream << operators
		template<typename V>
		void print_CCKHSH_container(cuckooNodeHashSet<V>& cckhs_cntr)
		{
			std::cout << std::setw(80) << "START PRINT SECTION [NODE SET]" << std::endl;
			uint32_t freeSlotsCnt = 0;
			uint32_t OccpSlotsCnt = 0;
			auto _data = cckhs_cntr.rawData();
			for (uint32_t tablesI = 0; tablesI < cckhs_cntr.tablesCount(); tablesI++)
			{
				for (uint32_t i = 0; i < cckhs_cntr.capacity(); i++)
				{
					if (_data[tablesI][i].value)
					{
						std::cout << "  " << " Value: " << *_data[tablesI][i].value << "  ";
						OccpSlotsCnt++;
					}
					else
					{
						std::cout << " " << "[-]" << " ";
						freeSlotsCnt++;
					}
				}
				std::cout << std::endl;
			}
			std::cout << " Total slots count: " << freeSlotsCnt + OccpSlotsCnt << " of which free slots: " << freeSlotsCnt << " occupied slots : " << OccpSlotsCnt << std::endl;
			std::cout << std::setw(80) << "END PRINT SECTION [NODE SET]" << std::endl;
		}

		// demo, only works for types with defined stream << operators
		template<typename K, typename V>
		void print_CCKHSH_container(cuckooHashMap<K, V>& cckhs_cntr)
		{
			std::cout << std::setw(80) << "START PRINT SECTION [MAP]" << std::endl;
			uint32_t freeSlotsCnt = 0;
			uint32_t OccpSlotsCnt = 0;
			auto _data = cckhs_cntr.rawData();
			for (uint32_t tablesI = 0; tablesI < cckhs_cntr.tablesCount(); tablesI++)
			{
				for (uint32_t i = 0; i < cckhs_cntr.capacity(); i++)
				{
					if (_data[tablesI][i].occupied)
					{
						std::cout << "  " << " Key: " << _data[tablesI][i].key << " Value: " << _data[tablesI][i].value << "  ";
						OccpSlotsCnt++;
					}
					else
					{
						std::cout << " " << "[-]" <<  " ";
						freeSlotsCnt++;
					}
				}
				std::cout << std::endl;
			}
			std::cout << " Total slots count: " << freeSlotsCnt + OccpSlotsCnt << " of which free slots: " << freeSlotsCnt << " occupied slots : " << OccpSlotsCnt << std::endl;
			std::cout << std::setw(80) << "END PRINT SECTION [MAP]" << std::endl;
		}

		// demo, only works for types with defined stream << operators
		template<typename K, typename V>
		void print_CCKHSH_container(cuckooNodeHashMap<K, V>& cckhs_cntr)
		{
			std::cout << std::setw(80) << "START PRINT SECTION [NODE MAP]" << std::endl;
			uint32_t freeSlotsCnt = 0;
			uint32_t OccpSlotsCnt = 0;
			auto _data = cckhs_cntr.rawData();
			for (uint32_t tablesI = 0; tablesI < cckhs_cntr.tablesCount(); tablesI++)
			{
				for (uint32_t i = 0; i < cckhs_cntr.capacity(); i++)
				{
					if (_data[tablesI][i].element)
					{
						std::cout << "  " << " Key: " << _data[tablesI][i].element->key << " Value: " << _data[tablesI][i].element->value << "  ";
						OccpSlotsCnt++;
					}
					else
					{
						std::cout << " " << "[-]" << " ";
						freeSlotsCnt++;
					}
				}
				std::cout << std::endl;
			}
			std::cout << " Total slots count: " << freeSlotsCnt + OccpSlotsCnt << " of which free slots: " << freeSlotsCnt << " occupied slots : " << OccpSlotsCnt << std::endl;
			std::cout << std::setw(80) << "END PRINT SECTION [NODE MAP]" << std::endl;
		}


		// Custom data type for testing hash container
		struct CstData
		{
			int data;
			char chr;

			friend std::ostream& operator<<(std::ostream& os, const CstData& dt)
			{
				os << " {[" << dt.data << "] [" << dt.chr << "]} ";
				return os;
			}

			bool operator==(const CstData& other) const
			{
				return data == other.data
					&& chr == other.chr;
			}

		};

	}

	namespace utils
	{
		using namespace std::chrono;
		// util func for easier and cleaner code 0_0
		uint32_t SeedFromTime() {
			return system_clock::now().time_since_epoch() /
				milliseconds(1);
		}
	}


}

// define custom std::hash for custom type
namespace std
{
	template<>
	struct hash<harz::demo::CstData>
	{
		size_t operator()(const harz::demo::CstData& x) const
		{
			return ((std::hash<int>()(x.data) + std::hash<char>() (x.chr)));
		}
	};
}