#ifndef MABI_H
#define MABI_H

/**
 * @file mabi.h
 * @brief Define memflow kernel module ABI
 *
 * Provides all structures and ioctls for interacting with memflow's kernel module
*/

/* On x86_64 it is a 512 limit, some other arches have less */
#define MAX_MEM_SLOTS 512

#include <linux/types.h>
#include <linux/ioctl.h>

/// @brief structure describing a guest to host memory mapping
struct vm_memslot {
	/// Base physical address in the guest
	__aligned_u64 base;
	/// Host virtual address where the guest base resides in
	__aligned_u64 host_base;
	/// Size of the mapping
	__aligned_u64 map_size;
};

/// @brief structure describing the virtual machine
typedef struct vm_info {
	/// PID of userspace VM monitor.
	__kernel_pid_t userspace_pid;
	/// Number of memory slots in the VM
	__u32 slot_count;
	/// The memory slots, sorted by base address
	struct vm_memslot slots[MAX_MEM_SLOTS];
} vm_info_t;

/// @brief structure describing memory layout of the mapped virtual machine
typedef struct vm_map_info {
	/// Number of memory slots that were mapped in the VM
	__u32 slot_count;
	/// The mapped memory slots, sorted by base address. Slots only include the regions that were mapped in,
	/// so the first, and the last slots may not be the full KVM memslots, if not everything is mapped in
	struct vm_memslot slots[MAX_MEM_SLOTS];
} vm_map_info_t;

#define MEMFLOW_IOCTL_MAGIC 0x6d

/**
 * @brief Get a file descriptor to KVM based virtual machine
 *
 * Returns a file descriptor to KVM VMM in the given process. If the pid is 0, handle from any VMM is returned.
 * If the process does not have a KVM instance, -1 is returned.
 *
 * The KVM instance will not be freed until the returned fd is closed.
*/
#define MEMFLOW_OPEN_VM _IOR(MEMFLOW_IOCTL_MAGIC, 0, __kernel_pid_t)

/**
 * @brief Describe the virtual machine
 *
 * Fills `vm_info_t` structure that describes the virtual machine. Note that the memory layout of the VM may
 * change before it gets mapped in, so it is advised to use MEMFLOW_VM_MAP_INFO after mapping the VM in.
*/
#define MEMFLOW_VM_INFO _IOWR(MEMFLOW_IOCTL_MAGIC, 1, vm_info_t)

/**
 * @brief Describe the memory layout of the mapped VM memory
 *
 * Fills `vm_map_info_t` structure that describes the memory layout.
*/
#define MEMFLOW_VM_MAP_INFO _IOWR(MEMFLOW_IOCTL_MAGIC, 2, vm_map_info_t)

#endif
