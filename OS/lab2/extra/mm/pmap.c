int page_alloc2(struct Page **pp){
    struct Page *ppage_temp;
    /* Step 1: Get a page from free memory. If fail, return the error code.*/
    // simply get the first usable i.e. free page from page_free_list
    ppage_temp = LIST_FIRST(&page_free_list);
    // if we got NULL from ppage_temp, we're out of memory
    if(ppage_temp == NULL) return -E_NO_MEM;
    // now we're sure that we've got an usable page, remove it from page_free_list
    LIST_REMOVE(ppage_temp, pp_link);
    /* Step 2: Initialize this page.
     * Hint: use `bzero`. */
    bzero(page2kva(ppage_temp), BY2PG);
    *pp = ppage_temp;
	printf("page number is %x, start from pa %x\n", page2ppn(ppage_temp), page2pa(ppage_temp));
    return 0;
}

void get_page_status(int pa){
	static int count = 1;
	struct Page* pp = pa2page(pa);
	int sig = 0;
	if(pp->pp_ref > 0) sig = 1;
	else{	// pp_ref == 0
		struct Page* t = NULL;
		int flag = 0;
		LIST_FOREACH(t, &page_free_list, pp_link){
			if(t == pp){
				flag = 1;
				break;
			}
		}
		if(flag) sig = 3;
		else sig = 2;
	}
	printf("times:%d, page status:%d\n", count++, sig);
}
