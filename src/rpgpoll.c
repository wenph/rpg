/****************************************************************************
   Program:     $Id: rpgpoll.c,v 0.0.1 2013/04/25 $
   Author:      $Author: wenph@bupt $
   Date:        $Date: 2013/04/25 $
   Description: RPG SNMP get dumps to MongoDB database
****************************************************************************/

#include "common.h"
#include "rpg.h"

/* Yes.  Globals. */
target_t *Targets = NULL;
target_t *current = NULL;
MYSQL mysql;
/* dfp is a debug file pointer.  Points to stderr unless debug=level is set */
FILE *dfp = NULL;
int make_target_file(long);

/* Main rpgpoll */
int main(int argc, char *argv[])
{
    long offset = 0;
    make_target_file(offset);
}

