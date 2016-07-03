#include <stdio.h>
//#include <io.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <getopt.h>

#define MY_RAND_MAX 2147483647
#define LEN_FILENAME 128
#define LEN_CMD 128
#define SCALE 100
#define MAX_STR_LEN 1024

extern char *optarg;
extern int opterr;

char *l_opt_arg;
char* short_options = "n:m:M:N:c:Ld:f:";
struct option long_options[] = {
     { "number-of-topology",     1,   NULL,    'n'     },
     { "minimum-node-number",  1,   NULL,    'm'     },
     { "maximum_node_number",     1,   NULL,    'M'     },
     {"number-of-set", 1, NULL, 'N'},
     {"connectivity-rate",1,NULL,'c'},
     {"filename-prefix",1,NULL,'f'},
     {"ignore-location",0, NULL,'L'},
     {"diirectory",1,NULL,'d'},
     {      0,     0,     0,     0},
};

void help(char *c)
{
                 printf("Usage : %s -n number_of_topology  -m minimum_node_number -M maximum_node_number\n"
                        "           [-c link_connectivity_rate] [-f filename_pre] [-N number_of_sets] [-L]\n"
                        "          [-d directory]\n"
                        "Generate random topologies, where each topology has at least minimum_node_number\n"
                        "nodes and at most maximum_node_number  nodes, and each pair of nodes have a pr-\n"
                        "obability of link_connectivity_rate to have an link between them. These topology\n"
                        "whould be grouped into number_of_sets sets, and each set would have number_of_t-\n"
                        "opology topologies, which would be stored in a txt file named as filename_pre_setId\n"
                        "and in the directory. (default values for optional parameters；link connectivity  \n"
                        "rate = 0.5, file_pre = \"req_\", number_of_sets = 1, directory = current directory)\n\n"
                 "Mandatory arguments to long options are mandatory for short options too.\n"
                 "-n, --number-of-topology = NUMBER-OF-TOPOLOGY : the number of topologoes per each set\n"
                 "-m, --minimum-node-number = MINIMUM-NODE-NUMBER: the minimum node number in each topology\n"
                 "-M, --maximum_node_number = MAXIMUM_NODE_NUMBER: the maximum node number in each topology\n"
                 "-N, --number-of-set = NUMBER-OF-SET: the number of sets\n"
                 "-c, --connectivity-rate = CONNECTIVITY-RATE: the probability of whether there should exist a link between any pair in the topology\n"
                 "-f, --filename-prefix = FILENAME-PREFIX: the prefix of filename to store your topologies\n"
                 "-L, --ignore-location: ignore the locations of nodes\n"
                 "-d, --diirectory = DIIRECTORY: the directory to store the topology file(s)\n",c);
}
int main(int argc,char **argv)
{
    int xseed = (unsigned)time(NULL);
    srand(xseed);

    // parameters
    int n = 0; // # of topo
    int max_node_num = 0; //
    int min_node_num = 0; //
    double link_conn_rate = 0.5; //
    int location_to_file = 0; //
    int topo_sets_num = 1;//
    char topo_folder[LEN_FILENAME];
    int fFlag = 0;
    char filename_pre[LEN_FILENAME];
    int dFlag = 0;

    char c;
    
    if(argc == 1) 
    {
     help(argv[0]);
     return 0;
    }


    opterr = 0;

    while((c = getopt_long(argc, argv, short_options, long_options, NULL))!= -1)
    {
       switch(c)
       {
           case 'n':
               n = atoi(optarg);
               //
               printf("%d\n",n);
               break;
            case 'm':
               min_node_num = atoi(optarg);
               //
               printf("%d\n",min_node_num);
               break;
            case 'M':
               max_node_num = atoi(optarg);
               //
               printf("%d\n",max_node_num);
               break;
            case 'N':
               topo_sets_num = atoi(optarg);
               //
               printf("%d\n",topo_sets_num);
               break;
            case 'c':
               link_conn_rate = atof(optarg);
               printf("%f\n",link_conn_rate);
               break;
             case 'L':
                location_to_file = 1;
                printf("ignore locations\n");
                break;
             case 'd':
                 strcpy(topo_folder,optarg);
                 printf("%s\n",topo_folder);
                 dFlag = 1;
                 break;
             case 'f':
                 fFlag = 1;
                 strcpy(filename_pre,optarg);
                 printf("%s\n",filename_pre);
                 break;
            default:
                 help(argv[0]);
                 return 0;
       }
    }

     if(fFlag == 0)
     {
       strcpy(filename_pre,"topo_");
     }
     if(dFlag == 0)
     {
        strcpy(topo_folder,"./");
     }
     if(n ==0 || min_node_num == 0 || max_node_num == 0)
     {
       printf("missing required parameters!\n"); 
       help(argv[0]);
       return 0;
     }

     // command test
     printf("%d %d %d %d %.4f %d %s %s\n",n,min_node_num,max_node_num,topo_sets_num,link_conn_rate,location_to_file,filename_pre,topo_folder);
     return 0;




}
