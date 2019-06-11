# mruby-plato-serial-nrf52   [![Build Status](https://travis-ci.org/mruby-plato-mgem/mruby-plato-serial-nrf52.svg?branch=master)](https://travis-ci.org/mruby-plato-mgem/mruby-plato-serial-nrf52)

PlatoNRF52::Serial class

## install by mrbgems

- add conf.gem line to `build_config.rb`

```ruby
MRuby::Build.new do |conf|

  # ... (snip) ...

  conf.gem :git => 'https://github.com/mruby-plato-mgem/mruby-plato-serial'
  conf.gem :git => 'https://github.com/mruby-plato-mgem/mruby-plato-serial-nrf52'
end
```

## example

```ruby
ser = Plato::Serial.open(9600)
name = ser.gets
ser.puts "Hello, #{name.chomp}!"
```

## License

under the MIT License:

- see LICENSE file
