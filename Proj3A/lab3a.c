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
int bsize;
int group_num;
int last_group;
int file_offset;

struct ext2_super_block *sb;
struct ext2_group_desc *gp;
struct ext2_inode inode;

void Pread(int fd, void *buf, size_t count, off_t offset) {
	if (pread(fd, buf, count, offset) < 0) {
		fprintf(stderr, "pread() failed: %s\n", strerror(errno));
		exit(2);
	}
}

void superblock_summary() {
	sb = malloc(sizeof(struct ext2_super_block));
	Pread(file_fd, sb, sizeof(struct ext2_super_block), 1024);
	bsize = EXT2_MIN_BLOCK_SIZE << sb->s_log_block_size;
	printf("SUPERBLOCK,%d,%d,%d,%d,%d,%d,%d\n", sb->s_blocks_count,
	 				sb->s_inodes_count, bsize, sb->s_inode_size,
	  			sb->s_blocks_per_group, sb->s_inodes_per_group, sb->s_first_ino);
}

void group_summary() {

	int group_start = bsize + SUPERBLOCK;
	group_num = ceil((double) sb->s_blocks_count / (double) sb->s_blocks_per_group);
	last_group = sb->s_blocks_count % sb->s_blocks_per_group;
	int group_block_num = sb->s_blocks_per_group;
	gp = malloc(sizeof(struct ext2_group_desc) * group_num);
	
	int i;
	for(i = 0; i < group_num; i++) {
		if(i == group_num -1 && last_group !=0)
			group_block_num = last_group;

		Pread(file_fd, &gp[i], GROUP_TABLE_SIZE, group_start);
		printf("GROUP,%d,%d,%d,%d,%d,%d,%d,%d\n",
				i, group_block_num, sb->s_inodes_per_group,
				gp[i].bg_free_blocks_count, gp[i].bg_free_inodes_count,
				gp[i].bg_block_bitmap, gp[i].bg_inode_bitmap,
				gp[i].bg_inode_table);
		group_start += GROUP_TABLE_SIZE;
	}

}

void bfree_summary() {
	int i;
	int j = 1;
	int start;
	__u8 bitmask = 0;
	__u8 tmp;
	int blocknum;

	for(i=0; i < group_num; i++){
		start = gp[i].bg_block_bitmap * bsize;
		blocknum = (i < group_num - 1) ? sb->s_inodes_per_group : last_group;
		while(j <= blocknum) {	
			if(!bitmask){
				Pread(file_fd, &tmp, 1, start++);
				bitmask = 1;
			}
			if (!(tmp & bitmask))
				printf("BFREE,%d\n", j);
			j++;
			bitmask <<= 1;
		}
	}
}

void ifree_summary() {
	int i;
	int j = 1;
	int inode_start;
	__u8 bitmask = 0;
	__u8 tmp;

	for(i=0;i<group_num;i++){
		inode_start=gp[i].bg_inode_bitmap * bsize;
		while(j <= sb->s_inodes_per_group){
			if(!bitmask){
				Pread(file_fd, &tmp,1, inode_start++);
				bitmask = 1;
			}
			if((tmp & bitmask)==0)
				printf("IFREE,%d\n",j);
			j++;
			bitmask <<= 1;
		}
	}
}

void dirent_summary(int Ninode) {
	int start_d, k;
	int offset = 0;
	struct ext2_dir_entry *dirent = malloc(sizeof(struct ext2_dir_entry));
	for (k=0; k<12; k++){
		start_d = bsize * inode.i_block[k];
		while(offset < bsize) {
			Pread(file_fd, dirent, sizeof(struct ext2_dir_entry), start_d + offset);
			if(!dirent->inode)
				break;		
			printf("DIRENT,%d,%d,%d,%d,%d,'%s'\n", Ninode, offset, dirent->inode,
																			 			dirent->rec_len, dirent->name_len, 
																			 			dirent->name);
			offset += dirent->rec_len;
		}	
	}
}

