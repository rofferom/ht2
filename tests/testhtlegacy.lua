table = ht.Table()

r = htl.loadTable("testlegacytable.tbl", "UTF-8", table)
print(r)
print(table:getEntryCount())

text = ht.Text()
print(text)
r = htl.loadText("testlegacytext.txt", "UTF-8", text)
print(r)
print(text:getBlockCount())

--r = htl.saveText(text, "testlegacytext-generated.txt", "UTF-8")
--print(r)

buffer = ht.Buffer()
print(buffer)

pointerList = ht.TextPointerList()
print(pointerList)

print(buffer:getSize())
r = text:encode(table, buffer, pointerList)
print(r)
print(buffer:getSize())

pointerCount = pointerList:getCount()
for i = 0, pointerCount-1 do
	pointer = pointerList:get(i)
	print(string.format("Id : %d, Offset : %d", pointer:getId(), pointer:getOffset()))
end

