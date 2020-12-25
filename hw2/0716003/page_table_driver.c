#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "page_table_driver.h"

int fd;
int A[64]={0};

int offset=0, pd=0, pt=0, pdpt=0, pml4=0;
// Obtain my cr3 value (a.k.a. PML4 table physical address)
uint64_t get_cr3_value()
{
	struct ioctl_arg cmd;
	int ret;
	cmd.request[0] = IO_CR3;
	ret = ioctl(fd, IOCTL_REQUEST, &cmd);
	return cmd.ret;
}

// Given a physical address, return the value
uint64_t read_physical_address(uint64_t physical_address)
{
	struct ioctl_arg cmd;
	int ret;
	cmd.request[0] = IO_READ;
	cmd.request[1] = physical_address;
	ret = ioctl(fd, IOCTL_REQUEST, &cmd);
	return cmd.ret;
}

// Write value to a physical address
void write_physical_address(uint64_t physical_address, uint64_t value)
{
	struct ioctl_arg cmd;
	int ret;
	cmd.request[0] = IO_WRITE;
	cmd.request[1] = physical_address;
	cmd.request[2] = value;
	ret = ioctl(fd, IOCTL_REQUEST, &cmd);
}
void trans(uint64_t un)//,offset, int pt, int pd, int pdpt, int pml4)
{
	int i, c=1;
	offset=0, pd=0, pt=0, pdpt=0, pml4=0;

	for (i = 0; un > 0; ++i)
	{
		A[i] = un%2;
		un/=2;
		//printf("%lx\n", un);
	}	

	for (i = 0, c=1; i < 12 ; ++i, c*=2)
	{
		offset+=A[i]*c;
		//printf("%x\n", offset);
	}


	for ( i = 12, c=1; i < 21; ++i, c*=2)
	{
		(pt)+=A[i]*c;	
	}
	(pt)*=8;

	for ( i = 21, c=1; i < 30; ++i, c*=2)
	{
		(pd)+=A[i]*c;	
	}
	(pd)*=8;
	for ( i = 30, c=1; i < 39; ++i, c*=2)
	{
		(pdpt)+=A[i]*c;	
	}
	(pdpt)*=8;
	//printf("%x\n", pdpt);
	for ( i = 39, c=1; i < 48; ++i, c*=2)
	{
		(pml4)+=A[i]*c;	
	}
	(pml4)*=8;
	
}
uint64_t get_entry_pt(uint64_t cr3, uint64_t virtual)
{

	uint64_t entry_addr, entry_pml4, content_pml4, entry_pdpt, content_pdpt, entry_pd, content_pd,entry_pt,content_pt;

	entry_pml4=cr3/(16*16*16);
	entry_pml4*=(16*16*16);
	entry_pml4+=pml4;

	content_pml4=read_physical_address(entry_pml4);	
	entry_pdpt=content_pml4/(16*16*16);
	entry_pdpt%=(16*16*16*16*16*16);
	entry_pdpt*=(16*16*16);
	entry_pdpt+=pdpt;//finish
	content_pdpt=read_physical_address(entry_pdpt);

	entry_pd=content_pdpt/(16*16*16);
	entry_pd%=(16*16*16*16*16*16);
	entry_pd*=(16*16*16);
	entry_pd+=pd;//finish

	content_pd=read_physical_address(entry_pd);

	entry_pt=content_pd/(16*16*16);
	entry_pt%=(16*16*16*16*16*16);
	entry_pt*=(16*16*16);
	entry_pt+=pt;//finish

	content_pt=read_physical_address(entry_pt);
	entry_addr=content_pt/(16*16*16);
	entry_addr%=(16*16*16*16*16*16);
	entry_addr*=(16*16*16);
	entry_addr+=(offset);

	return entry_addr;
}


int main()
{
	char *x = (char*)aligned_alloc(4096, 4096) + 0x123;
	char *y = (char*)aligned_alloc(4096, 4096) + 0x123;
	strcpy(x, "This is a simple HW.");
	strcpy(y, "You have to modify my page table.");
	fd = open("/dev/os", O_RDONLY);
	if(fd < 0) 
	{
		printf("ok\n");
		printf("Cannot open device!\n");
		return 0;
	}

	printf("Before\n");
	printf("x : %s\n", x);
	printf("y : %s\n", y);

	uint64_t entry_addr_x, entry_addr_y, value_pt_x, value_pt_y, cr3, tem_y;
	uint64_t virtual;
	cr3=get_cr3_value();

	virtual=&x;

	trans(virtual);	
	entry_addr_x = get_entry_pt(cr3, virtual);
	virtual=&y;
	trans(virtual);	
	entry_addr_y = get_entry_pt(cr3, virtual);
	
	value_pt_x=read_physical_address(entry_addr_x);
	value_pt_y=read_physical_address(entry_addr_y);
	tem_y=value_pt_y;

	write_physical_address(entry_addr_y, value_pt_x);
	getchar();

	printf("After modifying page table\n");
	printf("x : %s\n", x);
	printf("y : %s\n", y);

	getchar();

	strcpy(y, "When you modify y, x is modified actually.");
	printf("After modifying string y\n");
	printf("x : %s\n", x);
	printf("y : %s\n", y);

	write_physical_address(entry_addr_y, tem_y);

	getchar();

	printf("After recovering page table of y\n");
	printf("x : %s\n", x);
	printf("y : %s\n", y);

	close(fd);
}
