# How the seven test cases work

Each run starts with empty frames. Read the requests from left to right. A hit (H) means the page is already present. A fault (F) means the page must be loaded. A fault can fill a free slot or replace an existing page.

In the extracted folder, compile and run the automatic checks.

```bash
clang++ -std=c++17 comparator.cpp -o comparator
./comparator --test
```

The tests() function runs every case once per algorithm and compares the returned fault count with a fixed expected count. Seven cases × three policies gives 21 checks. A PASS means the counts match. The tables below show the actual request-by-request results.

## 1. Empty stream

Frames **3** · Requests **empty**

There is room for three pages, but the process requests nothing. All counters stay at zero.

| Algorithm | Result for each request | Faults | Hits |
| --- | --- | ---: | ---: |
| FIFO | No requests | 0 | 0 |
| LRU | No requests | 0 | 0 |
| Optimal | No requests | 0 | 0 |

Replay with `./comparator 3`.

## 2. Repeated page

Frames **1** · Requests **0 0 0**

The first 0 fills the frame. The next two requests reuse it. This checks that the simulator treats page 0 as a valid page number.

| Algorithm | Result for each request | Faults | Hits |
| --- | --- | ---: | ---: |
| FIFO | F H H | 1 | 2 |
| LRU | F H H | 1 | 2 |
| Optimal | F H H | 1 | 2 |

Replay with `./comparator 1 0 0 0`.

## 3. One frame

Frames **1** · Requests **1 2 2 1**

Page 1 fills the frame. Page 2 replaces it. The second 2 hits, then the final 1 replaces 2. With only one slot, all three algorithms must choose the same victim.

| Algorithm | Result for each request | Faults | Hits |
| --- | --- | ---: | ---: |
| FIFO | F F H F | 3 | 1 |
| LRU | F F H F | 3 | 1 |
| Optimal | F F H F | 3 | 1 |

Replay with `./comparator 1 1 2 2 1`.

## 4. Spare frames

Frames **5** · Requests **1 2 1 3**

Pages 1 and 2 take free slots. The second request for 1 hits. Page 3 takes another free slot. Each distinct page faults once, and nothing is evicted.

| Algorithm | Result for each request | Faults | Hits |
| --- | --- | ---: | ---: |
| FIFO | F F H F | 3 | 1 |
| LRU | F F H F | 3 | 1 |
| Optimal | F F H F | 3 | 1 |

Replay with `./comparator 5 1 2 1 3`.

## 5. Hit refreshes recency

Frames **2** · Requests **1 2 1 3 1**

At step 3, page 1 gets a hit. When 3 arrives at step 4, FIFO removes 1 because it arrived first. LRU removes 2 because it was used less recently. Optimal also removes 2 because only 1 is requested again. The final request for 1 therefore faults under FIFO and hits under LRU and Optimal.

| Algorithm | Result for each request | Faults | Hits |
| --- | --- | ---: | ---: |
| FIFO | F F H F F | 4 | 1 |
| LRU | F F H F H | 3 | 2 |
| Optimal | F F H F H | 3 | 2 |

Replay with `./comparator 2 1 2 1 3 1`.

## 6. Anomaly with three frames

Frames **3** · Requests **1 2 3 4 1 2 5 1 2 3 4 5**

Under FIFO, requests 4, 1 and 2 each force a replacement after the first three loads. When 5 arrives at step 7, the frames hold 5, 1 and 2. Requests 1 and 2 then hit, and 5 survives until the final request. FIFO gets three hits in total.

| Algorithm | Result for each request | Faults | Hits |
| --- | --- | ---: | ---: |
| FIFO | F F F F F F F H H F F H | 9 | 3 |
| LRU | F F F F F F F H H F F F | 10 | 2 |
| Optimal | F F F F H H F H H F F H | 7 | 5 |

Replay with `./comparator 3 1 2 3 4 1 2 5 1 2 3 4 5`.

## 7. Anomaly with four frames

Frames **4** · Requests **1 2 3 4 1 2 5 1 2 3 4 5**

With four frames, FIFO gets early hits on 1 and 2 at steps 5 and 6. They still have the oldest arrival times. From step 7 onward, requests 5, 1, 2, 3, 4 and 5 each remove a page that is needed later or, on the final request, fill the slot of the oldest page. All six requests fault. FIFO ends with 10 faults versus 9 with three frames. This increase is Belady’s anomaly. LRU improves from 10 to 8 faults and Optimal from 7 to 6.

| Algorithm | Result for each request | Faults | Hits |
| --- | --- | ---: | ---: |
| FIFO | F F F F H H F F F F F F | 10 | 2 |
| LRU | F F F F H H F H H F F F | 8 | 4 |
| Optimal | F F F F H H F H H H F H | 6 | 6 |

Replay with `./comparator 4 1 2 3 4 1 2 5 1 2 3 4 5`.
