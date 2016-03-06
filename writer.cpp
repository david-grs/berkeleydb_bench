#include <db_cxx.h>
#include <dbstl_map.h>

#include <iostream>
#include <string>
#include <chrono>

using namespace std;
using namespace dbstl;

int compare_int(Db *dbp, const Dbt *a, const Dbt *b, size_t *locp)
{
    locp = NULL;

    int64_t ai, bi;
    memcpy(&ai, a->get_data(), sizeof(ai));
    memcpy(&bi, b->get_data(), sizeof(bi));
    return (ai - bi);
}

int main(int argc, char **argv)
{
    try {
        Db pdb(NULL, DB_CXX_NO_EXCEPTIONS);
        pdb.set_bt_compare(compare_int);

        unsigned gbytes = 0;
        unsigned bytes = 128 * 1024 * 1024;
        int caches = 1;
        cout << pdb.set_cachesize(gbytes, bytes, caches) << endl;

        pdb.open(NULL, "access.db", NULL, DB_BTREE, DB_CREATE, 0);
        dbstl::db_map<int64_t, ElementHolder<int>> v(&pdb, NULL);

        auto now = std::chrono::high_resolution_clock::now();
        int64_t ts = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();

        static long long SecondsInOneDay = 24 * 60 * 60;
        static long long EventsCount = 100 * SecondsInOneDay;

        for(long long i = 0; i < EventsCount; ++i, ts += 1)
        {
            v.insert(std::make_pair(ts, 0xdeadbeef));
        }

        auto now2 = std::chrono::high_resolution_clock::now();
        auto elapsed_us = std::chrono::duration_cast<std::chrono::microseconds>(now2 - now).count();
        cout << "events = " << EventsCount << endl;
        cout << "useful bytes = " << (sizeof(int64_t) + sizeof(int)) * EventsCount << endl;
        cout << elapsed_us / (double)EventsCount << " us/write"  << endl;

        pdb.close(0);
    }
    catch (std::exception& e) {
        cerr << "fail: " << e.what() << endl;
        return 1;
    }

    return 0;
}
