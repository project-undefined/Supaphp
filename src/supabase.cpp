#include <phpcpp.h>
#include <regex>
#include <string>
#include <cpr/cpr.h>
#include "lib/patterns.hpp"
#include "lib/tools.hpp"
using namespace mpark::patterns;
using namespace std;


class SupaObject : public Php::Base {
    public:
        
        void __construct() {
            Php::Value self(this);
            self.set("api_key",   "null");
            self.set("query_url", "null");
            self.set("query",     "?");
            self.set("list_help", "null");
        }

        

        // sets the url to query from
        Php::Value set_query_url(Php::Parameters &param) {
            Php::Value self(this);
            string p = param[0].stringValue();
            if (p[p.length()-1] != '/') {
                tools::phperr("Please add a '/' to the end of query URL");
            }
            self.set("query_url", param[0].stringValue() + "rest/v1/"); 
            return self;   
        }
        
        // sets the api key used for supabase
        Php::Value set_api_key(Php::Parameters &param) {
          Php::Value self(this);
          self.set("api_key", param[0].stringValue());
          return self;
        }
        
        // gets the current built query
        Php::Value get_query() {
            Php::Value self(this);
            return self.get("query");
        }
        
        // selects a table to query
        Php::Value table(Php::Parameters &param) {
           Php::Value self(this);

           if (self.get("api_key")=="null") {
              tools::phperr("API key is not set");
              return -1; 
           }
           else if (self.get("query_url")=="null") {
              tools::phperr("Query URL is not set");
              return -1; 
           }
           
           self.set("query",param[0].stringValue() + "?");
 

           return self;
        }
        
        // selects a record based on values
        // TODO
        Php::Value where(Php::Parameters &param) {
            Php::Value self(this);
            string value       = param[0].stringValue();
            string conditional = param[1].stringValue();
            Php::Value value2  = param[2];
            

            // refrence for horizontal query filtering 
            // https://postgrest.org/en/stable/api.html#horizontal-filtering-rows
            // TODO: OR operations and more conditionals
            string querystr = self.get("query").stringValue();
            string v2str    = value2.stringValue();
            match(param[1].stringValue()) (
                // equals
                pattern("==")       = [&self, &value, &conditional, &value2, querystr, v2str] {
                    if (value2.isString()) value2='"' + v2str + '"';
                    if (querystr[querystr.length()-1]=='?') self.set("query",querystr+value+"="+"eq."+v2str);
                    else self.set("query",querystr+"&"+value+"="+"eq."+v2str);
                },
                pattern("=")        = [&self, &value, &conditional, &value2,  querystr, v2str] {
                    if (value2.isString()) value2='"' + v2str + '"';
                    if (querystr[querystr.length()-1]=='?') self.set("query",querystr+value+"="+"eq."+v2str);
                    else self.set("query",querystr+"&"+value+"="+"eq."+v2str);
                },
                pattern(">=")       = [&self, &value, &conditional, &value2, querystr, v2str] {
                    if (!value2.isNumeric()) tools::phperr("value2 is not numeric");
                    if (querystr[querystr.length()-1]=='?') self.set("query",querystr+value+"="+"gte."+v2str);
                    else self.set("query",querystr+"&"+value+"="+"gte."+v2str);
                },
                pattern("<=")       = [&self, &value, &conditional, &value2, querystr, v2str] {
                    if (!value2.isNumeric()) tools::phperr("value2 is not numeric");
                    if (querystr[querystr.length()-1]=='?') self.set("query",querystr+value+"="+"lte."+v2str);
                    else self.set("query",querystr+"&"+value+"="+"lte."+v2str);
                },
                pattern("<")        = [&self, &value, &conditional, &value2, querystr, v2str] {
                    if (!value2.isNumeric()) tools::phperr("value2 is not numeric");
                    if (querystr[querystr.length()-1]=='?') self.set("query",querystr+value+"="+"lt."+v2str);
                    else self.set("query",querystr+"&"+value+"="+"lt."+v2str);
                },
                pattern(">")        = [&self, &value, &conditional, &value2, querystr, v2str] {
                    if (!value2.isNumeric()) tools::phperr("value2 is not numeric");
                    if (querystr[querystr.length()-1]=='?') self.set("query",querystr+value+"="+"gt."+v2str);
                    else self.set("query",querystr+"&"+value+"="+"gt."+v2str);
                },  
                pattern("@>")       = [&self, &value, &conditional, &value2, querystr, v2str] {
                    string v = tools::liststr(value2);
                    if (querystr[querystr.length()-1]=='?') self.set("query",querystr+value+"="+"cs."+v);
                    else self.set("query",querystr+"&"+value+"="+"cs."+v);


                },
                pattern("<@")       = [&self, &value, &conditional, &value2, querystr, v2str] {
                    string v = tools::liststr(value2);
                    if (querystr[querystr.length()-1]=='?') self.set("query",querystr+value+"="+"cd."+v);
                    else self.set("query",querystr+"&"+value+"="+"cd."+v);
                },
                // not
                //
                pattern("!=")       = [&self, &value, &conditional, &value2, querystr, v2str] {
                    if (value2.isString()) value2='"' + v2str + '"';
                    if (querystr[querystr.length()-1]=='?') self.set("query",querystr+value+"="+"not.eq."+v2str);
                    else self.set("query",querystr+"&"+value+"="+"not.eq."+v2str);
                },
                pattern("!>=")       = [&self, &value, &conditional, &value2,  querystr, v2str] {
                    if (!value2.isNumeric()) tools::phperr("Numeric value for value2 is required");
                    if (querystr[querystr.length()-1]=='?') self.set("query",querystr+value+"="+"not.gte."+v2str);
                    else self.set("query",querystr+"&"+value+"="+"not.gte."+v2str);
                },
                pattern("!<=")       = [&self, &value, &conditional, &value2, querystr, v2str] {
                    if (!value2.isNumeric()) tools::phperr("Numeric value for value2 is required");
                    if (querystr[querystr.length()-1]=='?') self.set("query",querystr+value+"="+"not.lte."+v2str);
                    else self.set("query",querystr+"&"+value+"="+"not.lte."+v2str);
                },
                pattern("!<")        = [&self, &value, &conditional, &value2, querystr, v2str] {
                    if (!value2.isNumeric()) tools::phperr("value2 is not numeric");
                    if (querystr[querystr.length()-1]=='?') self.set("query",querystr+value+"="+"not.lt."+v2str);
                    else self.set("query",querystr+"&"+value+"="+"not.lt."+v2str);
                },
                pattern("!>")        = [&self, &value, &conditional, &value2, querystr, v2str] {
                    if (!value2.isNumeric()) tools::phperr("value2 is not numeric");
                    if (querystr[querystr.length()-1]=='?') self.set("query",querystr+value+"="+"not.gt."+v2str);
                    else self.set("query",querystr+"&"+value+"="+"not.gt."+v2str);
                },
                pattern("!@>")       = [&self, &value, &conditional, &value2,  querystr, v2str] {
                    string v = tools::liststr(value2);
                    if (querystr[querystr.length()-1]=='?') self.set("query",querystr+value+"="+"not.cs."+v);
                    else self.set("query",querystr+"&"+value+"="+"not.cs."+v);


                },
                pattern("!<@")       = [&self, &value, &conditional, &value2,  querystr, v2str] {
                    string v = tools::liststr(value2);
                    if (querystr[querystr.length()-1]=='?') self.set("query",querystr+value+"="+"not.cd."+v);
                    else self.set("query",querystr+"&"+value+"="+"not.cd."+v);
                },
                pattern(_)          = [param] {  
                    tools::phperr("Conditional '" + param[1].stringValue() + "' is invalid");
                    return ; 
                }
            );
            return self; 
        }
        
