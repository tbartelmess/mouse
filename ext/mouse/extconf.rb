require 'mkmf'

$CFLAGS << ' -std=c99 -Wall -Werror -pedantic'
$LIBS   << ' -framework Foundation -framework ApplicationServices'
$LIBS   << ' -framework CoreGraphics' unless `sw_vers -productVersion`.to_f == 10.7

if RUBY_ENGINE == 'macruby'
  $LIBS << ' -framework Cocoa'
  $CFLAGS << ' -ObjC -fobjc-gc'
else
  unless RbConfig::CONFIG["CC"].match /clang/
    clang = `which clang`.chomp
    if clang.empty?
      $stdout.puts "Clang not installed. Cannot build C extension"
      raise "Clang not installed. Cannot build C extension"
    else
      RbConfig::MAKEFILE_CONFIG["CC"]  = clang
      RbConfig::MAKEFILE_CONFIG["CXX"] = clang
    end
  end
  $CFLAGS << ' -DNOT_MACRUBY'
end

create_makefile 'mouse/mouse'
