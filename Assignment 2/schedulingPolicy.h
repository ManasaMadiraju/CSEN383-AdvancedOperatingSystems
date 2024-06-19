#ifndef _scheduling_policy_h
#define _scheduling_policy_h



//enum State {UNUSED, EMBRYO, SLEEPING, RUNNING, ZOMBIE};

struct process_stat_s {
	(Process *) proc;
	unsigned float wait_time;
	unsigned float completion_time;
	unsigned float turnaround_time;
	unsigned float response_time;

	enum State state;
}
typedef struct process_stat_s ProcessStat;

struct stat_s {
	(linked_list *) process_queue;
	(linked_list *) time_chart;
	unsigned float avg_turnaround_time;
	unsigned float avg_waiting_time;
	unsigned float avg_response_time;
	unsigned float throughput;
	int total_quanta;
}
typedef struct stat_s SchedulingStat;
/*
SchedulingStat * first_come_first_serve_np(linked_list * processes);
SchedulingStat * shortest_job_first_np(linked_list * processes);
SchedulingStat * shortest_remaining_time_p(linked_list * processes);
SchedulingStat * round_robin_p(linked_list * processes,int time_slice);
SchedulingStat * highest_priority_first_p(linked_list * processes);
SchedulingStat * highest_priority_first_np(linked_list * processes);
SchedulingStat * highest_priority_first_p_aging(linked_list * processes);
SchedulingStat * highest_priority_first_np_aging(linked_list * processes);
*/
#endif
