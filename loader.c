// Don't need any headers....

// Some libc defines.
#define PT_LOAD 1
#define MAP_ANONYMOUS 0x20
#define MAP_PRIVATE 0x02
#define PROT_READ 0x1
#define PROT_WRITE 0x2
#define PROT_EXEC 0x4
#define O_RDONLY 00000000

unsigned long loader_syscall(int num, ...);

void loader_exit(int code) {
  loader_syscall(231, code);
}

int loader_open(const char* pathname, int flags) {
  return loader_syscall(2, pathname, flags);
}

int loader_close(int fd) {
  return loader_syscall(3, fd);
}

int loader_read(int fd, void* buf, unsigned long count) {
  return loader_syscall(0, fd, buf, count);
}

void* loader_mmap(void* addr,
                  unsigned long length,
                  int prot,
                  int flags,
                  int fd,
                  unsigned long offset) {
  return (void*)loader_syscall(9, addr, length, prot, flags, fd, offset);
}

int loader_mprotect(void* addr, unsigned long len, int prot) {
  return loader_syscall(10, addr, len, prot);
}

unsigned long loader_lseek(int fd, unsigned long offset, int whence) {
  return loader_syscall(8, fd, offset, whence);
}

// Entry point.
int _start() {
  loader_exit(main());
}

#define HEADERSIZE 4096

/* Type for a 16-bit quantity.  */
typedef unsigned short Elf64_Half;

/* Types for signed and unsigned 32-bit quantities.  */
typedef unsigned int Elf64_Word;
typedef int Elf64_Sword;

/* Types for signed and unsigned 64-bit quantities.  */
typedef unsigned long Elf64_Xword;
typedef long Elf64_Sxword;

typedef unsigned long Elf64_Addr;

/* Type of file offsets.  */
typedef unsigned long Elf64_Off;

typedef struct {
  Elf64_Word p_type;    /* Segment type */
  Elf64_Word p_flags;   /* Segment flags */
  Elf64_Off p_offset;   /* Segment file offset */
  Elf64_Addr p_vaddr;   /* Segment virtual address */
  Elf64_Addr p_paddr;   /* Segment physical address */
  Elf64_Xword p_filesz; /* Segment size in file */
  Elf64_Xword p_memsz;  /* Segment size in memory */
  Elf64_Xword p_align;  /* Segment alignment */
} Elf64_Phdr;

void* LoadTarget(const char* path) {
  char header[HEADERSIZE];
  int fd = loader_open(path, O_RDONLY);

  if (loader_read(fd, header, sizeof(header)) != sizeof(header)) {
    return 0;
  }

  unsigned long phoff = *((unsigned long*)(header + 0x20));
  unsigned short num_entries = *((unsigned short*)(header + 0x38));

  int i;
  for (i = 0; i < num_entries; i++) {
    Elf64_Phdr* ph = (Elf64_Phdr*)(header + phoff + (i * sizeof(Elf64_Phdr)));
    if (ph->p_type == PT_LOAD) {
      void* va =
          loader_mmap((void*)ph->p_vaddr, ph->p_memsz + (ph->p_vaddr & 0xFFF),
                      PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
      loader_lseek(fd, ph->p_offset, 0);
      loader_read(fd, (void*)ph->p_vaddr, ph->p_filesz);
      int flags = 0;
      if (ph->p_flags & 1)
        flags |= PROT_EXEC;
      if (ph->p_flags & 2)
        flags |= PROT_WRITE;
      if (ph->p_flags & 4)
        flags |= PROT_READ;
      loader_mprotect(va, ph->p_memsz, flags);

      // TODO(leecam): Now verify hash of this section with
      // a public key in this loader.
    }
  }

  loader_close(fd);

  return *((void**)(header + 0x18));
}

int main() {
  _jump_to_start(LoadTarget("test_prog"));

  return 0;
}