#include <kcpolydb.h>

#include <chrono>
#include <string>
#include <iostream>

using namespace std;
using namespace kyotocabinet;

class IntComparator final : public Comparator
{
   public:
    explicit IntComparator()
    {
    }
    int32_t compare(const char* akbuf, size_t aksiz, const char* bkbuf, size_t bksiz) override
    {
        int64_t ai, bi;
        memcpy(&ai, akbuf, aksiz);
        memcpy(&bi, bkbuf, bksiz);
        return (ai - bi);
    }
};

// main routine
int main(int argc, char** argv)
{
    // create the database object
    TreeDB db;

    IntComparator cmp;
    db.tune_comparator(&cmp);

    if (!db.open("casket.kch", PolyDB::OWRITER | PolyDB::OCREATE))
    {
        cerr << "open error: " << db.error().name() << endl;
        return 1;
    }

    auto now = std::chrono::high_resolution_clock::now();

    static long long EventsCount = 1e6;
    static std::string deadbeef("beef");

    for (long long i = 0; i < EventsCount; ++i)
    {
        db.add(to_string(i), deadbeef);
    }

    auto now2 = std::chrono::high_resolution_clock::now();
    auto elapsed_us = std::chrono::duration_cast<std::chrono::microseconds>(now2 - now).count();

    cout << "events = " << EventsCount << endl;
    cout << "elapsed time = " << elapsed_us / 1e3 << "ms" << endl;
    cout << "rate = " << elapsed_us / (double)EventsCount << " us/insert" << endl;

    db.close();
    return 0;
}
