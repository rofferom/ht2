t = ht.Table()

print(string.format("Max KeySize %d", t:getMaxKeySize()))
print(string.format("Max MaxValue %d", t:getMaxValueSize()))

r = t:addEntry("AA", "A")
print(r)

r = t:addEntry("AB", "B")
print(r)

r = t:addEntry("ABC", "Bob")
print(r)

r = t:addEntry("ABCD", "ÉÇÀ€ù")
print(r)

print(string.format("Max KeySize %d", t:getMaxKeySize()))
print(string.format("Max MaxValue %d", t:getMaxValueSize()))

r = t:findFromValue("John")
print(r)

r = t:findFromValue("Bob")
print(string.format("%s=%s", r:getKey(), r:getValue()))

r = t:findFromKey("AA")
print(string.format("%s=%s", r:getKey(), r:getValue()))

r = t:findFromKey("ABCD")
print(string.format("%s=%s", r:getKey(), r:getValue()))

r = t:findFromValue("ÉÇÀ€ù")
print(string.format("%s=%s", r:getKey(), r:getValue()))

