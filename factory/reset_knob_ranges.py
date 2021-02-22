from libgemini import gem_settings, gemini

gem = gemini.Gemini()

default_settings = gem_settings.GemSettings()

settings = gem.read_settings()

settings.castor_knob_min = default_settings.castor_knob_min
settings.castor_knob_max = default_settings.castor_knob_max
settings.pollux_knob_min = default_settings.pollux_knob_min
settings.pollux_knob_max = default_settings.pollux_knob_max

gem.save_settings(settings)

print(gem.read_settings())

gem.soft_reset()
