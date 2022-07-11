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
            self.set("list_help", "false");
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
        Php::Value select(Php::Parameters &param) {
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
        
        Php::Value lwhere(Php::Parameters &param) {
             Php::Value self(this);
             string query = build_query_from_conditional(param, ".");
             if (query[0]=='&') query.erase(0,1);
             return query;
        }
        // adds output from build query
        Php::Value where(Php::Parameters &param) {
            Php::Value self(this);
            string query = build_query_from_conditional(param, "=");
            self.set("query", self.get("query").stringValue()+query);
            return self;
        }
        
        // OR & AND conditional lists
        // TODO: Errors (checking if value not string)
        Php::Value OR(Php::Parameters &param) {
            Php::Value self(this);
            string querystr = self.get("query").stringValue();
            
            string out="(";
            vector<Php::Value> list = param[0].vectorValue<Php::Value>();
            int len = count(list);
            for (int i=0;i<len;i++) {
                if (!list[i].isString()) tools::phperr("List object is not a string, if you are using a supaphp function try adding an 'l' to the beginning for use in lists.");
                if (out=="(") out+=list[i].stringValue();
                else {
                  out+=","+list[i].stringValue();
                };
            }
            out = "or="+out+")";
            if (querystr[querystr.length()-1]!='?') out="&"+out;
            self.set("query", self.get("query").stringValue()+out);
            return self;
        } 
        Php::Value AND(Php::Parameters &param) {
            Php::Value self(this);
            string querystr = self.get("query").stringValue();
            
            string out="(";
            vector<Php::Value> list = param[0].vectorValue<Php::Value>();
            int len = count(list);
            for (int i=0;i<len;i++) {
                if (!list[i].isString()) tools::phperr("List object is not a string, if you are using a supaphp function try adding an 'l' to the beginning for use in lists.");
                if (out=="(") out+=list[i].stringValue();
                else out+=","+list[i].stringValue();
            }
            out = "and="+out+")";
            if (querystr[querystr.length()-1]!='?') out="&"+out;
            self.set("query", self.get("query").stringValue()+out);
            return self;
        }
        
        // for use in lists
        // this method may be temporary
        Php::Value lOR(Php::Parameters &param) {
            Php::Value self(this);
            string querystr = self.get("query").stringValue();
            
            string out="(";
            vector<Php::Value> list = param[0].vectorValue<Php::Value>();
            int len = count(list);
            for (int i=0;i<len;i++) {
                if (!list[i].isString()) tools::phperr("List object is not a string, if you are using a supaphp function try adding an 'l' to the beginning for use in lists.");
                if (out=="(") out+=list[i].stringValue();
                else out+=","+list[i].stringValue();
            }
            out = "or"+out+")";
            return out;
        } 
        Php::Value lAND(Php::Parameters &param) {
            Php::Value self(this);
            string querystr = self.get("query").stringValue();
            
            string out="(";
            vector<Php::Value> list = param[0].vectorValue<Php::Value>();
            int len = count(list);
            for (int i=0;i<len;i++) {
                if (!list[i].isString()) tools::phperr("List object is not a string, if you are using a supaphp function try adding an 'l' to the beginning for use in lists.");
                if (out=="(") out+=list[i].stringValue();
                else out+=","+list[i].stringValue();
            }
            out = "and"+out+")";
            return out;
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
    private:
                
        // builds a query from a conditional
        // TODO
        string build_query_from_conditional(Php::Parameters &param, string r) {
            Php::Value self(this);
            string value       = param[0].stringValue();
            string conditional = param[1].stringValue();
            Php::Value value2  = param[2];
            string bv="null";
            if (value2.isBool()) {
                        if (value2.boolValue() == true) {
                            bv = "true";
                        }
                        else if (value2.boolValue()==false) {
                            bv = "false";
                        }      
            }

            
            string querystr = self.get("query").stringValue();
            string v2str;
            if (!value2.isArray()) v2str    = value2.stringValue();
            string qu;
            if (value2.isString()) {
                v2str = regex_replace(v2str, regex("\""), "\\\"");
                v2str='"' + v2str + '"';
            }
            // refrence for horizontal query filtering 
            // https://postgrest.org/en/stable/api.html#horizontal-filtering-rows
            match(param[1].stringValue()) (
                // equals
                pattern("==")       = [&self, &value, &conditional, &value2, querystr, v2str, bv, &qu, r] {
                    
                    if (!value2.isBool() && !value2.isNull()) {
                        qu=value+r+"eq."+v2str;
                    }
                    else qu = (value+r+"is."+bv);
                },
                pattern("=")        = [&self, &value, &conditional, &value2,  querystr, v2str, bv, &qu, r] {        
                    if (!value2.isBool() && !value2.isNull()) {
                        qu = (value+r+"eq."+v2str);
                     }
                    else qu = (value+r+"is."+bv);
                
                },
                pattern(">=")       = [&self, &value, &conditional, &value2, querystr, v2str, &qu, r] {
                    if (!value2.isNumeric()) tools::phperr("value2 is not numeric");
                    qu = (value+r+"gte."+v2str);
                },
                pattern("<=")       = [&self, &value, &conditional, &value2, querystr, v2str, &qu, r] {
                    if (!value2.isNumeric()) tools::phperr("value2 is not numeric");
                    qu = (value+r+"lte."+v2str);
                },
                pattern("<")        = [&self, &value, &conditional, &value2, querystr, v2str, &qu, r] {
                    if (!value2.isNumeric()) tools::phperr("value2 is not numeric");
                    qu = (value+r+"lt."+v2str);
                },
                pattern(">")        = [&self, &value, &conditional, &value2, querystr, v2str, &qu, r] {
                    if (!value2.isNumeric()) tools::phperr("value2 is not numeric");
                    qu = (value+r+"gt."+v2str);
                },  
                pattern("@>")       = [&self, &value, &conditional, &value2, querystr, v2str, &qu, r] {
                    string v = tools::liststr(value2);
                    qu = (value+r+"cs."+v);
                },
                pattern("<@")       = [&self, &value, &conditional, &value2, querystr, v2str, &qu, r] {
                    string v = tools::liststr(value2);
                    qu = (value+r+"cd."+v);
                },
                // not
                //
                pattern("!=")       = [&self, &value, &conditional, &value2, querystr, v2str,bv, &qu, r] {
                    if (!value2.isBool() && !value2.isNull()) {
                            qu = (value+r+"not.eq."+v2str);
                    }
                    else qu = value+r+"not.is."+bv;
                },
                pattern("!>=")       = [&self, &value, &conditional, &value2,  querystr, v2str, &qu, r] {
                    if (!value2.isNumeric()) tools::phperr("Numeric value for value2 is required");
                    qu = (value+r+"not.gte."+v2str);
                },
                pattern("!<=")       = [&self, &value, &conditional, &value2, querystr, v2str, &qu, r] {
                    if (!value2.isNumeric()) tools::phperr("Numeric value for value2 is required");
                    qu = (value+r+"not.lte."+v2str);
                },
                pattern("!<")        = [&self, &value, &conditional, &value2, querystr, v2str, &qu, r] {
                    if (!value2.isNumeric()) tools::phperr("value2 is not numeric");
                    qu = (value+r+"not.lt."+v2str);
                },
                pattern("!>")        = [&self, &value, &conditional, &value2, querystr, v2str, &qu, r] {
                    if (!value2.isNumeric()) tools::phperr("value2 is not numeric");
                    qu = (value+r+"not.gt."+v2str);
                },
                pattern("!@>")       = [&self, &value, &conditional, &value2,  querystr, v2str, &qu, r] {
                    string v = tools::liststr(value2);
                    qu = (value+r+"not.cs."+v);
                },
                pattern("!<@")       = [&self, &value, &conditional, &value2,  querystr, v2str, &qu, r] {
                    string v = tools::liststr(value2);
                    qu = (value+r+"not.cd."+v);
                },
                pattern(_)          = [param, &qu, r] {  
                    tools::phperr("Conditional '" + param[1].stringValue() + "' is invalid");
                    return ; 
                }
            );
            if (querystr[querystr.length()-1]!='?') qu="&"+qu;
            return qu; 
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
        supaObject.property("list_help", "false",   Php::Private);
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
        supaObject.method<&SupaObject::select>         ("select", {
            Php::ByVal("query", Php::Type::String, false)
        });
        supaObject.method<&SupaObject::lwhere>  ("lwhere", {
            Php::ByVal("value", Php::Type::String),
            Php::ByVal("conditional", Php::Type::String),
            Php::ByVal("value2", Php::Type::Resource)
        });
        supaObject.method<&SupaObject::AND> ("and", {
            Php::ByVal("qu_list", Php::Type::Array)
        });
        supaObject.method<&SupaObject::OR> ("or", {
            Php::ByVal("qu_list", Php::Type::Array)
        });
        supaObject.method<&SupaObject::lAND> ("land", {
            Php::ByVal("qu_list", Php::Type::Array)
        });
        supaObject.method<&SupaObject::lOR> ("lor", {
            Php::ByVal("qu_list", Php::Type::Array)
        });
        supaObject.method<&SupaObject::where>  ("where", {
            Php::ByVal("value", Php::Type::String),
            Php::ByVal("conditional", Php::Type::String),
            Php::ByVal("value2", Php::Type::Resource)
        });
        // end of SupaObject funtions
        
        supaphp.add(std::move(supaObject));
        return supaphp;
    }
}