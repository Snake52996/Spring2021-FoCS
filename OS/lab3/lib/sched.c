#include <env.h>
#include <pmap.h>
#include <printf.h>

#define GET_PRI(pri) ((pri)&0xff)
#define GET_FUNC1(pri) (((pri)>>8)&0xff)
#define GET_FUNC2(pri) (((pri)>>16)&0xff)
#define GET_FUNC3(pri) (((pri)>>24)&0xff)
#define SET_PRI(ppri,val) do{\
(*(ppri))=(((*(ppri))&0xffffff00)|(val&0xff));\
}while(0);
#define SET_FUNC1(ppri,val) do{\
(*(ppri))=(((*(ppri))&0xffff00ff)|((val&0xff)<<8));\
}while(0);
#define SET_FUNC2(ppri,val) do{\
(*(ppri))=(((*(ppri))&0xff00ffff)|((val&0xff)<<16));\
}while(0);
#define SET_FUNC3(ppri,val) do{\
(*(ppri))=(((*(ppri))&0x00ffffff)|((val&0xff)<<24));\
}while(0);

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
    static unsigned int clock_interrupt_count = 0;
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
    struct Env* tempe = NULL;
    int new_env_pri;
    e = NULL;
    ++clock_interrupt_count;
    LIST_FOREACH(tempe, &env_sched_list[0], env_sched_link){
    	if(tempe->countdown > 0) --(tempe->countdown);
    	if(tempe->countdown == 0) tempe->env_status = ENV_RUNNABLE;
    }
    LIST_FOREACH(tempe, &env_sched_list[1], env_sched_link){
    	if(tempe->countdown > 0) --(tempe->countdown);
    	if(tempe->countdown == 0) tempe->env_status = ENV_RUNNABLE;
    }
    if(curenv != NULL){
    	if(GET_FUNC2(curenv->env_pri) == clock_interrupt_count){
    		curenv->countdown = GET_FUNC3(curenv->env_pri);
    		curenv->env_status = ENV_NOT_RUNNABLE;
    	}
    }
    if(curenv != NULL){
    	new_env_pri = GET_PRI(curenv->env_pri) - GET_FUNC1(curenv->env_pri);
    	if(new_env_pri < 0) new_env_pri = 0;
    	SET_PRI(&(curenv->env_pri), new_env_pri);
    }
    LIST_FOREACH(tempe, &env_sched_list[0], env_sched_link){
    	if(tempe->env_status == ENV_RUNNABLE && (e == NULL || GET_PRI(tempe->env_pri) > GET_PRI(e->env_pri))){
    		e = tempe;
    	}
    }
    LIST_FOREACH(tempe, &env_sched_list[1], env_sched_link){
    	if(tempe->env_status == ENV_RUNNABLE && (e == NULL || GET_PRI(tempe->env_pri) > GET_PRI(e->env_pri))){
    		e = tempe;
    	}
    }
    if(e != curenv){
    	if(curenv != NULL){
    		LIST_REMOVE(curenv, env_sched_link);
    		LIST_INSERT_TAIL(&env_sched_list[1 - point], curenv, env_sched_link);
    	}
    }
	env_run(e);
}
