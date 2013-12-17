#include "common.h"
#include "rpg.h"
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

extern FILE *dfp;

void usage(char *prog)
{
    printf("Usage:  %s [OPTIONS]\n", prog);
    printf("-h, --help\t\tdisplay this usage message\n");
    printf("-c, \t\t\tthe route of the config file\n");
    printf("-f, \t\t\tthe route of the ipaddr file\n");
}

void print_cur_time()
{
    
    struct timeval now;
//    struct timezone tz;
//    struct tm *tm;

//    gettimeofday(&now, &tz);
    gettimeofday(&now, NULL);
//    tm = localtime(&now.tv_sec);
//    fprintf(stdout, "%.2d:%.2d:%.2d.%.6ld \n", tm->tm_hour, tm->tm_min, tm->tm_sec, now.tv_usec);
    printf("%d:%d\n", now.tv_sec, now.tv_usec);
    /*
    time_t timep;
    time(&timep);
    fprintf(stdout,"%s",asctime(gmtime(&timep)));
    */
}

void config_defaults(config_t * set)
{
//   set->interval = DEFAULT_INTERVAL;
//   set->highskewslop = DEFAULT_HIGHSKEWSLOP;
//   set->lowskewslop = DEFAULT_LOWSKEWSLOP;
//   set->out_of_range = DEFAULT_OUT_OF_RANGE;
//   set->snmp_ver = DEFAULT_SNMP_VER;
//   set->snmp_port = DEFAULT_SNMP_PORT;
//   set->threads = DEFAULT_THREADS;
//    strncpy(set->dbhost, DEFAULT_DB_HOST, sizeof(set->dbhost));
//    strncpy(set->dbdb, DEFAULT_DB_DB, sizeof(set->dbdb));
//    strncpy(set->dbuser, DEFAULT_DB_USER, sizeof(set->dbuser));
//    strncpy(set->dbpass, DEFAULT_DB_PASS, sizeof(set->dbpass));
//    strncpy(set->config_file, DEFAULT_CONFIG_FILE, sizeof(set->config_file));
//    strncpy(set->ipaddr_file, DEFAULT_IPADDR_FILE, sizeof(set->ipaddr_file));
//    set->n_forks = 1;
//    set->dboff = FALSE;
//   set->withzeros = FALSE;
//    set->verbose = OFF;
//   strncpy(config_paths[0], CONFIG_PATH_1, sizeof(config_paths[0]));
//   snprintf(config_paths[1], sizeof(config_paths[1]), "%s/etc/", RTG_HOME);
//   strncpy(config_paths[2], CONFIG_PATH_2, sizeof(config_paths[1]));
    strncpy(set->config_file, DEFAULT_CONFIG_FILE, sizeof(set->config_file));
    strncpy(set->ipaddr_file, DEFAULT_IPADDR_FILE, sizeof(set->ipaddr_file));
    return;
}

void copy_config_file_arg(config_t *set, char *optarg)
{
    strncpy(set->config_file, optarg, sizeof(set->config_file));
}

void copy_ipaddr_file_arg(config_t *set, char *optarg)
{
    strncpy(set->ipaddr_file, optarg, sizeof(set->ipaddr_file));
}

int split_para(char *buffer, char *para_name, char *para_value)
{
    int i, j = 0, flag = 1;//i for para_name, j for para_value, flag for the flag between para_name and para_value
    for(i = 0; i < strlen(buffer); i++)
    {
        if(buffer[i] != '=' && flag)
        {
            para_name[i] = buffer[i];
            para_name[i+1] = '\0';
        }
        else
        {
            if(flag == 1)
            {
                i++;
                flag = 0;
            }
            para_value[j++] = buffer[i];
            para_value[j] = '\0';
        }
    }
    para_value[j - 1] = '\0'; //delete the '\n'
    return 0;
}

void switch_para(config_t *set, char *para_name, char *para_value)
{
    //printf("switch_para:%s:%s\n", para_name, para_value);
    if(!strcmp(para_name, "n_forks"))
    {
        set->n_forks = atoi(para_value);
    }
    else if(!strcmp(para_name, "dbhost"))
    {
        strcpy(set->dbhost, para_value);
    }
    else if(!strcmp(para_name, "dbdb"))
    {
        strcpy(set->dbdb, para_value);
    }
    else if(!strcmp(para_name, "dbuser"))
    {
        strcpy(set->dbuser, para_value);
    }
    else if(!strcmp(para_name, "dbpass"))
    {
        strcpy(set->dbpass, para_value);
    }
    else if(!strcmp(para_name, "verbose"))
    {
        strcpy(set->dbpass, para_value);
    }
    else if(!strcmp(para_name, "ip_count_interval"))
    {
        set->ip_count_interval = atoi(para_value);
    }
    else
    {
        fprintf(stderr, "** config file format error - check configuration.\n");
        exit(1);
    }
}

