#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
using namespace std;

enum Policy { FIFO, LRU, Optimal };
const char* names[] = {"FIFO", "LRU", "Optimal"};

// A page keeps its arrival time until eviction. Its last use changes on every hit.
struct Frame {
    int page;
    int arrived;
    int lastUsed;
};

int firstIn(const vector<Frame>& frames) {
    int oldest = 0;
    for (int i = 1; i < int(frames.size()); ++i) {
        if (frames[i].arrived < frames[oldest].arrived) oldest = i;
    }
    return oldest;
}

int leastRecentlyUsed(const vector<Frame>& frames) {
    int oldest = 0;
    for (int i = 1; i < int(frames.size()); ++i) {
        if (frames[i].lastUsed < frames[oldest].lastUsed) oldest = i;
    }
    return oldest;
}

int usedFarthestInFuture(const vector<Frame>& frames, const vector<int>& pages, int step) {
    int victim = 0;
    int farthest = -1;
    for (int i = 0; i < int(frames.size()); ++i) {
        int next = step + 1;
        while (next < int(pages.size()) && pages[next] != frames[i].page) {
            ++next;
        }
        if (next == int(pages.size())) return i;  // Never used again
        if (next > farthest) {
            farthest = next;
            victim = i;
        }
    }
    return victim;
}

int simulate(const vector<int>& pages, int capacity, Policy policy, bool trace = false) {
    if (capacity < 1) throw invalid_argument("Frame count must be positive.");
    vector<Frame> frames;
    int faults = 0;
    for (int step = 0; step < int(pages.size()); ++step) {
        int page = pages[step];
        int slot = -1;
        for (int i = 0; i < int(frames.size()); ++i) {
            if (frames[i].page == page) {
                slot = i;
                break;
            }
        }

        string decision;
        if (slot >= 0) {
            frames[slot].lastUsed = step;
            decision = "Hit: already present";
        } else {
            ++faults;
            Frame incoming = {page, step, step};
            if (int(frames.size()) < capacity) {
                frames.push_back(incoming);
                decision = "Fault: filled a free frame";
            } else {
                if (policy == FIFO) {
                    slot = firstIn(frames);
                    decision = "Fault: earliest arrival";
                } else if (policy == LRU) {
                    slot = leastRecentlyUsed(frames);
                    decision = "Fault: oldest last use";
                } else {
                    slot = usedFarthestInFuture(frames, pages, step);
                    decision = "Fault: needed latest or never again";
                }
                decision += ", evicted " + to_string(frames[slot].page);
                frames[slot] = incoming;
            }
        }
        if (trace) {
            cout << step + 1 << "\t" << page << "\t";
            for (Frame frame : frames) cout << frame.page << ' ';
            cout << "\t" << decision << '\n';
        }
    }
    return faults;
}

void compare(const vector<int>& pages, int count, bool trace) {
    cout << "\nPages:";
    for (int page : pages) cout << ' ' << page;
    cout << " | Frames: " << count << '\n';
    if (!trace) {
        cout << "FIFO\tLRU\tOptimal  (faults)\n";
        for (Policy policy : {FIFO, LRU, Optimal}) {
            cout << simulate(pages, count, policy) << '\t';
        }
        cout << '\n';
        return;
    }
    for (Policy policy : {FIFO, LRU, Optimal}) {
        cout << names[policy] << '\n';
        cout << "Step\tPage\tFrames\tDecision\n";
        int faults = simulate(pages, count, policy, trace);
        cout << "Faults: " << faults << "  Hits: " << pages.size() - faults << '\n';
    }
}

int tests() {
    struct Case { string name; vector<int> pages; int frames; vector<int> expected; };
    vector<int> anomaly = {1,2,3,4,1,2,5,1,2,3,4,5};
    vector<Case> cases = {
        {"Empty stream", {}, 3, {0,0,0}},
        {"Repeated page", {0,0,0}, 1, {1,1,1}},
        {"One frame", {1,2,2,1}, 1, {3,3,3}},
        {"Spare frames", {1,2,1,3}, 5, {3,3,3}},
        {"Hit refreshes recency", {1,2,1,3,1}, 2, {4,3,3}},
        {"Anomaly with three frames", anomaly, 3, {9,10,7}},
        {"Anomaly with four frames", anomaly, 4, {10,8,6}}
    };
    int failures = 0;
    for (const Case& test : cases) {
        for (Policy policy : {FIFO, LRU, Optimal}) {
            int actual = simulate(test.pages, test.frames, policy);
            bool pass = actual == test.expected[policy];
            cout << (pass ? "PASS " : "FAIL ") << test.name << " / " << names[policy]
                 << " expected=" << test.expected[policy] << " actual=" << actual << '\n';
            if (!pass) ++failures;
        }
    }
    cout << failures << " failed checks\n";
    return failures == 0 ? 0 : 1;
}

int main(int argc, char* argv[]) {
    try {
        if (argc == 2 && string(argv[1]) == "--test") return tests();
        if (argc == 1) {
            for (const vector<int>& pages : vector<vector<int>>{
                    {1,2,3,4,1,2,5,1,2,3,4,5}, {1,2,1,3,1}})
                for (int count : {2,3,4}) compare(pages, count, false);
        } else {
            vector<int> numbers;
            for (int i = 1; i < argc; ++i) {
                string token = argv[i];
                size_t used;
                int value = stoi(token, &used);
                if (used != token.size() || value < 0)
                    throw invalid_argument("Use nonnegative whole numbers.");
                numbers.push_back(value);
            }
            compare(vector<int>(numbers.begin()+1, numbers.end()), numbers[0], true);
        }
    } catch (const exception& error) {
        cerr << "Invalid input (" << error.what() << "). Usage: ./comparator FRAMES PAGE ...\n";
        return 1;
    }
}
