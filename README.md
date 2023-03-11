# Cuckoo_hashing-containers
Custom implementation of hash map and set based on cuckoo hashing.<br/>
// for c++14 and newer versions // <br/>
Simple to use and designed to be similar(mostly) with std implementations <br/>
## Use examples
##### Cuckoo set
![UseExmpl1](https://user-images.githubusercontent.com/113302630/224480148-93fafdc4-fbe5-46d0-ae3b-0394a71328c3.png)
##### Cuckoo map
![UseExmpl2](https://user-images.githubusercontent.com/113302630/224480150-c548e435-8a4a-42a4-84d9-8f56761636f1.png)
More in demo.. <br/>
## Test comparison with std implementation(on release) 30000 elements(uint32_t for key, int for value)
### STD
#### Map:
Average time for inserting  is:  3.23505 ms <br/>
Average time for search     is:	 4.00882 ms <br/>
Average time for deletion   is:  4.00882 ms <br/>
#### Unordered map:
Average  time for inserting is:  2.18345 ms <br/>
Average  time for search    is:  1.39273 ms <br/>
Average  time for deletion  is:  1.39273 ms <br/>
#### Set:
Average time for inserting  is:  4.77142 ms <br/>
Average  time for search    is:  3.67988 ms <br/>
Average  time for deletion  is:  3.67988 ms <br/>
#### Unordered set:
Average time for inserting  is:   1.84786 ms <br/>
Average time for search     is:   1.07018 ms <br/>
Average time for deletion   is:   1.07018 ms <br/>
### HARZ
#### Cuckoo map:
Average  time for inserting is:   1.75998 ms <br/>
Average time for search     is:   0.87011 ms <br/>
Average  time for deletion  is:   0.87011 ms <br/>
#### Cuckoo Node map:
Average time for inserting  is:   4.85056 ms <br/>
Average  time for search    is:   1.08851 ms <br/>
Average  time for deletion  is:   1.08851 ms <br/>
#### Cuckoo set:
Average time for inserting  is:   1.53632 ms <br/>
Average time for search     is:   1.92009 ms <br/>
Average time for deletion   is:   1.92009 ms <br/>
#### Cuckoo Node set:
Average time for inserting  is:   3.66870 ms <br/>
Average time for search     is:   2.27876 ms <br/>
Average time for deletion   is:   2.27876 ms <br/>