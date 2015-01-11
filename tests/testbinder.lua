print("---------")
print("Managed")
print("---------")
t = Test()

r = t:testVoid()
print(r)

r = t:testReturnValue()
print(r)

r = t:testOneArg(44)
print(r)

r = t:testOneArgReturn(45)
print(r)

r = t:customHandler(46)
print(r)

r = t:testTwoArgs(47, 48)
print(r)

r = t:testStringArg("TestArg")
print(r)

print("getA")
r = t:getA()
print(r)

print("setA(10)")
t:setA(10)
r = t:getA()
print(r)

print("---------")
print("Ref")
print("---------")
r = testRef:testVoid()
print(r)

r = testRef:testReturnValue()
print(r)

r = testRef:testOneArg(44)
print(r)

r = testRef:testOneArgReturn(45)
print(r)

r = testRef:customHandler(46)
print(r)

print("---------")
print("ConstRef")
print("---------")
r = testRefConst:testOneArg(44)
print(r)

print("getA")
r = testRefConst:getA()
print(r)

--testRefConst:setA(10)

r = testRefConst:customHandler(46)
print(r)

