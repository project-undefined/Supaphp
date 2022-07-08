#include <phpcpp.h>
#include <regex>
#include <string>
#include <cpr/cpr.h>
#include "lib/patterns.hpp"

//using namespace mpark::patterns;
using namespace std;




class SupaObject : public Php::Base {
    public:
        
        void __construct() {
            Php::Value self(this);
            self.set("api_key",   "null");
            self.set("query_url", "null");
            self.set("query",     "null");
            self.set("list_help", "null");
        }

        //simple error tool
        void phperr(string err) {
            Php::Value current_line = Php::Base::__get("__line__");
            string complete_err = regex_replace(err, std::regex("__line__"), current_line.stringValue());
            Php::error << complete_err << std::flush;
        }

        // sets the url to query from
        void set_query_url(Php::Parameters &param) {
            Php::Value self(this);
            if (!param[0].isString()) {
                phperr("[Supaphp/Err __line__]: Query URL is not a string.");
                return ; 
            }

            self.set("query_url", param[0].stringValue());
        }
        
        // sets the api key used for supabase
        void set_api_key(Php::Parameters &param) {
          Php::Value self(this);

          if (!param[0].isString()) {
                phperr("[Supaphp/Err __line__]: API key is not a string.");
                return ; 
          }

           self.set("api_key", param[0].stringValue());
            
           
        }
        
        // selects a table to query
        Php::Value table(Php::Parameters &param) {
           Php::Value self(this);

           if (self.get("api_key")=="null") {
              phperr("[Supaphp/Err __line__]: API key is not set.");
              return -1; 
           }
           else if (self.get("query_url")=="null") {
              phperr("[Supaphp/Err __line__]: Query URL is not set.");
              return -1; 
           }
           
           self.set("query",param[0].stringValue());
 

           return self;
        }
        
        // selects a record based on values
        // TODO
        Php::Value where(Php::Parameters &param) {
            Php::Value self(this);
            /* match(param[1].stringValue())(
                pattern("==")       = [] {},
                pattern("!=")       = [] {},
                pattern("=")        = [] {},
                pattern(">=")       = [] {},
                pattern("<=")       = [] {},
                pattern("<")        = [] {},
                pattern(">")        = [] {},
                pattern("@>")       = [] {},
                pattern("<@")       = [] {},
                pattern("in")       = [] {},
                pattern(_)          = [] {  
                    phperr("[Supaphp/Err __line__]: Conditional is invalid.");
                    return ; 
                }
            );*/
            phperr("[Supaphp/err __line__]: where function is currently unavailable.");
            return self; 
        }

        
        // execute the query
        Php::Value execute()  {
            Php::Value self(this);
            if (self.get("query_url") == "null") {
                phperr("[Supaphp/Err __line__]: Query URL is not set.");
                return -1;
            }
            if (self.get("api_key") == "null") {
                phperr("[Supaphp/Err __line__]: API key is not set.");
                return -1;
            }
            if (self.get("query")   == "null" || self.get("query")=="") {
                 phperr("[Supaphp/Err __line__]: No query is built.");
                return -1;
            }
            cpr::Response res = cpr::Get(
                cpr::Url{self.get("query_url").stringValue()+self.get("query").stringValue()},
                cpr::Header{{"apikey", self.get("api_key").stringValue()}},
                cpr::Bearer{self.get("api_key").stringValue()}
            );
            return res.text;

        }
};



extern "C" {
    PHPCPP_EXPORT void *get_module() {
        static Php::Extension  supaphp("supaphp", "1.0");
        
        
        // set up the SupaObject class for use in php
        Php::Class<SupaObject> supaObject("SupaObject");
    
        // set SupaObject properties
        supaObject.property("api_key",   "null", Php::Private);
        supaObject.property("query_url", "null", Php::Private);
        supaObject.property("query",     "null",   Php::Private);
        supaObject.property("list_help", "null",   Php::Private);
        // end of SupaObject properties
        
        // set SupaObject functions
        supaObject.method<&SupaObject::__construct> ("__construct");
        supaObject.method<&SupaObject::set_api_key> ("set_api_key", {
            Php::ByVal("key", Php::Type::String, false)
        });
        supaObject.method<&SupaObject::set_query_url> ("set_query_url", {
            Php::ByVal("url", Php::Type::String, false)
        });
        supaObject.method<&SupaObject::table> ("table", {
            Php::ByVal("query", Php::Type::String, false)
        });
        supaObject.method<&SupaObject::where> ("where", {
            Php::ByVal("value", Php::Type::String),
            Php::ByVal("conditional", Php::Type::String),
            Php::ByVal("value2", Php::Type::String)
        });
        supaObject.method<&SupaObject::execute> ("execute");
        // end of SupaObject funtions
        
        supaphp.add(std::move(supaObject));
        return supaphp;
    }
}