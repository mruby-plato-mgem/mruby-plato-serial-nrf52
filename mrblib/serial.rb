#
# PlatoNRF52::Serial class
#

module PlatoNRF52
  class Serial
    include Plato::Serial
  end
end

# register NRF52 device
Plato::Serial.register_device(PlatoNRF52::Serial)