        // execute the query and flushes the query value
        Php::Value execute()  {
            Php::Value self(this);
            if (self.get("query_url") == "null") {
                tools::phperr("Query URL is not set");
                return -1;
            }
            if (self.get("api_key") == "null") {
                tools::phperr("API key is not set");
                return -1;
            }
            if (self.get("query")   == "null" || self.get("query")=="?") {
                 tools::phperr("No query is built");
                return -1;
            }
            cpr::Response res = cpr::Get(
                cpr::Url{self.get("query_url").stringValue()+self.get("query").stringValue()},
                cpr::Header{{"apikey", self.get("api_key").stringValue()}},
                cpr::Bearer{self.get("api_key").stringValue()}
            );
            self.set("query", "?");
            return res.text;

        }

        
};


extern "C" {
    PHPCPP_EXPORT void *get_module() {
        static Php::Extension  supaphp("supaphp", "1.0");
        
        
        // set up the SupaObject class for use in php
        Php::Class<SupaObject> supaObject("SupaObject");
    
        // set SupaObject properties
        supaObject.property("api_key",   "null", Php::Public);
        supaObject.property("query_url", "null", Php::Public);
        supaObject.property("query",     "?",   Php::Private);
        supaObject.property("list_help", "null",   Php::Private);
        // end of SupaObject properties
        
        // set SupaObject functions
        supaObject.method<&SupaObject::__construct>   ("__construct");
        supaObject.method<&SupaObject::get_query>     ("get_query");
        supaObject.method<&SupaObject::execute>       ("execute");
        supaObject.method<&SupaObject::set_api_key>   ("set_api_key", {
            Php::ByVal("key", Php::Type::String, false)
        });
        supaObject.method<&SupaObject::set_query_url> ("set_query_url", {
            Php::ByVal("url", Php::Type::String, false)
        });
        supaObject.method<&SupaObject::table>         ("table", {
            Php::ByVal("query", Php::Type::String, false)
        });
        supaObject.method<&SupaObject::where>         ("where", {
            Php::ByVal("value", Php::Type::String),
            Php::ByVal("conditional", Php::Type::String),
            Php::ByVal("value2", Php::Type::Resource)
        });
        // end of SupaObject funtions
        
        supaphp.add(std::move(supaObject));
        return supaphp;
    }
}