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
        unsigned bytes = 1024 * 1024;
        int caches = 1;
        pdb.set_cachesize(gbytes, bytes, caches);

        static const std::string DBFilename = "test.db";
        pdb.open(NULL, DBFilename.c_str(), NULL, DB_BTREE, 0, 0);
        dbstl::db_map<int64_t, ElementHolder<int>> v(&pdb, NULL);

        int sum = 0;
        auto now = std::chrono::high_resolution_clock::now();
        static int EventsCount = 1e6;

        // benchmark lookup of 1e3 ranges of 1e3 elements
        if (argc == 2 && std::string("range") == argv[1])
        {
            for (int i = 0; i < EventsCount / 1e3; ++i)
            {
                int n = rand() % (int)1e6;

                for (auto it = v.lower_bound(n, true); it != v.lower_bound(n + 1e3, true); ++it)
                    sum += it->second;
            }
        }
        // benchmark 1e6 random lookups
        else
        {
            for (int i = 0; i < EventsCount; ++i)
            {
                int n = rand() % (int)1e6;
                sum += v.lower_bound(n, true)->second;
            }
        }

        auto now2 = std::chrono::high_resolution_clock::now();
        auto elapsed_us = std::chrono::duration_cast<std::chrono::microseconds>(now2 - now).count();

        cout << "events = " << EventsCount << " lookups" << endl;
        cout << "elapsed time = " << elapsed_us / 1e3 << "ms" << endl;
        cout << "rate = " << elapsed_us / (double)EventsCount << " us/insert"  << endl;

        pdb.close(0);
    }
    catch (std::exception& e) {
        cerr << "fail: " << e.what() << endl;
        return 1;
    }

    return 0;
}
