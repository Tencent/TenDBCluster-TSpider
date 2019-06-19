package My::Suite::Spider;

@ISA = qw(My::Suite);

return "Not run for embedded server" if $::opt_embedded_server;
return "Test needs --big-test" unless $::opt_big_test;

sub is_default { 1 }

bless { };

