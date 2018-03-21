struct supplemental_page_table {
  void *page;
  bool writable;
  struct file *file;
  size_t offset;
  size_t read_bytes;
  size_t zero_bytes;

  struct supplemental_page_table *next_elem;
};

bool sup_page_table_add_entry (struct file *file, int32_t ofs, uint8_t *upage,
			     uint32_t read_bytes, uint32_t zero_bytes,
			     bool writable);

struct supplemental_page_table * get_supp_page_table_entry(void *fault_addr);

bool add_page_to_stack (void *fault_addr);

bool load_page(struct supplemental_page_table  *sp_table);
