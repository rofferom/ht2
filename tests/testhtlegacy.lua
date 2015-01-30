table = ht.Table()

r = htl.loadTable("testlegacytable.tbl", "UTF-8", table)
print(r)
print(table:getEntryCount())

text = ht.Text()
print(text)
r = htl.loadText("testlegacytext.txt", "UTF-8", text)
print(r)
print(text:getBlockCount())

r = htl.saveText(text, "testlegacytext-generated.txt", "UTF-8")
print(r)

