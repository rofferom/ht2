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

t:testCallback(
	function (a, b)
		print(string.format("Callback, got a=%d, b=%d", a, b))
		return a + b
	end
)

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

--r = testRefConst:customHandler(46)
--print(r)

print("---------")
print("Param forward")
print("---------")
tParam = Test()
tParam:setA(43)

t2 = Test2()
t3 = Test3()
--t2:testOtherClass(t3)
--t2:testOtherClass(testRefConst)


print("---------")
print("Function binder")
print("---------")

r = testFunction1(1)
print(r)

r = testFunction2(2, 3)
print(r)

r = testFunction3(4, 5, 6)
print(r)

r = testFunction4(7, 8, 9)
print(r)
