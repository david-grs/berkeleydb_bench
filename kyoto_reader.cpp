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
    TreeDB db;

    IntComparator cmp;
    db.tune_comparator(&cmp);

    if (!db.open("casket.kch", PolyDB::OREADER))
    {
        cerr << "open error: " << db.error().name() << endl;
        return 1;
    }

    DB::Cursor* cur = db.cursor();
    cur->jump_back(std::to_string(55));
    std::string val;
    cur->get_value(&val);

    cout << val << endl;

    db.close();
    return 0;
}
