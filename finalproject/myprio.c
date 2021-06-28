#include "sched.h"
static void put_prev_task_myprio(struct rq *rq, struct task_struct *p); //need to change all to myprio !
static int select_task_rq_myprio(struct task_struct *p, int cpu, int sd_flag, int flags);
static void set_curr_task_myprio(struct rq *rq);
static void task_tick_myprio(struct rq *rq,struct task_struct *p, int oldprio);
static void switched_to_myprio(struct rq *rq, struct task_struct *p);
void init_myprio_rq(struct myprio_rq *myprio_rq);
static void update_curr_myprio(struct rq *rq);
static void enqueue_task_myprio(struct rq *rq, struct task_struct *p, int flags);
static void dequeue_task_myprio(struct rq *rq, struct task_struct *p, int flags);
static void check_preempt_curr_myprio(struct rq *rq, struct task_struct *p,int flags);
struct task_struct *pick_next_task_myprio(struct rq *rq, struct task_struct *prev);
static void prio_changed_myprio(struct rq *rq, struct task_struct *p, int oldprio);

//This is priority scheduler 
//preemption and aging implemented
//2021-1 OS final project !!!!

const struct sched_class myprio_sched_class={
	.next=&fair_sched_class, //myrr -> myprio -> fair , also need to change .next of myrr!!!
	.enqueue_task=&enqueue_task_myprio,
	.dequeue_task=dequeue_task_myprio,
	.check_preempt_curr=check_preempt_curr_myprio,
	.pick_next_task=pick_next_task_myprio,
	.put_prev_task=put_prev_task_myprio,
#ifdef CONFIG_SMP
	.select_task_rq=select_task_rq_myprio,
#endif
	.set_curr_task=set_curr_task_myprio,
	.task_tick=task_tick_myprio,
	.prio_changed=prio_changed_myprio,
	.switched_to=switched_to_myprio,
	.update_curr=update_curr_myprio,
};

//for agigin
#define AGING 4000000000 // about 5 seconds

void init_myprio_rq (struct myprio_rq *myprio_rq)
{
	myprio_rq -> nr_running = 0; //initialize nr_running
	unsigned int i=0;
	for(i=0 ; i<5 ;i++){
		INIT_LIST_HEAD(&myprio_rq ->queue[i]); //initialize each queue
		myprio_rq -> prio_running[i] = 0; // each counting
	} 
	printk(KERN_INFO "***[MYPRIO] Myprio class is online!\n");
}
static void update_curr_myprio(struct rq *rq){
	struct task_struct * curr = rq->curr;
	struct sched_myprio_entity * prio_se = &curr -> myprio;
	u64 delta_exec;
	delta_exec = rq_clock_task(rq) - curr->se.exec_start;
	//compute exec time 
	unsigned int high = 0;
	unsigned int i=0;
	for(i=0;i<5;i++){ //find current most highest prio - high
		if(rq->myprio.prio_running[i]>=1){
			high=i;
			break;
		}
	}
	if(delta_exec > AGING && prio_se->myprio > high){
	//if exec time exceeds AGING, proceed aging
		int pre = prio_se->myprio;
		list_del_init(&prio_se->run_list);
		rq->myprio.prio_running[prio_se->myprio]-=1;
		prio_se->myprio = 0; //change priority to 0 
		curr -> mypriority = 0;
		list_add_tail(&prio_se->run_list, &rq->myprio.queue[0]);
		rq->myprio.prio_running[0]+=1;
		printk(KERN_INFO "***[MYPRIO] update_curr_myprio :success aging pid=%d , pre prio = %d, cur prio = 0\n",curr->pid, pre);
		resched_curr(rq);//resched
	}
		
}

static void enqueue_task_myprio(struct rq *rq, struct task_struct *p, int flags) {
	p->myprio.myprio = p->mypriority;
	unsigned int pr = p-> mypriority;
	rq->myprio.prio_running[pr]+=1; //increment number of queue
	rq->myprio.nr_running+=1; //total number
	list_add_tail(&p->myprio.run_list, &rq->myprio.queue[pr]); //add
	printk(KERN_INFO "***[MYPRIO] enqueue : success nr_running=%d , prio = %d , pid=%d\n",rq->myprio.nr_running, pr , p->pid);	
}
static void dequeue_task_myprio(struct rq *rq, struct task_struct *p, int flags) 
{
	if(rq->myprio.nr_running <=0) //if empty, return 
		return;
	unsigned int pr = p->myprio.myprio;
	p->mypriority= pr;
	list_del_init(&p->myprio.run_list);
	//delete 
	rq->myprio.nr_running-=1;
	rq->myprio.prio_running[pr]-=1;
	printk(KERN_INFO "***[MYPRIO] dequeue:success nr_running=%d, prio=%d, pid=%d\n",rq->myprio.nr_running , pr, p->pid);
	
}
void check_preempt_curr_myprio(struct rq *rq, struct task_struct *p, int flags) {
	printk("***[MYPRIO] check_preempt_curr_myprio\n");
}
struct task_struct *pick_next_task_myprio(struct rq *rq, struct task_struct *prev)
{
	if(rq->myprio.nr_running <1 ) // no task, just return 
		return NULL;

	unsigned int i=0;
	unsigned int pick;
	for(i=0;i<5;i++){ // find current most highest priority - pick
		if(rq->myprio.prio_running[i] >=1){
			pick = i;
			break;
		}
	}
	struct task_struct * par; //parent. same as myrr
	struct sched_myprio_entity * next = NULL;
	next = list_entry(rq->myprio.queue[pick].next , struct sched_myprio_entity,run_list); 
	par = container_of(next, struct task_struct, myprio);
	printk(KERN_INFO "***[MYPRIO] pick_next_task : success nr_running =%d , prio=%d, prev->pid=%d next->pid=%d\n",rq->myprio.nr_running,pick,prev->pid, par->pid );
	return par;
}
void put_prev_task_myprio(struct rq *rq, struct task_struct *p) {
	printk(KERN_INFO "\t***[MYPRIO] put_prev_task: do_nothing, p->pid=%d\n",p->pid);
}
int select_task_rq_myprio(struct task_struct *p, int cpu, int sd_flag, int flags){return task_cpu(p);}
void set_curr_task_myprio(struct rq *rq){
	printk(KERN_INFO"***[MYPRIO] set_curr_task_myprio\n");
}
void task_tick_myprio(struct rq *rq, struct task_struct *p, int queued) {
	update_curr_myprio(rq);
}
void prio_changed_myprio(struct rq *rq, struct task_struct *p, int oldprio) { }
/*This routine is called when a task migrates between classes*/
void switched_to_myprio(struct rq *rq, struct task_struct *p)
{
	resched_curr(rq);
}
