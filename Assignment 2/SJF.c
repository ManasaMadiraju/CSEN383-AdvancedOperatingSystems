#include <stdio.h>
#include <stdlib.h>
#include "stat.h"
#include "utility.h"

process_stat * create_process_stat(process* proc);

int compareRunTime(void * data1, void * data2) {
	process_stat * ps1 = (process_stat *) data1;
	process_stat * ps2 = (process_stat *) data2;
	if(((process *)ps1->proc)->run_time < ((process *)ps2->proc)->run_time) {
		return -1;
	} else {
		return 1;
	}
}

average_stats shortest_job_first_np(linked_list * processes) {
	int t = 0;

	//Create Process Queue
	queue *process_queue = (queue *)create_queue();
	node * proc_ptr = processes->head;
	if(processes->head == NULL) {
		fprintf(stderr,"No Process to schedule\n");
	}
	//while process queue is not empty or time quanta is less than 100
	process_stat * scheduled_process = NULL;

	linked_list *ll = create_linked_list();
	printf("\nShortest Job First:\n");
	while(t<100 || scheduled_process!=NULL) {
		//check for incoming new process and enqueue it in the queue
		if(proc_ptr != NULL) {
			process * new_process = (process *)(proc_ptr->data);
			while(proc_ptr!=NULL && new_process->arrival_time <= t) {
				enqueue(process_queue,create_process_stat(new_process));
				sort(process_queue,compareRunTime);
				proc_ptr = proc_ptr->next;
				if(proc_ptr!=NULL)
					new_process = (process *)(proc_ptr->data);
			}
		}

		//if there is no scheduled process, then check process queue and schedule it
		if(scheduled_process == NULL && process_queue->size > 0) {
			scheduled_process = (process_stat *) dequeue(process_queue);
		}

		if(scheduled_process != NULL) {
			process * proc = scheduled_process->proc;

			//add current running process to the time chart
			printf("%c",proc->pid);

			//update current processes stat
			if(scheduled_process->start_time == -1) {
				scheduled_process->start_time = t;
			}
			scheduled_process->run_time++;

			if(scheduled_process->run_time >= proc->run_time) {
				scheduled_process->end_time = t;
				add_node(ll,scheduled_process);
				scheduled_process = NULL;
			}
		} else {
			printf("_");
		}
		//increase the time
		t++;
	}
	printf("\n");

	//Print Process Stat
	return print_policy_stat(ll);
	/*
	printf("\nProcess Name\t| Arrival Time | Run Time | Response Time | Wait Time | Turn Around Time | Priority |\n");
	node * ptr = ll->head;
	float avg_response_time = 0;
	float avg_wait_time = 0;
	float avg_turnaround = 0;
	int process_count = 0;
	while(ptr!=NULL) {
		process_stat *stat = (process_stat *)ptr->data;
		if(stat == NULL) printf("No Stat\n");
		process *proc = (process *)stat->proc;
		if(proc == NULL) printf("No Process\n");
		float arrival_time = proc->arrival_time;
		float run_time = proc->run_time;
		float response_time = stat->start_time - arrival_time;
		float turnaround = stat->end_time - proc->arrival_time + 1;
		float wait_time = turnaround - run_time;
		unsigned char priority = proc->priority;
		avg_response_time += response_time;
		avg_wait_time += wait_time;
		avg_turnaround += turnaround;
		process_count++;
		printf("%16c|%14.1f|%10.1f|%15.1f|%11.1f| %17.1f| %9u|\n",proc->pid,arrival_time, run_time, response_time, wait_time, turnaround, priority);
		ptr = ptr->next;
	}
	avg_response_time = avg_response_time / process_count;
	avg_wait_time = avg_wait_time / process_count;
	avg_turnaround = avg_turnaround / process_count;
	printf("-----------------------------------------------------------------------------------------------------\n");
	printf("%16c|%14.1f|%10.1f|%15.1f|%11.1f| %17.1f\n",' ',0.0, 0.0, avg_response_time, avg_wait_time, avg_turnaround);
	//compute overall stat
	//return stat
	*/
}
