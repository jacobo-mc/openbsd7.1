/*	$OpenBSD: mmfile.c,v 1.19 2019/01/27 14:43:09 deraadt Exp $	*/

/*-
 * Copyright (c) 1999 James Howard and Dag-Erling Co?dan Sm?rgrav
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/mman.h>
#include <sys/stat.h>

#include <err.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include "grep.h"

#ifndef SMALL

#define MAX_MAP_LEN 1048576

mmf_t *
mmopen(int fd, struct stat *st)
{
	mmf_t *mmf;

	mmf = grep_malloc(sizeof *mmf);
	if (st->st_size > SIZE_MAX) /* too big to mmap */
		goto ouch;
	mmf->len = (size_t)st->st_size;
	mmf->fd = fd;
	mmf->base = mmap(NULL, mmf->len, PROT_READ, MAP_PRIVATE, mmf->fd, (off_t)0);
	if (mmf->base == MAP_FAILED)
		goto ouch;
	mmf->ptr = mmf->base;
	mmf->end = mmf->base + mmf->len;
	madvise(mmf->base, mmf->len, MADV_SEQUENTIAL);
	return mmf;

ouch:
	free(mmf);
	return NULL;
}

void
mmclose(mmf_t *mmf)
{
	munmap(mmf->base, mmf->len);
	close(mmf->fd);
	free(mmf);
}

char *
mmfgetln(mmf_t *mmf, size_t *l)
{
	static char *p;

	if (mmf->ptr >= mmf->end)
		return NULL;
	for (p = mmf->ptr; mmf->ptr < mmf->end; ++mmf->ptr)
		if (*mmf->ptr == '\n')
		    break;

	*l = mmf->ptr - p;
	++mmf->ptr;
	return p;
}

#endif
