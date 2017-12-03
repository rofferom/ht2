function debug(s, ...)
	print(s:format(...))
end

function info(s, ...)
	print(s:format(...))
end

function extract(cfg)
  info("Opening table %s", cfg.tbl_path)
	tbl = ht.Table()
	ret = htl.loadTable(cfg.tbl_path, "UTF-8", tbl)
	if ret < 0 then
    info("htl.loadTable() failed: %d", ret)
		return ret
	end

  f = ht.File()
	info("Opening input file %s", cfg.input_path)
	ret = f:open(cfg.input_path, "rb")
	if ret < 0 then
    info("ht.File.open() failed: %d", ret)
		return ret
	end

  debug("Read encoded text")
  encoded_txt = ht.Buffer()
	ret = f:readBuffer(cfg.txt_offset, encoded_txt, cfg.txt_size)
	if ret < 0 then
    info("ht.File.readBuffer() failed: %d", ret)
		return ret
	end

  ptr_tbl = ht.PointerTable()
  ret = ptr_tbl:read(f, cfg.ptr_offset, cfg.ptr_count, cfg.ptr_size, ht.PointerEndianness.Little)
  if ret < 0 then
    info("ht.PointerTable.read() failed: %d", ret)
		return ret
	end

  -- Update pointers addresses (pointer address in pointer table)
	function update_offset(addr)
		return addr - cfg.txt_offset
	end

	ptr_tbl:updateOffset(update_offset)

  info("Decode text")
  txt = ht.Text()
  ret = txt:decode(encoded_txt, tbl, ptr_tbl)
  if ret < 0 then
    info("ht.Text.decode() failed: %d", ret)
		return ret
	end

  info("Save decoded text to %s", cfg.txt_path)
  ret = htl.saveText(txt, cfg.txt_path, "UTF-8")
  if ret < 0 then
    info("htl.saveText() failed: %d", ret)
		return ret
	end

  f:close()
end

function extract_list(cfg_list)
	for idx, cfg in ipairs(cfg_list) do
		extract(cfg)
	end
end
