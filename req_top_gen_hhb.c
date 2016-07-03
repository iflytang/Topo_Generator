#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#include <time.h>


#define MY_RAND_MAX 2147483647
#define LEN_FILENAME 128
#define LEN_CMD 128
#define SCALE 100
#define MAX_STR_LEN 1024

#define POISSON_MEAN 5 //per 100 time
//#define POISSON_MEAN 50 //per 1000 time
#define TOTAL_TIME 10000 //for the 100 requests

#define MAX_BW 100
#define MAX_CPU 100

int poisson(double lambda) {
  double p;
  int r;
  p=0;
  r=0;
  while (1) {
    p=p-log(rand()/(double)MY_RAND_MAX);
    if (p<lambda) {
      r++;
    } else {
      break;
    }
  }
  return r;
}

int main(int argc,char **argv)
{
    int xseed = (unsigned)time(NULL);  // Current Time
    srand(xseed); // seed

    if (argc != 7)
    {
        printf("Wrong input parameters!\n Usage:\n top_gen <n> <link_r> <max_node_num> <min_node_num>  <location> <dir_name>\nn: the number of requests\nlink_r: the link connectivity rate\nmax_node_num: maximum node number\nmin_node_num: minimum node number\ndir_name: the directory the generated requests would be put in\n\n");
        exit(0);
    }

    //USAGE: top_gen <#req> <link connectivity rate> <directory name>
    int n = atoi(argv[1]);// number of requests
    double link_conn_rate = atof(argv[2]);// link connectivity rate
    int max_node_num = atoi(argv[3]);//maximum node number
    int min_node_num = atoi(argv[4]);//minimum node number
    int location_to_file = atoi(argv[5]);

    FILE * fp_spec;
    FILE * fp_req;
    char specfilename[LEN_FILENAME], reqfilename[LEN_FILENAME];
    char cmd[LEN_CMD];

    int i,j,node_num,num_nodes,num_edges,from,to,t1,t2;
	int k = 0,  countk = 0, p = 0, start, time, duration;
    int *n_x,*n_y;
    int check_delete = system("rm -fr spec/itm*");
    if(check_delete == -1)
    {
        printf("Error by hhb: fail to delete files\n");
        exit(0);
    }
    check_delete = system("rm -fr alt/*.alt");
    if(check_delete == -1)
    {
        printf("Error by hhb: fail to delete files\n");
        exit(0);
    }
	for (i = 0; i < n; i ++)
	{
		sprintf(specfilename,"spec/itm-spec%d",i);
		fp_spec = fopen(specfilename,"w");
		fprintf(fp_spec,"geo 1 %d\n",rand() % MY_RAND_MAX);


		node_num = (rand() % (max_node_num - min_node_num + 1)) + min_node_num;

		printf("req%d, # of nodes:%d\n",i,node_num);
		
		//  1: Waxman 1
		//  2: Waxman 2
		//  3: Pure random
		//  4: Doar-Leslie
		//  5: Exponential
		//  6: Locality

		fprintf(fp_spec,"%d %d 3 %.2lf \n",node_num,SCALE,link_conn_rate);  // scale: SCALE; method: 3; alpha: 0.5; beta: 0.2; gamma: default  3 - pure random
		printf("%d %d 3 %.2lf \n",node_num,SCALE,link_conn_rate);
		fclose(fp_spec);
	}
	sleep(1);

	for (i = 0; i < n; i ++)
	{
		sprintf(cmd,"itm spec/itm-spec%d",i);
		system(cmd);
	}

	for (i = 0; i < n; i ++)
	{
		sprintf(cmd,"sgb2alt spec/itm-spec%d-0.gb alt/%d.alt",i,i);
		system(cmd);
	}

	char req_folder[LEN_FILENAME];

	sprintf(req_folder,"./req_topos/%s",argv[6]);
	
	if(access(req_folder,2) != 0)
	{
		mkdir(req_folder,0777);
	}
	else
	{
		char common_d[LEN_FILENAME];
		DIR *req_dir = opendir(req_folder);
		struct dirent *dir_cont;//dirent是为了获得某个文件夹下的目录而使用的结构体
		int file_num = 0;
		while((dir_cont = readdir(req_dir))!=NULL)//求得文件夹下的文件的个数
		{
			file_num ++;
		}
		closedir(req_dir);
		if (file_num > 2)
		{
			printf("There exist some files in the folder %s, do you want to delete them (y or n)?\n",req_folder);
			char ch;
			scanf("%c",&ch);
			if (ch == 'y' || ch == 'Y')
			{
				sprintf(common_d,"rm -fr %s/*",req_folder);
				system(common_d);
			}
			else
			{
				sprintf(req_folder,"./req_topos/new_%s",argv[6]);
				if(access(req_folder,2) != 0)
				{
					mkdir(req_folder,0777);
				}
			}
		}
		//sprintf(common_d,"rm -fr %s/*",req_folder);

	}

	char str[MAX_STR_LEN];
	for (i = 0; i < n; i ++)
	{
		sprintf(reqfilename,"%s/req%d.txt",req_folder, i);

		fp_req = fopen(reqfilename, "w");
		if (fp_req == NULL)
		{
			printf("Couldn't open file %s\n", reqfilename);
			exit(1);
		}

		//fprintf(fp_req,"request info: number of requests (%d) link connectivity rate (%.2lf) maximum node number (%d) minimum node number (%d)\n\n",n,link_conn_rate,max_node_num,min_node_num);
		printf("generate req %d\n",i);
		sprintf(specfilename,"alt/%d.alt",i);

		fp_spec = fopen(specfilename,"r");

		if (fp_spec == NULL)
		{
			printf("Couldn't open file %s\n",specfilename);
			exit(0);
		}

		// skip
		for (j = 0;; j ++)
		{
			fscanf(fp_spec, "%s", str);
			if (strcmp("zz):",str) == 0)
				break;
		}

		fscanf(fp_spec,"%d %d %*d %*d",&num_nodes,&num_edges);
		num_edges /= 2;
		
		//请求的达到服从泊松分布，请求的持续时间服从指数分布
		if (countk == k) {
			k = 0;
			while( k==0)
				k = poisson(POISSON_MEAN);
			countk = 0;
			printf("k %d\n", k);
			start = (p * TOTAL_TIME*POISSON_MEAN)/n;
			p ++; 
		}
		time = start + (countk+1)*TOTAL_TIME*POISSON_MEAN/(n*(k+1));
		countk ++;
		/*if (rand()/(double)MY_RAND_MAX < LONG_REQ_RATE) {
			duration = LONG_REQ_DURATION;
		} else {
			duration = SHORT_REQ_DURATION;
		}*/
		duration = (int)(-log(rand()/(double)MY_RAND_MAX)*1000);

		fprintf(fp_req,"%d %d 0 %d %d\n",num_nodes,num_edges, time, duration);
		printf("time %d, duration %d\n", time, duration);

		if (location_to_file == 1)
		{
			//skip
			for (j = 0;; j ++)
			{
				fscanf(fp_spec,"%s",str);
				if (strcmp("z):",str) == 0)
					break;
			}
			n_x = (int *)malloc(num_nodes * sizeof(int));
			n_y = (int *)malloc(num_nodes * sizeof(int));

			//fprintf(fp_req, "LOCATIONS (x,y)\n");
			for (j = 0; j < num_nodes; j ++)
			{
				fscanf(fp_spec, "%d %d %d %d", &t1, &t2, &n_x[j], &n_y[j]);
				//printf("%d %d %d %d\n", t1, t2, n_x[j], n_y[j]);
				double t = rand();
				fprintf(fp_req, "%d %d %.2lf\n", n_x[j], n_y[j], t/(double)MY_RAND_MAX * (double)MAX_CPU);
			}
			//fprintf(fp_req,"\n");
		}
		// skip
		for (j = 0;; j ++)
		{
			fscanf(fp_spec, "%s", str);
			if (strcmp("b):",str) == 0)
				break;
		}
		//fprintf(fp_req,"EDGES (from-node to-node)\n");
		for (j = 0; j < num_edges; j ++)
		{
			fscanf(fp_spec, "%d %d %*d %*d", &from, &to);
			fprintf(fp_req, "%d %d %.2lf\n", from, to, rand()/(double)MY_RAND_MAX * (double)MAX_BW);
		}
		//fprintf(fp_req,"\n");
		fclose(fp_spec);
		fclose(fp_req);
		printf("the %d-th request generation finished!\n",i);

	}

    return 0;
}
