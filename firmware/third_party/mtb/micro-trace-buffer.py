# MIT License
#
# Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

# SAMD21 MTB register values. Adjust accordingly.
REG_MTB_POSITION = 0x41006000
REG_MTB_MASTER   = 0x41006004
REG_MTB_FLOW     = 0x41006008
REG_MTB_BASE     = 0x4100600C

MTB_ARRAY_NAME   = "mtb"

class MicroTraceBuffer (gdb.Command):
  """Greet the whole world."""

  def __init__ (self):
    super (MicroTraceBuffer, self).__init__ ("micro-trace-buffer", gdb.COMMAND_STATUS, gdb.COMPLETE_NONE)

  def invoke (self, arg, from_tty):
    mtb, method_field = gdb.lookup_symbol(MTB_ARRAY_NAME)
    if mtb and mtb.type.code == gdb.TYPE_CODE_ARRAY and mtb.type.target().name == "uint32_t":
      mtb_values = mtb.value()
      last_sym_line_pc = None

      uint32_t_ptr = gdb.lookup_type("uint32_t").pointer()

      position = gdb.Value(REG_MTB_POSITION)
      position = position.cast(uint32_t_ptr).dereference()
      position = int(position)

      base = gdb.Value(REG_MTB_BASE)
      base = base.cast(uint32_t_ptr).dereference()
      base = int(base)

      position += base
      mtb_address = long(mtb.value().address)

      oldest_index = (position - mtb_address) / 4 - 1
      repeats = 0
      mtb_size = mtb.type.sizeof / 4
      indices = range(oldest_index, mtb_size) + range(0, oldest_index)
      for i, index in enumerate(reversed(indices)):
        pc = mtb_values[index]

        sym_line = gdb.find_pc_line(int(pc))
        next_sym_line = gdb.find_pc_line(int(mtb_values[(index + 1) % mtb_size]))
        if i == len(indices) - 1 or not sym_line or not next_sym_line or sym_line.pc != next_sym_line.pc:
          if sym_line.symtab:
            gdb.write("0x{:08x} {} {} {} times\n".format(int(pc), sym_line.symtab.filename, sym_line.line, repeats + 1))
          else:
            gdb.write("0x{:08x} no symtab {}\n".format(int(pc), sym_line))
          repeats = 0
        else:
          repeats += 1
        last_sym_line_pc = sym_line.pc
    else:
      gdb.write("No mtb symbol.\n")

MicroTraceBuffer()

# Catch any errors in case this file is sourced a second time and the alias has
# already been defined.
try:
  gdb.execute("alias -a mtb = micro-trace-buffer")
except:
  pass
