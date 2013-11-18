#include "common.h"
#include "rpg.h"

/* Yes.  Globals. */
target_t *Targets = NULL;
target_t *current = NULL;
MYSQL mysql;
/* dfp is a debug file pointer.  Points to stderr unless debug=level is set */
FILE *dfp = NULL;

int main (int argc, char **argv)
{
    target_session_t    target_session;
    crew_t              crew;
    int                 arg;
    //int               count = 0;
    char                options[128] = "hc:f:";
    int		            i, maxfd, fds, rc, nchildren;
    int                 r, w;
    //void		        sig_int(int);
    fd_set		        rset, masterset;
    filestruct          *fsp;

    config_defaults(&set);/* Set default environment */
    /*
    if(argc < 2){
    usage(argv[0]);
    exit(0);
    }
    */
    while ((arg = getopt(argc, argv, options)) != EOF)
    {
        switch (arg)
        {
        case 'c':
            if (optarg != NULL)
            {
                copy_config_file_arg(&set, optarg);
            }
            else
            {
                usage(argv[0]);
                exit(0);
            }
            break;
        case 'f':
            if (optarg != NULL)
            {
                copy_ipaddr_file_arg(&set, optarg);
            }
            else
            {
                usage(argv[0]);
                exit(0);
            }
            break;
        case 'h':
            usage(argv[0]);
            exit(0);
        default:
            usage(argv[0]);
            exit(0);
        }
    }
    config_file(&set);

    dfp = stderr;
    print_cur_time();
    /* Attempt to connect to the MySQL Database */
    /*
    if (!(set.dboff))
    {
        if (rpg_dbconnect(set.dbdb, &mysql) < 0)
        {
            fprintf(stderr, "** Database error - check configuration.\n");
            exit(-1);
        }
        if (!mysql_ping(&mysql))
        {
            if (set.verbose >= LOW)
                printf("connected.\n");
        }
        else
        {
            printf("server not responding.\n");
            exit(-1);
        }
    }
    */
    /* Attempt to connect to the mongodb Database */

    if( mongo_client( &(set.conn), "127.0.0.1", 27017 ) != MONGO_OK )
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
        exit( 1 );
    }


    snmp_oid_initialize();
    //target_session.file = "/home/apple/workplace/rpg/src/ipaddr.txt";

    if(set.n_forks == 1)
    {
        //TODO
        int n;
        if((n = make_target_list(&set, 0)) != 0)
            printf("sucess make %d Targets in list\n", n);
        else
            printf("make hash entry err\n");

        if((n = snmp_asynchronous_poll(0)) == 0)
            printf("sucess poll Targets list\n");
        else
            printf("poll Targets list err\n");

        if((n = del_all_hash_entry()) != 0)
            printf("sucess delete %d Targets in list\n", n);
        else
            printf("delete hash entry err\n");
        print_cur_time();
    }
    else if(set.n_forks > 1)
    {
        mongo_destroy( &(set.conn) );
        fsp = (filestruct *)malloc(sizeof(struct filestruct));
        fsp->iseof = 0;
        fsp->offset = 0;
        char *read_buf = (char*)calloc(1 , 30);
        //socklen_t	addrlen, clilen;
        //struct sockaddr	*cliaddr;

        FD_ZERO(&masterset);

        nchildren = set.n_forks - 1;//the number of children

        cptr = calloc(nchildren, sizeof(child_struct));

        // prefork all the children
        for (i = 0; i < nchildren; i++)
        {
            child_make(i);	// parent returns
            FD_SET(cptr[i].child_pipefd, &masterset);
            maxfd = max(maxfd, cptr[i].child_pipefd);
        }

        //Signal(SIGINT, sig_int);
        //main process to control the children process
        while(1)
        {
            rset = masterset;

            // check for new available children
            if(!(fsp->iseof))
            {
                printf("assign task\n");
                for (i = 0; i < nchildren; i++)
                {
                    if (cptr[i].child_status == 0)  //0 = ready
                    {

                        if(write(cptr[i].child_pipefd, &(fsp->offset), sizeof(fsp->offset)) == -1)//write offset to the children
                        {
                            printf("write offset to children error\n");
                            exit(0);
                        }
                        printf("P NO.%d write %ld\n", i, fsp->offset);
                        cptr[i].child_status = 1;	// mark child as busy
                        cptr[i].child_count++;
                        fetchNextOffset(&set, fsp);//调整offset并判断是否文件末尾
                        if(fsp->iseof)
                            break;
                    }
                }

                if (i == nchildren)
                {
                    printf("no available children\n");
                    //exit(0);
                }
            }
            else
            {
                //文件末尾了，算时间，看是否sleep
                printf("success complete the big task*********************************\n");
                fsp->iseof = 0;
                continue;
                //exit(0);
            }
            // find any newly-available children
            while(1)
            {
                //printf("before select\n");
                fds = select(maxfd + 1, &rset, NULL, NULL, NULL);//err!!!
                //printf("after select\n");
                if(fds < 0)
                {
                    printf("select err\n");
                    continue;
                }
                else if(fds == 0)
                {
                    printf("timeout\n");
                    continue;
                }
                else
                {
                    for (i = 0; i < nchildren; i++)
                    {
                        if (FD_ISSET(cptr[i].child_pipefd, &rset))
                        {
                            if ( (r = read(cptr[i].child_pipefd, read_buf, 30)) == 0)
                            {
                                printf("child %d terminated unexpectedly\n", i);
                                exit(0);
                            }
                            print_cur_time();
                            printf("P NO.%d read %s\n", i, read_buf);
                            cptr[i].child_status = 0;
                            if (--fds == 0)
                            {
                                printf("goto the goto\n");
                                break;	//there is no need to poll the rest of fds
                            }
                        }
                    }
                }
                printf("goto the assign task\n");
                break;
            }
        }
    }
    else printf("n_forks setting err!\n");
    return 0;
}
