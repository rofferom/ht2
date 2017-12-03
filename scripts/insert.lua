function debug(s, ...)
	print(s:format(...))
end

function info(s, ...)
	print(s:format(...))
end

function insert(cfg)
	info("Opening table %s", cfg.tbl_path)
	tbl = ht.Table()
	ret = htl.loadTable(cfg.tbl_path, "UTF-8", tbl)
	if ret < 0 then
		return ret
	end

	info("Opening text file %s", cfg.txt_path)
	txt = ht.Text()
	ret = htl.loadText(cfg.txt_path, "UTF-8", txt)
	if ret < 0 then
		return ret
	end


	debug("Generate encoded text")
	encoded_txt = ht.Buffer()
	ptr_tbl = ht.PointerTable()
	ret = txt:encode(tbl, encoded_txt, ptr_tbl)
	if ret < 0 then
		return ret
	end

	debug("Done. %d bytes generated. Got %d pointers",
		encoded_txt:getSize(), ptr_tbl:getCount())

	-- Update pointers addresses (pointer address in pointer table)
	function update_src_addr(idx)
		return cfg.ptr_offset + cfg.ptr_size * (idx - 1)
	end

	ptr_tbl:updateSourceAddress(update_src_addr)

	-- Update pointers offsets (pointer value in pointer table)
	function update_ptr_offset(offset)
		return cfg.txt_offset + offset
	end

	ptr_tbl:updateOffset(update_ptr_offset)

	f = ht.File()
	info("Opening output file %s", cfg.out_path)
	ret = f:open(cfg.out_path, "rb+")
	if ret < 0 then
		return ret
	end

	debug("Insert encoded text")
	ret = f:writeBuffer(cfg.txt_offset, encoded_txt)
	if ret < 0 then
		return ret
	end

	debug("Insert pointers")
	ret = ptr_tbl:write(f, cfg.ptr_size, ht.PointerEndianness.Little)
	if ret < 0 then
		return ret
	end

	info("Insertion done")

	f:close()
end

function insert_list(cfg_list)
	for idx, cfg in ipairs(cfg_list) do
		insert(cfg)
	end
end

