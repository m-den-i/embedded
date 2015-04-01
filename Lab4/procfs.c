#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <asm-generic/uaccess.h>

static char msg[128];
static int len = 0;
static int len_check = 1;
int a = 0, b = 0;
static int res = 0;
 
//Simple read/write/open/release functions
int simple_proc_open(struct inode * sp_inode, struct file *sp_file){
	printk(KERN_INFO "proc called open\n");
	return 0;
}
int simple_proc_release(struct inode *sp_indoe, struct file *sp_file){
	printk(KERN_INFO "proc called release\n");
	return 0;
}
int simple_proc_write(struct file *sp_file,const char __user *buf, size_t size, loff_t *offset)
{
	printk(KERN_INFO "proc called write %d\n",(int)size);
	len = size;
	copy_from_user(msg,buf,len);
	return len;
}
int simple_proc_read(struct file *sp_file,char __user *buf, size_t size, loff_t *offset){
	if (len_check)
	 len_check = 0;
	else {
	 len_check = 1;
	 return 0;
	}
	printk(KERN_INFO "proc called read %d\n",(int)size);
	copy_to_user(buf,msg,len);
	return len;
}

//Calculating functions
void add(int a, int b){
	res = a + b;
}
void neg(int a, int b){
	res = a - b;
}
void mul(int a, int b){
	res = a * b;
}
void div(int a, int b){
	if (b != 0)	
	res = a / b;
else res = -1
}

//Input/output for operators
int res_read(struct file *sp_file,char __user *buf, size_t size, loff_t *offset){
	if (len_check)
	 len_check = 0;
	else {
	 len_check = 1;
	 return 0;
	}
	printk(KERN_INFO "proc called read %d\n",size);
	sprintf(msg, "%d", res);
	copy_to_user(buf,msg,len);
	return len;
}
int a_write(struct file *sp_file,const char __user *buf, size_t size, loff_t *offset){
	printk(KERN_INFO "proc called write %d\n",size);
	len = size;
	copy_from_user(msg,buf,len);
	sscanf(msg, "%d", &a);
	return len;
}
int b_write(struct file *sp_file,const char __user *buf, size_t size, loff_t *offset){
	printk(KERN_INFO "proc called write %d\n",size);
	len = size;
	copy_from_user(msg,buf,len);
	sscanf(msg, "%d", &b);
	return len;
}
int op_write(struct file *sp_file,const char __user *buf, size_t size, loff_t *offset){
	printk(KERN_INFO "proc called write %d\n",size);
	len = size;
	copy_from_user(msg,buf,len);
	switch (msg[0]){
		case '+': add(a,b); break;
		case '-': neg(a,b); break;
		case '*': mul(a,b); break;
		case '/': div(a,b); break;
	}
	return len;
}

//Operations for procs
struct file_operations resops = {
	.open = simple_proc_open,
	.read = res_read,
	.write = simple_proc_write,
	.release = simple_proc_release
};

struct file_operations aops = {
	.open = simple_proc_open,
	.read = simple_proc_read,
	.write = a_write,
	.release = simple_proc_release
};

struct file_operations bops = {
	.open = simple_proc_open,
	.read = simple_proc_read,
	.write = b_write,
	.release = simple_proc_release
};

struct file_operations opops = {
	.open = simple_proc_open,
	.read = simple_proc_read,
	.write = op_write,
	.release = simple_proc_release
};

//Creating proc
int create_proc(char * name, struct file_operations * ops){
	if (! proc_create(name,0666,NULL,ops)) {
		remove_proc_entry(name,NULL);
		return 0;
	}
	return 1;
}

static int __init init_simpleproc (void){
	printk(KERN_INFO "init simple proc\n");
	if (create_proc("a", &aops) && create_proc("b", &bops) && create_proc("res", &resops) && create_proc("op", &opops)){
		return 0;
	}
	else {
		printk(KERN_INFO "ERROR! proc_create\n");
		return -1;
	}

}
static void __exit exit_simpleproc(void){
	remove_proc_entry("a",NULL);
	remove_proc_entry("b",NULL);
	remove_proc_entry("res",NULL);
	remove_proc_entry("op",NULL);
	printk(KERN_INFO "exit simple proc\n");
}

module_init(init_simpleproc);
module_exit(exit_simpleproc);
MODULE_AUTHOR("Denis");
MODULE_LICENSE("GPL v3");
MODULE_DESCRIPTION("A simple module to calculate");
