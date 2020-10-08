import datetime

Import("env")

timestamp = datetime.datetime.now().strftime("%Y%m%dT%H%M%S")
my_flags = env.ParseFlags(env['BUILD_FLAGS'])
defines = {k: v for (k, v) in my_flags.get("CPPDEFINES")}

#print(defines)
#env.Replace(PROGNAME="s0device-monitor_%s_v%s" % (defines.get("ENVIRONMENT"), timestamp))
