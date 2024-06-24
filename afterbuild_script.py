# Script to run after build/upload is done
# in this case we convert the firmware.bin file to firmware.u2f

Import("env")

def after_build(source, target, env):
  print("after_build convert firmware.bin to firmware.uf2")
  #do some actions
  #env.Execute("uf2conv.py -c -o firmware.u2f .pio/build/adafruit_itsybitsy_m0/firmware.bin")
  env.Execute("uf2conv.py -c -o firmware.u2f .pio/build/$PIOENV/firmware.bin")

env.AddPostAction("$BUILD_DIR/firmware.bin", after_build)