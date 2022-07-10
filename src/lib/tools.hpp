#include <phpcpp.h>
#include <string>
#include <regex>

using namespace std;
//tools used
namespace tools {
    //simple error tool
    void phperr(string err) {
        Php::error << err << std::flush;
    };

    //generate list string
    string liststr(Php::Value value2) {
        if (!value2.isArray()) tools::phperr("value2 is not an Array");

        string sthelp           = "{";
        vector<Php::Value> val = value2.vectorValue<Php::Value>();
        int len = count(val);    
        if (len<1) tools::phperr("array has to have at least one element");

        for (int x=0;x<len;x++) {
            string v = val[x].stringValue();
            if (val[x].isString()) v='"'+v+'"';
            if (sthelp=="{") sthelp+=v;
            else sthelp+=','+v;
        };
        sthelp+="}";
        return sthelp;
    }

   

}