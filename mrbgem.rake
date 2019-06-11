MRuby::Gem::Specification.new('mruby-plato-serial-nrf52') do |spec|
  spec.license = 'MIT'
  spec.authors = 'Hiroshi Mimaki'
  spec.description = 'PlatoNRF52::Serial class'

  spec.cc.defines << %w(NO_NRFSDK)

  spec.add_test_dependency('mruby-plato-serial')
end
