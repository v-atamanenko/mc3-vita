/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef SOLOADER_ERRNO_BIONIC_H
#define SOLOADER_ERRNO_BIONIC_H

/**
 * @file _errno_bionic.h
 * @brief Error codes as defined in Bionic (Android).
 */

#define EPERM_BIONIC 1
#define ENOENT_BIONIC 2
#define ESRCH_BIONIC 3
#define EINTR_BIONIC 4
#define EIO_BIONIC 5
#define ENXIO_BIONIC 6
#define E2BIG_BIONIC 7
#define ENOEXEC_BIONIC 8
#define EBADF_BIONIC 9
#define ECHILD_BIONIC 10
#define EAGAIN_BIONIC 11
#define ENOMEM_BIONIC 12
#define EACCES_BIONIC 13
#define EFAULT_BIONIC 14
#define ENOTBLK_BIONIC 15
#define EBUSY_BIONIC 16
#define EEXIST_BIONIC 17
#define EXDEV_BIONIC 18
#define ENODEV_BIONIC 19
#define ENOTDIR_BIONIC 20
#define EISDIR_BIONIC 21
#define EINVAL_BIONIC 22
#define ENFILE_BIONIC 23
#define EMFILE_BIONIC 24
#define ENOTTY_BIONIC 25
#define ETXTBSY_BIONIC 26
#define EFBIG_BIONIC 27
#define ENOSPC_BIONIC 28
#define ESPIPE_BIONIC 29
#define EROFS_BIONIC 30
#define EMLINK_BIONIC 31
#define EPIPE_BIONIC 32
#define EDOM_BIONIC 33
#define ERANGE_BIONIC 34
#define EDEADLK_BIONIC 35
#define ENAMETOOLONG_BIONIC 36
#define ENOLCK_BIONIC 37
#define ENOSYS_BIONIC 38
#define ENOTEMPTY_BIONIC 39
#define ELOOP_BIONIC 40
#define EWOULDBLOCK_BIONIC EAGAIN_BIONIC
#define ENOMSG_BIONIC 42
#define EIDRM_BIONIC 43
#define ECHRNG_BIONIC 44
#define EL2NSYNC_BIONIC 45
#define EL3HLT_BIONIC 46
#define EL3RST_BIONIC 47
#define ELNRNG_BIONIC 48
#define EUNATCH_BIONIC 49
#define ENOCSI_BIONIC 50
#define EL2HLT_BIONIC 51
#define EBADE_BIONIC 52
#define EBADR_BIONIC 53
#define EXFULL_BIONIC 54
#define ENOANO_BIONIC 55
#define EBADRQC_BIONIC 56
#define EBADSLT_BIONIC 57
#define EDEADLOCK_BIONIC EDEADLK_BIONIC
#define EBFONT_BIONIC 59
#define ENOSTR_BIONIC 60
#define ENODATA_BIONIC 61
#define ETIME_BIONIC 62
#define ENOSR_BIONIC 63
#define ENONET_BIONIC 64
#define ENOPKG_BIONIC 65
#define EREMOTE_BIONIC 66
#define ENOLINK_BIONIC 67
#define EADV_BIONIC 68
#define ESRMNT_BIONIC 69
#define ECOMM_BIONIC 70
#define EPROTO_BIONIC 71
#define EMULTIHOP_BIONIC 72
#define EDOTDOT_BIONIC 73
#define EBADMSG_BIONIC 74
#define EOVERFLOW_BIONIC 75
#define ENOTUNIQ_BIONIC 76
#define EBADFD_BIONIC 77
#define EREMCHG_BIONIC 78
#define ELIBACC_BIONIC 79
#define ELIBBAD_BIONIC 80
#define ELIBSCN_BIONIC 81
#define ELIBMAX_BIONIC 82
#define ELIBEXEC_BIONIC 83
#define EILSEQ_BIONIC 84
#define ERESTART_BIONIC 85
#define ESTRPIPE_BIONIC 86
#define EUSERS_BIONIC 87
#define ENOTSOCK_BIONIC 88
#define EDESTADDRREQ_BIONIC 89
#define EMSGSIZE_BIONIC 90
#define EPROTOTYPE_BIONIC 91
#define ENOPROTOOPT_BIONIC 92
#define EPROTONOSUPPORT_BIONIC 93
#define ESOCKTNOSUPPORT_BIONIC 94
#define EOPNOTSUPP_BIONIC 95
#define EPFNOSUPPORT_BIONIC 96
#define EAFNOSUPPORT_BIONIC 97
#define EADDRINUSE_BIONIC 98
#define EADDRNOTAVAIL_BIONIC 99
#define ENETDOWN_BIONIC 100
#define ENETUNREACH_BIONIC 101
#define ENETRESET_BIONIC 102
#define ECONNABORTED_BIONIC 103
#define ECONNRESET_BIONIC 104
#define ENOBUFS_BIONIC 105
#define EISCONN_BIONIC 106
#define ENOTCONN_BIONIC 107
#define ESHUTDOWN_BIONIC 108
#define ETOOMANYREFS_BIONIC 109
#define ETIMEDOUT_BIONIC 110
#define ECONNREFUSED_BIONIC 111
#define EHOSTDOWN_BIONIC 112
#define EHOSTUNREACH_BIONIC 113
#define EALREADY_BIONIC 114
#define EINPROGRESS_BIONIC 115
#define ESTALE_BIONIC 116
#define EUCLEAN_BIONIC 117
#define ENOTNAM_BIONIC 118
#define ENAVAIL_BIONIC 119
#define EISNAM_BIONIC 120
#define EREMOTEIO_BIONIC 121
#define EDQUOT_BIONIC 122
#define ENOMEDIUM_BIONIC 123
#define EMEDIUMTYPE_BIONIC 124
#define ECANCELED_BIONIC 125
#define ENOKEY_BIONIC 126
#define EKEYEXPIRED_BIONIC 127
#define EKEYREVOKED_BIONIC 128
#define EKEYREJECTED_BIONIC 129
#define EOWNERDEAD_BIONIC 130
#define ENOTRECOVERABLE_BIONIC 131
#define ERFKILL_BIONIC 132
#define EHWPOISON_BIONIC 133

#ifndef ENOTSUP_BIONIC
/** On Linux, ENOTSUP and EOPNOTSUPP are the same despite POSIX saying they should be distinct. */
#define ENOTSUP_BIONIC EOPNOTSUPP_BIONIC
#endif

#endif // SOLOADER_ERRNO_BIONIC_H
