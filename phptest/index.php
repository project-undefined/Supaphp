<?php
for ($x=0;$x<count(get_loaded_extensions());$x++) {
    echo(get_loaded_extensions()[$x] . "\n");
};
echo("\n");

$supa = new SupaObject();
$supa->set_api_key("boo");
$supa->set_query_url("boo");
$supa->get();