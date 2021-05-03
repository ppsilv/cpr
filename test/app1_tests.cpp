#include <cpr/cpr.h>
#include <iostream>
#include <cstdlib>
#include <unistd.h>

//                      cpr::Authentication{"user", "pass"},
char cUrl[1024];
int index=181;
int temp1 = 4;
int temp2 = temp1;
int temp3 = temp1;

void put()
{    
    char myjson[1024];
    sprintf(myjson,"{\"sensor1\":\"20.%02d\",\"sensor2\":\"21.%02d\",\"sensor3\":\"22.%02d\"}",temp1,temp2,temp3);   
     
    sprintf(cUrl,"http://192.168.15.6:9200/temperatura/leitura/%d",index);
    cpr::Response r = cpr::Post(cpr::Url{cUrl}, cpr::Header{{"Content-Type", "application/json"}},cpr::Body{myjson},cpr::ConnectTimeout{3000}, cpr::Timeout{3000} );
    r.status_code;                  // 200
    r.header["content-type"];       // application/json; charset=utf-8
    r.text;                         // JSON text string
    
    if( r.status_code == 200 || r.status_code == 201 ){
        printf("Success!!! [%ld]\n",r.status_code);
        printf("r.text[%s]\n",r.text.c_str());
    }else{
        printf("Error [%ld]\n",r.status_code);
    }
}

int main(int argc, char** argv) 
{
    put();
    usleep(5000);
    sprintf(cUrl,"http://192.168.15.6:9200/temperatura/_search?q=_id:%d",index);
    cpr::Response r = cpr::Get(cpr::Url{cUrl});
    r.status_code;                  // 200
    r.header["content-type"];       // application/json; charset=utf-8
    r.text;                         // JSON text string
    
    if( r.status_code == 200 ){
        printf("Success!!! [%ld]\n",r.status_code);
        printf("r.text[%s]\n",r.text.c_str());
    }else{
        printf("Error [%ld]\n",r.status_code);
    }
} 
