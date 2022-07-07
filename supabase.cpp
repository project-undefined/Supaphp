#include <phpcpp.h>
#include <cpr/cpr.h>
#include "lib/patterns.hpp"

using namespace mpark::patterns;
class SupaObject : public Php::Base {
    public:
        
        // sets the url to query from
        void set_query_url(Php::Parameters &param) {
            Php::Value self(this);
            if (!param[0].isString()) {
                Php::echo("[Supaphp/Err]: Query URL is not a string.");
                return ; 
            }

            self["query_url"] = param[0].stringValue();
        }
        
        // sets the api key used for supabase
        void set_api_key(Php::Parameters &param) {
          Php::Value self(this);

          if (!param[0].isString()) {
                Php::echo("[Supaphp/Err]: API key is not a string.");
                return ; 
          }

           self["api_key"] = param[0].stringValue();
            
           
        }
        
        // selects a table to query
        Php::Value table(Php::Parameters &param) {
           Php::Value self(this);

           if (self["api_key"]==NULL) {
              Php::echo("[Supaphp/Err]: API key is not set.");
              return ; 
           }
           else if (self["query_url"]==NULL) {
              Php::echo("[Supaphp/Err]: Query URL is not set.");
              return ; 
           }
           
           self["query"] = param[0].stringValue();
 

           return self;
        }
        
        // selects a record based on values
        Php::Value where(Php::Parameters &param) {
            Php::Value self(this);
            match(param[1].stringValue())(
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
                    Php::echo("[Supaphp/Err]: Conditional is invalid.");
                    return ; 
                }
            );

            return self; 
        }

        
        // execute the query
        Php::Value get()  {
            Php::Value self(this);
            cpr::Response res = cpr::Get(
                cpr::Url{self.get("query_url").stringValue()+self.get("query").stringValue()},
                cpr::Header{{"apikey", self.get("api_key").stringValue()}},
                cpr::Bearer{self.get("api_key").stringValue()}
            );
            

        }
}



extern "C" {
    PHPCPP_EXPORT void *get_module {
        static Php::Extension  supaphp("supaphp", "1.0");
        
        
        // set up the SupaObject class for use in php
        Php::Class<SupaObject> supaObject("SupaObject");
        
        // set SupaObject properties
        supaObject.property("api_key",   NULL, Php::Private);
        supaObject.property("query_url", NULL, Php::Private);
        supaObject.property("query",     "",   Php::Private);
        supaObject.property("list_help", "",   Php::Private);
        // end of SupaObject properties

        // set SupaObject functions
        supaObject.method<&SupaObject::set_api_key> ("set_api_key", {
            Php::ByVal("key", Php::Type::String)
        });
        supaObject.method<&SupaObject::set_query_url> ("set_query_url", {
            Php::ByVal("url", Php::Type::String)
        });
        supaObject.method<&SupaObject::table> ("table", {
            Php::ByVal("query", Php::Type::String)
        });
        supaObject.method<&SupaObject::where> ("where", {
            Php::ByVal("value", Php::Type::String),
            Php::ByVal("conditional", Php::Type::String),
            Php::ByVal("value2", Php::Type::String)

        });
        // end of SupaObject funtions
        
        supaphp.add(std::move(SupaObject))
        return supaphp;
    }
}