/****************************************************************************
   Program:     $Id: rpg.h,v 0.1 2013/04/25 $
   Author:      $Author: wenph@bupt $
   Date:        $Date: 2013/04/25 $
   Description: RPG headers
****************************************************************************/
#ifndef FALSE
# define FALSE 0
#endif
#ifndef TRUE
# define TRUE !FALSE
#endif

/* Constants */
#define MAX_THREADS 10
#define BUFSIZE 512

/* TODO can put these into the config file*/
#define DEFAULT_CONFIG_FILE "/home/apple/workplace/rpg/src/rpgd.conf"
#define DEFAULT_IPADDR_FILE "/home/apple/workplace/rpg/src/ipaddr.conf"

/* pthread error messages */
#define PML_ERR "pthread_mutex_lock error\n"
#define PMU_ERR "pthread_mutex_unlock error\n"
#define PCW_ERR "pthread_cond_wait error\n"
#define PCB_ERR "pthread_cond_broadcast error\n"
#define PCS_ERR "pthread_cond_signal error\n"

/* pthread macros */
#define PT_MUTEX_LOCK(x) if (pthread_mutex_lock(x) != 0) printf(PML_ERR);
#define PT_MUTEX_UNLOCK(x) if (pthread_mutex_unlock(x) != 0) printf(PMU_ERR);
#define PT_COND_WAIT(x,y) if (pthread_cond_wait(x, y) != 0) printf(PCW_ERR);
#define PT_COND_BROAD(x) if (pthread_cond_broadcast(x) != 0) printf(PCB_ERR);
#define PT_COND_SIGNAL(x) if (pthread_cond_signal(x) != 0) printf(PCS_ERR);

#define max(a,b) ((a) > (b) ? (a) : (b))
/* Verbosity levels LOW=info HIGH=info+SQL DEBUG=info+SQL+junk */
enum debugLevel {OFF, LOW, HIGH, DEBUG, DEVELOP};


typedef struct target_struct
{
    char host[64];
//    char objoid[128];
//    unsigned short bits;
    char community[64];
//    char table[64];
//    struct target_struct *prev;
    struct target_struct *next;
} target_t;

//TODO there is no thread, these is needed?
typedef struct target_session_struct
{
    char *file;
    pthread_mutex_t mutex;
    pthread_cond_t done;
    pthread_cond_t go;
} target_session_t;

/* Typedefs */
typedef struct worker_struct
{
    int index;
    pthread_t thread;
    struct crew_struct *crew;
} worker_t;

//全局变量，在程序运行时不允许改变里面的值
typedef struct config_struct {
//    unsigned int interval;
//    unsigned long long out_of_range;
    char config_file[BUFSIZE];
    char ipaddr_file[BUFSIZE];
    int n_forks;
    char dbhost[80];
    char dbdb[80];
    char dbuser[80];
    char dbpass[80];
    enum debugLevel verbose;
    int ip_count_interval;
    mongo conn;
//    unsigned short withzeros;
    unsigned short dboff;
//    unsigned short multiple;
//    unsigned short snmp_ver;
//    unsigned short snmp_port;
//    unsigned short threads;
//    float highskewslop;
//    float lowskewslop;
} config_t;

typedef struct crew_struct
{
    int work_count;
    worker_t member[MAX_THREADS];
    pthread_mutex_t mutex;
    pthread_cond_t done;
    pthread_cond_t go;
} crew_t;

typedef struct
{
    pid_t		child_pid;		/* process ID */
    int		child_pipefd;	/* parent's stream pipe to/from child */
    int		child_status;	/* 0 = ready */
    long		child_count;	/* # connections handled */
    mongo conn;
} child_struct;

typedef struct filestruct
{
    long offset;//文件当前的offset
    int iseof;//文件末尾的标志 0:not end, 1:end of the file
} filestruct;


/*Precasts: rtgsnmp.c*/
//void initialize (void);
void snmp_oid_initialize();
//int print_result (int , void *, void *);
//int asynch_response_snmpv1(int , void *, int ,void *, void *);
void *snmp_asynchronous(void *);
void *snmp_synchronous(void *);
int make_target_list(config_t *set, long);
int snmp_asynchronous_poll(int);


/* Precasts: rtgpoll.c */
void *poller(void *);
pid_t child_make(int);

/* Precasts: rtgmysql.c */
//int db_insert(char *, MYSQL *);
//int rtg_dbconnect(char *, MYSQL *);
//void rtg_dbdisconnect(MYSQL *);

/* Precasts: rtgutil.c */
//int read_rtg_config(char *, config_t *);
//int write_rtg_config(char *, config_t *);
void print_cur_time();
void convert_dot2line(char *);
long fetchNextOffset(config_t *set, filestruct *fsp);
void config_defaults(config_t * set);
void copy_config_file_arg(config_t *set, char *optarg);
void copy_ipaddr_file_arg(config_t *set, char *optarg);
void usage(char *prog);
int config_file(config_t *set);

/* Precasts: rtghash.c */
//void init_hash();
//void init_hash_walk();
//target_t *getNext();
//void free_hash();
//unsigned long make_key(const void *);
//void mark_targets(int);
//int delete_targets(int);
//void walk_target_hash();
//void *in_hash(target_t *, target_t *);
//int compare_targets(target_t *, target_t *);
//int del_hash_entry(target_t *);
int print_all_hash_entry(void);
int del_all_hash_entry(void);
int add_hash_entry(target_t *);
void *hash_target_file(void *);
void *hash_target_file2(void *arg);

/* Globals */
config_t set;
child_struct	*cptr;		/* array of Child structures; calloc'ed */
//int lock;
//int waiting;
//char config_paths[CONFIG_PATHS][BUFSIZE];
//hash_t hash;
