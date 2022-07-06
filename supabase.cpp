#include <phpcpp.h>
#include <cpr/cpr.h>

class SupaObject : public Php::Base {
    public:
        
        //sets the api key for supabase
        void set_api_key(Php::Parameters &param) {
          if (!param[0].isString()) {
                Php::echo("[Supaphp/Err]: api key is not a string.");
                return ; 
          }

          this->api_key = param[0].stringValue();
            
           
        }
        Php::Value select(Php::Parameters &param) {
           
        }
    private:
       std::string api_key;
}



extern "C" {
    PHPCPP_EXPORT void *get_module {
        static Php::Extension  supaphp("supaphp", "1.0");

        Php::Class<SupaObject> supaObject("SupaObject");
        supaObject.method<&SupaObject::select> ("select", {
            Php::ByVal("query", Php::Type::String)
        });
        supaObject.method<&SupaObject::set_api_key> ("set_api_key", {
            Php::ByVal("key", Php::Type::String)
        })
        supaphp.add(std::move(SupaObject))
        return supaphp;
    }
}