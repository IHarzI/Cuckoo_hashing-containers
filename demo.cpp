#include <iostream>
#include <iomanip>

#include "DEMO_harz_cckhash.h"

#include "include/harz_cckhash_map.h"
#include "include/harz_cckhash_set.h"
// custom params for tests

const int maxIters =70;

using namespace harz::demo;
using namespace harz::utils;

void demo_set_test() {
	auto start = SeedFromTime();
	srand(SeedFromTime());

	// you could specify start capacity and tables count in hash set, like harz::cuckooHashSet<CstData> hashSet(100,5), then hash set will have 5 tables with 100 capacity each
	harz::cuckooHashSet<CstData> hashSet;
	int iter = 0;
	uint32_t totalInserts = 0;
	uint32_t totalErases = 0;
	
	CstData myval = { 2004, 'Z' };
	totalInserts += hashSet.insert(myval);
	if (hashSet.find(myval))
		std::cout << "\n\n Here start [SET] test!" << std::endl;;
	

	// inserting values into hash set
	while (iter < maxIters)
	{
		int i = rand() % 379;
		char chr = rand() % 255;
		CstData cdta(i, chr);
		totalInserts += hashSet.insert(cdta);
		++iter;
	}

	// erase some elements, if they present in container (if luck give us chance, because CstData have 2 members, each have random value and each must be == for successful erasure)
	iter = 0;
	while (iter < maxIters)
	{
		int i = rand() % 379;
		char chr = rand() % 255;
		CstData cdta(i, chr);
		totalErases += hashSet.erase(cdta);
		++iter;
	}

	// insertion by init list
	auto insertionsFromInitList = hashSet.insert({ { 5,'R' }, { 1534632,'^' }, { 153 ,'$'}});

	for (auto bol : insertionsFromInitList)
		totalInserts += bol;
	
	auto erasesFromInitList = hashSet.erase({ { 5,'R' }, { 1534632,'^' }, { 153 ,'$'} });

	for (auto bol : erasesFromInitList)
		totalErases += bol;

	CstData val(4, '4');

	// search for possible value, if it is in the set, printing location of value in set..
	if (hashSet[val])
	{
		std::cout << "\n Find CstData with '4' in hash set in location: " << hashSet[val] << std::endl;
	}
	else
	{
		std::cout << "\n Didn't find '4' in hash set, so make one insertion with our [val] and find if it is actualy in the set(it is)\n";
		totalInserts += hashSet.insert(val);
		if (hashSet[val])
		{
			auto item = hashSet[val];
			std::cout << "\n Find CstData with '4' in hash set in location: " << item << " here data: " << item->data << " chr: " << item->chr << std::endl;
		}
	}

	// printing set
	harz::demo::print_CCKHSH_container(hashSet);

	std::cout << "\n My value, inserted at the beginning of the test, located here : " << hashSet.find(myval) << std::endl;
	std::cout << " End of [SET] test, wait 5 seconds..." << std::endl;

	std::cout << "Set test time duration is: " << SeedFromTime() - start << " ms" << std::endl;
	std::cout << "Total count of successful insertion : " << totalInserts << " Total count of successful erasures : " << totalErases << std::endl;
	std::this_thread::sleep_for(5000ms);
}


void demo_map_test() {
	auto start = SeedFromTime();
	srand(SeedFromTime());

	// you could specify start capacity and tables count in hash map, like harz::cuckooHashMap<int, CstData> hashMap(100,5), then hash map will have 5 tables with 100 capacity each
	harz::cuckooHashMap<int, CstData> hashMap;
	int iter = 0;
	uint32_t totalInserts = 0;
	uint32_t totalErases = 0;
	CstData myval = { 2004, 'Z' };
	totalInserts += hashMap.insert(myval.data, myval);

	if(hashMap.find(myval.data))
		std::cout << "\n\n Here start [MAP] test!" << std::endl;;
	// inserting values into hash set
	while (iter < maxIters)
	{
		int key = rand() % 999;
		int i = rand() % 379;
		char chr = rand() % 255;
		CstData cdta(i, chr);
		totalInserts += hashMap.insert(key, cdta);
		++iter;
	}
	// erase some elements, if they present in container
	iter = 0;
	while (iter < maxIters)
	{
		int key = rand() % 999;
		totalErases +=  hashMap.erase(key);
		++iter;
	}

	// insertion by init list
	auto insertionsFromInitList = hashMap.insert({ {256,{5,'%'} }, { -5345645,{25,'2'} }, { -19,{35,'P'} }
});

	for (auto bol : insertionsFromInitList)
		totalInserts += bol;

	auto erasesFromInitList = hashMap.erase({ 256, -5345645, -19 });

	for (auto bol : erasesFromInitList)
		totalErases += bol;

	const int val_key = -26234;
	CstData val(4, '4');

	// search for possible value, if it is in the set, printing location of value in set..
	if (hashMap[val_key])
	{
		std::cout << "\n Find CstData with '4' in hash set in location: " << hashMap[val_key] << std::endl;
	}
	else
	{
		std::cout << "\n Didn't find '4' in hash set, so make one insertion with our [val] and find if it is actualy in the set(it is)\n";
		totalInserts += hashMap.insert(val_key, val);
		if (hashMap[val_key])
		{
			auto item = hashMap[val_key];
			std::cout << "\n Find CstData with '4' in hash set in location: " << item << " here data: " <<  item->data << " chr: " << item->chr << std::endl;
		}
	}
	// printing set after insertion, after resizing, after restraining...
	print_CCKHSH_container(hashMap);

	std::cout << "\n My value, inserted at the beginning of the test, located here : " << hashMap.find(myval.data) << std::endl;
	std::cout << " End of [MAP] test, wait 5 seconds..." << std::endl;
	std::cout << "Map test time duration is: " << SeedFromTime() - start << " ms" << std::endl;
	std::cout << "Total count of successful insertion : " << totalInserts << " Total count of successful erasures : " << totalErases << std::endl;
	std::this_thread::sleep_for(5000ms);
}

void main()
{
	// program could lag and don't output all data after the end of the tests(std::cout have limits, after all..), just rebuild the solution and restart(possibly need a few "rebuilds")

	demo_map_test();

	demo_set_test();

}