int scan_block(int blocknum, int level, int Ninode) {
	__u32 childblock;
	int read_offset = bsize * blocknum;
	int ret = 0;
	if (level == 1) {
		Pread(file_fd, &childblock, sizeof(childblock), read_offset);
		if (childblock > 0) ret = file_offset;
		else printf("%d: no childblock\n", blocknum);
		while (childblock) {
			printf("INDIRECT,%d,%d,%d,%d,%d\n", Ninode, level, file_offset, blocknum, childblock);
			file_offset++;
			read_offset += sizeof(childblock);
			Pread(file_fd, &childblock, sizeof(childblock), read_offset);
		}
		return ret;
	} else {
		Pread(file_fd, &childblock, sizeof(childblock), read_offset);
		while (childblock) {
			int res = scan_block(childblock, level - 1, Ninode);
			if (ret == 0) ret = res;
			printf("INDIRECT,%d,%d,%d,%d,%d\n", Ninode, level, res, blocknum, childblock);
			read_offset += sizeof(childblock);
			Pread(file_fd, &childblock, sizeof(childblock), read_offset);
		}
		return ret;
	}
}


void indirect_summary(int Ninode) {
	int indirect_block = inode.i_block[12];
	int d_indirect_block = inode.i_block[13];
	int t_indirect_block = inode.i_block[14];
	if (indirect_block) scan_block(indirect_block, 1, Ninode);
	if (d_indirect_block) scan_block(d_indirect_block, 2, Ninode);
	if (t_indirect_block) scan_block(t_indirect_block, 3, Ninode);
}

void inode_summary() {
	int i,j,k;
	char file_type;
	struct tm* tm_tmp;
	time_t t_tmp;
	int start;
	for (i = 0; i < group_num; i++) {
		start = bsize * gp[i].bg_inode_table;
		for (j = 1;j <= sb->s_inodes_per_group; j++) {
			Pread(file_fd, &inode, sizeof(inode), start);
			start += sizeof(inode);

			//INODE
			if (inode.i_mode && inode.i_links_count){

				if ((inode.i_mode >> 12) == 0xA) file_type = 's';
				else if ((inode.i_mode >> 12) == 0x8) file_type ='f';
				else if ((inode.i_mode >> 12) == 0x4) file_type ='d';
				else file_type= '?';
				//printf("rrrrrrrrrrrr%d\n", inode.i_mode & 0x8000);
				printf("INODE,%d,%c,%o,%d,%d,%d,",
						j, file_type, inode.i_mode & 511, inode.i_uid, inode.i_gid,
						inode.i_links_count);

				//creation time
				t_tmp = (time_t) inode.i_ctime;
				tm_tmp = gmtime(&t_tmp);
				printf("%02d/%02d/%2d %02d:%02d:%02d",
						tm_tmp->tm_mon+1,tm_tmp->tm_mday,(tm_tmp->tm_year)%100,
						tm_tmp->tm_hour,tm_tmp->tm_min,tm_tmp->tm_sec);
				//modification time
				t_tmp=(time_t)inode.i_mtime;
				tm_tmp=gmtime(&t_tmp);
				printf(",%02d/%02d/%2d %02d:%02d:%02d",
						tm_tmp->tm_mon+1,tm_tmp->tm_mday,(tm_tmp->tm_year)%100,
						tm_tmp->tm_hour,tm_tmp->tm_min,tm_tmp->tm_sec);
				//time of last access
				t_tmp=(time_t)inode.i_atime;
				tm_tmp=gmtime(&t_tmp);
				printf(",%02d/%02d/%2d %02d:%02d:%02d",
						tm_tmp->tm_mon+1,tm_tmp->tm_mday,(tm_tmp->tm_year)%100,
						tm_tmp->tm_hour,tm_tmp->tm_min,tm_tmp->tm_sec);


				printf(",%d,%d",inode.i_size,inode.i_blocks);
				for (k=0; k<15; k++)
					printf(",%d",inode.i_block[k]);
				printf("\n");

				if ((file_type == 'd') || (file_type == 'f')) {
					if (file_type == 'd') {
						dirent_summary(j);
					}
					file_offset = 12;
					indirect_summary(j);
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
	return 0;
}