#include <env.h>
#include <pmap.h>
#include <printf.h>

/* Overview:
 *  Implement simple round-robin scheduling.
 *
 *
 * Hints:
 *  1. The variable which is for counting should be defined as 'static'.
 *  2. Use variable 'env_sched_list', which is a pointer array.
 *  3. CANNOT use `return` statement!
 */
/*** exercise 3.14 ***/
void sched_yield(void)
{

    static int count = 0; // remaining time slices of current env
    static int point = 0; // current env_sched_list index
    static struct Env* e = NULL;
    //  hint:
    //  1. if (count==0), insert `e` into `env_sched_list[1-point]`
    //     using LIST_REMOVE and LIST_INSERT_TAIL.
    //  2. if (env_sched_list[point] is empty), point = 1 - point;
    //     then search through `env_sched_list[point]` for a runnable env `e`,
    //     and set count = e->env_pri
    //  3. count--
    //  4. env_run()
    //
    //  functions or macros below may be used (not all):
    //  LIST_INSERT_TAIL, LIST_REMOVE, LIST_FIRST, LIST_EMPTY
    //
	if(count == 0){
		if(curenv != NULL){
			LIST_REMOVE(curenv, env_sched_link);
			LIST_INSERT_TAIL(&env_sched_list[1 - point], curenv, env_sched_link);
		}
		if(LIST_EMPTY(&env_sched_list[1 - point])) point = 1 - point;
		LIST_FOREACH(e, &env_sched_list[1 - point], env_sched_link){
			if(e->env_status == ENV_RUNNABLE) break;
		}
		count = e->env_pri;
	}
	--count;
	env_run(e);
}