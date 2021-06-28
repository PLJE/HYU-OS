#include "sched.h"
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/linkage.h>

static void put_prev_task_mysched(struct rq *rq , struct task_struct *p);

void init_mysched_rq(struct mysched_rq *mysched_rq)
{
	printk(KERN_INFO "***[MYSCHED] Myschedclass is online \n");
	INIT_LIST_HEAD(&mysched_rq->queue);
}

static void update_curr_mysched(struct rq *rq)
{
}
static void enqueue_task_mysched(struct rq *rq, struct task_struct *p, int flags) 
{
	list_add_tail(&p->mysched.run_list , &rq->mysched.queue);
	rq->mysched.nr_running++;
	printk(KERN_INFO "***[MYSCHED] enqueue : success cpu=%ld ,nr_running=%d, p->staate =%d,p->pid=%ld\n", rq->cpu, rq->mysched.nr_running,p->state,p->pid);
}

static void dequeue_task_mysched(struct rq *rq, struct task_struct *p, int flags)
{
	printk(KERN_INFO "***[MYSCHED]dequeue : start \n");
	int count = (int)rq->mysched.nr_running;
	if(count >=1){
		list_del_init(&p->mysched.run_list);
		rq->mysched.nr_running--;
		printk(KERN_INFO "***[MYSCHED] dequeue : the dequeued task is curr, set TIF_NEED_RESCHED flag cpu=%d p->state=%d p->pid=%ld curr->pid=%ld\n",rq->cpu, p->state,p->pid,rq->curr->pid);
	}
	printk(KERN_INFO "***[MYSCHED] dequeue: end\n");	
}
static void check_preempt_curr_mysched(struct rq *rq, struct task_struct *p, int flags) 
{ 
}
struct task_struct *pick_next_task_mysched(struct rq *rq, struct task_struct *prev)
{
	if(list_empty(rq->mysched.queue.next))
		return NULL;
	struct sched_mysched_entity * next_entity = NULL;
	struct task_struct * par;

	put_prev_task(rq,prev);
	next_entity = list_entry(rq->mysched.queue.next,struct sched_mysched_entity,run_list);
	par = container_of(next_entity , struct task_struct , mysched);
		
	printk(KERN_INFO "***[MYSCHED] pick_next_task : cpu=%d, prev->pid=%ld , next_p->pid=%ld, nr_running=%d\n", rq->cpu,prev->pid,par->pid,rq->mysched.nr_running);
	printk(KERN_INFO "***[MYSCHED] pick_next_task : end\n");

	return par;
}
static void put_prev_task_mysched(struct rq *rq, struct task_struct *p) 
{
 	//struct task_struct * p;
	//p=get_current()
	printk(KERN_INFO "***[MYSCHED] put_prev_task : do nothing, p->pid :  %ld\n",p->pid);
}	
static int select_task_rq_mysched(struct task_struct *p, int cpu, int sd_flag, int flags)
{
	return task_cpu(p);
}
static void set_curr_task_mysched(struct rq *rq) 
{ 
}
static void task_tick_mysched(struct rq *rq, struct task_struct *p, int queued) 
{ 
}
static void prio_changed_mysched(struct rq *rq, struct task_struct *p, int oldprio) 
{ 
}
/* This routine is called when a task migrates between classes */
static void switched_to_mysched(struct rq *rq, struct task_struct *p)
{
	resched_curr(rq);
}
const struct sched_class mysched_sched_class = {
	.next			= &idle_sched_class,
	.enqueue_task		= enqueue_task_mysched,
	.dequeue_task		= dequeue_task_mysched,
	.check_preempt_curr	= check_preempt_curr_mysched,
	.pick_next_task		= pick_next_task_mysched,
	.put_prev_task		= put_prev_task_mysched,

#ifdef CONFIG_SMP
	.select_task_rq		= select_task_rq_mysched,
#endif
	.set_curr_task		= set_curr_task_mysched,
	.task_tick		= task_tick_mysched,
	.prio_changed		= prio_changed_mysched,
	.switched_to		= switched_to_mysched,
	.update_curr		= update_curr_mysched,
};
