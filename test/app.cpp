#include "search.h"
#include "client.h"
#include <iostream>

int main(int argc, char *argv[])
{
    ESClient client("http://localhost:9200/");
    Search search(MatchQuery("name", "Charlie") + TermQuery("user_id", 11));
    auto results = client.search(search);
    for(auto result : results.hits()){
        std::cout << result.id();
    }
    return 0;
} 

