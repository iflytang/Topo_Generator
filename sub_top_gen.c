#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#define LEN_FILENAME 128
#define LEN_CMD 128

#define MY_RAND_MAX 2147483647

double dis(int x1, int y1, int x2, int y2) {
  return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}
//计算两点之间的距离
int main(int argc, char **argv) {
  int xseed = (unsigned)time(NULL);  // Current Time
  srand( xseed );

  if (argc != 5) {
    printf("mksub <n> <scale> <link_connectivity_rate> <dir_name>\n");
    exit(1);
  }

  int n = atoi(argv[1]);  // number of nodes in graph
  int scale = atoi(argv[2]);  // grid scale
  double link_conn_rate = atof(argv[3]); // link connectivity rate

  FILE * fp;
  FILE * reqfile;
  char filename[LEN_FILENAME], reqfilename[LEN_FILENAME];
  char cmd[LEN_CMD];
  int *n_x, *n_y, t1, t2;

  int num_nodes, num_edges, from, to;
  int check_delete = system("rm -fr spec/itm*");//删除当前路径下spec目录下的以itm开头的文件
   if(check_delete == -1)
    {
        printf("Error by LonGon: fail to delete files\n");
		exit(0);
    }
    check_delete = system("rm -fr alt/*.alt");//删除当前路径下alt目录下后缀名是.alt的所有文件
    if(check_delete == -1)
    {
        printf("Error by LonGon: fail to delete files\n");
		exit(0);
    }

  sprintf(filename, "spec/itm-specsub");//在spec目录下新建一个itm-specsub文件
  fp = fopen(filename, "w");

//  1: Waxman 1
//  2: Waxman 2
//  3: Pure random
//  4: Doar-Leslie
//  5: Exponential
//  6: Locality

  fprintf(fp, "geo 1 %d\n",xseed); // generate one substrate graph
  fprintf(fp, "%d %d 2 %.2lf 0.2\n", n, scale,link_conn_rate); // scale: 100; method: 2; alpha: 0.5; beta: 0.2; gamma: default
  //fprintf(fp, "ts 1\n");
  //fprintf(fp, "100 1 10\n");
  //fprintf(fp, "%d %d 3 %.2lf\n", n, scale,link_conn_rate);
  fclose(fp);

  sleep(1);

  sprintf(cmd, "itm spec/itm-specsub");
  printf("%s\n", cmd);
  system(cmd);

  sprintf(cmd, "sgb2alt spec/itm-specsub-0.gb alt/sub.alt");
  printf("%s\n", cmd);
  system(cmd);

  char str[1000];
  int j;
  sprintf(filename, "alt/sub.alt");
  fp = fopen(filename, "r");

  	char req_folder[LEN_FILENAME];

	sprintf(req_folder,"./sub_topos/%s",argv[4]);

	if(access(req_folder,2) != 0)//检查写权限，如果文件存在access值为0，否则为-1
    {
        mkdir(req_folder,0777);//权限为777的意思是权限全开，是最高权限
    }
	else
    {
        char common_d[LEN_FILENAME];
        sprintf(common_d,"rm -fr %s/*",req_folder);
        system(common_d);
    }
  sprintf(reqfilename, "%s/sub_%s_%s.txt",req_folder,argv[1],argv[2]);
  reqfile = fopen(reqfilename, "w");

  // remove preamble
  for (j = 0; j < 10; j ++)
    fscanf(fp, "%s", str);

  fscanf(fp, "%d %d %*d %*d", &num_nodes, &num_edges);
  num_edges /= 2;
  
  fprintf(reqfile,"SIZES (number-of-nodes number-of-edges)\n%d %d\n\n",num_nodes,num_edges);
  //fprintf(reqfile, "%d %d\n", num_nodes, num_edges);

  for (j = 0; j < 11; j ++) {
    fscanf(fp, "%s", str);
    //printf("%s\n", str);
  }

  n_x = (int *)malloc(num_nodes * sizeof(int));
  n_y = (int *)malloc(num_nodes * sizeof(int));

  // generate CPU resources for the nodes
  fprintf(reqfile, "LOCATIONS (x,y)\n");
  for (j = 0; j < num_nodes; j ++) {
    fscanf(fp, "%d %d %d %d", &t1, &t2, &n_x[j], &n_y[j]);
    //printf("%d %d %d %d\n", t1, t2, n_x[j], n_y[j]);
    fprintf(reqfile, "%d %d\n", n_x[j], n_y[j]);
  }

  for (j = 0; j < 6; j ++) {
    fscanf(fp, "%s", str);
    //printf("%s\n", str);
  }

  fprintf(reqfile,"EDGES (from-node to-node)\n");
  // generate bandwidth for the links
  for (j = 0; j < num_edges; j ++) {
    fscanf(fp, "%d %d %*d %*d", &from, &to);
    fprintf(reqfile, "%d %d %.2lf\n", from, to,dis(n_x[from],n_y[from],n_x[to],n_y[to]));
  }

  free(n_x);
  free(n_y);

  fclose(fp);
  fclose(reqfile);
  //int check_delete = system("rm -fr spec/itm*");
   if(check_delete == -1)
    {
        printf("Error by LonGon: fail to delete files\n");
		exit(0);
    }
    check_delete = system("rm -fr alt/*.alt");
    if(check_delete == -1)
    {
        printf("Error by LonGon: fail to delete files\n");
		exit(0);
    }
  return 0;
}

