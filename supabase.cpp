#include <phpcpp.h>


class SupaObject : public Php::Base {
    public:
        Php::Value select(Php::Parameters &param) {

        }
}



extern "C" {
    PHPCPP_EXPORT void *get_module {
        static Php::Extension  supaphp("supaphp", "1.0");

        Php::Class<SupaObject> supaObject("SupaObject");
        supaObject.method<&SupaObject::select> ("select", {
            Php::ByVal("query", Php::Type::String)
        });

        supaphp.add(std::move(SupaObject))
        return supaphp;
    }
}