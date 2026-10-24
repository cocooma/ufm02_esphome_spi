
import esphome.codegen as cg

CODEOWNERS = []
DEPENDENCIES = []

ufm02_ns = cg.esphome_ns.namespace("ufm02")
UFM02Component = ufm02_ns.class_(
    "UFM02Component",
    cg.PollingComponent,
)

# Download the official ScioSense UFM02 Arduino library.
async def to_code(config):
    cg.add_library(
        "https://github.com/sciosense/ufm02-arduino",
        None,
    )
