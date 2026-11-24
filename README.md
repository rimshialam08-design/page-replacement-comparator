# Page Replacement Algorithm Comparator

Open a terminal in the extracted project folder. Build with a C++17 compiler.

    clang++ -std=c++17 -Wall -Wextra -pedantic comparator.cpp -o comparator
    ./comparator
    ./comparator --test
    ./comparator 2 1 2 1 3 1

The default demo compares two reference strings at 2, 3 and 4 frames. For a custom trace, the first number gives the frame count. The remaining numbers are page requests in time order. Each policy starts with empty frames.

Read simulate() first. Each Frame stores the page number, arrival step and most recent use. A hit updates only the most recent use. A free frame takes the incoming page. When all frames are full, one of three functions picks the slot to replace.

| Policy | Function | Question |
| --- | --- | --- |
| FIFO | firstIn | Which page arrived first? |
| LRU | leastRecentlyUsed | Which page was touched longest ago? |
| Optimal | usedFarthestInFuture | Which page can we wait longest to load again? |

For the viva, run the custom trace above. Pause before step 4. Page 1 arrived before page 2 but was used more recently. Predict which page each policy evicts, then use step 5 to show the consequence.

The default comparison also shows Belady's anomaly. On the 12-request example FIFO records 9 faults with three frames and 10 with four. The simulator counts page faults for a single process. Optimal assumes the entire request sequence is known.

tests() compares fixed expected counts with actual counts. results.txt contains output from the tests, default demo and custom trace.

See [the test-case guide](TEST_CASES.md) for all seven inputs, their hit and fault patterns, explanations, and commands to replay them.
