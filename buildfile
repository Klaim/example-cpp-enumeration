
cxx.std = 20

using cxx

exe{test} : cxx{*}
exe{test} : test = true

