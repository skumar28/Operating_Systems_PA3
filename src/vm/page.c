#include <string.h>
#include <stdbool.h>
#include "filesys/file.h"
#include "threads/interrupt.h"
#include "threads/malloc.h"
#include "threads/palloc.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "userprog/pagedir.h"
#include "userprog/process.h"
#include "userprog/syscall.h"
#include "vm/page.h"

struct supplemental_page_table *head = NULL;

bool add_sup_page_table_entry (struct file *file, int32_t ofs, uint8_t *upage,
			     uint32_t read_bytes, uint32_t zero_bytes,
			     bool writable)
{
  struct supplemental_page_table *spte = malloc(sizeof(struct supplemental_page_table));
  if (!spte)
    {
      return false;
    }
  spte->file = file;
  spte->offset = ofs;
  spte->page = upage;
  spte->read_bytes = read_bytes;
  spte->zero_bytes = zero_bytes;
  spte->writable = writable;
  spte->next_elem = NULL;

  if(thread_current()->s_page_table == NULL){
	  thread_current()->s_page_table = spte;
	  return true;
  }
  struct supplemental_page_table *temp = thread_current()->s_page_table;
  while(temp->next_elem != NULL){
	  temp = temp->next_elem;
  }
  temp->next_elem = spte;

  return true;
}

struct supplemental_page_table * get_supp_page_table_entry(void *fault_addr){
	void *page = pg_round_down(fault_addr);

	 struct supplemental_page_table *temp = thread_current()->s_page_table;

	 if(temp != NULL){
		 while(temp != NULL){
			 if(temp->page == page){
				 return temp;
			 }
			 temp = temp->next_elem;
		 }
	 }

	 return NULL;
}

bool add_page_to_stack (void *fault_addr)
{
	struct supplemental_page_table *spte = malloc(sizeof(struct supplemental_page_table));
	if (!spte) {
		return false;
	}
	spte->page = pg_round_down(fault_addr);
	spte->writable = true;
	spte->next_elem = NULL;

	uint8_t *kpage = palloc_get_page(PAL_ZERO);

	if (!install_page(spte->page, kpage, spte->writable)) {
		palloc_free_page(kpage);
		free(spte);
		return false;
	}

	struct supplemental_page_table *temp = thread_current()->s_page_table;
	while (temp != NULL) {
		temp = temp->next_elem;
	}
	temp = spte;

	return true;
}


bool load_page(struct supplemental_page_table  *sp_table){
#if 1
		file_seek(sp_table->file, sp_table->offset);
	 /* Get a page of memory. */
	      uint8_t *kpage = palloc_get_page (PAL_USER);
	      if (kpage == NULL)
	       return false;

	      /* Load this page. */
	      if (file_read (sp_table->file, kpage, sp_table->read_bytes) != (int) sp_table->read_bytes)
	       {
	         palloc_free_page (kpage);
	          return false;
	        }
	      memset (kpage + sp_table->read_bytes, 0, sp_table->zero_bytes);
#endif
	      /* Add the page to the process's address space. */
	      if (!install_page (sp_table->page, kpage, sp_table->writable))
	        {
	          palloc_free_page (kpage);
	          return false;
	        }
	      return true;
}

