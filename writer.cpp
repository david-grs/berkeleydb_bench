#include <db_cxx.h>
#include <dbstl_map.h>

#include <iostream>
#include <string>
#include <chrono>
#include <fstream>

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

        static const std::string DBFilename = "test.db";
        pdb.open(NULL, DBFilename.c_str(), NULL, DB_BTREE, DB_CREATE, 0);
        dbstl::db_map<int64_t, ElementHolder<int>> v(&pdb, NULL);

        auto now = std::chrono::high_resolution_clock::now();

        static long long EventsCount = 1e6;
        for(long long i = 0; i < EventsCount; ++i)
        {
            v.insert(std::make_pair(i, 0xdeadbeef));
        }

        auto now2 = std::chrono::high_resolution_clock::now();
        auto elapsed_us = std::chrono::duration_cast<std::chrono::microseconds>(now2 - now).count();

        cout << "events = " << EventsCount << endl;
        cout << "elapsed time = " << elapsed_us / 1e3 << "ms" << endl;
        cout << "rate = " << elapsed_us / (double)EventsCount << " us/insert"  << endl;

        int useful_bytes = (sizeof(int64_t) + sizeof(int)) * EventsCount;
        int filesize;
        {
            std::ifstream in(DBFilename.c_str(), std::ifstream::ate | std::ifstream::binary);
            filesize = in.tellg();
        }
        cout << "written bytes = " << filesize << ", information bytes = " << useful_bytes << " (" << (100 * useful_bytes / (double)filesize) << "%)" << endl;

        pdb.close(0);
    }
    catch (std::exception& e) {
        cerr << "fail: " << e.what() << endl;
        return 1;
    }

    return 0;
}
