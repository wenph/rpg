INCLUDES = -I/usr/include/mysql/ -I ../include -I/usr/local/net-snmp/include/ -I/usr/local/include
LIBS = -L/usr/local/net-snmp/lib/ -L/usr/local/lib

bin_PROGRAMS = my_rpgpoll
my_rpgpoll_SOURCES = my_rpgpoll.c rpgsnmp.c rpghash.c rpgutil.c rpgmysql.c
my_rpgpoll_LDADD = -lmysqlclient -lpthread -lnetsnmp -lmongoc
