#pragma once

#include <signal.h>
#include <stdint.h>
#include <string>
#include <sys/types.h>
#include <vector>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int32_t i32;
typedef int64_t i64;

typedef size_t  usize;
typedef ssize_t isize;

/**
 * @brief Send a signal to a process using its pidfd.
 *
 * @param pidfd The pidfd of the target process.
 * @param sig The signal to send.
 * @param info Optional siginfo_t structure (can be NULL).
 * @param flags Flags for the operation (currently unused, should be 0).
 * @return i32 Returns 0 on success, -1 on failure and sets errno.
 */
i32 pidfd_send_signal(i32 pidfd, i32 sig, const siginfo_t* info, u32 flags);

/**
 * @brief Open a pidfd for a given process ID.
 *
 * @param pid The process ID to open a pidfd for.
 * @param flags The flags argument either has the value 0, or contains the following flag:
 *              - PIDFD_NONBLOCK (since Linux 5.6): Returns a non-blocking file descriptor.
 * @return i32 Returns the pidfd on success, -1 on failure and sets errno.
 */
i32 pidfd_open(pid_t pid, u32 flags);

/**
 * @brief Split a command line string into arguments, respecting shell-like quoting.
 *
 * This function splits the input string into a vector of strings, treating spaces as
 * delimiters but respecting quoted substrings (both single and double quotes).
 *
 * @param str The input command line string to split.
 * @return std::vector<std::string> A vector of strings representing the split arguments.
 */
std::vector<std::string> split_shell(const std::string& str);