int config_file(config_t *set)
{
    FILE    *fp;
    int     str_len, locate_sig;
    char    buffer[BUFSIZE], para_name[BUFSIZE], para_value[BUFSIZE];
    if ((fp = fopen(set->config_file, "r")) == NULL)
    {
        fprintf(stderr, "\nCould not open file for reading '%s'.\n", set->config_file);
        exit (1);
    }
    while(!feof(fp))
    {
        if(fgets(buffer, BUFSIZE, fp) ==0)
            continue;
        if (!feof(fp) && buffer[0] != '#' && buffer[0] != ' ' && buffer[0] != '\n')
        {
            split_para(buffer, para_name, para_value);
            switch_para(set, para_name, para_value);
        }
    }
    fclose(fp);
    return 0;
}

void convert_dot2line(char *source)
{
    int i=0;
    while(source[i] != '\0')
    {
        if(source[i] == '.')
            source[i] = '_';
        i++;
    }
}

pid_t child_make(int i)
{
    int	sockfd[2];
    pid_t	pid;
    void	child_main(int, int);

    socketpair(AF_LOCAL, SOCK_STREAM, 0, sockfd);

    if ( (pid = fork()) > 0)
    {
        close(sockfd[1]);
        cptr[i].child_pid = pid;
        cptr[i].child_pipefd = sockfd[0];
        cptr[i].child_status = 0;
        return(pid);		/* parent */
    }

    close(sockfd[0]);
    child_main(i, sockfd[1]);	/* never returns */
}
/* end child_make */

/* include child_main */
void child_main(int i, int sockfd_1)
{
    char			c;
    long		    offset;
    int 			w,r;
    char *string = "over";//task is over and tell the parent
    int             n;
    //char *buf = (char*)calloc(1 , 30);
    //void			web_child(int);

    //printf("child %ld starting\n", (long) getpid());
    if( mongo_client( &(cptr[i].conn), "127.0.0.1", 27017 ) != MONGO_OK )
    {
        switch( set.conn.err )
        {
        case MONGO_CONN_NO_SOCKET:
            printf( "FAIL: Could not create a socket!\n" );
            break;
        case MONGO_CONN_FAIL:
            printf( "FAIL: Could not connect to mongod. Make sure it's listening at 127.0.0.1:27017.\n" );
            break;
        }
        mongo_destroy( &(cptr[i].conn) );
        exit( 1 );
    }
    while(1)
    {
        //if ( (n = read_fd(STDERR_FILENO, &c, 1, &connfd)) == 0){
        if( (r = read(sockfd_1, &offset , sizeof(offset) )) == -1) //read offset from parent
        {
            printf("read_fd returned 0");
            exit(0);
        }
        //n = atol(buf);
        printf("C NO.%d read %ld\n",i, offset); /* process request */

        if((n = make_target_list(&set, offset)) != 0)
            printf("sucess make %d Targets in list\n", n);
        else
            printf("delete hash entry err\n");

        if((n = snmp_asynchronous_poll(i)) == 0)
            printf("sucess poll Targets list\n");
        else
            printf("poll Targets list err\n");

        if((n = del_all_hash_entry()) != 0)
            printf("sucess delete %d Targets in list\n", n);
        else
            printf("delete hash entry err\n");

        //sleep(5);// i am doing task, yeah i am working.

        /* tell parent we're ready again */
        if( ( w = write(sockfd_1, string , strlen(string) ) ) == -1 )
        {
            printf("write socket to parent error\n");
            exit(0);
        }
        printf("C NO.%d write over\n",i);
    }
}
/* end child_main */

long fetchNextOffset(config_t *set, filestruct *fsp)
{
    FILE *fp;
    int count = set->ip_count_interval;
    char buffer[512];
    if ((fp = fopen(set->ipaddr_file, "r")) == NULL)
    {
        fprintf(stderr, "Could not open %s for reading.\n", set->ipaddr_file);
        exit (1);
    }
    fseek(fp, fsp->offset, SEEK_SET);
    while (count && !feof(fp))
    {
        if(fgets(buffer, 512, fp) ==0)
            continue;
        if (!feof(fp) && buffer[0] != '#' && buffer[0] != ' ' && buffer[0] != '\n')
        {
            //printf("pid %ld: %s", getpid(), buffer);
            count--;
        }
    }
    if(feof(fp))//end of the file
    {
        //printf("pid %ld: end of the file and the offset is: %ld\n", (long) pid, ftell(fp));
        fsp->iseof = 1;
        fsp->offset = 0;
    }
    else//count == 0
    {
        fsp->offset = ftell(fp);
    }
    fclose(fp);
    //return (fsp->offset);
}

