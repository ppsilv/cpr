#include "search.h"
#include "client.h"
#include <iostream>

int main(int argc, char *argv[])
{
    ESClient client("http://192.168.15.6:9200/my_index");
    printf("Vou buscar via search...\n");
    //Search search(MatchQuery("user", "Phil") + TermQuery("user_id", 11));
    Search search(MatchQuery("user", "Phil"));
    printf("Vou buscar resultado...\n");
    auto results = client.search(search);
    printf("Vou listar...\n");
    for(auto result : results.hits()){
        std::cout << result.id();
    }
    return 0;
} 

