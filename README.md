# File-Indexer
Multi-threaded text file indexing command line application. 

# Configuration:
+ git clone https://github.com/prafful1/File-Indexer.git
+ Run "make"
+ Generates "bin" and "obj" directory.
+ Generates "file_indexer" binary in "bin" folder.
+ Run following command in "bin" folder: "./file_indexer <Directory_Path>".

# Output:

+ Input Command: ./file_indexer /root/text_files-dir

+ Sample Output:
+ ######################################################
+ Scanner Thread started
+ ######################################################
+ Worker Thread 1 started
+ ######################################################
+ Worker Thread 2 started
+ ######################################################
+ Worker Thread 3 started
+ ######################################################
+ Worker Thread 4 started
+ ######################################################
+ File Path extracted from message queue /root/text_files-dir/file2.txt
+ File Path extracted from message queue /root/text_files-dir/pqrs/pqrs2/zmap.txt
+ File Path extracted from message queue /root/text_files-dir/pqrs/text.txt
+ File Path extracted from message queue /root/text_files-dir/pqrs/file3.txt
+ File Path extracted from message queue /root/text_files-dir/pqrs/file.txt
+ File Path extracted from message queue /root/text_files-dir/b.txt
+ File Path extracted from message queue /root/text_files-dir/file.txt
+ File Path extracted from message queue /root/text_files-dir/abcd/e.txt
+ File Path extracted from message queue /root/text_files-dir/abcd/c.txt
+ File Path extracted from message queue /root/text_files-dir/abcd/file3.txt
+ Word: the, Frequency: 95
+ Word: and, Frequency: 70
+ Word: to, Frequency: 67
+ Word: of, Frequency: 50
+ Word: that, Frequency: 49
+ Word: a, Frequency: 48
+ Word: in, Frequency: 38
+ Word: is, Frequency: 33
+ Word: about, Frequency: 30
+ Word: authors, Frequency: 25

# Design:
+ 1 scanner thread that recursively iterates through <Directory_Path> and searches for all ".txt" files.
+ Scanner thread adds all paths to message queue. 
+ Message Queue is implemented with a singly linked list.
+ A Scanner thread acquires write lock on linked-list and adds a new node to the tail of list with ".txt" file path in it.
+ Linked-list is used as message queue because it can dynamically grow in size.
+ 4 worker threads, read file paths from Message Queue in parallel.
+ Each worker thread acquires write lock on message queue and reads file path from top. Worker thread removes top element and reads file path.
+ After node is removed from front, lock on message queue is released.
+ Each worker thread reads a file provided to them, tokenizes words in each line on the basis of following delimeter " .,-".
+ Each word is added to hash_map. If word is not available in hash_map, then word is added to map with counter set to 1. If word is already available, counter is incremented.
+ I provided my own implementaion of thread safe Hash Map. 
+ Every time, a word is added to map, a write lock is aquired and then added.
+ In case of collison, next available is being looked and word is added to that bucket.
+ Current implementation of hash map only supports a maximum of 1000000 words. 
+ After all worker threads are done writing into hash map, they exit.
+ main thread runs through hash_map and searches for Top 10 words with maximum frequency and prints that on console. 

# Limitations:
+ 1 Scanner Thread.
+ 4 Worker Threads.
+ Hash_Map can store a maximum of 1000000.
