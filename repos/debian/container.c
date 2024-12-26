#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/types.h>

void chroot_container(char *container_dir)
{
  // 检查容器目录是否存在
  if (!container_dir)
  {
    fprintf(stderr, "\033[31m错误: 容器目录未设置！\033[0m\n");
    exit(1);
  }
  // 拒绝使用“/”作为容器目录
  if (strcmp(container_dir, "/") == 0)
  {
    fprintf(stderr, "\033[31m错误: 不允许将“/”用作容器目录。\033[0m\n");
    exit(1);
  }
  // 检查是否以 root 权限运行
  if (getuid() != 0)
  {
    fprintf(stderr, "\033[31m错误: 此程序应以 root 权限运行！\033[0m\n");
    exit(1);
  }
  // 检查是否已经设置 $LD_PRELOAD 环境变量
  char *ld_preload = getenv("LD_PRELOAD");
  if (ld_preload && strcmp(ld_preload, "") != 0)
  {
    fprintf(stderr, "\033[31m错误: 请在运行此程序之前取消设置 $LD_PRELOAD\033[0m\n");
    exit(1);
  }
  // 检查容器目录是否存在
  DIR *datadir = opendir("/data");
  if (datadir)
  {
    system("mount -o remount,exec,suid,dev /data");
  }
  closedir(datadir);

  DIR *rootfsdir = opendir(container_dir);
  if (!rootfsdir)
  {
    fprintf(stderr, "\033[31m错误: 容器目录不存在！\033[0m\n");
    exit(1);
  }
  closedir(rootfsdir);

  // 进入容器文件系统环境
  chroot(container_dir);
  chdir("/");
  mkdir("/sys", S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH | S_IRGRP | S_IWGRP);
  mkdir("/proc", S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH | S_IRGRP | S_IWGRP);
  mkdir("/dev", S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH | S_IRGRP | S_IWGRP);
  // 检查系统运行文件是否已经创建
  DIR *sysmounted;
  if ((sysmounted = opendir("/sys/kernel")) == NULL)
  {
    // 修复在 archlinux 容器中的问题
    mount("/", "/", NULL, MS_BIND, NULL);
    // 挂载 /proc 目录
    mount("proc", "/proc", "proc", MS_NOSUID | MS_NOEXEC | MS_NODEV, NULL);
    // 挂载 /sys 目录为只读
    mount("sysfs", "/sys", "sysfs", MS_NOSUID | MS_NOEXEC | MS_NODEV | MS_RDONLY, NULL);
    mount("tmpfs", "/dev", "tmpfs", MS_NOSUID, "size=65536k,mode=755");
    // 继续挂载 /dev 中的一些其他目录
    mkdir("/dev/pts", S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH | S_IRGRP | S_IWGRP);
    mount("devpts", "/dev/pts", "devpts", 0, "gid=4,mode=620");
    mkdir("/dev/shm", S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH | S_IRGRP | S_IWGRP);
    mount("tmpfs", "/dev/shm", "tmpfs", MS_NOSUID | MS_NOEXEC | MS_NODEV, "mode=1777");
    mkdir("/dev/mqune", S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH | S_IRGRP | S_IWGRP);
    mount("mqune", "/dev/mqune", "mqune", 0, NULL);
    // 保护一些系统运行时目录
    mount("/proc/bus", "/proc/bus", "proc", MS_BIND | MS_RDONLY, NULL);
    mount("/proc/fs", "/proc/fs", "proc", MS_BIND | MS_RDONLY, NULL);
    mount("/proc/irq", "/proc/irq", "proc", MS_BIND | MS_RDONLY, NULL);
    mount("/proc/sys", "/proc/sys", "proc", MS_BIND | MS_RDONLY, NULL);
    mount("/proc/asound", "/proc/asound", "proc", MS_BIND | MS_RDONLY, NULL);
    mount("/proc/scsi", "/proc/scsi", "proc", MS_BIND | MS_RDONLY, NULL);
    mount("/sys/firmware", "/sys/firmware", "sysfs", MS_BIND | MS_RDONLY, NULL);
    // 创建设备节点
    int dev;
    // 在 /dev 中创建系统运行时的节点并修复权限
    dev = makedev(1, 3);
    mknod("/dev/null", S_IFCHR, dev);
    chmod("/dev/null", S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    dev = makedev(5, 1);
    mknod("/dev/console", S_IFCHR, dev);
    chown("/dev/console", 0, 5);
    chmod("/dev/console", S_IRUSR | S_IWUSR | S_IWGRP | S_IWOTH);
    dev = makedev(1, 5);
    mknod("/dev/zero", S_IFCHR, dev);
    chmod("/dev/zero", S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    dev = makedev(5, 2);
    mknod("/dev/ptmx", S_IFCHR, dev);
    chown("/dev/ptmx", 0, 5);
    chmod("/dev/ptmx", S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    dev = makedev(5, 0);
    mknod("/dev/tty", S_IFCHR, dev);
    chown("/dev/tty", 0, 5);
    chmod("/dev/tty", S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    dev = makedev(1, 8);
    mknod("/dev/random", S_IFCHR, dev);
    chmod("/dev/random", S_IRUSR | S_IRGRP | S_IROTH);
    dev = makedev(1, 9);
    mknod("/dev/urandom", S_IFCHR, dev);
    chmod("/dev/urandom", S_IRUSR | S_IRGRP | S_IROTH);
    mkdir("/dev/net", S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH | S_IRGRP | S_IWGRP);
    dev = makedev(10, 200);
    mknod("/dev/net/tun", S_IFCHR, dev);
    // 在 /dev 下创建一些系统运行时的链接文件
    symlink("/proc/self/fd", "/dev/fd");
    symlink("/proc/self/fd/0", "/dev/stdin");
    symlink("/proc/self/fd/1", "/dev/stdout");
    symlink("/proc/self/fd/2", "/dev/stderr");
    symlink("/dev/null", "/dev/tty0");
    // 修复在 Arch Linux 容器中出现的问题
    remove("/etc/mtab");
    unlink("/etc/mtab");
    symlink("/proc/mounts", "/etc/mtab");
  }
  closedir(sysmounted);
}

int main(int argc, char **argv)
{
  char *container_dir = NULL;

  if (argc > 1) {
    container_dir = argv[1];
  }

  if (container_dir)
  {
    chroot_container(container_dir);
  }
  else
  {
    fprintf(stderr, "\033[31m错误：未设置容器目录！\033[0m\n");
    exit(1);
  }

  exit(0);
}
