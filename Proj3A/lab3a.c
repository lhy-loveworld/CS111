//Arthor: Hongyang Li, Zhenli Jiang
//This is the source module for lab3a of CS111.

#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "ext2_fs.h"

#define SUPERBLOCK 1024
#define GROUP_TABLE_SIZE 32

int file_fd;
int write_fd;
int bsize;
int group_num;
int last_group;

struct ext2_super_block *sb;
struct ext2_group_desc *gp;
struct ext2_inode inode;

__u32 int32;
__u16 int16;
__u8 int8;

void superblock_summary() {
	sb = malloc(sizeof(struct ext2_super_block));
	if (pread(file_fd, sb, sizeof(struct ext2_super_block), 1024) < 0) {
		fprintf(stderr, "pread() failed: %s\n", strerror(errno));
		exit(2);
	}
	bsize = EXT2_MIN_BLOCK_SIZE << sb->s_log_block_size;
	printf("SUPERBLOCK,%d,%d,%d,%d,%d,%d,%d\n", sb->s_blocks_count,
	 				sb->s_inodes_count, bsize, sb->s_inode_size,
	  			sb->s_blocks_per_group, sb->s_inodes_per_group, sb->s_first_ino);
	printf("%d\n", sb->s_block_group_nr);
}

void group_summary() {

	//first block following the superblock
	int group_start = bsize + SUPERBLOCK;
	//not sure...
	group_num = ceil((double) sb->s_blocks_count / (double) sb->s_blocks_per_group);
	last_group = sb->s_blocks_count % sb->s_blocks_per_group;
	int group_block_num = sb->s_blocks_per_group;
	gp = malloc(sizeof(struct ext2_group_desc) * group_num);
	
	//assume that # of inodes/blocks in each group is the same?
	int i;
	for(i = 0; i < group_num; i++) {
		if(i == group_num -1 && last_group !=0)
			group_block_num = last_group;

		pread(file_fd, &gp[i], GROUP_TABLE_SIZE, group_start);
		printf("GROUP,%d,%d,%d,%d,%d,%d,%d,%d\n",
				i, group_block_num, sb->s_inodes_per_group,
				gp[i].bg_free_blocks_count, gp[i].bg_free_inodes_count,
				gp[i].bg_block_bitmap, gp[i].bg_inode_bitmap,
				gp[i].bg_inode_table);
		group_start += GROUP_TABLE_SIZE;
	}

}

void bfree_summary() {
	int i,j;
	int start;
	char res = -1;
	char tmp;
	int blocknum;

	for(i=0; i<group_num; i++){
		start = gp[i].bg_block_bitmap * bsize;
		blocknum = (i < group_num - 1) ? sb->s_inodes_per_group : last_group;
		while(j <= blocknum){	//the last one might be less than it.
			if(res == -1){
				pread(file_fd, &tmp, 1, start++);
				res = 7;
			}
			if((int8 & (1 << res))==0)
				printf("BFREE,%d\n", j);
			j++;
			res--;
		}
	}
}

void ifree_summary() {
	int i,j=1;
	int inode_start;
	int res=-1;
	char tmp;

	for(i=0;i<group_num;i++){
		inode_start=gp[i].bg_inode_bitmap*bsize;
		while(j<=sb->s_inodes_per_group){	//the last one might be less than it.
			if(res==-1){
				pread(file_fd, &tmp,1, inode_start++);
				res=7;
			}
			if((tmp & (1<<res))==0)
				printf("IFREE,%d\n",j);
			j++;
			res--;
		}
	}
}

/*void indirect_summary(int Ninode) {
	printf("INDIRECT,%d", Ninode);
	int indirect = 
}*/

void inode_summary() {
	int i,j,k;
	char file_type;
	char* name=malloc(sizeof(char)*256);
	struct tm* tm_tmp;
	time_t t_tmp;
	for(i=0;i<group_num;i++){
		int start=bsize*(gp[i].bg_inode_table);
		for(j=1;j<=sb->s_inodes_per_group;j++){
			pread(file_fd,&inode,sizeof(inode),start);
			start+=sizeof(inode);

			//INODE
			if(inode.i_mode && inode.i_links_count){

				if(inode.i_mode & 0xA000) file_type = 's';
				else if(inode.i_mode & 0x8000) file_type='f';
				else if (inode.i_mode & 0x4000) file_type='d';
				else file_type= '?';

				printf("INODE,%d,%c,%o,%d,%d,%d,",
						j,file_type,inode.i_mode,inode.i_uid,inode.i_gid,
						inode.i_links_count);

				//creation time
				t_tmp=(time_t)inode.i_ctime;
				tm_tmp=gmtime(&t_tmp);
				printf("%02d/%02d/%2d %02d:%02d:%02d, GMT",
						tm_tmp->tm_mon+1,tm_tmp->tm_mday,(tm_tmp->tm_year)%100,
						tm_tmp->tm_hour,tm_tmp->tm_min,tm_tmp->tm_sec);
				//modification time
				t_tmp=(time_t)inode.i_mtime;
				tm_tmp=gmtime(&t_tmp);
				printf(",%02d/%02d/%2d %02d:%02d:%02d, GMT",
						tm_tmp->tm_mon+1,tm_tmp->tm_mday,(tm_tmp->tm_year)%100,
						tm_tmp->tm_hour,tm_tmp->tm_min,tm_tmp->tm_sec);
				//time of last access
				t_tmp=(time_t)inode.i_atime;
				tm_tmp=gmtime(&t_tmp);
				printf(",%02d/%02d/%2d %02d:%02d:%02d, GMT",
						tm_tmp->tm_mon+1,tm_tmp->tm_mday,(tm_tmp->tm_year)%100,
						tm_tmp->tm_hour,tm_tmp->tm_min,tm_tmp->tm_sec);


				printf(",%d,%d",inode.i_size,inode.i_blocks);
				for(k=0;k<15;k++)
					printf(",%d",inode.i_block[k]);
				printf("\n");

				//DIRECT
				if(file_type== 'd'){
					int start_d, length=0, offset=0;
					for(k=0;k<12;k++){
						start_d=bsize*inode.i_block[k];
						while(offset<1024){
							pread(file_fd,&int32,4,start_d+offset);

							/******when to terminate?*****/
							if(int32==0) continue;		

							printf("DIRECT,%d,%d,%d,",j,offset,int32);
							//entry length
							pread(file_fd,&int16,2,start_d+offset+4);
							//name length
							pread(file_fd,&int8,1,start_d+offset+6);						
							printf("%d,%d,",int16,int8);
							//name
							pread(file_fd,name,int8,start_d+offset+8);
							name[int8]='\0';
							printf("%s\n",name);

							offset+=int16;
						}	
					}
				}
			}
		}
	}
	
}

int main(int argc, char **argv){
	if(argc<2){
		fprintf(stderr, "Error: number of argument.\n");
		exit(1);
	}

	file_fd = open(argv[1], O_RDONLY);

	if(file_fd == -1){
		fprintf(stderr, "Fail to open the image.\n");
		exit(1);
	}	

	superblock_summary();
	group_summary();
	bfree_summary();
	ifree_summary();
	inode_summary();
	if (close(file_fd) < 0) {
		fprintf(stderr, "close() failed: %s\n", strerror(errno));
		exit(2);
	}
}