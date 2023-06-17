def filt(evt):
  print("In python")
  print(hm.print.content(evt))
  print("In python")
  return True


def py_dummy(evt):
  for ip in evt.particles():
    if (ip.status() == 1) and (ip.pid() == 13):
      return ip.momentum().e()